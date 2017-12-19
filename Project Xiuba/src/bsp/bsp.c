/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.c
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序模块的主文件。主要提供 bsp_Init()函数供主程序调用。
*			   主程序的每个c文件可以在开头	添加 #include "bsp.h" 来包含所有的外设驱动模块。
*
*	修改记录 :
*		版本号  日期        作者        
*		V1.0    2017-12-01  Shikun Liu  
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "bsp_limit_switch.h"

void bsp_Init(void)
{
	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为72MHz，如果需要更改，可以修改 system_stm32f10x.c 文件
	*/

	/* 优先级分组设置为4 */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	bsp_InitTimer();
	
	LEVEL_SENSOR_INPUT_Configuration();
	NVIC_Configuration();    //测试额外timer，配置中断
  //RELAY_Configuration();	 //继电器初始化
  LED_Configuration();	 //LED初始化
	bsp_InitUart(); 	     //串口初始化
	//Key_Init();
	//Set_Keyint(KEY1);

}

/***************************** (END OF FILE) *********************************/
