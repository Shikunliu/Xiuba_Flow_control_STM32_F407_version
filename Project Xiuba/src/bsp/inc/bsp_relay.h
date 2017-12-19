/*
*********************************************************************************************************
*
*	模块名称 : 继电器驱动模块
*	文件名称 : bsp_relay.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __BSP_RELAY_H
#define __BSP_RELAY_H

#include "stm32f10x_conf.h"

/* RELAY */
#define RCC_APB2Periph_RELAY	RCC_APB2Periph_GPIOB
#define RELAY_PORT	GPIOB
#define RELAY1_PIN	GPIO_Pin_10
#define RELAY2_PIN	GPIO_Pin_11

#define RELAY1(x)   ((x) ? (GPIO_SetBits(RELAY_PORT, RELAY1_PIN)) : (GPIO_ResetBits(RELAY_PORT, RELAY1_PIN)));	//若x为0，Reset；反之为Set
#define RELAY2(x)   ((x) ? (GPIO_SetBits(RELAY_PORT, RELAY2_PIN)) : (GPIO_ResetBits(RELAY_PORT, RELAY2_PIN)));


void RELAY_Configuration(void);



#endif
/*****************************  (END OF FILE) *********************************/
