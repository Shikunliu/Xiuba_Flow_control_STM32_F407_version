/*
*********************************************************************************************************
*
*	ģ������ : MODSͨ��ģ��, ��վ
*	�ļ����� : modbus_slave.c
*	��    �� : V1.0
*	˵    �� : ���� MODS_Poll()�� MODS_ReciveNew(), MODS_SendWithCRC(), MODS_RxTimeOut(), MODS_AnalyzeApp()
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
*	�� �� ��: MODS_Poll
*	����˵��: ��������. �����������������á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_Poll(void)
{
	uint8_t wrong_message_report[4]={0xF9, 0xF1, 0x05, 0x01};			//У�������,����λ�����ԣ������ط�
	uint8_t wrong_message_report_bit_number[4]={0xF9, 0xF1, 0x05, 0x02};	 //λ������
	uint8_t wrong_message_report_head[4]={0xF9, 0xF1, 0x05, 0x03};			 //��ͷ����
	//uint16_t addr;
	uint16_t crc1;
	
	
	// ����3.5���ַ�ʱ���ִ��MODH_RxTimeOut()������ȫ�ֱ��� g_rtu_timeout = 1; ֪ͨ������ʼ���� 
	if (g_mods_timeout == 0)	
	{
		return;								// û�г�ʱ���������ա���Ҫ���� g_tModS.RxCount 
	}
	
	g_mods_timeout = 0;	 					// ���־ 
	 USART_SendData(USART3, 0XEE);  
	
	if (g_tModS.RxCount != g_tModS.RxBuf[2])				// ���յ��������ֽ�������
	{
		MODS_SendWithCRC(wrong_message_report_bit_number, 4);
		g_tModS.RxCount = 0;
		return;	
	}
	if (0xF8 != g_tModS.RxBuf[0])				// ���յ��ı�ͷ����
	{
		MODS_SendWithCRC(wrong_message_report_head, 4);
		g_tModS.RxCount = 0;
		return;	
	}

	crc1 = sum_check(g_tModS.RxBuf, g_tModS.RxCount-1);	  	// ����У���, ȥ��У��λ  
	if (crc1 != g_tModS.RxBuf[g_tModS.RxCount-1])
	{
		MODS_SendWithCRC(wrong_message_report, 4);
		g_tModS.RxCount = 0;
		return;
	}

	// ����Ӧ�ò�Э��
	if (0xF1 == g_tModS.RxBuf[1]) 
	{	
		MODS_AnalyzeApp();
	}
	else if (0xF2 == g_tModS.RxBuf[1])
	{
		MODS_AnalyzeApp_F2();
	}						

	g_tModS.RxCount = 0;					// ��������������������´�֡ͬ�� 
	
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_ReciveNew
*	����˵��: ���ڽ����жϷ���������ñ����������յ�һ���ֽ�ʱ��ִ��һ�α�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_ReciveNew(uint8_t _byte)
{
	uint32_t timeout;
	g_mods_timeout = 0;	
	timeout = 35000000 / UART2_BAUD;			// ���㳬ʱʱ�䣬��λus 35000000   ???Ӧ����3500000
	
	// Ӳ����ʱ�жϣ���ʱ����us Ӳ����ʱ��1����ADC, ��ʱ��2����Modbus
	bsp_StartHardTimer(1, timeout, (void *)MODS_RxTimeOut);

	if (g_tModS.RxCount < S_RX_BUF_SIZE)
	{	
		g_tModS.RxBuf[g_tModS.RxCount++]=_byte;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC�������޸ĺ��Ϊһλ���У�麯����δ�ı�
*	��    ��: _pBuf ���ݣ�
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
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
*	�� �� ��: MODS_RxTimeOut
*	����˵��: ����3.5���ַ�ʱ���ִ�б������� ����ȫ�ֱ��� g_mods_timeout = 1; ֪ͨ������ʼ���롣
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void MODS_RxTimeOut(void)
{
	g_mods_timeout = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��, �������ָ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	uint8_t txbuf[4] = {0xF9, 0xF1, 0x05, 0x00};		//������������
	uint8_t txbuf_limit_switch_error[4] = {0xF9, 0xF1, 0x05, 0x04};		//������е��δ��λ����λ����δ����
	uint32_t counter = 0;
	uint8_t indicator = 0;
	limit_switch_flag_1 = 0;
	limit_switch_flag_2 = 0;
	
	if(g_tModS.RxBuf[3]==0x01)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)==1)
			{ 
			  delay_ms(1000);
			 	LEDOn(LED1);
				//valveOn(valve1);
				
			}
			else
			{
			 	LEDOff(LED1); 
				//valveOff(valve1);
				indicator = 1;
			}
		}
		timer_flag = 0;
		LEDOff(LED1);
		//valveOff(valve1);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
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
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)==1)
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
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
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
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==1)
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
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
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
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)==1)
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
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
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
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)==1)
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
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
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
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)==1)
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
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x07)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED7);
			}
			else
			{
			 	LEDOff(LED7);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED7);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x08)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED8);
			}
			else
			{
			 	LEDOff(LED8);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED8);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x09)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED9);
			}
			else
			{
			 	LEDOff(LED9);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED9);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x10)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED10);
			}
			else
			{
			 	LEDOff(LED10);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED10);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x11)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED11);
			}
			else
			{
			 	LEDOff(LED11);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED11);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x12)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED12);
			}
			else
			{
			 	LEDOff(LED12);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED12);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x13)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED13);
			}
			else
			{
			 	LEDOff(LED13);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED13);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x14)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED14);
			}
			else
			{
			 	LEDOff(LED14);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED14);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x15)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED15);
			}
			else
			{
			 	LEDOff(LED15);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED15);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
		{
			MODS_SendWithCRC(txbuf, 4);
		}
		else
		{
			MODS_SendWithCRC(txbuf_limit_switch_error, 4);
		}
	}
	//----------------------------------------------------------------------------------------------------	
	if(g_tModS.RxBuf[3]==0x16)
	{
		counter = (g_tModS.RxBuf[4]/10)*2000;  //������ƿ�һ���10ml������ÿ�����ƿڲ�ͬ����g_tModS.RxBuf[4]�д�����Ҫ��ml������Ϊ50ml��50/10=5sec��5sec * 2000Ϊ����timer��ֵ
	
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)==1)	
		{
			TIMER_Configuration(counter);//  ���Զ���timer
			TIM_Cmd(TIM4, ENABLE);
		}
		else 
		{
			 MODS_SendWithCRC(txbuf_limit_switch_error, 4);
			 return;
		}
		while(timer_flag!=2)
		{
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)==1)
			{ 
			 	delay_ms(1000);
			 	LEDOn(LED16);
			}
			else
			{
			 	LEDOff(LED16);
				indicator = 1;

			}
		}

		timer_flag = 0;
		LEDOff(LED16);
		if (indicator != 1)	    //indicatorָʾ����;��е����û���뿪
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
*	�� �� ��: MODS_AnalyzeApp_F2
*	����˵��: ����Ӧ�ò�Э��, �����ѯ����ָ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_AnalyzeApp_F2(void)	 
{
	 uint8_t txbuf_0[8] = {0xF9, 0xF2, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00};
	 
	 if(g_tModS.RxBuf[3]==0x00)				//��ѯ����
	{
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+1;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+2;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+4;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+8;
		   }

		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+16;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+32;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+64;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+128;
		   }
			 
			 
			 if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+1;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+2;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+4;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+8;
		   }

		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+16;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+32;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+64;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+128;
		   }
		   
		   MODS_SendWithCRC(txbuf_0, 8);		   
	}
	else if(g_tModS.RxBuf[3]==0x01)			  //��ѯ���
	{
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+1;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+2;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+4;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+8;
		   }

		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+16;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+32;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)==1)
		   {
		  	  txbuf_0[7] = txbuf_0[7]+64;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7)==1)	
		   {
		   	  txbuf_0[7] = txbuf_0[7]+128;
		   }
			 
			 
			 if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+1;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+2;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+4;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+8;
		   }

		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+16;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+32;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14)==1)
		   {
		  	  txbuf_0[6] = txbuf_0[6]+64;
		   }
		   if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15)==1)	
		   {
		   	  txbuf_0[6] = txbuf_0[6]+128;
		   }
		   
		   MODS_SendWithCRC(txbuf_0, 8);
		
	}
}

/************************END OF FILE************************************/
