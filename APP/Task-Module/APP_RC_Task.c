#include "APP_RC_Task.h"

// Power Task
#define Power_Task_NAME       "Power_Task"
#define Power_Task_STACK_SIZE 128
#define Power_Task_PRIORITY   4
#define Power_Task_CYCLE      pdMS_TO_TICKS(2000)
TaskHandle_t Power_Task_Handle;
void Power_Task(void *pvParameters);

// Debug Task
#define Debug_Task_NAME       "Debug_Task"
#define Debug_Task_STACK_SIZE 256
#define Debug_Task_PRIORITY   4
#define Debug_Task_CYCLE      pdMS_TO_TICKS(100)
TaskHandle_t Debug_Task_Handle;
void Debug_Task(void *pvParameters);

// 按键扫描任务
#define Key_Scan_Task_NAME       "Key_Scan_Task"
#define Key_Scan_Task_STACK_SIZE 128
#define Key_Scan_Task_PRIORITY   2
#define Key_Scan_Task_CYCLE      pdMS_TO_TICKS(10)
TaskHandle_t Key_Scan_Task_Handle;
void Key_Scan_Task(void *pvParameters);

// 摇杆扫描任务
#define JoyStick_Scan_Task_NAME       "JoyStick_Scan_Task"
#define JoyStick_Scan_Task_STACK_SIZE 128
#define JoyStick_Scan_Task_PRIORITY   2
#define JoyStick_Scan_Task_CYCLE      pdMS_TO_TICKS(10)
TaskHandle_t JoyStick_Scan_Task_Handle;
void JoyStick_Scan_Task(void *pvParameters);

// 按键数据处理任务
#define Key_Data_Process_Task_NAME       "Key_Data_Process_Task"
#define Key_Data_Process_Task_STACK_SIZE 128
#define Key_Data_Process_Task_PRIORITY   2
#define Key_Data_Process_Task_CYCLE      pdMS_TO_TICKS(10)
TaskHandle_t Key_Data_Process_Task_Handle;
void Key_Data_Process_Task(void *pvParameters);

// 摇杆数据处理任务
#define JoyStick_Data_Process_Task_NAME       "JoyStick_Data_Process_Task"
#define JoyStick_Data_Process_Task_STACK_SIZE 128
#define JoyStick_Data_Process_Task_PRIORITY   2
#define JoyStick_Data_Process_Task_CYCLE      pdMS_TO_TICKS(10)
TaskHandle_t JoyStick_Data_Process_Task_Handle;
void JoyStick_Data_Process_Task(void *pvParameters);

// 通讯任务
#define Communication_Task_NAME       "Communication_Task"
#define Communication_Task_STACK_SIZE 256
#define Communication_Task_PRIORITY   3
#define Communication_Task_CYCLE      pdMS_TO_TICKS(5)
TaskHandle_t Communication_Task_Handle;
void Communication_Task(void *pvParameters);

// Display任务
#define Display_Task_NAME       "Display_Task"
#define Display_Task_STACK_SIZE 128
#define Display_Task_PRIORITY   2
#define Display_Task_CYCLE      pdMS_TO_TICKS(10)
TaskHandle_t Display_Task_Handle;
void Display_Task(void *pvParameters);

