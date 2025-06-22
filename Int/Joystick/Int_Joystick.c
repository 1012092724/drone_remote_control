#include "Int_Joystick.h"

/* 存储摇杆数据:
    0: left杆->   左右 -> 偏航
    1: left杆->   前后 -> 油门
    2: right杆->  左右 -> 横滚
    3: right杆->  前后 -> 俯仰
 */

JoyStick_Struct joystick; // 原始摇杆数据

uint16_t joyStickBuff[4];

/**
 * @description: 摇杆和按键初始化
 * @return {*}
 */
void Int_JoyStick_Init(void)
{
    /* 启动ADC采集, 因为有4个通道, 所以要开启DMA */
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)joyStickBuff, 4);
    debug_printfln("JoyStick Init Success!");
}
/**
 * @description: 扫描遥感数据
 * @return {*}
 */
void Int_JoyStick_Scan(void)
{
    joystick.THR = joyStickBuff[0];
    joystick.YAW = joyStickBuff[1];
    joystick.PIT = joyStickBuff[2];
    joystick.ROL = joyStickBuff[3]; // 4074
    // 打印遥感数据
    // debug_printfln("JoyStick: THR:%d, YAW:%d, PIT:%d, ROL:%d", joystick.THR, joystick.YAW, joystick.PIT, joystick.ROL);
}
