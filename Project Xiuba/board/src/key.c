#include "key.h"
#include "delay.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Set_Keyint(Key_Def key)//将按键设置成中断方式
{
  	EXTI_InitTypeDef EXTI_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, KEY_PNS[key]);
    EXTI_InitStructure.EXTI_Line = KEY_SRC[key];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
	if(key==KEY3)EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	else EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
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
	if(key_up&&(Get_Key(KEY1)==0||Get_Key(KEY2)==0||Get_Key(KEY3)==1||Get_Key(KEY4)==0))
	{
		Delay(0xFFFFF); 
		key_up=0;
		if(Get_Key(KEY1)==0)return 1;
		else if(Get_Key(KEY2)==0)return 2;
		else if(Get_Key(KEY3)==1)return 3;
		else if(Get_Key(KEY4)==0)return 4;
	}
	else if(Get_Key(KEY1)==1&&Get_Key(KEY2)==1&&Get_Key(KEY3)==0&&Get_Key(KEY4)==1)key_up=1; 	    
	return 0;
}


