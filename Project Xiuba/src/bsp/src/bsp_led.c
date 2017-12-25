/****************************Copyright(c)*****************************
**                      http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           bsp_LED.c
** Last modified Date:  2017-11-27
** Last Version:        V1.00
** Descriptions:        Ӳ�������ļ�
**
**----------------------------------------------------------------------------------------------------
** Created by:          Shikun
** Created date:        2017-11-27
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


#include "bsp_led.h"



void LED_Configuration(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;  //�ÿ���

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void LEDOn(Led_Def Led)//�ÿ�
{
  	LED_PORT[Led]->BSRRL=LED_PIN[Led];
}

void LEDOff(Led_Def Led)//�ù�
{
  	LED_PORT[Led]->BSRRH=LED_PIN[Led];  
}

void LEDTog(Led_Def Led)//״̬��ת
{
  	LED_PORT[Led]->ODR^=LED_PIN[Led];
}


void valve_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void valveOn(valve_Def valve)//���ſ�
{
  	valve_PORT[valve]->BSRRL=valve_PIN[valve];
}

void valveOff(valve_Def valve)//���Ź�
{
  	valve_PORT[valve]->BSRRH=valve_PIN[valve];  
}

void valveTog(valve_Def valve)//״̬��ת
{
  	valve_PORT[valve]->ODR^=valve_PIN[valve];
}

/***************************** (END OF FILE) *********************************/
