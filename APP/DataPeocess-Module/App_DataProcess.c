#include "App_DataProcess.h"

JoyStickBias_Struct JoyStickBias;
RC_DATA rc_data;
Joy_Status joy_status = Joy_LOCK; // 摇杆状态

/* 均值滤波(滑动窗口) */
#define MEAN_FILTER(filter, valuePointer)             \
    filter.sum -= filter.values[filter.index];        \
    filter.values[filter.index] = *valuePointer;      \
    filter.sum += filter.values[filter.index];        \
    filter.index  = (filter.index + 1) % FILTER_SIZE; \
    *valuePointer = filter.sum / FILTER_SIZE

/* 初始化滤波缓冲区 */
static FilterBuffer_Struct THRFilter = {{0}, 0, 0};
static FilterBuffer_Struct YAWFilter = {{0}, 0, 0};
static FilterBuffer_Struct PITFilter = {{0}, 0, 0};
static FilterBuffer_Struct ROLFilter = {{0}, 0, 0};

// /**
//  * @description: 处理摇杆数据
//  * @return {*}
//  */
void App_DataProcess_JoyStickDataProcess(void)
{

    // taskENTER_CRITICAL();

    /* 对摇杆数据进行极性处理 */
    uint16_t THR = 1000 - joystick.THR * 1000 / 4095;
    uint16_t YAW = 1000 - joystick.YAW * 1000 / 4095;
    uint16_t PIT = 1000 - joystick.PIT * 1000 / 4095;
    uint16_t ROL = 1000 - joystick.ROL * 1000 / 4095;

    /* 摇杆偏差处理 */
    THR = LIMIT(THR - JoyStickBias.THRBias, 0, 1000);
    YAW = LIMIT(YAW - JoyStickBias.YAWBias, 0, 1000);
    PIT = LIMIT(PIT - JoyStickBias.PITBias, 0, 1000);
    ROL = LIMIT(ROL - JoyStickBias.ROLBias, 0, 1000);

    /* 对摇杆数据进行滤波 */
    MEAN_FILTER(THRFilter, &THR);
    MEAN_FILTER(YAWFilter, &YAW);
    MEAN_FILTER(PITFilter, &PIT);
    MEAN_FILTER(ROLFilter, &ROL);

    /* 更新处理后摇杆数据到RC_DATA结构体中 */
    rc_data.THR = THR;
    rc_data.YAW = YAW;
    rc_data.PIT = PIT;
    rc_data.ROL = ROL;

    // taskEXIT_CRITICAL();
}

/**
 * @description: 对摇杆进行偏移量校准.  读取100次数据, 求出平均偏移量
 *  当长按右上按键的时候,调用次函数进行校准
 * @return {*}
 */
static void App_DataProcess_JoyStickCalcBias()
{
    JoyStickBias.THRBias = 0;
    JoyStickBias.YAWBias = 0;
    JoyStickBias.PITBias = 0;
    JoyStickBias.ROLBias = 0;
    /* 连续读取100次, 求取平均偏移量 */
    for (uint8_t i = 0; i < 100; i++) {
        Int_JoyStick_Scan(); /* 扫描摇杆数据 */
        /* 对摇杆数据进行极性处理 */
        /* 摇杆的极性, 与我们的感知正好相反, 先修改极性 */

        int16_t THR = 1000 - joystick.THR * 1000 / 4095;
        int16_t YAW = 1000 - joystick.YAW * 1000 / 4095;
        int16_t PIT = 1000 - joystick.PIT * 1000 / 4095;
        int16_t ROL = 1000 - joystick.ROL * 1000 / 4095;

        JoyStickBias.THRBias += THR - 0;
        JoyStickBias.YAWBias += YAW - 500;
        JoyStickBias.PITBias += PIT - 500;
        JoyStickBias.ROLBias += ROL - 500;
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
            }
            break;
        case KEY_EVENT_SHORT_RELEASE: // 短按释放事件 (<500ms)
            switch (id) {
                case KEY_ID_LEFT_X:
                    break;
                case KEY_ID_RIGHT_X:
                    rc_data.isFixHeightPoint = !rc_data.isFixHeightPoint; // 短按释放 右上按键 启动定高飞行
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
                    break;
            }
            break;
        case KEY_EVENT_LONG_HOLD: // 长按持续事件 (周期性触发)
            switch (id) {
                case KEY_ID_LEFT_X:
                    App_DataProcess_JoyStickCalcBias();
                    // 如果 该按键被按下 并且 rc_status == RC_CONNECTED 并且 joy_status == Joy_LOCK 持续 1.5s 则 joy_status == Joy_UNLOCK
                    static uint32_t hold_time = 0;
                    if (rc_status == RC_CONNECTED && joy_status == Joy_LOCK && joystick.THR >= 4074) {
                        if (hold_time == 0) {
                            hold_time = xTaskGetTickCount();
                        } else if (xTaskGetTickCount() - hold_time > 1500) {
                            joy_status = Joy_UNLOCK;
                        }
                    } else {
                        hold_time = 0; // 重置计时器
                    }
                    rc_data.isUnlockFlight = 1; // 持续长按 左上按键 远程解锁飞机
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
                    rc_data.isPowerDonw = 1; // 持续长按 下按键 远程关闭飞控
                    break;
            }
            break;
        case KEY_EVENT_LONG_RELEASE: // 长按释放事件 (≥500ms)
            switch (id) {
                case KEY_ID_LEFT_X:
                    rc_data.isUnlockFlight = 0; // 长按释放 左上按键 锁定飞机
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
                    rc_data.isPowerDonw = 0;
                    break;
            }
            break;
    }
}
