/****************************Copyright(c)*****************************
**                      http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           bsp_Relay.c
** Last modified Date:  2017-12-01
** Last Version:        V1.00
** Descriptions:        硬件配置文件
**
**----------------------------------------------------------------------------------------------------
** Created by:          Shikun
** Created date:        2017-12-01
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


#include "bsp_relay.h"



/**
	*名称：void RELAY_Configuration(void)
	*输入：无
	*输出：无
	*功能：继电器初始化配置
	*说明：
**/
void RELAY_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_RELAY, ENABLE);
	GPIO_InitStructure.GPIO_Pin = RELAY1_PIN | RELAY2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //io口八种工作方式的一种：PP，push&pull， 推挽输出
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);

	/*---------初始化状态------------*/
	RELAY1(0);
	RELAY2(0);
}

/***************************** (END OF FILE) *********************************/
