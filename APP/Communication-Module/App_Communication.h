#ifndef __APP_COMMUNICATION_H__
#define __APP_COMMUNICATION_H__

#include "Com_Debug.h"
#include "Int_Si24R1.h"
#include "Com_Config.h"

void App_Communication_Init(void);
void App_Communication_SendJoyStickData(JoyStick_Struct *joyStick);
;

#endif /* __APP_COMMUNICATION_H__ */