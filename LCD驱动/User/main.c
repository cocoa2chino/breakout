/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   LCD��ʾӢ��
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
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{

	ILI9341_Init ();         //LCD ��ʼ��

	USART_Config();		
	
	printf("\r\n ********** Һ����Ӣ����ʾ����*********** \r\n"); 
	printf("\r\n ������֧�����ģ���ʾ���ĵĳ�����ѧϰ��һ�� \r\n"); 
	
 //����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  ILI9341_GramScan ( 6 );

	while ( 1 )
	{
		LCD_Test1();
	}
	
	
}

//��ʾ������x ����Ϊ230 y ����Ϊ200
void LCD_Rec(int x,int y)
{
  static uint8_t testCNT = 0;	
	char dispBuff[100];
  LCD_SetTextColor(YELLOW);
  ILI9341_DrawRectangle(x,y+100,20,20,1);
}
//�������
void LCD_Del(int x,int y)
{
  ILI9341_Clear(x,y+100,20,20);
}


void LCD_begin(void)
{
  static uint8_t testCNT = 0;	
	char dispBuff[100];

  testCNT++;
  /* ������������ɫ */
  LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  ILI9341_DispStringLine_EN(LINE(0),"LCD Breakout Game");

  LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
  /*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(1));	/* ����������� */
	
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	ILI9341_DispStringLine_EN(LINE(1),dispBuff);

  
	/*******��ʾͼ��ʾ��******/
	LCD_SetFont(&Font24x32);
  /* ��ֱ�� */
  
  LCD_ClearLine(LINE(2));/* ����������� */
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
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

}

  /* ԭ�ļ��в��� */
void LCD_Test1(void)
{
  static uint8_t testCNT = 0;	
	char dispBuff[100];

  testCNT++;
  /* ������������ɫ */
  LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  ILI9341_DispStringLine_EN(LINE(0),"LCD Breakout Game");

  LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);
  /*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(1));	/* ����������� */
	
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	ILI9341_DispStringLine_EN(LINE(1),dispBuff);

  
	/*******��ʾͼ��ʾ��******/
	LCD_SetFont(&Font24x32);
  /* ��ֱ�� */
  
  LCD_ClearLine(LINE(2));/* ����������� */
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
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

}




/*���ڲ��Ը���Һ���ĺ���*/
void LCD_Test(void)
{
	/*��ʾ��ʾ����*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	/********��ʾ�ַ���ʾ��*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH 3.2 inch LCD para:");
  ILI9341_DispStringLine_EN(LINE(1),"Image resolution:240x320 px");
  ILI9341_DispStringLine_EN(LINE(2),"ILI9341 LCD driver");
  ILI9341_DispStringLine_EN(LINE(3),"XPT2046 Touch Pad driver");
  
	/********��ʾ����ʾ��*******/
	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(GREEN);

	/*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"Count : %d ",testCNT);
  LCD_ClearLine(LINE(4));	/* ����������� */
	
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	ILI9341_DispStringLine_EN(LINE(4),dispBuff);

	/*******��ʾͼ��ʾ��******/
	LCD_SetFont(&Font24x32);
  /* ��ֱ�� */
  
  LCD_ClearLine(LINE(4));/* ����������� */
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
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
  
  
  /*������*/

  LCD_ClearLine(LINE(4));	/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN(LINE(4),"Draw Rect:");

	LCD_SetTextColor(RED);
  ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(160,200,20,40,0);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawRectangle(170,200,50,20,1);
  
  
  Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

  /* ��Բ */
  LCD_ClearLine(LINE(4));	/* ����������� */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN(LINE(4),"Draw Cir:");

	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

}


/**
  * @brief  ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

//���ƶ�����(��һʱ��)
void BallMove(ball *bal)
{
  switch(bal->direct)
  {
    //��ǰ������
    case 0:
      if(/*�����ұ��ϰ�*/)
      {
        if(/*����ש��*/)
        {
          bal->direct = 2;

          /*
            ��������
            ������ש��
          */

          BallMove(bal);
        }
        else if(bal->pos_x + bal->radius >= 240 - WALL_WIDTH)
        {
          bal->direct = 2;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
      }
      else if(/*�����ϱ��ϰ�*/)
      {
        if(/*����ǽ��*/)
        {
          bal->direct = 1;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 1;

          /*
            ��������
            ������ש��
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
    //��ǰ������
    case 1:
      if(/*�����ұ��ϰ�*/)
      {
        if(/*����ǽ��*/)
        {
          bal->direct = 3;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 3;

          /*
            ��������
            ������ש��
          */

          BallMove(bal);
        }
      }
      else if(/*�����±��ϰ�*/)
      {
        if(/*����ǽ��(����)*/)
        {
          bal->direct = 0;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 0;

          /*
            ��������
            ������ש��
          */

          BallMove(bal);
        }
      }
      else if(/*�����±߽�*/)
      {
        /*
          ��Ϸ���� or ���¿�ʼ
        */
      }
      else
      {
        bal->pos_x = bal->pos_x + bal->speed;
        bal->pos_y = bal->pos_y + bal->speed;
      }
      break;
    //��ǰ������
    case 2:
      if(/*��������ϰ�*/)
      {
        if(/*����ǽ��*/)
        {
          bal->direct = 0;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 0;

          /*
            ��������
            ������ש��
          */

          BallMove(bal);
        }
      }
      else if(/*�����ϱ��ϰ�*/)
      {
        if(/*����ǽ��*/)
        {
          bal->direct = 3;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 3;

          /*
            ��������
            ������ש��
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
    //��ǰ������
    case 3:
      if(/*��������ϰ�*/)
      {
        if(/*����ǽ��*/)
        {
          bal->direct = 1;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 1;

          /*
            ��������
            ������ש��
          */

          BallMove(bal);
        }
      }
      else if(/*�����±��ϰ�*/)
      {
        if(/*����ǽ��(����)*/)
        {
          bal->direct = 2;
          //�ٵ���һ��,��֤һ��ʱ��������ƶ�
          BallMove(bal);
        }
        else if(/*����ש��*/)
        {
          bal->direct = 2;

          /*
            ��������
            ������ש��
          */

          BallMove(bal);
        }
      }
      else if(/*�����±߽�*/)
      {
        /*
          ��Ϸ���� or ���¿�ʼ
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

