/*
*********************************************************************************************************
*
*	ģ������ : MODEBUS ͨ��ģ�� (��վ��
*	�ļ����� : modbus_slave.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/

#ifndef __MODBUY_SLAVE_H
#define __MODBUY_SLAVE_H

void MODS_Poll(void);
static void MODS_RxTimeOut(void);
static void MODS_AnalyzeApp(void);
static void MODS_AnalyzeApp_F2(void);
static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
void MODS_ReciveNew(uint8_t _byte);


#define S_RX_BUF_SIZE		30
#define S_TX_BUF_SIZE		128

typedef struct
{
	uint8_t RxBuf[S_RX_BUF_SIZE];
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;

	uint8_t RspCode;

	uint8_t TxBuf[S_TX_BUF_SIZE];
	uint8_t TxCount;
}MODS_T;

typedef struct
{
	/* 03H 06H ��д���ּĴ��� */
	uint16_t P01;
	uint16_t P02;

	/* 04H ��ȡģ�����Ĵ��� */
	uint16_t A01;

	/* 01H 05H ��д����ǿ����Ȧ */
	uint16_t D01;
	uint16_t D02;
	uint16_t D03;
	uint16_t D04;

}VAR_T;

#endif

/**************************************************************/
