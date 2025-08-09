#ifndef __DEBUG_PROCESS_H
#define __DEBUG_PROCESS_H

#include "main.h"

static void vTask_DebugProcess(void* pvParameters);
void DebugProcess_TaskCreate(void);
void FreeRTOS_DebugProcess_InitComponents();

#endif /* __DEBUG_PROCESS_H */