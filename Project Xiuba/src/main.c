////////////////////////////////////////////////////////////////////
//                          _ooOoo_                               //
//                         o8888888o                              //
//                         88" . "88                              //
//                         (| ^_^ |)                              //
//                         O\  =  /O                              //
//                      ____/`---'\____                           //
//                    .'  \\|     |//  `.                         //
//                   /  \\|||  :  |||//  \                        //
//                  /  _||||| -:- |||||-  \                       //
//                  |   | \\\  -  /// |   |                       //
//                  | \_|  ''\---/''  |   |                       //
//                  \  .-\__  `-`  ___/-. /                       //
//                ___`. .'  /--.--\  `. . ___                     //
//              ."" '<  `.___\_<|>_/___.'  >'"".                  //
//            | | :  `- \`.;`\ _ /`;.`/ - ` : | |                 //
//            \  \ `-.   \_ __\ /__ _/   .-` /  /                 //
//      ========`-.____`-.___\_____/___.-`____.-'========         //
//                           `=---='                              //
//      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^        //
//                  佛祖保佑         永无BUG                      //
////////////////////////////////////////////////////////////////////

/*******************************************************
**------------------------File Info-------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2017-12-18
** Last Version:        V1.00
** Descriptions:        秀吧机器人流量控制模块主函数，F107 移植 到F407
**
**----------------------------------------------------------------------------------------------------
** Created by:          Shikun Liu
** Created date:        2017-12-18
** Version:             V1.00
** Descriptions:        秀吧机器人流量控制模块主函数
**
**----------------------------------------------------------------------------------------------------
** Modified by:         	
** Modified date:        
** Version:            
** Descriptions:        
**
*****************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"
#include "bsp.h"
#include "modbus_slave.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void calibration_valve(void);

uint8_t cal_flag;
/** 
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
	extern MODS_T g_tModS;
	SystemInit();     	//系统初始化, 配置始终，在启动文件里已经被调用
  bsp_Init();			//初始化
		
	while(1)
	{		
		MODS_Poll();		//Modbus通信
		calibration_valve();
	}
}



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART3, (uint8_t) ch);   

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

void calibration_valve(void)
{
	uint8_t count = 0;
	uint16_t Pin_arr[16] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
	uint16_t valve_arr[16] = {valve1, valve2, valve3, valve4, valve5, valve6, valve7, valve8,  valve9, valve10, valve11, valve12, valve13, valve14, valve15, valve16};
	uint16_t LED_arr[16] = {LED1, LED2, LED3, LED4, LED5, LED6, LED6, LED8,  LED9, LED10, LED11, LED12, LED13, LED14, LED15, LED16};
	
	if (cal_flag == 0)	
	{	 
		return;			
	}
	cal_flag = 0;	
	
	for(count=0; count<16; count++)
	{
		while(GPIO_ReadInputDataBit(GPIOD, Pin_arr[count])==0)	
		{				
				valveOn(valve_arr[count]);	
				LEDOn(LED_arr[count]);			
		}
		valveOff(valve_arr[count]);	
		LEDOff(LED_arr[count]);	
	}
}

