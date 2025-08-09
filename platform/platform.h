#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "main.h"
#include "def_settings.h"
// #include "platform_inc_m0.h"

#ifndef PL_NOP
#define PL_NOP()			__NOP()
#endif /* PL_NOP */

#ifndef PL_SET_BKPT
#define PL_SET_BKPT(v)		__BKPT(v)
#endif /* PL_SET_BKPT */

#define PL_USART_DEF_TIMEOUT		50

typedef enum { GPIO_RST, GPIO_SET, GPIO_REV } GPIO_ACTION_t;

typedef void (*Pl_Common_Clbk_t)(void);
typedef void (*Pl_HardFault_Clbk_t)(u32 pcVal);
typedef void (*Pl_USART_ClbkTx_t)(void);
typedef void (*Pl_USART_ClbkRx_t)(void);
typedef void (*Pl_TIM_PWM_Clbk_t)(void);
typedef void (*Pl_Motor_ClbkHall_t)(u32 idHall);
typedef void (*Pl_Sensors_ADCLowFreqClbk_t)(u16* buffPtr, u16 buffLen);
typedef void (*Pl_ADC_Motor_SensHighFreqClbk_t)(u16* buffPtr, u16 buffLen);
typedef void (*Pl_TIM_Motor_SpeedControlClbk_t)(void);
typedef void (*Pl_Motor_HallToggleTimeCalcClbk_t)(void);

__STATIC_FORCEINLINE void Pl_IrqOn(void) {
	__enable_irq();
}

__STATIC_FORCEINLINE void Pl_IrqOff(void) {
	__disable_irq();
}

void Pl_Stub_CommonClbk(void);
void Pl_Stub_HardFaultClbk(u32 pcVal);
void Pl_Stub_HallClbk(u32 idHall);
void Pl_Stub_Sensors_ADCLowFreqClbk(u16* buffPtr, u16 buffLen);
void Pl_Stub_Motor_AdcHighFreqClbk(u16* buffPtr, u16 buffLen);

void Pl_Init(Pl_HardFault_Clbk_t pHardFault_Clbk);
void Pl_Delay_Init(Pl_Common_Clbk_t pDelayTimerClbk);
void Pl_JumpToAddr(u32 appAddr);

void Pl_IWDG_Init(void);
void Pl_IWDG_ReloadCounter(void);

void Pl_USART_Debug_Init(u8* pTxBuff, u16 TxBuffLen, Pl_USART_ClbkTx_t pTxClbk,
						 u8* pRxBuff, u16 RxBuffLen, Pl_USART_ClbkRx_t pRxClbk);
void Pl_USART_Debug_Enable_Tx(void);
void Pl_USART_Debug_Enable_Rx(void);
void Pl_USART_Debug_Disable_Tx(void);
void Pl_USART_Debug_Disable_Rx(void);
void Pl_USART_Debug_SetDataLengthTx(u32 NbData);
void Pl_USART_Debug_SetDataLengthRx(u32 NbData);
u32 Pl_USART_Debug_GetDataLengthTx(void);
u32 Pl_USART_Debug_GetDataLengthRx(void);
u8 Pl_USART_Debug_GetRxByte(void);
RET_STATE_t Pl_USART_Debug_TxData(u8* pBuff, u16 size);

u32 Pl_Motor_GetStateHalls(void);
void Pl_Motor_SetKeyAL(void);
void Pl_Motor_ResetKeyAL(void);
void Pl_Motor_SetKeyBL(void);
void Pl_Motor_ResetKeyBL(void);
void Pl_Motor_SetKeyCL(void);
void Pl_Motor_ResetKeyCL(void);

void Pl_Motor_SensorsInit(Pl_Motor_ClbkHall_t pHallClbk);
void Pl_Motor_PWMInit(Pl_TIM_PWM_Clbk_t pTimPwmCntTopClbk, Pl_TIM_PWM_Clbk_t pTimPwmCntBottomClbk);
void Pl_Motor_ControlGpioInit(void);
void Pl_Motor_Init(
	Pl_TIM_PWM_Clbk_t pTimPwmCntTopClbk, 
	Pl_TIM_PWM_Clbk_t pTimPwmCntBottomClbk,
	Pl_TIM_Motor_SpeedControlClbk_t pSpeedControlClbk,
	Pl_ADC_Motor_SensHighFreqClbk_t pAdcHighFreqClbk
);

void Pl_Motor_PWMA_Enable(void);
void Pl_Motor_PWMA_Disable(void);
void Pl_Motor_PWMB_Enable(void);
void Pl_Motor_PWMB_Disable(void);
void Pl_Motor_PWMC_Enable(void);
void Pl_Motor_PWMC_Disable(void);
void Pl_Motor_SetComparePWMA(u32 compareValue);
void Pl_Motor_SetComparePWMB(u32 compareValue);
void Pl_Motor_SetComparePWMC(u32 compareValue);
u32 Pl_Motor_GetComparePWM(void);

void Pl_ADC_LowFreqSens_Init(Pl_Sensors_ADCLowFreqClbk_t pADCLowFreqClbk);
void Pl_ADC_HighFreqSens_Init(Pl_ADC_Motor_SensHighFreqClbk_t pAdcHighFreqClbk);

u32 Pl_TIM_SpeedControl_GetPrescaler(void);
u32 Pl_TIM_SpeedControl_GetAutoReload(void);

void Pl_TIM_HallToggleTimeCalc_Init(Pl_Motor_HallToggleTimeCalcClbk_t pHallToggleTimeCalcClbk);
void Pl_TIM_HallToggleTimeCalc_Enable(void);
void Pl_TIM_HallToggleTimeCalc_Disable(void);
void Pl_TIM_HallToggleTimeCalc_ResetCnt(void);
u32 Pl_TIM_HallToggleTimeCalc_GetCnt(void);

void Pl_TIM_SpeedControl_Init(Pl_TIM_Motor_SpeedControlClbk_t pSpeedControlClbk);

void Pl_DMA_AdcHighFreq_Enable(void);
void Pl_ADC_StartHighFreqConv(void);

void Pl_Led_Init(void);
void Pl_Led_Set(void);
void Pl_Led_Reset(void);
void Pl_Led_Toggle(void);

void Pl_Hall1_CallIrq(void);

#endif /* __PLATFORM_H */
