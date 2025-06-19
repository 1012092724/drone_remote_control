#include "Int_key.h"

// 创建事件队列（全局）
QueueHandle_t xKeyEventQueue = NULL;

// 按键状态机状态定义
typedef enum {
    KEY_STATE_RELEASED,  // 释放状态
    KEY_STATE_DEBOUNCE,  // 消抖中（20ms）
    KEY_STATE_PRESSED,   // 稳定按下
    KEY_STATE_LONG_PRESS // 长按触发
} KeyState;

// 单个按键的运行时状态
typedef struct {
    GPIO_TypeDef *port;    // 按键GPIO端口（如 KEY_LEFT_X_GPIO_Port）
    uint16_t pin;          // 按键GPIO引脚（如 KEY_LEFT_X_Pin）
    KeyState state;        // 当前状态
    TickType_t press_time; // 按下时刻时间戳
} KeyRuntime;

// 初始化所有按键状态
static KeyRuntime keys[KEY_ID_COUNT] = {
    {KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin, KEY_STATE_RELEASED, 0},
    {KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin, KEY_STATE_RELEASED, 0},
    {KEY_UP_GPIO_Port, KEY_UP_Pin, KEY_STATE_RELEASED, 0},
    {KEY_DOWN_GPIO_Port, KEY_DOWN_Pin, KEY_STATE_RELEASED, 0},
    {KEY_LEFT_GPIO_Port, KEY_LEFT_Pin, KEY_STATE_RELEASED, 0},
    {KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin, KEY_STATE_RELEASED, 0}};

void Int_Key_Init(void)
{
    xKeyEventQueue = xQueueCreate(20, sizeof(KeyEvent)); // 队列深度20
}

// 发送事件到队列（非阻塞）
void sendKeyEvent(KeyID id, KeyEventType event_type)
{
    KeyEvent event = {id, event_type};
    xQueueSend(xKeyEventQueue, &event, 0); // 立即发送，不阻塞
}

// 处理按键状态
void processKeyState(KeyRuntime *key, KeyID id, GPIO_PinState pin_state)
{
    TickType_t now = xTaskGetTickCount();

    switch (key->state) {
        case KEY_STATE_RELEASED:
            if (pin_state == GPIO_PIN_RESET) { // 检测到按下
                key->state      = KEY_STATE_DEBOUNCE;
                key->press_time = now;
            }
            break;

        case KEY_STATE_DEBOUNCE:
            if (now - key->press_time > pdMS_TO_TICKS(20)) {
                if (pin_state == GPIO_PIN_RESET) { // 确认有效按下
                    key->state = KEY_STATE_PRESSED;
                    sendKeyEvent(id, KEY_EVENT_DOWN);
                } else {
                    key->state = KEY_STATE_RELEASED; // 抖动忽略
                }
            }
            break;

        case KEY_STATE_PRESSED:
            if (pin_state == GPIO_PIN_SET) { // 释放
                key->state = KEY_STATE_RELEASED;
                if (now - key->press_time < pdMS_TO_TICKS(500)) {
                    sendKeyEvent(id, KEY_EVENT_SHORT_RELEASE);
                }
            } else if (now - key->press_time > pdMS_TO_TICKS(500)) {
                key->state = KEY_STATE_LONG_PRESS;
                sendKeyEvent(id, KEY_EVENT_LONG_HOLD);
            }
            break;

        case KEY_STATE_LONG_PRESS:
            if (pin_state == GPIO_PIN_SET) { // 长按释放
                key->state = KEY_STATE_RELEASED;
                sendKeyEvent(id, KEY_EVENT_LONG_RELEASE);
            } else if (now - key->press_time > pdMS_TO_TICKS(200)) {
                key->press_time = now; // 重置时间戳以支持连发
                sendKeyEvent(id, KEY_EVENT_LONG_HOLD);
            }
            break;
    }
}

// 按键扫描
void Int_Key_Scan(void)
{
    for (int i = 0; i < KEY_ID_COUNT; i++) {
        GPIO_PinState state = HAL_GPIO_ReadPin(keys[i].port, keys[i].pin);
        processKeyState(&keys[i], (KeyID)i, state);
    }
}