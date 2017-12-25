/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f4xx_conf.h"

typedef enum   //泵
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
	LED5 = 4,
	LED6 = 5,
	LED7 = 6,
  LED8 = 7,
  LED9 = 8,
  LED10 = 9,
	LED11 = 10,
	LED12 = 11,
	LED13 = 12,
  LED14 = 13,
  LED15 = 14,
  LED16 = 15
} Led_Def;

static GPIO_TypeDef* LED_PORT[16]={GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOB, GPIOB, GPIOB, GPIOB};
static const u16 LED_PIN[16]={GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4,GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_4, GPIO_Pin_5};

void LEDOn(Led_Def Led);
void LEDOff(Led_Def Led);
void LEDTog(Led_Def Led);
//-----------------------------------------------------------------------------
typedef enum   //阀
{
  valve1 = 0,
  valve2 = 1,
  valve3 = 2,
  valve4 = 3,
	valve5 = 4,
	valve6 = 5,
	valve7 = 6,
  valve8 = 7,
  valve9 = 8,
  valve10 = 9,
	valve11 = 10,
	valve12 = 11,
	valve13 = 12,
  valve14 = 13,
  valve15 = 14,
  valve16 = 15
} valve_Def;

static GPIO_TypeDef* valve_PORT[16]={GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC};
static const u16 valve_PIN[16]={GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2,GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

void valveOn(valve_Def valve);
void valveOff(valve_Def valve);
void valveTog(valve_Def valve);

//-----------------------------------------------------------------------------
void LED_Configuration(void);
void valve_Configuration(void);

#endif
/***************************** (END OF FILE) *********************************/
