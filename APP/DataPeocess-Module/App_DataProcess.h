#ifndef __APP_DATAPROCESS_H__
#define __APP_DATAPROCESS_H__

#include "Com_Debug.h"
#include "Com_Config.h"
#include "Int_Joystick.h"
#include "Int_key.h"

/* 对x限幅度处理 */
#define LIMIT(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/* 均值滤波缓冲区大小 */
#define FILTER_SIZE 10
/* 滤波缓冲区结构体 */
typedef struct
{
    int32_t values[10]; /* 存储最近的10个值 */
    int32_t index;      /* 索引 */
    int32_t sum;        /* 存储10个值的和 */
} FilterBuffer_Struct;

void App_DataProcess_Init(void);

void App_DataProcess_JoyStickDataProcess(void);

void App_DataProcess_KeyDataProcess(void);

#endif /* __APP_DATAPROCESS_H__ */