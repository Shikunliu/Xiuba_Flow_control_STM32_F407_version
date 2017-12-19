/*
*********************************************************************************************************
*
*	模块名称 : 到位开关模块
*	文件名称 : bsp_limit_switch.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_LIMIT_SWITCH_H
#define __BSP_LIMIT_SWITCH_H

#include "stm32f4xx_conf.h"

#include "stm32f4xx_conf.h"

typedef enum 
{  
  KEY1 = 0,

} Key_Def;

static const u16 KEY_PIN[1]={GPIO_Pin_0};
static const u16 KEY_SRC[1]={EXTI_Line0};
static const u8  KEY_PNS[1]={EXTI_PinSource0};
static const u8  KEY_IRQ[1]={EXTI0_IRQn};

void Key_Init(void);
void Set_Keyint(Key_Def key);
u8 Get_Key(Key_Def key);
u8 KEY_Scan(void);


//void BUTTON_Configuration(Key_Def key);
//void EXTI_Configuration(void);



#endif

/**************************************************************/
