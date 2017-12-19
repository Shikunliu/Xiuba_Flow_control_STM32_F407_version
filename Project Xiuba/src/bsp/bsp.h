/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : �ײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

#include "bsp_led.h"
//#include "bsp_relay.h"
#include "bsp_timer.h"
#include "bsp_uart_fifo.h"
#include "bsp_user_lib.h"
#include "bsp_limit_switch.h"
#include "bsp_level_sensor_input.h"


#include <stm32f4xx.h>
#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "delay.h"
#include "usart.h"


void bsp_Init(void);
void USART_Configuration(void);	 
void NVIC_Configuration(void);
void bsp_InitUart(void);
void bsp_InitHardTimer(void);

#endif

/***************************** (END OF FILE) *********************************/
