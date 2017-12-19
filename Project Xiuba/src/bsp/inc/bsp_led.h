/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f4xx_conf.h"

typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
	LED5 = 4,
	LED6 = 5
} Led_Def;

static GPIO_TypeDef* LED_PORT[6]={GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD};
static const u16 LED_PIN[6]={GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14,GPIO_Pin_15, GPIO_Pin_5, GPIO_Pin_6};

void LED_Init(void);
void LEDOn(Led_Def Led);
void LEDOff(Led_Def Led);
void LEDTog(Led_Def Led);


void LED_Configuration(void);

#endif
/***************************** (END OF FILE) *********************************/
