#ifndef __INT_KEY_H__
#define __INT_KEY_H__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"

// 按键ID枚举
typedef enum {
    KEY_ID_LEFT_X,
    KEY_ID_RIGHT_X,
    KEY_ID_UP,
    KEY_ID_DOWN,
    KEY_ID_LEFT,
    KEY_ID_RIGHT,
    KEY_ID_COUNT
} KeyID;

// 按键事件类型（压缩为3bit）
typedef enum {
    KEY_EVENT_NONE          = 0,
    KEY_EVENT_DOWN          = 1,
    KEY_EVENT_SHORT_RELEASE = 2,
    KEY_EVENT_LONG_RELEASE  = 3,
    KEY_EVENT_LONG_HOLD     = 4
} KeyEventType;

extern TaskHandle_t xReceiverTask;

// 设置接收任务句柄
void Int_Key_SetReceiverTask(TaskHandle_t xTaskHandle);

// 按键扫描
void Int_Key_Scan(void);

#endif /* __INT_KEY_H__ */