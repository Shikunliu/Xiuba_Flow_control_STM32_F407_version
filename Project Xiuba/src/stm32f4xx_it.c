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
	USART_SendData(USART1, 0X22);	
	Delay(0xFFFFF); 
  	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  	{
		if(Get_Key(KEY1)==0)LEDTog(LED1);		
    	EXTI_ClearITPendingBit(EXTI_Line0);
  	}
}
