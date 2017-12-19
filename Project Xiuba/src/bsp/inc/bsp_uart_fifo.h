/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/
#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f4xx_conf.h"

/*
	���ڷ��䣺

	������1�� RS485 ͨ��
		PD5/USART2_TX
		PD6/USART2_RX
		PD7/RS485_DIR 
	
	������2�� RS232 
		PA9/USART1_TX	  --- ��ӡ���Կ�
		PA10/USART1_RX
*/
/* RS485оƬ����ʹ��GPIO, PD7 */
#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOD
#define PORT_RS485_TXEN  GPIOD
#define PIN_RS485_TXEN	 GPIO_Pin_7

#define RS485_RX_EN()	PORT_RS485_TXEN->BRR = PIN_RS485_TXEN
#define RS485_TX_EN()	PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN


/* ����˿ں� */
typedef enum
{
	COM1 = 0,	/* USART1  PA9, PA10 */
	COM2 = 1,	/* USART2, PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
}COM_PORT_E;


	#define UART1_BAUD			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024

	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	1*1024

/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;			/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;			/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;			/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;			/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;			/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;			/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
}UART_T;

void bsp_InitUart(void);
void RS485_InitTXE(void);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void USART2_SendString(uint8_t *ch);
void USART1_SendString(uint8_t *ch);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void RS485_ReciveNew(uint8_t _byte);

void COM1Init(u32 BaudRate);
#endif

/**************************************************************/
