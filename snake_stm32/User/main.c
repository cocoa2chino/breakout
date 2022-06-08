/**
  ******************************************************************************
  * @file    main.c
  * @author  mr
  * @version V1.0
  * @date    2021-02-28
  * @brief   ̰������Ϸ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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

	//LCD ��ʼ��
	ILI9341_Init();  
	
	//��������ʼ��
	XPT2046_Init();
	//��FLASH���ȡУ����������FLASH�޲�������ʹ��ģʽ3����У��
	Calibrate_or_Get_TouchParaWithFlash(6,0);
	
	BASIC_TIM_Init();

//	USART_Config();  
	LED_GPIO_Config();
	
 
	//���ƴ���Ϸ����
	Palette_Init(LCD_SCAN_MODE);
 
	while ( 1 )
	{
		//Play();
    //snakeMove();  //һֱѭ�����ƶ��غ���
	}
		
}





/* ------------------------------------------end of file---------------------------------------- */

