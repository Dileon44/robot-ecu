#ifndef __IWDG_H
#define __IWDG_H

#include "main.h"

void IWDG_Init(void);
void IWDG_ResetCnt(void);
void IWDG_TaskCreate(void);
void IWDG_TaskDelete(void);
static void vTask_WatchDogProcess(void* pvParameters);
void FreeRTOS_IWDG_InitComponents(void);

#endif /* __IWDG_H */
