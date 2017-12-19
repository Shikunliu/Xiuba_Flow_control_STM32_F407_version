#include "delay.h"

__IO u16 ntime;								    

void delay_ms(u16 nms)
{	 		  	  
	ntime=nms;
	SysTick_Config(168000);//1ms����һ���ж�
	while(ntime);
	SysTick->CTRL=0x00;			  	    
}   
		    								   
void delay_us(u32 nus)
{		
	ntime=nus;
	SysTick_Config(168);//1us����һ���ж�
	while(ntime);
	SysTick->CTRL=0x00;
}

void Delay(u32 count)
{
	while(count--);
}

