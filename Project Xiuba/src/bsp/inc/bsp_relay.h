/*
*********************************************************************************************************
*
*	ģ������ : �̵�������ģ��
*	�ļ����� : bsp_relay.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
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

#define RELAY1(x)   ((x) ? (GPIO_SetBits(RELAY_PORT, RELAY1_PIN)) : (GPIO_ResetBits(RELAY_PORT, RELAY1_PIN)));	//��xΪ0��Reset����֮ΪSet
#define RELAY2(x)   ((x) ? (GPIO_SetBits(RELAY_PORT, RELAY2_PIN)) : (GPIO_ResetBits(RELAY_PORT, RELAY2_PIN)));


void RELAY_Configuration(void);



#endif
/*****************************  (END OF FILE) *********************************/