void APP_Sart_ALL_Task()
{
    // printf(">>>>>>>>>> Remote Control Start <<<<<<<<<<\n");
    // Display 初始化
    App_Display_Start();
    // 2.4G模块 初始化
    App_Communication_Init();
    // 摇杆 初始化
    App_JoyStick_Init();
    // 创建 Display任务
    xTaskCreate(Display_Task, Display_Task_NAME, Display_Task_STACK_SIZE, NULL, Display_Task_PRIORITY, &Display_Task_Handle);
    // 创建 按键扫描任务
    xTaskCreate(Key_Scan_Task, Key_Scan_Task_NAME, Key_Scan_Task_STACK_SIZE, NULL, Key_Scan_Task_PRIORITY, &Key_Scan_Task_Handle);
    // 创建 按键数据处理任务
    xTaskCreate(Key_Data_Process_Task, Key_Data_Process_Task_NAME, Key_Data_Process_Task_STACK_SIZE, NULL, Key_Data_Process_Task_PRIORITY, &Key_Data_Process_Task_Handle);
    // 创建 摇杆扫描任务
    xTaskCreate(JoyStick_Scan_Task, JoyStick_Scan_Task_NAME, JoyStick_Scan_Task_STACK_SIZE, NULL, JoyStick_Scan_Task_PRIORITY, &JoyStick_Scan_Task_Handle);
    // 创建 摇杆数据处理任务
    xTaskCreate(JoyStick_Data_Process_Task, JoyStick_Data_Process_Task_NAME, JoyStick_Data_Process_Task_STACK_SIZE, NULL, JoyStick_Data_Process_Task_PRIORITY, &JoyStick_Data_Process_Task_Handle);
    // 创建 电源任务
    xTaskCreate(Power_Task, Power_Task_NAME, Power_Task_STACK_SIZE, NULL, Power_Task_PRIORITY, &Power_Task_Handle);
    // 创建 Debug任务
    // xTaskCreate(Debug_Task, Debug_Task_NAME, Debug_Task_STACK_SIZE, NULL, Debug_Task_PRIORITY, &Debug_Task_Handle);
    // 创建 2.4G通讯任务
    xTaskCreate(Communication_Task, Communication_Task_NAME, Communication_Task_STACK_SIZE, NULL, Communication_Task_PRIORITY, &Communication_Task_Handle);
    // 启动调度器
    vTaskStartScheduler();
}

// 电源控制任务
void Power_Task(void *pvParameters)
{
    debug_printfln("Power Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    vTaskDelay(15000);
    while (1) {
        APP_Power_Open();
        vTaskDelayUntil(&pxPreviousWakeTime, Power_Task_CYCLE);
    }
}

// Debug任务
void Debug_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("Debug Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        // >>>>>>>>>>>>>>>>>>>>>>>>>> 打印摇杆数据 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        // printf("PIT:%d, ROL:%d, THR:%d, YAW:%d, FHP:%d, PD:%d \n",
        //        JoyStick.PIT, JoyStick.ROL, JoyStick.THR, JoyStick.YAW, JoyStick.isFixHeightPoint, JoyStick.isPowerDonw);
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
        // printf("THR:%d, PIT:%d, ROL:%d,  YAW:%d, FHP:%d, PD:%d, Unlock:%d\n",
        //        rc_data.THR, rc_data.PIT, rc_data.ROL, rc_data.YAW, rc_data.isFixHeightPoint, rc_data.isPowerDonw, rc_data.isUnlockFlight);
        // 打印摇杆原始数据

        vTaskDelayUntil(&pxPreviousWakeTime, Debug_Task_CYCLE);
    }
}

// 按键扫描任务
void Key_Scan_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("Key Scan Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        APP_Key_Scan();
        vTaskDelayUntil(&pxPreviousWakeTime, Key_Scan_Task_CYCLE);
    }
}
// 摇杆扫描任务
void JoyStick_Scan_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("JoyStick Scan Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        APP_JoyStick_Scan();
        vTaskDelayUntil(&pxPreviousWakeTime, JoyStick_Scan_Task_CYCLE);
    }
}

// 按键数据处理任务
void Key_Data_Process_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("Key Data Process Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        App_DataProcess_KeyDataProcess();
        vTaskDelayUntil(&pxPreviousWakeTime, Key_Data_Process_Task_CYCLE);
    }
}

// 摇杆按键数据处理任务
void JoyStick_Data_Process_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("JoyStick Data Process Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        App_DataProcess_JoyStickDataProcess();
        vTaskDelayUntil(&pxPreviousWakeTime, JoyStick_Data_Process_Task_CYCLE);
    }
}

// 2.4G 通讯任务
void Communication_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("Communication Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        App_Communication_SendJoyStickData(&rc_data);
        vTaskDelayUntil(&pxPreviousWakeTime, Communication_Task_CYCLE);
    }
}

// Display 任务
void Display_Task(void *pvParameters)
{
    vTaskDelay(1000);
    debug_printfln("Display Task: Start!");
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    while (1) {
        App_Display_Show();
        vTaskDelayUntil(&pxPreviousWakeTime, Display_Task_CYCLE);
    }
}
