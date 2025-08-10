#include "main.h"
#include "platform.h"
#include "iwdg.h"
#include "delay.h"
#include "debug/debug_interface.h"
#include "debug/debug_process.h"
#include "motor/motor.h"
#include "motor/sensors.h"
#include "uart_process.h"

#if DEBUG_ENABLE
#warning DEBUG_ENABLE
#endif /* DEBUG_ENABLE */

u8 ucHeap[configTOTAL_HEAP_SIZE];

void FreeRTOS_InitComponents(void);

void HardFault_Clbk(u32 pcVal)
{
	// BkpStorage_SetRegister(BKP_REG_SYS_FAULT_EXEPTION_ADDR, pcVal);

	// Pl_Motor_ResetKeyAL();
	// Pl_Motor_ResetKeyBL();
	// Pl_Motor_ResetKeyCL();
	// Pl_Motor_PWMA_Disable();
	// Pl_Motor_PWMB_Disable();
	// Pl_Motor_PWMC_Disable();
	__NOP();
}

int main(void) {
	FreeRTOS_InitComponents();
	Pl_Init(HardFault_Clbk);
	Pl_Led_Init();
	
	Delay_Init();
	// IWDG_Init();
	DebugInterface_Init();
	// Motor_Init();

	vTaskStartScheduler();
	
	for(;;)
		__NOP();
}

void ErrorHandler(char *pFile, int line)
{
	DISCARD_UNUNSED(pFile);
	DISCARD_UNUNSED(line);

	Pl_IrqOff();
	PL_SET_BKPT();
	while(1);
}

void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

void FreeRTOS_InitComponents(void)
{
	// FreeRTOS_IWDG_InitComponents();
	FreeRTOS_DebugProcess_InitComponents();
	// FreeRTOS_Motor_InitComponents();
	// FreeRTOS_Sensors_InitComponents();
	// FreeRTOS_MotorUartProcess_InitComponents();
}
