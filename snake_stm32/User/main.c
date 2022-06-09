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

  /*初始化USART1*/
  USART_Config();			
	//触摸屏初始化
	XPT2046_Init();
	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
	Calibrate_or_Get_TouchParaWithFlash(6,0);
	
	BASIC_TIM_Init();

//	USART_Config();  
	LED_GPIO_Config();
	

	/* 配置按键用到的I/O口 */
	Key_GPIO_Config();
	LCD_SetColors(CL_RED,CL_WHITE);
		LCD_SetFont(&Font24x32);
		ILI9341_DispString_EN_CH(12,120,"Game");
		LCD_SetFont(&Font8x16);
		ILI9341_DispString_EN_CH(64,157,"k1 breakout k2 snake");
	while(1)                            
	{	   
		if(KEY2_LongPress())
		{
			
			LED_RED;	
			Delay(0xFFFF);
			
			/*清除WU状态位*/
			PWR_ClearFlag (PWR_FLAG_WU);
			
			/* 使能WKUP引脚的唤醒功能 ，使能PA0*/
			PWR_WakeUpPinCmd (ENABLE);
			
			/* 进入待机模式 */
			PWR_EnterSTANDBYMode();
		}
		
		
		
		
		
		//按下k1
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
				Palette_Init(LCD_SCAN_MODE);
 
	while ( 1 )
	{
	  Play();
	}

		} 
		//按下k2
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			Palette_Init1(LCD_SCAN_MODE);

	while ( 1 )
  	{
	snakeMove();  //一直循环蛇移动地函数
	}		
		}		
	}
}





/**
  * @brief  用于检测按键是否被长时间按下
  * @param  无
  * @retval 1 ：按键被长时间按下  0 ：按键没有被长时间按下
  */
static uint8_t KEY2_LongPress(void)
{			
	uint8_t downCnt =0;																				//记录按下的次数
	uint8_t upCnt =0;																					//记录松开的次数			

	while(1)																										//死循环，由return结束
	{	
		Delay(0x2FFFF);																					//延迟一段时间再检测

		if(GPIO_ReadInputDataBit ( KEY2_GPIO_PORT, KEY2_GPIO_PIN ) == SET)	//检测到按下按键
		{
			downCnt++;																					//记录按下次数
			upCnt=0;																						//清除按键释放记录

			if(downCnt>=100)																		//按下时间足够
			{
				return 1; 																				//检测到按键被时间长按下
			}
		}
		else 
		{
			upCnt++; 																						//记录释放次数
			if(upCnt>5)																					//连续检测到释放超过5次
			{
				return 0;																				//按下时间太短，不是按键长按操作
			}
		}//	if(GPIO_ReadInputDataBit 
	}//while
}
/*********************************************END OF FILE**********************/


/* ------------------------------------------end of file---------------------------------------- */

