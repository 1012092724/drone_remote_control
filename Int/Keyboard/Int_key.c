#include "Int_key.h"

// 接收任务句柄
TaskHandle_t xReceiverTask = NULL;

// 按键状态机状态定义（压缩为2bit）
typedef enum {
    KEY_STATE_RELEASED,
    KEY_STATE_DEBOUNCE,
    KEY_STATE_PRESSED,
    KEY_STATE_LONG_PRESS
} KeyState;

// 优化后的按键运行时结构体（节省4字节/按键）
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t state : 2; // 使用位域
    uint32_t press_time;
} KeyRuntime;

// 初始化所有按键状态
KeyRuntime keys[KEY_ID_COUNT] = {
    {KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin, KEY_STATE_RELEASED, 0},
    {KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin, KEY_STATE_RELEASED, 0},
    {KEY_UP_GPIO_Port, KEY_UP_Pin, KEY_STATE_RELEASED, 0},
    {KEY_DOWN_GPIO_Port, KEY_DOWN_Pin, KEY_STATE_RELEASED, 0},
    {KEY_LEFT_GPIO_Port, KEY_LEFT_Pin, KEY_STATE_RELEASED, 0},
    {KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin, KEY_STATE_RELEASED, 0}};

void Int_Key_SetReceiverTask(TaskHandle_t xTaskHandle)
{
    xReceiverTask = xTaskHandle;
}

// 发送事件通过任务通知
static void sendKeyEvent(KeyID id, KeyEventType event_type)
{
    if (xReceiverTask != NULL) {
        // 将按键ID和事件类型编码到通知值中
        uint32_t notify_value = (id << 3) | (event_type & 0x07);
        xTaskNotifyFromISR(xReceiverTask,
                           notify_value,
                           eSetValueWithOverwrite,
                           NULL);
    } else {
        debug_printfln("xReceiverTask is NULL");
    }
}
// 优化后的状态处理函数
static void processKeyState(KeyRuntime *key, KeyID id, GPIO_PinState pin_state)
{
    TickType_t now          = xTaskGetTickCount();
    uint32_t press_duration = now - key->press_time;

    switch (key->state) {
        case KEY_STATE_RELEASED:
            if (pin_state == GPIO_PIN_RESET) {
                key->state      = KEY_STATE_DEBOUNCE;
                key->press_time = now;
            }
            break;

        case KEY_STATE_DEBOUNCE:
            if (press_duration > pdMS_TO_TICKS(20)) {
                key->state = (pin_state == GPIO_PIN_RESET) ? KEY_STATE_PRESSED : KEY_STATE_RELEASED;
                if (key->state == KEY_STATE_PRESSED)
                    sendKeyEvent(id, KEY_EVENT_DOWN);
            }
            break;

        case KEY_STATE_PRESSED:
            if (pin_state == GPIO_PIN_SET) {
                key->state = KEY_STATE_RELEASED;
                sendKeyEvent(id, (press_duration < pdMS_TO_TICKS(500)) ? KEY_EVENT_SHORT_RELEASE : KEY_EVENT_LONG_RELEASE);
            } else if (press_duration > pdMS_TO_TICKS(500)) {
                key->state = KEY_STATE_LONG_PRESS;
                sendKeyEvent(id, KEY_EVENT_LONG_HOLD);
            }
            break;

        case KEY_STATE_LONG_PRESS:
            if (pin_state == GPIO_PIN_SET) {
                key->state = KEY_STATE_RELEASED;
                sendKeyEvent(id, KEY_EVENT_LONG_RELEASE);
            } else if (press_duration > pdMS_TO_TICKS(200)) {
                key->press_time = now;
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