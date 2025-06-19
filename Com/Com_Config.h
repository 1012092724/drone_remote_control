#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include "stdint.h"
#include "main.h"

/* 0. 定义通讯的帧头 */
#define FRAME_0 0xAA
#define FRAME_1 0xAF
#define FRAME_2 0x03

/* 1. 通用状态枚举类型 */
typedef enum {
    Common_OK = 0,
    Common_ERROR,
    Common_OTHER
} Common_Status;

/* 2. 遥感和按键数据封装结构体  */
typedef struct
{
    uint16_t THR; /* 油门 Throttle */
    uint16_t YAW; /* 偏航 */
    uint16_t ROL; /* 横滚 */
    uint16_t PIT; /* 俯仰 */

    uint8_t isFixHeightPoint; /* 是否定高定点 */
    uint8_t isPowerDonw;      /* 是否关机 */
} JoyStick_Struct;

typedef struct
{
    int16_t THRBias; /* 油门 偏差 */
    int16_t YAWBias; /* 偏航 偏差*/
    int16_t ROLBias; /* 横滚 偏差*/
    int16_t PITBias; /* 俯仰 偏差*/
} JoyStickBias_Struct;

#endif /* __COM_CONFIG_H__ */