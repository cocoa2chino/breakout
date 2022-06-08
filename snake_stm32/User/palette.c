/**
  ******************************************************************************
  * @file    palette.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��������Ӧ�ú���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32  F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "palette.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include <stdlib.h>
#include <stdio.h>
#include "bsp_adc.h"
#include "bsp_TiMbase.h"   

#define MAX_X	60
#define MAX_Y	20


#define DISPLAY_X_MAX 240     //��Ϸ��������ʾ����x�������ֵ
#define DISPLAY_Y_MAX 240     //��Ϸ��������ʾ����y�������ֵ
#define WALL_WIDTH    5       //ǽ���
#define BOARD_LENGTH  40      //���峤��
#define BOARD_WIDTH   3       //������
#define BLOCK_LENGTH  10      //ש�鳤��
#define BLOCK_WIDTH   8       //ש����

block *blk;                                             //ש������ͷָ��
uint16_t row = 2;                                   //����ש������
int pixel[40][40];    //״̬����,�洢������Ϣ
extern __IO uint16_t ADC_ConvertedValue;

board *brd;                                             //����ָ��
ball *bal;                                              //С��ָ��
/*��ť�ṹ������*/
Touch_Button button[BUTTON_NUM];
snake *head;//ͷָ��
snake *food1;//ͷָ��
uint8_t status = 'L';
uint8_t score;
uint8_t die = 0; //die == 0˵������
uint8_t MODE = 1; // MODE == 1˵����Ϸ���ڽ���

static void Draw_Mode_Button(void *btn);
static void Draw_Direction_Button(void *btn);

static void Command_Control_Direction(void *btn);
static void Command_Change_Mode(void *btn);  



//��ʱ����
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

/**
* @brief  Palette_Init ��Ϸ��ʼ��
* @param  ��
* @retval ��
*/
void Palette_Init(uint8_t LCD_Mode)
{
  
  uint8_t i;
	
	ADCx_Init();
	ILI9341_GramScan ( LCD_Mode );
	score = 0;
	status = 'L';
	
  /* ������Ϊ��ɫ */
	LCD_SetBackColor(CL_WHITE);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
	
	Touch_Button_Init();  
    Square_Init();
    GenWall();
	GenBlock();
    DelBlock(1,1);
  	DrawBlock();
    
    BoardInit();
  	BoardDraw();
    BallInit();
    BallDraw();
 	TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
  /* ��水ť */
  for(i=0;i<BUTTON_NUM;i++)
  {
    button[i].draw_btn(&button[i]);
  }

    

}



/**
* @brief  GenWall ������/��/�Ϸ�ǽ��
* @param  --
* @retval --
*/
void GenWall()
{
	int i;
	//LCD��ʾ
    ILI9341_DrawRectangle(0, 0, 6, 240, 1);
    ILI9341_DrawRectangle(240-6, 0, 6, 240, 1);
    ILI9341_DrawRectangle(6, 0, 240, 6, 1);
	for (i = 0; i < 40; i++)
    {
        pixel[0][i] = 1;
    }
	for (i = 0; i < 40; i++)
    {
        pixel[39][i] = 1;
    }
	for (i = 0; i < 40; i++)
    {
        pixel[i][0] = 1;
    }
}



/**
* @brief  GenBlock ��ʼ��ש��
* @param  blk ש������ָ������
* @param  row ����ש������
* @retval --
*/
void GenBlock()
{
    block *tmp;
	int i,j;
    //��ʼ����һ��ש��
    blk = (block*)malloc(sizeof(block));
    blk->pos_x = 1;
    blk->pos_y = 1;
    blk->prev = NULL;
    blk->next = NULL;

//����Ҫ���ɵ�ש��������ש�鳤��,��ʼ������ש��
    tmp = blk;
    for(i = 1; i <= row; i++)
    {
        for(j = 1; j < 39; j++)
        {
            tmp->next = (block*)malloc(sizeof(block));
            tmp->next->prev = tmp;
            tmp = tmp->next;
            tmp->pos_x = j;
            tmp->pos_y = i;
            pixel[tmp->pos_x][tmp->pos_y] = 1;
            tmp->next = NULL;
        }
   }
}



