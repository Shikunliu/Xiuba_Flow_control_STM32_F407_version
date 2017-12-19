/****************************Copyright(c)*****************************
**                      http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           bsp_limit_switch.c
** Last modified Date:  2017-11-27
** Last Version:        V1.00
** Descriptions:        硬件配置文件
**
**----------------------------------------------------------------------------------------------------
** Created by:          Shikun
** Created date:        2017-11-27
** Version:             V1.00
** Descriptions:        编写示例代码
**
**----------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:        
** Version:            
** Descriptions:        
**
*****************************************************************/ 


#include "bsp_limit_switch.h"
#include "bsp_led.h"
#include "bsp_timer.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Set_Keyint(Key_Def key)//将按键设置成中断方式
{
  	EXTI_InitTypeDef EXTI_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, KEY_PNS[key]);
    EXTI_InitStructure.EXTI_Line = KEY_SRC[key];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
	//if(key==KEY3)EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = KEY_IRQ[key];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//优先级最低
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
}

u8 Get_Key(Key_Def key)
{
  	if ((GPIOB->IDR&KEY_PIN[key])!=0)return 1;
  	else return 0;
}

u8 KEY_Scan(void)
{	 
	static u8 key_up=1;
	if(key_up&&(Get_Key(KEY1)==0))
	{
		Delay(0xFFFFF); 
		key_up=0;
		if(Get_Key(KEY1)==0)return 1;

	}
	else if(Get_Key(KEY1)==1)key_up=1; 	    
	return 0;
}




/*************************************************************/
