/****************************Copyright(c)*****************************
**                      http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           bsp_Relay.c
** Last modified Date:  2017-12-01
** Last Version:        V1.00
** Descriptions:        Ӳ�������ļ�
**
**----------------------------------------------------------------------------------------------------
** Created by:          Shikun
** Created date:        2017-12-01
** Version:             V1.00
** Descriptions:        ��дʾ������
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
	*���ƣ�void RELAY_Configuration(void)
	*���룺��
	*�������
	*���ܣ��̵�����ʼ������
	*˵����
**/
void RELAY_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_RELAY, ENABLE);
	GPIO_InitStructure.GPIO_Pin = RELAY1_PIN | RELAY2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //io�ڰ��ֹ�����ʽ��һ�֣�PP��push&pull�� �������
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);

	/*---------��ʼ��״̬------------*/
	RELAY1(0);
	RELAY2(0);
}

/***************************** (END OF FILE) *********************************/
