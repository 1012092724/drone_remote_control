#ifndef __INT_JOYSTICK_H__
#define __INT_JOYSTICK_H__

#include "main.h"
#include "Com_Config.h"
#include "adc.h"

typedef struct {
    uint16_t THR; /* 油门 Throttle */
    uint16_t YAW; /* 偏航 */
    uint16_t ROL; /* 横滚 */
    uint16_t PIT; /* 俯仰 */
} JoyStick_Struct;

extern JoyStick_Struct joystick; // 原始摇杆数据

void Int_JoyStick_Init(void);

void Int_JoyStick_Scan(void);

#endif /* __INT_JOYSTICK_H__ */