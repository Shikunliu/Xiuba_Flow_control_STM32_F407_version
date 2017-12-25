/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.c
*	版    本 : V1.0
*	说    明 : 采用串口中断+FIFO模式实现多个串口的同时访问
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "bsp_uart_fifo.h"

/* 定义每个串口结构体变量 */

	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */

	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */

static void UartVarInit(void);
static void InitHardUart(void);
static void UartIRQ(UART_T *_pUart);
static void ConfigUartNVIC(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendBefor(void);
void RS485_SendOver(void);
void MODS_ReciveNew(uint8_t _byte);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();		/* 必须先初始化全局变量,再配置硬件 */

	InitHardUart();		/* 配置串口的硬件参数(波特率等) */

	//RS485_InitTXE();	/* 配置RS485芯片的发送使能硬件，配置为推挽输出 */

	ConfigUartNVIC();	/* 配置串口中断 */
}



/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: uart指针
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		//#if UART1_FIFO_EN == 1
			return &g_tUart1;
		//#else
//			return 0;
		//#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_tUart4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_tUart5;
		#else
			return 0;
		#endif
	}
	else
	{
		/* 不做任何处理 */
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
		//USART1_SendString("\r\n 进入SendBefor\r\n");//
	}
	UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_InitTXE
*	功能说明: 配置RS485发送使能口线 TXE
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
/*
void RS485_InitTXE(void)

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;     //pin5  #define PIN_RS485_TXEN	 GPIO_Pin_5 
	GPIO_Init(GPIOD, &GPIO_InitStructure);   //port D #define PORT_RS485_TXEN  GPIOD
}
*/
/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
	#if 0
		/*
			在调试GPRS例程时，下面的代码出现死机，while 死循环
			原因： 发送第1个字节时 _pUart->usTxWrite = 1；_pUart->usTxRead = 0;
			将导致while(1) 无法退出
		*/
		while (1)
		{
			uint16_t usRead;

			DISABLE_INT();
			usRead = _pUart->usTxRead;		  //  发送缓冲区，读指针
			ENABLE_INT();

			if (++usRead >= _pUart->usTxBufSize)	//1024
			{
				usRead = 0;
			}

			if (usRead != _pUart->usTxWrite)		//发送缓冲区写指针
			{
				break;
			}
		}
	#else
		/* 当 _pUart->usTxBufSize == 1 时, 下面的函数会死掉(待完善) */
		while (1)
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
		}
	#endif

		/* 将新数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)		 //Buf 指针大于1024的话
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;			//传出计数加一
		ENABLE_INT();
	}

	USART_ITConfig(_pUart->uart, USART_IT_TXE, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBefor
*	功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void RS485_SendBefor(void)
{
	//RS485_TX_EN();	// 切换RS485收发芯片为发送模式
}


/*
*********************************************************************************************************
*	函 数 名: RS485_SendOver
*	功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void RS485_SendOver(void)
{
	//RS485_RX_EN();	// 切换RS485收发芯片为接收模式
}


/*
*********************************************************************************************************
*	函 数 名: RS485_SendBuf
*	功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/


void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	//USART1_SendString("\r\n 进入RS485_SendBuf \r\n");
	comSendBuf(COM1, _ucaBuf, _usLen);
	
}


/*
*********************************************************************************************************
*	函 数 名: RS485_ReciveNew
*	功能说明: 接收到新的数据
*	形    参: _byte 接收到的新数据
*	返 回 值: 无
*********************************************************************************************************
*/

extern void MODS_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
	MODS_ReciveNew(_byte);
}


