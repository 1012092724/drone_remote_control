#include "App_DataProcess.h"

JoyStickBias_Struct JoyStickBias;

/* 均值滤波(滑动窗口) */
#define MEAN_FILTER(filter, valuePointer)             \
    filter.sum -= filter.values[filter.index];        \
    filter.values[filter.index] = *valuePointer;      \
    filter.sum += filter.values[filter.index];        \
    filter.index  = (filter.index + 1) % FILTER_SIZE; \
    *valuePointer = filter.sum / FILTER_SIZE

/**
 * @description: 摇杆和按键数据处理模块启动
 * @return {*}
 */
void App_DataProcess_Init(void)
{
    Int_JoyStick_Init();
    Int_JoyStick_Scan();
    debug_printfln("JoyStick Data Process Start!");
}

/**
 * @description: 摇杆极性数据处理
 * @return {*}
 */
static void App_DataProcess_JoyStickDataPolarity(void)
{
    /* 摇杆的极性, 与我们的感知正好相反, 先修改极性 */
    joyStick.THR = 1000 - joyStick.THR * 1000 / 4095;
    joyStick.YAW = 1000 - joyStick.YAW * 1000 / 4095;
    joyStick.PIT = 1000 - joyStick.PIT * 1000 / 4095;
    joyStick.ROL = 1000 - joyStick.ROL * 1000 / 4095;
}

/**
 * @description: 考虑摇杆的读取的值的偏移
 * @return {*}
 */
static void App_DataProcess_JoyStickDataWithBias(void)
{
    /* 为了防止出现负数, 对移除偏移量的摇杆数据进行限幅处理 */
    joyStick.THR = LIMIT(joyStick.THR - JoyStickBias.THRBias, 0, 1000);
    joyStick.YAW = LIMIT(joyStick.YAW - JoyStickBias.YAWBias, 0, 1000);
    joyStick.PIT = LIMIT(joyStick.PIT - JoyStickBias.PITBias, 0, 1000);
    joyStick.ROL = LIMIT(joyStick.ROL - JoyStickBias.ROLBias, 0, 1000);
}

/* 初始化滤波缓冲区 */
static FilterBuffer_Struct THRFilter = {{0}, 0, 0};
static FilterBuffer_Struct YAWFilter = {{0}, 0, 0};
static FilterBuffer_Struct PITFilter = {{0}, 0, 0};
static FilterBuffer_Struct ROLFilter = {{0}, 0, 0};
/**
 * @description: 对摇杆4个方向数据进行滤波处理
 *              这次我们使用平均值滤波(滑动窗口滤波)
 *              取最近的10个值的平均值
 * @return {*}
 */
static void App_DataProcess_JoyStickDataFilter(void)
{
    /*
        // 1. 减去旧值
        THRFilter.sum -= THRFilter.values[THRFilter.index];
        // 2. 旧值的地方存储新值
        THRFilter.values[THRFilter.index] = joyStick.THR;
        // 3. 加上新值
        THRFilter.sum += THRFilter.values[THRFilter.index];
        // 4. 更新索引
        THRFilter.index = (THRFilter.index + 1) % FILTER_SIZE;
        //5. 计算均值
        joyStick.THR = THRFilter.sum / FILTER_SIZE;

        为了提高可读性,使用宏定义滤波算法
    */
    MEAN_FILTER(THRFilter, &joyStick.THR);
    MEAN_FILTER(YAWFilter, &joyStick.YAW);
    MEAN_FILTER(PITFilter, &joyStick.PIT);
    MEAN_FILTER(ROLFilter, &joyStick.ROL);
}

/**
 * @description: 处理摇杆数据
 * @return {*}
 */
void App_DataProcess_JoyStickDataProcess(void)
{
    Int_JoyStick_Scan(); /* 扫描摇杆数据 */

    App_DataProcess_JoyStickDataPolarity(); /* 对摇杆数据进行极性处理 */

    App_DataProcess_JoyStickDataWithBias(); /* 考虑摇杆的偏差 */

    App_DataProcess_JoyStickDataFilter(); /* 对摇杆数据进行滤波 */
}

/**
 * @description: 对摇杆进行偏移量校准.  读取100次数据, 求出平均偏移量
 *  当长按右上按键的时候,调用次函数进行校准
 * @return {*}
 */
static void App_DataProcess_JoyStickCalcBias()
{
    JoyStickBias.PITBias = 0;
    JoyStickBias.ROLBias = 0;
    JoyStickBias.YAWBias = 0;
    JoyStickBias.ROLBias = 0;
    /* 连续读取100次, 求取平均偏移量 */
    for (uint8_t i = 0; i < 100; i++) {
        Int_JoyStick_Scan();                    /* 扫描摇杆数据 */
        App_DataProcess_JoyStickDataPolarity(); /* 对摇杆数据进行极性处理 */
        JoyStickBias.THRBias += joyStick.THR - 0;
        JoyStickBias.YAWBias += joyStick.YAW - 500;
        JoyStickBias.PITBias += joyStick.PIT - 500;
        JoyStickBias.ROLBias += joyStick.ROL - 500;
    }

    JoyStickBias.THRBias /= 100;
    JoyStickBias.YAWBias /= 100;
    JoyStickBias.PITBias /= 100;
    JoyStickBias.ROLBias /= 100;
}

