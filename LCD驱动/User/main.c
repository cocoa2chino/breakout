/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   LCD显示英文
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
#include "./lcd/bsp_ili9341_lcd.h"
#include "./usart/bsp_usart.h" 
#include <stdio.h>

static void LCD_Test(void);	
static void LCD_Test1(void);	
static void Delay ( __IO uint32_t nCount );
static void LCD_Rec(int x,int y);
static void LCD_begin(void);
void Printf_Charater(void)   ;


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{

	ILI9341_Init ();         //LCD 初始化

	USART_Config();		
	
	printf("\r\n ********** 液晶屏英文显示程序*********** \r\n"); 
	printf("\r\n 本程序不支持中文，显示中文的程序请学习下一章 \r\n"); 
	
 //其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
  ILI9341_GramScan ( 6 );

	while ( 1 )
	{
		LCD_Test1();
	}
	
	
}

//显示函数，x 上限为230 y 上限为200
void LCD_Rec(int x,int y)
{
  static uint8_t testCNT = 0;	
	char dispBuff[100];
  LCD_SetTextColor(YELLOW);
  ILI9341_DrawRectangle(x,y+100,20,20,1);
}
//清除函数
void LCD_Del(int x,int y)
{
  ILI9341_Clear(x,y+100,20,20);
}


void LCD_begin(void)
{
  static uint8_t testCNT = 0;	
	char dispBuff[100];

  testCNT++;
  /* 设置字体与颜色 */
  LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  ILI9341_DispStringLine_EN(LINE(0),"LCD Breakout Game");

  LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
  /*使用c标准库把变量转化成字符串*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(1));	/* 清除单行文字 */
	
	/*然后显示该字符串即可，其它变量也是这样处理*/
	ILI9341_DispStringLine_EN(LINE(1),dispBuff);

  
	/*******显示图形示例******/
	LCD_SetFont(&Font24x32);
  /* 画直线 */
  
  LCD_ClearLine(LINE(2));/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(2),"Draw line:");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(0,100,230,100);
	LCD_SetTextColor(BLUE);
	ILI9341_DrawLine(0,100,0,300);
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(0,300,230,300);
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(230,100,230,300);
  LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(50,200,100,30,1);
  LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(50,250,100,10,1);
  Delay(0xFFFFFF);
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

}

  /* 原文件中测试 */
void LCD_Test1(void)
{
  static uint8_t testCNT = 0;	
	char dispBuff[100];

  testCNT++;
  /* 设置字体与颜色 */
  LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  ILI9341_DispStringLine_EN(LINE(0),"LCD Breakout Game");

  LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
  /*使用c标准库把变量转化成字符串*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(1));	/* 清除单行文字 */
	
	/*然后显示该字符串即可，其它变量也是这样处理*/
	ILI9341_DispStringLine_EN(LINE(1),dispBuff);

  
	/*******显示图形示例******/
	LCD_SetFont(&Font24x32);
  /* 画直线 */
  
  LCD_ClearLine(LINE(2));/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(2),"Draw line:");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(0,100,230,100);
	LCD_SetTextColor(BLUE);
	ILI9341_DrawLine(0,100,0,300);
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(0,300,230,300);
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(230,100,230,300);
  LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(50,200,100,30,1);
  LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(50,250,100,10,1);
  LCD_Rec(2,1);
  Delay(0xFFFFFF);
  LCD_Del(2,1);
  Delay(0xFFFFFF);
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

}




/*用于测试各种液晶的函数*/
void LCD_Test(void)
{
	/*演示显示变量*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	/********显示字符串示例*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
  ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x320 px");
  ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD driver");
  ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
  
	/********显示变量示例*******/
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);

	/*使用c标准库把变量转化成字符串*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	
	/*然后显示该字符串即可，其它变量也是这样处理*/
	ILI9341_DispStringLine_EN(LINE(4),dispBuff);

	/*******显示图形示例******/
	LCD_SetFont(&Font24x32);
  /* 画直线 */
  
  LCD_ClearLine(LINE(4));/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw line:");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(50,170,210,230); 
	ILI9341_DrawLine(50,170,50,200); 
  ILI9341_DrawLine(50,200,210,240);
  
	LCD_SetTextColor(GREEN);
  ILI9341_DrawLine(100,170,200,230);  
  ILI9341_DrawLine(200,200,220,240);
	ILI9341_DrawLine(0,0,0,300);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(110,170,110,230);  
  ILI9341_DrawLine(130,200,220,240);
  
  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */
  
  
  /*画矩形*/

  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");

	LCD_SetTextColor(RED);
  ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(160,200,20,40,0);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawRectangle(170,200,50,20,1);
  
  
  Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

  /* 画圆 */
  LCD_ClearLine(LINE(4));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* 清屏，显示全黑 */

}


/**
  * @brief  简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

//球移动函数(下一时刻)
void BallMove(ball *bal)
{
  switch(bal->direct)
  {
    //当前朝右上
    case 0:
      if(/*碰到右边障碍*/)
      {
        if(/*碰到砖块*/)
        {
          bal->direct = 2;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
        else if(bal->pos_x + bal->radius >= 240 - WALL_WIDTH)
        {
          bal->direct = 2;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
      }
      else if(/*碰到上边障碍*/)
      {
        if(/*碰到墙壁*/)
        {
          bal->direct = 1;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 1;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else
      {
        bal->pos_x = bal->pos_x + bal->speed;
        bal->pos_y = bal->pos_y - bal->speed;
      }
      break;
    //当前朝右下
    case 1:
      if(/*碰到右边障碍*/)
      {
        if(/*碰到墙壁*/)
        {
          bal->direct = 3;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 3;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else if(/*碰到下边障碍*/)
      {
        if(/*碰到墙壁(板子)*/)
        {
          bal->direct = 0;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 0;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else if(/*碰到下边界*/)
      {
        /*
          游戏结束 or 重新开始
        */
      }
      else
      {
        bal->pos_x = bal->pos_x + bal->speed;
        bal->pos_y = bal->pos_y + bal->speed;
      }
      break;
    //当前朝左上
    case 2:
      if(/*碰到左边障碍*/)
      {
        if(/*碰到墙壁*/)
        {
          bal->direct = 0;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 0;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else if(/*碰到上边障碍*/)
      {
        if(/*碰到墙壁*/)
        {
          bal->direct = 3;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 3;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else
      {
        bal->pos_x = bal->pos_x - bal->speed;
        bal->pos_y = bal->pos_y - bal->speed;
      }
      break;
    //当前朝左下
    case 3:
      if(/*碰到左边障碍*/)
      {
        if(/*碰到墙壁*/)
        {
          bal->direct = 1;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 1;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else if(/*碰到下边障碍*/)
      {
        if(/*碰到墙壁(板子)*/)
        {
          bal->direct = 2;
          //再调用一次,保证一个时刻内球会移动
          BallMove(bal);
        }
        else if(/*碰到砖块*/)
        {
          bal->direct = 2;

          /*
            分数增加
            消除该砖块
          */

          BallMove(bal);
        }
      }
      else if(/*碰到下边界*/)
      {
        /*
          游戏结束 or 重新开始
        */
      }
      else
      {
        bal->pos_x = bal->pos_x - bal->speed;
        bal->pos_y = bal->pos_y + bal->speed;
      }
      break;
  }
}

/* ------------------------------------------end of file---------------------------------------- */

