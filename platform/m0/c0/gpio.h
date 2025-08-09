#ifndef __GPIO_H
#define __GPIO_H

#include "main.h"
#include "platform.h"

void GPIO_USART_Debug_Tx_Init(void);
void GPIO_USART_Debug_Rx_Init(void);
void GPIO_Motor_AH_Init(void);
void GPIO_Motor_BH_Init(void);
void GPIO_Motor_CH_Init(void);
void GPIO_Motor_AL_Init(void);
void GPIO_Motor_BL_Init(void);
void GPIO_Motor_CL_Init(void);
void GPIO_Motor_Hall1_Init(void);
void GPIO_Motor_Hall2_Init(void);
void GPIO_Motor_Hall3_Init(void);
void GPIO_Motor_Hall_Init(Pl_Motor_ClbkHall_t pHallClbk);
// void GPIO_Motor_Throttle_Init(void);

u32 GPIO_Motor_GetStateHalls(void);
void GPIO_Motor_SetKeyAL(GPIO_ACTION_t action);
void GPIO_Motor_SetKeyBL(GPIO_ACTION_t action);
void GPIO_Motor_SetKeyCL(GPIO_ACTION_t action);
void GPIO_Motor_HallIrqEnable(void);
void GPIO_Hall1_CallIrq(void);

void GPIO_SensThrottle_Init(void);
void GPIO_SensBrakeLeft_Init(void);
// void GPIO_SensBrakeRight_Init(void);
// void GPIO_SensTempMotor_Init(void);
// void GPIO_SensTempMOSFET_Init(void);
void GPIO_ADCLowFreq_Init(void);

void GPIO_SensCurA_Init(void);
void GPIO_SensCurB_Init(void);
void GPIO_SensCurC_Init(void);
void GPIO_ADCHighFreq_Init(void);

void GPIO_Led_Init(void);
void GPIO_Led_Set(void);
void GPIO_Led_Reset(void);
void GPIO_Led_Toggle(void);

#endif /* __GPIO_H */