/*
*********************************************************************************************************
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

static void UartVarInit(void)
{

	g_tUart1.uart = USART3;						/* STM32 串口设备 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* 发送缓冲区指针 */
	g_tUart1.pRxBuf = g_RxBuf1;					/* 接收缓冲区指针 */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart1.SendBefor = RS485_SendBefor;		/* 发送数据前的回调函数 */
	g_tUart1.SendOver = RS485_SendOver;			/* 发送完毕后的回调函数 */
	g_tUart1.ReciveNew = RS485_ReciveNew;	    /* 接收到新数据后的回调函数 */

	g_tUart2.uart = USART2;						/* STM32 串口设备 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* 发送缓冲区指针 */
	g_tUart2.pRxBuf = g_RxBuf2;					/* 接收缓冲区指针 */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart2.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart2.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart2.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart2.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart2.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart2.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart2.SendBefor = 0;						/* 发送数据前的回调函数 */
	g_tUart2.SendOver = 0;						/* 发送完毕后的回调函数 */
	g_tUart2.ReciveNew = 0;						/* 接收到新数据后的回调函数 */

}

/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-F4开发板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{	
		GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure; 

	/* 第1步：打开GPIO和USART部件的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// 第2步：将USART Tx的GPIO配置为推挽复用模式

	// 第3步：将USART Rx的GPIO配置为浮空输入模式由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
	
		
 	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);  
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOD, &GPIO_InitStructure); 


	
	// 第4步： 配置串口硬件参数
	/*--------------USART1 USART2配置-------------------*/
	USART_InitStructure.USART_BaudRate = UART1_BAUD;//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  	USART_Init(USART3, &USART_InitStructure);
  	USART_Cmd(USART3, ENABLE);
  	USART_ClearFlag(USART3, USART_FLAG_TC);
		
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: USART1_SendString, USART2_SendString
*	功能说明: USART 输出字节
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void USART2_SendString(uint8_t *ch)
{
	while(*ch!=0)
	{		
		while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));
		USART_SendData(USART2, *ch);
		ch++;
	}   	
}

void USART1_SendString(uint8_t *ch)
{
	while(*ch!=0)
	{		
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, *ch);
		ch++;
	}   	
}

void UART4_SendString(uint8_t *ch)
{
	while(*ch!=0)
	{		
		while(!USART_GetFlagStatus(UART4, USART_FLAG_TXE));
		USART_SendData(UART4, *ch);
		ch++;
	}   	
}
/*
*********************************************************************************************************
*	函 数 名: ConfigUartNVIC
*	功能说明: 配置串口硬件中断.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler
*	功能说明: USART中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void USART3_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}

void USART2_IRQHandler(void)
{
	//UartIRQ(&g_tUart2);
}


/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	形    参: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
//void UartIRQ(void)
{
  
	// 处理接收中断  
	if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
	{
		//从串口接收数据寄存器读取数据存放到接收FIFO
		uint8_t ch;

		//USART1_SendString("\r\n 进入 处理处理接收中断！！\r\n");
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);//???

		ch = USART_ReceiveData(_pUart->uart);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;		   //[缓冲区指针]
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)		//指针加一，大于等于缓冲区尺寸（1024）的话
		{
			_pUart->usRxWrite = 0;					//缓冲区归零
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;						   	//在缓冲区中等待发送的数据个数
		}

		 //回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch);
			}
		}
	}

	// 处理发送缓冲区空中断
	if (USART_GetITStatus(_pUart->uart, USART_IT_TXE) != RESET)
	{
		//USART1_SendString("\r\n 进入 处理发送缓冲区空中断！！\r\n");
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			//发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕
			USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);

			// 使能数据发送完毕中断
			USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
		}
		else
		{
			// 从发送FIFO取1个字节写入串口发送数据寄存器
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);	 //uart口，发送缓冲区[发送缓冲区读指针]
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)	 //如果超过尺寸1024，读指针归零
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;				   //减少一个未读量
		}

	}
	// 数据bit位全部发送完毕的中断		  TXE,TC 两种发送方式
	else if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			// 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断
			USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);

			// 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
		}
		else
		{
			// 正常情况下，不会进入此分支

			// 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}

/***************************** (END OF FILE) *********************************/
