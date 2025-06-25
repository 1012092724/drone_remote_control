#include "APP_Display.h"

void App_Display_ShowProgressBar(uint8_t temp,
                                 uint8_t chr_star,
                                 uint8_t chr_default,
                                 uint8_t x,
                                 uint8_t y,
                                 uint8_t size,
                                 uint8_t mode);
void App_Display_Start(void)
{
    OLED_Init();
}

// 宏定义用于提升可读性和可维护性
#define REFRESH_INTERVAL_MS  pdMS_TO_TICKS(1000)
#define RC_DATA_SCALE_FACTOR 41
#define RC_DATA_OFFSET       500

void App_Display_Show(void)
{
    static TickType_t lastTick      = 0;
    static uint8_t isBlinkOn        = 0;
    static RC_Status lastRcStatus   = RC_UNCONNECTED; // 记录上一次状态
    static Joy_Status lastJoyStatus = Joy_LOCK;       // 记录上一次摇杆状态
    uint8_t temp;

    TickType_t currentTick = xTaskGetTickCount();

    // 如果状态发生变化，则清屏并更新状态
    if (rc_status != lastRcStatus) {
        OLED_Clear();
        lastRcStatus = rc_status;
        isBlinkOn    = 0;
        lastTick     = 0;
    }

    if (rc_status == RC_UNCONNECTED) {
        if ((currentTick - lastTick) >= REFRESH_INTERVAL_MS) {
            lastTick = currentTick;
            if (isBlinkOn) {
                OLED_Clear(); // 清屏
                isBlinkOn = 0;
            } else {
                OLED_ShowString(8, 30, "2.4G No Signal", 12, 1);
                isBlinkOn = 1;
            }
        }
    } else if (rc_status == RC_CONNECTED) {

        // 如果状态发生变化，则清屏并更新状态
        if (joy_status != lastJoyStatus) {
            OLED_Clear();
            lastJoyStatus = joy_status;
            lastTick      = 0;
        }
        if (joy_status == Joy_LOCK) {
            if ((currentTick - lastTick) >= REFRESH_INTERVAL_MS) {
                lastTick = currentTick;
                if (isBlinkOn) {
                    OLED_Clear(); // 清屏
                    isBlinkOn = 0;
                } else {
                    OLED_ShowString(5, 30, "JoyStick Locked", 12, 1);
                    isBlinkOn = 1;
                }
            }
        } else if (joy_status == Joy_UNLOCK) {

            // 显示遥控数据
            OLED_ShowString(Line3_Begin + 00, Y1, "THR:", 12, 1);
            temp = rc_data.THR / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 24, Line4_Begin + 6, Y1, 12, 1);
            temp = (rc_data.THR - RC_DATA_OFFSET) / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 12, Line4_Begin + 18, Y1, 12, 1);

            OLED_ShowString(Line3_Begin + 64, Y1, "ROL:", 12, 1);
            temp = rc_data.ROL / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 24, Line4_Begin + 70, Y1, 12, 1);
            temp = (rc_data.ROL - RC_DATA_OFFSET) / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 12, Line4_Begin + 82, Y1, 12, 1);

            OLED_ShowString(Line3_Begin + 00, Y3, "YAW:", 12, 1);
            temp = rc_data.YAW / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 24, Line4_Begin + 6, Y3, 12, 1);
            temp = (rc_data.YAW - RC_DATA_OFFSET) / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 12, Line4_Begin + 18, Y3, 12, 1);

            OLED_ShowString(Line3_Begin + 64, Y3, "PIT:", 12, 1);
            temp = rc_data.PIT / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 24, Line4_Begin + 70, Y3, 12, 1);
            temp = (rc_data.PIT - RC_DATA_OFFSET) / RC_DATA_SCALE_FACTOR;
            App_Display_ShowProgressBar(temp, 12, 12, Line4_Begin + 82, Y3, 12, 1);
        }
    }

    OLED_Refresh_Gram(); // 统一刷新屏幕
}

// 进度条显示函数
void App_Display_ShowProgressBar(uint8_t temp,
                                 uint8_t chr_star,
                                 uint8_t chr_default,
                                 uint8_t x,
                                 uint8_t y,
                                 uint8_t size,
                                 uint8_t mode)
{
    switch (temp) {
        case 0:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 1:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 2:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 3:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 4:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 5:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 6:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 7:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 8:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 9:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 10:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 11:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;
        case 12:
            OLED_Show_CH(x, y, chr_star + temp, size, size);
            break;

        default:
            OLED_Show_CH(x, y, chr_default, size, size);
            break;
    }
}
