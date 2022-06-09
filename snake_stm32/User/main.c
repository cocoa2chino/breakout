/**
  ******************************************************************************
  * @file    main.c
  * @author  mr
  * @version V1.0
  * @date    2021-02-28
  * @brief   贪吃蛇游戏
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./led/bsp_led.h" 
#include "./key/bsp_key.h" 
#include "palette.h"
#include <string.h>
#include "bsp_TiMbase.h" 

static uint8_t KEY2_LongPress(void);

int main(void)
{		

	//LCD 初始化
	ILI9341_Init();  
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);

  /*��ʼ��USART1*/
  USART_Config();			
	//触摸屏初始化
	XPT2046_Init();
	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
	Calibrate_or_Get_TouchParaWithFlash(6,0);
	
	BASIC_TIM_Init();

//	USART_Config();  
	LED_GPIO_Config();
	

	/* ?????????????????????????????? */
	Key_GPIO_Config();
	LCD_SetColors(CL_RED,CL_WHITE);
		LCD_SetFont(&Font24x32);
		ILI9341_DispString_EN_CH(12,120,"Game");
		LCD_SetFont(&Font8x16);
		ILI9341_DispString_EN_CH(64,157,"k1 breakout k2 snake");
	/* ????????????????????��?????????????????????????????????????????????��?LED */
	while(1)                            
	{	   
		if(KEY2_LongPress())
		{
			
			LED_RED;	
			Delay(0xFFFF);
			
			/*���WU״̬λ*/
			PWR_ClearFlag (PWR_FLAG_WU);
			
			/* ʹ��WKUP���ŵĻ��ѹ��� ��ʹ��PA0*/
			PWR_WakeUpPinCmd (ENABLE);
			
			/* �������ģʽ */
			PWR_EnterSTANDBYMode();
		}
		
		
		
		
		
		
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
				Palette_Init(LCD_SCAN_MODE);
 
	while ( 1 )
	{
	  Play();
	}

		} 

		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			Palette_Init1(LCD_SCAN_MODE);

	while ( 1 )
  	{
	snakeMove();  //??�?????????��???��???��??��?????��
	}		
		}		
	}
}





/**
  * @brief  ���ڼ�ⰴ���Ƿ񱻳�ʱ�䰴��
  * @param  ��
  * @retval 1 ����������ʱ�䰴��  0 ������û�б���ʱ�䰴��
  */
static uint8_t KEY2_LongPress(void)
{			
	uint8_t downCnt =0;																				//��¼���µĴ���
	uint8_t upCnt =0;																					//��¼�ɿ��Ĵ���			

	while(1)																										//��ѭ������return����
	{	
		Delay(0x2FFFF);																					//�ӳ�һ��ʱ���ټ��

		if(GPIO_ReadInputDataBit ( KEY2_GPIO_PORT, KEY2_GPIO_PIN ) == SET)	//��⵽���°���
		{
			downCnt++;																					//��¼���´���
			upCnt=0;																						//��������ͷż�¼

			if(downCnt>=100)																		//����ʱ���㹻
			{
				return 1; 																				//��⵽������ʱ�䳤����
			}
		}
		else 
		{
			upCnt++; 																						//��¼�ͷŴ���
			if(upCnt>5)																					//������⵽�ͷų���5��
			{
				return 0;																				//����ʱ��̫�̣����ǰ�����������
			}
		}//	if(GPIO_ReadInputDataBit 
	}//while
}
/*********************************************END OF FILE**********************/


/* ------------------------------------------end of file---------------------------------------- */

