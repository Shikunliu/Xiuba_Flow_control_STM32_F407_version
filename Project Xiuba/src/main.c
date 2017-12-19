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
  USART_SendData(USART1, (uint8_t) ch);   

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

