/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

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
