/*
*********************************************************************************************************
*
*	模块名称 : MODS通信模块, 从站
*	文件名称 : modbus_slave.c
*	版    本 : V1.0
*	说    明 : 包含 MODS_Poll()， MODS_ReciveNew(), MODS_SendWithCRC(), MODS_RxTimeOut(), MODS_AnalyzeApp()
*
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "bsp_uart_fifo.h"
#include "modbus_slave.h"
#include <inttypes.h>
#include <string.h>

uint8_t g_mods_timeout = 0;
uint32_t timer_flag = 0;
uint32_t limit_switch_flag_1 =0;
uint32_t limit_switch_flag_2 =0;
uint32_t c = 0;
MODS_T g_tModS;
VAR_T g_tVar;

/*
*********************************************************************************************************
*	函 数 名: MODS_Poll
*	功能说明: 解析数据. 在主程序中轮流调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODS_Poll(void)
{
	uint8_t wrong_message_report[4]={0xF9, 0xF1, 0x05, 0x01};			//校验错误反馈,或者位数不对，请求重发
	uint8_t wrong_message_report_bit_number[4]={0xF9, 0xF1, 0x05, 0x02};	 //位数错误
	uint8_t wrong_message_report_head[4]={0xF9, 0xF1, 0x05, 0x03};			 //报头错误
	//uint16_t addr;
	uint16_t crc1;
	
	
	// 超过3.5个字符时间后执行MODH_RxTimeOut()函数。全局变量 g_rtu_timeout = 1; 通知主程序开始解码 
	if (g_mods_timeout == 0)	
	{
		return;								// 没有超时，继续接收。不要清零 g_tModS.RxCount 
	}
	
	g_mods_timeout = 0;	 					// 清标志 
	
	
	if (g_tModS.RxCount != g_tModS.RxBuf[2])				// 接收到的数据字节数错误
	{
		MODS_SendWithCRC(wrong_message_report_bit_number, 4);
		g_tModS.RxCount = 0;
		return;	
	}
	if (0xF8 != g_tModS.RxBuf[0])				// 接收到的报头错误
	{
		MODS_SendWithCRC(wrong_message_report_head, 4);
		g_tModS.RxCount = 0;
		return;	
	}

	crc1 = sum_check(g_tModS.RxBuf, g_tModS.RxCount-1);	  	// 计算校验和, 去掉校验位  
	if (crc1 != g_tModS.RxBuf[g_tModS.RxCount-1])
	{
		MODS_SendWithCRC(wrong_message_report, 4);
		g_tModS.RxCount = 0;
		return;
	}

	// 分析应用层协议
	if (0xF1 == g_tModS.RxBuf[1]) 
	{	
		MODS_AnalyzeApp();
	}
	else if (0xF2 == g_tModS.RxBuf[1])
	{
		MODS_AnalyzeApp_F2();
	}						

	g_tModS.RxCount = 0;					// 必须清零计数器，方便下次帧同步 
	
}

/*
*********************************************************************************************************
*	函 数 名: MODS_ReciveNew
*	功能说明: 串口接收中断服务程序会调用本函数。当收到一个字节时，执行一次本函数。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODS_ReciveNew(uint8_t _byte)
{
	uint32_t timeout;
	g_mods_timeout = 0;	
	timeout = 35000000 / UART2_BAUD;			// 计算超时时间，单位us 35000000   ???应该是3500000
	
	// 硬件定时中断，定时精度us 硬件定时器1用于ADC, 定时器2用于Modbus
	bsp_StartHardTimer(1, timeout, (void *)MODS_RxTimeOut);

	if (g_tModS.RxCount < S_RX_BUF_SIZE)
	{	
		g_tModS.RxBuf[g_tModS.RxCount++]=_byte;
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC；经过修改后变为一位求和校验函数名未改变
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[S_TX_BUF_SIZE];

	memcpy(buf, _pBuf, _ucLen);
	crc = sum_check(_pBuf, _ucLen);
	buf[_ucLen++] = crc;
	RS485_SendBuf(buf, _ucLen);
}


/*
*********************************************************************************************************
*	函 数 名: MODS_RxTimeOut
*	功能说明: 超过3.5个字符时间后执行本函数。 设置全局变量 g_mods_timeout = 1; 通知主程序开始解码。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void MODS_RxTimeOut(void)
{
	g_mods_timeout = 1;
}

/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议, 处理出料指令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	uint8_t txbuf[4] = {0xF9, 0xF1, 0x05, 0x00};		//反馈工作正常
	uint8_t txbuf_limit_switch_error[4] = {0xF9, 0xF1, 0x05, 0x04};		//反馈机械手未到位，到位开关未开启
	uint32_t counter = 0;
	uint8_t indicator = 0;
	limit_switch_flag_1 = 0;
	limit_switch_flag_2 = 0;
	
	if(g_tModS.RxBuf[3]==0x01)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //假设出酒口一秒出10ml（次数每个出酒口不同），g_tModS.RxBuf[4]中存着需要的ml数，设为50ml。50/10=5sec，5sec * 2000为填入timer的值
	
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)==1)	
		{
			TIMER_Configuration(counter);//  测试额外timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)==1)
			{ 
			  delay_ms(1000);
			 	LEDOn(LED1);
				
			}
			else
			{
			 	LEDOff(LED1); 
				indicator = 1;
			}
		}
		timer_flag = 0;
		LEDOff(LED1);
		if (indicator != 1)	    //indicator指示了中途机械手有没有离开
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
//-----------------------------------------------------------------------------------------
	
   	if(g_tModS.RxBuf[3]==0x02)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //假设出酒口一秒出10ml（次数每个出酒口不同），g_tModS.RxBuf[4]中存着需要的ml数，设为50ml。50/10=5sec，5sec * 2000为填入timer的值
	
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)==1)	
		{
			TIMER_Configuration(counter);//  测试额外timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED2); 
			}
			else
			{
			 	LEDOff(LED2);
				indicator = 1;
			}
		}

		timer_flag = 0;
		LEDOff(LED2); 
		if (indicator != 1)	    //indicator指示了中途机械手有没有离开
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}

	}
	
//-----------------------------------------------------------------------------------------
	if(g_tModS.RxBuf[3]==0x03)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //假设出酒口一秒出10ml（次数每个出酒口不同），g_tModS.RxBuf[4]中存着需要的ml数，设为50ml。50/10=5sec，5sec * 2000为填入timer的值
	
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)==1)	
		{
			TIMER_Configuration(counter);//  测试额外timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED3); 
			}
			else
			{
			 	LEDOff(LED3);
				indicator = 1;
			
			}
		}
	
		timer_flag = 0;
		LEDOff(LED3);
		if (indicator != 1)	    //indicator指示了中途机械手有没有离开
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//------------------------------------------------------------------------------
		if(g_tModS.RxBuf[3]==0x04)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //假设出酒口一秒出10ml（次数每个出酒口不同），g_tModS.RxBuf[4]中存着需要的ml数，设为50ml。50/10=5sec，5sec * 2000为填入timer的值
	
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==1)	
		{
			TIMER_Configuration(counter);//  测试额外timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED4);
			}
			else
			{
			 	LEDOff(LED4);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED4);
		if (indicator != 1)	    //indicator指示了中途机械手有没有离开
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	
	//------------------------------------------------------------------------------
		if(g_tModS.RxBuf[3]==0x05)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //假设出酒口一秒出10ml（次数每个出酒口不同），g_tModS.RxBuf[4]中存着需要的ml数，设为50ml。50/10=5sec，5sec * 2000为填入timer的值
	
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)==1)	
		{
			TIMER_Configuration(counter);//  测试额外timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED5);
			}
			else
			{
			 	LEDOff(LED5);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED5);
		if (indicator != 1)	    //indicator指示了中途机械手有没有离开
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	
	//------------------------------------------------------------------------------
		if(g_tModS.RxBuf[3]==0x06)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //假设出酒口一秒出10ml（次数每个出酒口不同），g_tModS.RxBuf[4]中存着需要的ml数，设为50ml。50/10=5sec，5sec * 2000为填入timer的值
	
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==1)	
		{
			TIMER_Configuration(counter);//  测试额外timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED6);
			}
			else
			{
			 	LEDOff(LED6);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED6);
		if (indicator != 1)	    //indicator指示了中途机械手有没有离开
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
}


/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp_F2
*	功能说明: 分析应用层协议, 处理查询余量指令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp_F2(void)	 
{
	 uint8_t txbuf_0[8] = {0xF9, 0xF2, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00};
	 
	 if(g_tModS.RxBuf[3]==0x00)				//查询所有
	{
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+1;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+2;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+4;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+8;
		   }

		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+16;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+32;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+64;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+128;
		   }
		   
		   MODS_SendWithCRC(txbuf_0, 8);		   
	}
	else if(g_tModS.RxBuf[3]==0x01)			  //查询多个
	{
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+1;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+2;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+4;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+8;
		   }

		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+16;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+32;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+64;
		   }
		   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+128;
		   }
		   
		   MODS_SendWithCRC(txbuf_0, 8);
		
	}
}

/************************END OF FILE************************************/