/**
* @brief  DrawBlock ש�����
* @param  blk ש������ͷָ������
* @retval --
*/
void DrawBlock()
{
    int i,j=0;
    LCD_SetColors(CL_GREEN,CL_WHITE);
    for(i=1;i<39;i++)
    {
        for(j=1;j<39;j++)
        if(pixel[i][j]==1)ILI9341_DrawRectangle(i*6, j*6, 5, 5, 1);
    }
}



/**
* @brief  DelBlock ש������
* @param  blki ש��ָ������
* @retval --
*/
void DelBlock(int x,int y)
{
    block *tmp = blk;
    ILI9341_Clear(x *6, y*6, 6,6);
    pixel[x][y] = 0;
    while(tmp->next != NULL)    /*��������*/
    {
        if(tmp->pos_x == x &&  tmp->pos_y == y )
        {
             if(tmp->prev == NULL)              /*ɾ�����������һ��*/
    {
        if(tmp->next == NULL)          /*ֻ��һ��ש��,��ͷָ����Ϊ��*/
        {
            pixel[tmp->pos_x][tmp->pos_y] = 0;
            free(blk);
            blk = NULL;
            return;
        }
        else                            /*��ͷָ����Ϊ�ڶ���ש��*/
        {
            pixel[tmp->pos_x][tmp->pos_y] = 0;
            blk = blk->next;
            blk->prev = NULL;
            free(tmp);
            return;
        }
    }
    else if(tmp->next == NULL)         /*ɾ�������������һ��*/
    {
        pixel[tmp->pos_x][tmp->pos_y] = 0;
        tmp->prev->next = NULL;
        free(tmp);
        return;
    }
    else
    {
        
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        pixel[tmp->pos_x][tmp->pos_y] = 0;
        free(tmp);
        return;
    }
        }
        tmp = tmp->next;
    } 
}



/**
* @brief  BallInit ��ʼ��С��
* @param  bal С��ָ������
* @retval --
*/
void BallInit()
{
    bal =	(ball*)malloc(sizeof(struct Ball));

    bal->pos_x = 20;
    bal->radius = 3;
    bal->pos_y = 20;
    bal->direct = 0;
    bal->speed = 1;
    return ;
}

/**
* @brief  BallRestart ʧ��֮�����¿�ʼ,С���ڵ�ǰ�����м�����
* @param  bal С��ָ������
* @param  pos_x �����е�x����
* @retval --
*/
void BallRestart()
{
    bal->pos_x = 20;
    bal->radius = 3;
    bal->pos_y = 20;
    bal->direct = 0;
    bal->speed = 1;
}

/**
* @brief  BallDraw С�����
* @param  bal С��ָ������
* @retval --
*/
void BallDraw()
{
    ILI9341_DrawCircle(bal->pos_x*6+3, bal->pos_y*6+3, bal->radius-0.5, 1);
}

void Play()
{
	BoardMove();
	BallMove();
    LCD_SetColors(CL_GREEN,CL_WHITE);
	/* ��Ϸ������Ϊ��ɫ */
	LCD_SetBackColor(CL_WHITE);
	//ILI9341_Clear(6, 6, DISPLAY_X_MAX-12, DISPLAY_Y_MAX-6);
	/* ������ʾ�µĻ��� */
	//GenWall();
	//DrawBlock();
	//BoardDraw();
	//BallDraw();
    Delay(0xfffff);
	
}


