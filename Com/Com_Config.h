#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include "stdint.h"
#include "main.h"

/* 0. 定义通讯的帧头 */
#define FRAME_0 0x20
#define FRAME_1 0x02
#define FRAME_2 0x26

/* 1. 通用状态枚举类型 */
typedef enum {
    Com_OK = 0,
    Com_ERROR,
    Com_OTHER
} Com_Status;

/* 遥控器状态 */
typedef enum {
    RC_UNCONNECTED,
    RC_CONNECTED
} RC_Status;

/* 摇杆状态 */
typedef enum {
    Joy_LOCK,
    Joy_UNLOCK
} Joy_Status;

/* 2. 遥感和按键数据封装结构体  */
typedef struct
{
    uint16_t THR; /* 油门 Throttle */
    uint16_t YAW; /* 偏航 */
    uint16_t ROL; /* 横滚 */
    uint16_t PIT; /* 俯仰 */

    uint8_t isFixHeightPoint; /* 是否定高定点 */
    uint8_t isPowerDonw;      /* 是否关机 */
    uint8_t isUnlockFlight;   /* 是否解锁飞机*/
} RC_DATA;

typedef struct
{
    int16_t THRBias; /* 油门 偏差 */
    int16_t YAWBias; /* 偏航 偏差*/
    int16_t ROLBias; /* 横滚 偏差*/
    int16_t PITBias; /* 俯仰 偏差*/
} JoyStickBias_Struct;

extern RC_DATA rc_data;       // 遥控数据
extern RC_Status rc_status;   // 遥控连接状态
extern Joy_Status joy_status; // 摇杆状态

#endif /* __COM_CONFIG_H__ */