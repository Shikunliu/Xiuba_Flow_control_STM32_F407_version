//V1.0.0
#include "stm32f4xx_it.h"
#include "main.h"

//NMI exception handler
void NMI_Handler(void)
{
}

//Hard Fault exception handler
void HardFault_Handler(void)
{
  	while (1)
  	{
  	}
}

//Memory Manage exception handler
void MemManage_Handler(void)
{
  	while (1)
  	{
  	}
}

//Bus Fault exception handler
void BusFault_Handler(void)
{
  	while (1)
  	{
  	}
}

//Usage Fault exception handler
void UsageFault_Handler(void)
{
  	while (1)
  	{
  	}
}

//SVCall exception handler
void SVC_Handler(void)
{
}

//Debug Monitor exception handler
void DebugMon_Handler(void)
{
}

//PendSVC exception handler
void PendSV_Handler(void)
{
}

//SysTick handler
extern u32 ntime;
void SysTick_Handler(void)
{
	ntime--;
}

void EXTI0_IRQHandler(void)
{
		extern void valveOn();
	  extern uint8_t cal_flag;

		Delay(0xFFFFF); 

  	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  	{
			cal_flag = 1;
					
    	EXTI_ClearITPendingBit(EXTI_Line0);
  	}
}
