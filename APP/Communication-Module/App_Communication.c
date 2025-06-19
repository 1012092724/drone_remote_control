#include "App_Communication.h"

/**
 * @description: 启动通讯模块
 * @return {*}
 */
void App_Communication_Start(void)
{
    /* 1. 等待自检通过 */
    debug_printfln("2.4G Module Checking");
    HAL_Delay(1000);
    while (Inf_Si24R1_Check() == 1);
    debug_printfln("2.4G Module Check Pass");
    // /* 2. 把2.4G模块设置为接收模式 */
    // debug_printfln("2.4G Module Set RX Mode");
    // Inf_Si24R1_RXMode();
    /* 2. 把2.4G模块设置为发送模式 */
    debug_printfln("2.4G Module Set TX Mode");
    Inf_Si24R1_TXMode();
    debug_printfln("Communication Module Start Success!");
}

/**
 * @description: 发送摇杆数据
 * 协议: 4字节帧头+9字节有效载荷(摇杆+定高)+4字节校验和
 * @return {*}
 */
void App_Communication_SendJoyStickData(JoyStick_Struct *joyStick)
{
    /* 填充要发送的数据 */
    uint8_t index = 0;

    TX_BUFF[index++] = FRAME_0; /* 0-3 为帧头 */
    TX_BUFF[index++] = FRAME_1;
    TX_BUFF[index++] = FRAME_2;
    TX_BUFF[index++] = 0x00; /* 存储有效数据字节数 */
    TX_BUFF[index++] = (joyStick->THR >> 8);
    TX_BUFF[index++] = (joyStick->THR & 0xff);
    TX_BUFF[index++] = (joyStick->YAW >> 8);
    TX_BUFF[index++] = (joyStick->YAW & 0xff);
    TX_BUFF[index++] = (joyStick->ROL >> 8);
    TX_BUFF[index++] = (joyStick->ROL & 0xff);
    TX_BUFF[index++] = (joyStick->PIT >> 8);
    TX_BUFF[index++] = (joyStick->PIT & 0xff);

    TX_BUFF[index++] = joyStick->isFixHeightPoint; /* 定高定点命令0:不定高定点 1:定高定点 */
    TX_BUFF[index++] = joyStick->isPowerDonw;      /* 关机命令0:不关机 1:关机 */

    /* 关机命令只发一次即可. 发完之后,把关机命令重新清零 */
    // joyStick->isPowerDonw = 0;

    // joyStick->isFixHeightPoint = 0;

    TX_BUFF[3] = index - 4; /* 有效载荷数 */

    /* 存储 校验和, 接收收到数据可用用同样的方式进行校验*/
    uint32_t checkSum = 0;
    for (uint8_t i = 0; i < index; i++) {
        checkSum += TX_BUFF[i];
    }
    TX_BUFF[index++] = (checkSum >> 24);
    TX_BUFF[index++] = (checkSum >> 16);
    TX_BUFF[index++] = (checkSum >> 8);
    TX_BUFF[index++] = checkSum;

    uint8_t r = Inf_Si24R1_TxPacket(TX_BUFF);
    // DEBUG打印
    // if (r == 0) {
    //     debug_printfln("Send Success : %d", checkSum);
    // } else {
    //     debug_printfln("Send Fail!");
    // }
}
