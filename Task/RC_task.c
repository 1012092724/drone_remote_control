#include "RC_task.h"

// Power Task
#define Power_Task_NAME       "Power_Task"
#define Power_Task_STACK_SIZE 64
#define Power_Task_PRIORITY   4
TaskHandle_t Power_Task_Handle;
void Power_Task(void *pvParameters);

// Debug Task
#define Debug_Task_NAME       "Debug_Task"
#define Debug_Task_STACK_SIZE 64
#define Debug_Task_PRIORITY   2
TaskHandle_t Debug_Task_Handle;
void Debug_Task(void *pvParameters);

// 按键扫描任务
#define Key_Scan_Task_NAME       "Key_Scan_Task"
#define Key_Scan_Task_STACK_SIZE 64
#define Key_Scan_Task_PRIORITY   4
TaskHandle_t Key_Scan_Task_Handle;
void Key_Scan_Task(void *pvParameters);

// 按键数据处理任务
#define Key_Data_Process_Task_NAME       "Key_Data_Process_Task"
#define Key_Data_Process_Task_STACK_SIZE 128
#define Key_Data_Process_Task_PRIORITY   4
TaskHandle_t Key_Data_Process_Task_Handle;
void Key_Data_Process_Task(void *pvParameters);

// 摇杆数据处理任务
#define JoyStick_Data_Process_Task_NAME       "JoyStick_Data_Process_Task"
#define JoyStick_Data_Process_Task_STACK_SIZE 64
#define JoyStick_Data_Process_Task_PRIORITY   2
TaskHandle_t JoyStick_Data_Process_Task_Handle;
void JoyStick_Data_Process_Task(void *pvParameters);

void Sart_ALL_Task()
{
    printf(">>>>>>>>>> Remote Control Start <<<<<<<<<<\n");
    // 摇杆初始化
    App_DataProcess_Start();
    // 按键初始化
    Int_Key_Init();
    // 创建电源任务
    xTaskCreate(Power_Task, Power_Task_NAME, Power_Task_STACK_SIZE, NULL, Power_Task_PRIORITY, &Power_Task_Handle);
    // 创建Debug任务
    xTaskCreate(Debug_Task, Debug_Task_NAME, Debug_Task_STACK_SIZE, NULL, Debug_Task_PRIORITY, &Debug_Task_Handle);
    // 创建按键扫描任务
    xTaskCreate(Key_Scan_Task, Key_Scan_Task_NAME, Key_Scan_Task_STACK_SIZE, NULL, Key_Scan_Task_PRIORITY, &Key_Scan_Task_Handle);
    // 创建摇杆数据处理任务
    xTaskCreate(JoyStick_Data_Process_Task, JoyStick_Data_Process_Task_NAME, JoyStick_Data_Process_Task_STACK_SIZE, NULL, JoyStick_Data_Process_Task_PRIORITY, &JoyStick_Data_Process_Task_Handle);
    // 创建按键数据处理任务
    xTaskCreate(Key_Data_Process_Task, Key_Data_Process_Task_NAME, Key_Data_Process_Task_STACK_SIZE, NULL, Key_Data_Process_Task_PRIORITY, &Key_Data_Process_Task_Handle);
    vTaskStartScheduler();
}

// 电源控制任务
void Power_Task(void *pvParameters)
{
    debug_printfln("Power Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    const TickType_t xInterval    = pdMS_TO_TICKS(20000);
    vTaskDelay(1500);
    while (1) {
        Int_IP5305T_Open();
        vTaskDelayUntil(&pxPreviousWakeTime, xInterval);
    }
}

// Debug任务
void Debug_Task(void *pvParameters)
{
    debug_printfln("Debug Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    const TickType_t xInterval    = pdMS_TO_TICKS(100);
    KeyEvent keyevent;
    while (1) {
        // >>>>>>>>>>>>>>>>>>>>>>>>>> 打印摇杆数据 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        // printf("PIT:%d, ROL:%d, THR:%d, YAW:%d, FHP:%d, PD:%d \n",
        //        joyStick.PIT, joyStick.ROL, joyStick.THR, joyStick.YAW, joyStick.isFixHeightPoint, joyStick.isPowerDonw);
        // >>>>>>>>>>>>>>>>>>>>>>>>>> 打印按键数据 < < < < < < < < < < < < < < < < < < < < < < < < < < < < < < < < < < <
        // if (xQueueReceive(xKeyEventQueue, &keyevent, portMAX_DELAY) == pdPASS) {
        //     switch (keyevent.event) {
        //         case KEY_EVENT_DOWN:
        //             printf("Key %d pressed\n", keyevent.key_id);
        //             break;
        //         case KEY_EVENT_SHORT_RELEASE:
        //             printf("Key %d short released\n", keyevent.key_id);
        //             break;
        //         case KEY_EVENT_LONG_RELEASE:
        //             printf("Key %d long released\n", keyevent.key_id);
        //             break;
        //         case KEY_EVENT_LONG_HOLD:
        //             printf("Key %d long holding\n", keyevent.key_id);
        //             break;
        //     }
        // }
        printf("PIT:%d, ROL:%d, THR:%d, YAW:%d, FHP:%d, PD:%d \n",
               joyStick.PIT, joyStick.ROL, joyStick.THR, joyStick.YAW, joyStick.isFixHeightPoint, joyStick.isPowerDonw);
        vTaskDelayUntil(&pxPreviousWakeTime, xInterval);
    }
}

// 按键扫描任务
void Key_Scan_Task(void *pvParameters)
{
    debug_printfln("Key Scan Task: Start!");
    vTaskDelay(1000);
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    const TickType_t xInterval    = pdMS_TO_TICKS(10);
    while (1) {
        Int_Key_Scan();
        vTaskDelayUntil(&pxPreviousWakeTime, xInterval);
    }
}

// 按键数据处理任务
void Key_Data_Process_Task(void *pvParameters)
{
    debug_printfln("Key Data Process Task: Start!");
    vTaskDelay(1000);
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    const TickType_t xInterval    = pdMS_TO_TICKS(10);
    while (1) {
        App_DataProcess_KeyDataProcess();
        vTaskDelayUntil(&pxPreviousWakeTime, xInterval);
    }
}

// 摇杆按键数据处理任务
void JoyStick_Data_Process_Task(void *pvParameters)
{
    debug_printfln("JoyStick Data Process Task: Start!");
    vTaskDelay(1000);
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    const TickType_t xInterval    = pdMS_TO_TICKS(10);
    while (1) {
        App_DataProcess_JoyStickDataProcess();
        vTaskDelayUntil(&pxPreviousWakeTime, xInterval);
    }
}