/**
 * @description: 按键数据处理:
 * @return {*}
 */
void App_DataProcess_KeyDataProcess(void)
{

    /* 处理按键事件 */
    /*
    KEY EVENT
        KEY_EVENT_NONE,          // 无事件
        KEY_EVENT_DOWN,          // 按键按下（瞬时事件）
        KEY_EVENT_SHORT_RELEASE, // 短按释放（<500ms）
        KEY_EVENT_LONG_RELEASE,  // 长按释放（≥500ms）
        KEY_EVENT_LONG_HOLD      // 长按持续（周期性触发）

    KEY ID
        KEY_ID_LEFT_X,
        KEY_ID_RIGHT_X,
        KEY_ID_UP,
        KEY_ID_DOWN,
        KEY_ID_LEFT,
        KEY_ID_RIGHT,
    */
    /*
     1. 如果是右上按键长按, 则进行校准
         THR 拉到最低, 实现零值校准  0
         其他由于会自动回中, 则进行中值校准  500
     2. 右上按键按一下启动定高定点
     3. 左上按键长按远程关闭飞控
     4. 上下左右按键对摇杆进行微调
  */
    // 设置任务接收句柄
    if (xReceiverTask == NULL) {
        Int_Key_SetReceiverTask(xTaskGetCurrentTaskHandle());
    }

    // 等待按键通知（无超时）
    uint32_t ulNotifiedValue;
    xTaskNotifyWait(0, ULONG_MAX, &ulNotifiedValue, portMAX_DELAY);
    // 解码按键ID和事件类型
    KeyID id              = (KeyID)((ulNotifiedValue >> 3) & 0x07);
    KeyEventType keyevent = (KeyEventType)(ulNotifiedValue & 0x07);
    switch (keyevent) {
        case KEY_EVENT_DOWN: // 按键按下(瞬时事件)
            switch (id) {
                case KEY_ID_LEFT_X:
                    break;
                case KEY_ID_RIGHT_X:
                    break;
                case KEY_ID_LEFT:
                    break;
                case KEY_ID_RIGHT:
                    break;
                case KEY_ID_UP:
                    break;
                case KEY_ID_DOWN:
                    break;
                default:
                    break;
            }
            break;
        case KEY_EVENT_SHORT_RELEASE: // 短按释放事件 (<500ms)
            switch (id) {
                case KEY_ID_LEFT_X:
                    break;
                case KEY_ID_RIGHT_X:
                    joyStick.isFixHeightPoint = !joyStick.isFixHeightPoint; // 短按释放 右上按键 启动定高飞行
                    break;
                case KEY_ID_LEFT:
                    JoyStickBias.YAWBias -= 1; // 短按释放 左按键 加偏航角
                    break;
                case KEY_ID_RIGHT:
                    JoyStickBias.ROLBias -= 1; // 短按释放 右按键 加滚转角
                    break;
                case KEY_ID_UP:
                    JoyStickBias.PITBias -= 1; // 短按释放 上按键 加俯仰角
                    break;
                case KEY_ID_DOWN:
                    JoyStickBias.THRBias -= 1; // 短按释放 下按键 加油门
                    break;
                default:
                    break;
            }
            break;
        case KEY_EVENT_LONG_HOLD: // 长按持续事件 (周期性触发)
            switch (id) {
                case KEY_ID_LEFT_X:
                    joyStick.isPowerDonw = 1; // 持续长按 左上按键 远程关闭飞控
                    break;
                case KEY_ID_RIGHT_X:
                    break;
                case KEY_ID_LEFT:
                    JoyStickBias.YAWBias += 1; // 持续长按 左按键 减偏航角
                    break;
                case KEY_ID_RIGHT:
                    JoyStickBias.ROLBias += 1; // 持续长按 右按键 减滚转角
                    break;
                case KEY_ID_UP:
                    JoyStickBias.PITBias += 1; // 持续长按 上按键 减俯仰角
                    break;
                case KEY_ID_DOWN:
                    JoyStickBias.THRBias += 1; // 持续长按 下按键 减油门
                    break;
                default:
                    break;
            }
            break;
        case KEY_EVENT_LONG_RELEASE: // 长按释放事件 (≥500ms)
            switch (id) {
                case KEY_ID_LEFT_X:
                    joyStick.isPowerDonw = 0; // 长按释放 左上按键 恢复关闭飞控命令初始值
                    break;
                case KEY_ID_RIGHT_X:
                    App_DataProcess_JoyStickCalcBias(); // 长按释放 右上按键 进行校准
                    break;
                case KEY_ID_LEFT:
                    break;
                case KEY_ID_RIGHT:
                    break;
                case KEY_ID_UP:
                    break;
                case KEY_ID_DOWN:
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}