/**
* @brief  BallMove С���ƶ����̺���
* @param  bal С��ָ������
* @retval --
*/
void BallMove()
{
    char disbuff[20];
    uint16_t x = bal->pos_x;
    uint16_t y = bal->pos_y;
    ILI9341_Clear(bal->pos_x *6, bal->pos_y*6, 6,6);
    if(!MODE)   return;     /*��Ϸ��ͣ,С��λ�ò��仯*/

    
    switch(bal->direct)
    {
    //��ǰ������
    case 0:
        if(x+1 >= 39)     /*�����ұ�ǽ��*/
        {
            bal->direct = 2;
            //�ٵ���һ��,��֤һ��ʱ��������ƶ�
            BallMove();
        }
        else if(y-1 <= 0)                /*��������ǽ��*/
        {
            bal->direct = 1;
            BallMove();
        }
        else if(pixel[x+1][y])                    /*�����ұ�ש��*/
        {
            bal->direct = 2;
            DelBlock(x + 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y-1])                    /*��������ש��*/
        {
            bal->direct = 1;
            DelBlock( x, y - 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x+1][y-1])                    /*����������ש��*/
        {
            bal->direct = 3;
            DelBlock( x+1, y - 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else
        {
            bal->pos_x = bal->pos_x + bal->speed;
            bal->pos_y = bal->pos_y - bal->speed;
        }
        break;
    //��ǰ������
    case 1:
        if(x + 1 >= 39)                 /*�����ұ�ǽ��*/
        {
            bal->direct = 3;
            BallMove();
        }
        else if(y + 1 >= 39)           /*�������浲��*/
        {
            if((brd->pos_x<=x)&&(brd->pos_x+8>=x))
            {
                bal->direct = 0;
                BallMove();
            }
            else
            {
                die=1;
            }
        }
        else if(pixel[x+1][y])                                /*�����ұ�ש��*/
        {
            bal->direct = 3;
            DelBlock( x + 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y+1])                                /*��������ש��*/
        {
            bal->direct = 0;
            DelBlock( x, y + 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x+1][y+1])                    /*����������ש��*/
        {
            bal->direct = 2;
            DelBlock( x+1, y +1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else
        {
            bal->pos_x = bal->pos_x + bal->speed;
            bal->pos_y = bal->pos_y + bal->speed;
        }
        break;
    //��ǰ������
    case 2:
        if(x - 1 <=0)                     /*�������ǽ��*/
        {
            bal->direct = 0;
            //�ٵ���һ��,��֤һ��ʱ��������ƶ�
            BallMove();
        }
        else if(y - 1 <= 0)                /*��������ǽ��*/
        {
            bal->direct = 3;
            BallMove();
        }
        else if(pixel[x - 1][y])                    /*�������ש��*/
        {
            bal->direct = 0;
            DelBlock( x - 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y-1])                    /*��������ש��*/
        {
            bal->direct = 3;
            DelBlock( x, y - 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x-1][y-1])                    /*����������ש��*/
        {
            bal->direct = 1;
            DelBlock( x-1, y -1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else
        {
            bal->pos_x = bal->pos_x - bal->speed;
            bal->pos_y = bal->pos_y - bal->speed;
        }
        break;
    //��ǰ������
    case 3:
        if(x - 1 <=0)                                 /*�������ǽ��*/
        {
            bal->direct = 1;
            BallMove();
        }
       else if(y + 1 >= 39)           /*�������浲��*/
        {
            if((brd->pos_x<=x)&&(brd->pos_x+8>=x))
            {
                bal->direct = 2;
                BallMove();
            }
            else
            {
                die=1;
            }
        }
        else if(pixel[x-1][y])                                /*�������ש��*/
        {
            bal->direct = 1;
            DelBlock( x - 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y+1])                                /*��������ש��*/
        {
            bal->direct = 2;
            DelBlock( x, y + 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x-1][y+1])                    /*����������ש��*/
        {
            bal->direct = 2;
            DelBlock( x-1, y +1);
            score++;
            sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else
        {
            bal->pos_x = bal->pos_x - bal->speed;
            bal->pos_y = bal->pos_y + bal->speed;
        }
        break;
    }
    ILI9341_DrawCircle(bal->pos_x*6+3, bal->pos_y*6+3, bal->radius-0.5, 1);
}



void BoardInit(void)
{
    brd = (board*)malloc(sizeof(struct Board));
    brd->pos_x = 18;
    brd->pos_y = 39;
    brd->speed = 3;
}

/**
* @brief  BoardMove �����ƶ�
* @param  brd ����ָ������
* @retval --
*/
void BoardMove()
{
    
    if(MODE && status == 'L')                                               /*��Ϸ���ڽ����������������*/
    {
        if(brd->pos_x > 1)                      /*û���������ǽ��*/
        {
            LCD_SetColors(CL_GREEN,CL_WHITE);
            ILI9341_Clear(brd->pos_x *6+48, brd->pos_y*6, 6,6);
            brd->pos_x = brd->pos_x - 1;
            ILI9341_DrawRectangle(brd->pos_x *6, brd->pos_y*6, 6, 6, 1);
        }
    }
    else if(MODE && status == 'R')                                          /*��Ϸ���ڽ������ҷ����������*/
    {
        if(brd->pos_x + 10 < 40)      /*û�������Ҳ�ǽ��*/
        {
            LCD_SetColors(CL_GREEN,CL_WHITE);
            ILI9341_Clear(brd->pos_x *6, brd->pos_y*6, 6,6);
            brd->pos_x = brd->pos_x + 1;
            ILI9341_DrawRectangle(brd->pos_x *6+48, brd->pos_y*6, 6, 6, 1);
        }
    }

    if(die == 1)    //������˾�����
	{
		LCD_SetColors(CL_RED,CL_WHITE);
		LCD_SetFont(&Font24x32);
		ILI9341_DispString_EN_CH(12,120,"Game over");
		LCD_SetFont(&Font8x16);
		ILI9341_DispString_EN_CH(64,157,"������⴦��ʼ");
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);  //��ʹ���жϣ���������5������
		while(XPT2046_TouchDetect() != TOUCH_PRESSED);//������λ�����³�ʼ��
		free(blk);
        free(brd);
        free(bal);
        free(pixel);
		Palette_Init(6);  
		die = 0;
	}
}

/**
* @brief  BoardDraw �������
* @param  brd ����ָ������
* @retval --
*/
void BoardDraw()
{
    int i=0;
	//LCD��ʾ
    ILI9341_DrawRectangle(brd->pos_x *6, brd->pos_y*6, 48, 6, 1);
	for(i=0;i<8;i++){
		pixel[brd->pos_x+i][brd->pos_y] = 1;
	}
    //����״̬����
}


/**
* @brief  Touch_Button_Init ��ʼ�������Ϸ����
* @param  ��
* @retval ��
*/
void Square_Init(void)
{
	LCD_SetColors(CL_BLUE,CL_WHITE);
	ILI9341_DrawLine(0,240,240,240);
	//������ʾ�����ĸ���
	LCD_SetColors(CL_WHITE,CL_WHITE);
  ILI9341_DrawRectangle(BUTTON_START_X + COLOR_BLOCK_WIDTH*2,
														320-COLOR_BLOCK_HEIGHT*2,
														COLOR_BLOCK_WIDTH,
														COLOR_BLOCK_HEIGHT,1);
	LCD_SetColors(CL_BLACK,CL_WHITE);
	LCD_SetFont(&Font8x16);
  ILI9341_DispString_EN_CH(162, 253, "Score:0");
	LCD_SetColors(CL_BLUE4,CL_WHITE);
  ILI9341_DrawRectangle(BUTTON_START_X + COLOR_BLOCK_WIDTH*2,
														320-COLOR_BLOCK_HEIGHT*2,
														COLOR_BLOCK_WIDTH,
														COLOR_BLOCK_HEIGHT,0);
}


/**
* @brief  Touch_Button_Init ��ʼ����ť����
* @param  ��
* @retval ��
*/
void Touch_Button_Init(void)
{
  /*�����*/
  button[0].start_x = BUTTON_START_X;
  button[0].start_y = 320-COLOR_BLOCK_HEIGHT;
  button[0].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
  button[0].end_y = 320;
  button[0].para = LEFT;
  button[0].touch_flag = 0;  
  button[0].draw_btn = Draw_Direction_Button;   
  button[0].btn_command = Command_Control_Direction ;  
  
  button[1].start_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
  button[1].start_y = 320-COLOR_BLOCK_HEIGHT;
  button[1].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH*2 ;
  button[1].end_y = 320;
  button[1].para = DOWN;
  button[1].touch_flag = 0;  
  button[1].draw_btn = Draw_Direction_Button;
  button[1].btn_command = Command_Control_Direction ;
  
  button[2].start_x = BUTTON_START_X + COLOR_BLOCK_WIDTH*2;
  button[2].start_y = 320-COLOR_BLOCK_HEIGHT;
  button[2].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH*3;
  button[2].end_y = 320;
  button[2].para = RIGHT;
  button[2].touch_flag = 0;  
  button[2].draw_btn = Draw_Direction_Button;
  button[2].btn_command = Command_Control_Direction ;
 
  button[3].start_x = BUTTON_START_X + COLOR_BLOCK_WIDTH;
  button[3].start_y = 320-COLOR_BLOCK_HEIGHT*2;
  button[3].end_x = BUTTON_START_X+COLOR_BLOCK_WIDTH*2;
  button[3].end_y = 320-COLOR_BLOCK_HEIGHT;
  button[3].para = UP;
  button[3].touch_flag = 0;  
  button[3].draw_btn = Draw_Direction_Button;
  button[3].btn_command = Command_Control_Direction ;
//�����ǿ�ʼ����ͣ��  
  button[4].start_x = BUTTON_START_X;
  button[4].start_y = 320-COLOR_BLOCK_HEIGHT*2;
  button[4].end_x = BUTTON_START_X+COLOR_BLOCK_WIDTH ;
  button[4].end_y = 320-COLOR_BLOCK_HEIGHT;
  button[4].para = 0;
  button[4].touch_flag = 0;  
  button[4].draw_btn = Draw_Mode_Button ;
  button[4].btn_command = Command_Change_Mode;

}

/**
* @brief  Touch_Button_Down ����������ʱ���õĺ������ɴ���������
* @param  x ����λ�õ�x����
* @param  y ����λ�õ�y����
* @retval ��
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
  uint8_t i;
  for(i=0;i<BUTTON_NUM;i++)
  {
    
    /* �������˰�ť */
    if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x )
    {
      if(button[i].touch_flag == 0)     /*ԭ����״̬Ϊû�а��£������״̬*/
      {
      button[i].touch_flag = 1;         /* ��¼���±�־ */
      
      button[i].draw_btn(&button[i]);  /*�ػ水ť*/
      }        
      
    }
    else if(button[i].touch_flag == 1) /* �����Ƴ��˰����ķ�Χ��֮ǰ�а��°�ť */
    {
      button[i].touch_flag = 0;         /* ������±�־���ж�Ϊ�����*/
      
      button[i].draw_btn(&button[i]);   /*�ػ水ť*/
    }

  }

}

/**
* @brief  Touch_Button_Up �������ͷ�ʱ���õĺ������ɴ���������
* @param  x ��������ͷ�ʱ��x����
* @param  y ��������ͷ�ʱ��y����
* @retval ��
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
   uint8_t i; 
   for(i=0;i<BUTTON_NUM;i++)
   {
     
     
     /* �����ڰ�ť�����ͷ� */
      if((x<button[i].end_x && x>button[i].start_x && y<button[i].end_y && y>button[i].start_y))
      {        
        button[i].touch_flag = 0;       /*�ͷŴ�����־*/
        
        button[i].btn_command(&button[i]);  /*ִ�а����Ĺ�������*/
				
        button[i].draw_btn(&button[i]); /*�ػ水ť*/
        break;
      }
    }  

}

/**
* @brief  Draw_Direction_Button ���������
* @param  &button[i]
* @retval ��
*/
static void Draw_Direction_Button(void *btn)
{
  Touch_Button *ptr = (Touch_Button *)btn;
  uint16_t RGB;

	if(ptr->touch_flag == 0)
  {
	
		LCD_SetColors(CL_BUTTON_GREY,CL_WHITE);
    ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
    RGB = CL_BUTTON_GREY;
  }
  else  /*��������*/
  {
		
		LCD_SetColors(CL_WHITE,CL_WHITE);
    ILI9341_DrawRectangle(ptr->start_x,
														ptr->start_y,
														ptr->end_x - ptr->start_x,
														ptr->end_y - ptr->start_y,1);
    
		RGB = CL_WHITE;

  } 
  LCD_SetColors(CL_BLUE4,CL_WHITE);
  ILI9341_DrawRectangle(ptr->start_x,
														ptr->start_y,
														ptr->end_x - ptr->start_x,
														ptr->end_y - ptr->start_y,0);
     
  LCD_SetColors(CL_BLACK,RGB);
	LCD_SetFont(&Font8x16);

  /*���ݰ�������д��*/
	switch(ptr->para)
  {
    		
		case LEFT:      
				LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"��");
      
      break;   
      
    case DOWN:
        LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"��");
    
      break;
        
    case RIGHT:
				LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"��");
 
      break;
    
		case UP:
        LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"��");
    
      break;
		
  }
  
}

