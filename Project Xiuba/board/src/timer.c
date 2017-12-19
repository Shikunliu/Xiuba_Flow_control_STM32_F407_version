#include "timer.h"

//��84MHz��TIM3ʱ��ԴΪ168MHz/2������TIM_scale��Ƶ����Ϊ����ʱ��
void TIM3_Init(u32 TIM_scale, u32 TIM_Period)//TIM_PeriodΪ16λ����
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = TIM_Period;//��������װֵ
  	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (TIM_scale-1), TIM_PSCReloadMode_Immediate);
  	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  	TIM_Cmd(TIM3, ENABLE);
}

//��ʼ��ʱ��Ƚ�ģʽ
void TIM3_Cinit(u32 TIM_scale ,u16 CCR1 ,u16 CCR2 ,u16 CCR3)//CCRΪ16λ����
{	 
  	NVIC_InitTypeDef  NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	  
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 65535;
  	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (TIM_scale-1), TIM_PSCReloadMode_Immediate);
  	//ͨ��1
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR1;
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//ͨ��2 	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR2;
  	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//ͨ��3
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR3;
  	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//�ж�ʹ��
  	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);		
}




