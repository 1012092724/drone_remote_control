#ifndef __INT_KEY_H__
#define __INT_KEY_H__

#include "main.h"
#include "queue.h"
extern QueueHandle_t xKeyEventQueue;

// 按键ID枚举
typedef enum {
    KEY_ID_LEFT_X,
    KEY_ID_RIGHT_X,
    KEY_ID_UP,
    KEY_ID_DOWN,
    KEY_ID_LEFT,
    KEY_ID_RIGHT,
    KEY_ID_COUNT // 按键总数
} KeyID;

// 按键事件类型
typedef enum {
    KEY_EVENT_NONE,          // 无事件
    KEY_EVENT_DOWN,          // 按键按下（瞬时事件）
    KEY_EVENT_SHORT_RELEASE, // 短按释放（<500ms）
    KEY_EVENT_LONG_RELEASE,  // 长按释放（≥500ms）
    KEY_EVENT_LONG_HOLD      // 长按持续（周期性触发）
} KeyEventType;

// 按键事件结构体（供其他任务读取）
typedef struct {
    KeyID key_id;       // 哪个按键（如 KEY_ID_LEFT_X）
    KeyEventType event; // 事件类型（如 KEY_EVENT_DOWN）
} KeyEvent;

// 按键初始化
void Int_Key_Init(void);
// 按键扫描
void Int_Key_Scan(void);

#endif /* __INT_KEY_H__ */