/**
* @brief  Draw_Mode_Button ģʽ������
* @param  &button[i]
* @retval ��
*/
static void Draw_Mode_Button(void *btn)
{
  Touch_Button *ptr = (Touch_Button *)btn;
  uint16_t RGB;

	if(ptr->touch_flag == 0)
  {
	
		LCD_SetColors(CL_BUTTON_GREY,CL_WHITE);
    ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
    RGB = CL_BUTTON_GREY;
  }
  else  /*��������*/
  {
		
		LCD_SetColors(CL_WHITE,CL_WHITE);
    ILI9341_DrawRectangle(ptr->start_x,
														ptr->start_y,
														ptr->end_x - ptr->start_x,
														ptr->end_y - ptr->start_y,1);
    
		RGB = CL_WHITE;

  } 
  LCD_SetColors(CL_BLUE4,CL_WHITE);
  ILI9341_DrawRectangle(ptr->start_x,
														ptr->start_y,
														ptr->end_x - ptr->start_x,
														ptr->end_y - ptr->start_y,0);
     
  LCD_SetColors(CL_BLACK,RGB);
	LCD_SetFont(&Font8x16);
	if(MODE)
	{
		ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16*2)/2,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"��ͣ");
	}
	else
	{
		ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16*2)/2,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"����");
	}
}

/**
* @brief  Command_Change_Mode ������Ϸ����ͣ�����
* @param  btn Touch_Button ���͵İ�������
* @retval ��
*/
static void Command_Change_Mode(void *btn)
{
	MODE = !MODE;
}

/**
* @brief  Command_Control_Direction �˶�������ִ�к���
* @param  btn Touch_Button ���͵İ�������
* @retval ��
*/
static void Command_Control_Direction(void *btn)
{
  Touch_Button *ptr = (Touch_Button *)btn;

	if(ptr->para==UP && status!='D')
		status='U';
	if(ptr->para==DOWN && status!='U')
		status='D';
	if(ptr->para==LEFT )
		status='L';
	if(ptr->para==RIGHT)
		status='R';
}



#define ABS(X)  ((X) > 0 ? (X) : -(X))





/* ------------------------------------------end of file---------------------------------------- */



