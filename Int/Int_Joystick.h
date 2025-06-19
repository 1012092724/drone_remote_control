#ifndef __INT_JOYSTICK_H__
#define __INT_JOYSTICK_H__

#include "main.h"
#include "Com_Config.h"
#include "adc.h"

extern JoyStick_Struct joyStick;

void Int_JoyStick_Init(void);
void Int_JoyStick_Scan(void);

#endif /* __INT_JOYSTICK_H__ */