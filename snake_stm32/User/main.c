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
#include "palette.h"
#include <string.h>
#include "bsp_TiMbase.h" 



int main(void)
{		

	//LCD 初始化
	ILI9341_Init();  
	
	//触摸屏初始化
	XPT2046_Init();
	//从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
	Calibrate_or_Get_TouchParaWithFlash(6,0);
	
	BASIC_TIM_Init();

//	USART_Config();  
	LED_GPIO_Config();
	
 
	//绘制触游戏界面
	Palette_Init(LCD_SCAN_MODE);
 
	while ( 1 )
	{
		snakeMove();  //一直循环蛇移动地函数
	}
		
}





/* ------------------------------------------end of file---------------------------------------- */

