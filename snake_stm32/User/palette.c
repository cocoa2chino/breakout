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

extern __IO uint16_t ADC_ConvertedValue;
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
	Snake_Init();
	
  TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
  /* ��水ť */
  for(i=0;i<BUTTON_NUM;i++)
  {
    button[i].draw_btn(&button[i]);
  }
	creatFood();
 
}


/**
* @brief  Snake_Init �����ʼ��������һ������4
* @param  ��
* @retval ��
*/
void Snake_Init(void)//
{
	int i;
	snake *tail;//βָ��
	tail =( snake*)malloc(sizeof(snake));//��һ���ڵ�/ͷ���
	tail->x = 30;//2�ı�������Ϊ����ĳ���������λ
	tail->y = 10;//1����λ
	tail->next=NULL;
	for(i=1;i<=4;i++)//β�巨
	{
		head=(snake*)malloc(sizeof(snake));//����һ���ڵ�
		head->next=tail;//���ӳ���
		head->x = 30-i;//��һ���ڵ��λ��
		head->y = 10;
		tail=head;
	} 
	//������ӡ����
	while(tail->next!=NULL)
	{
		LCD_SetColors(CL_GREEN,CL_WHITE);
		ILI9341_DrawRectangle(tail->x*6,tail->y*6,6,6,1);
		tail = tail->next;
	}
}

/**
* @brief snakeMove ���ƶ����̺��������Ĳ���
* @param  ��
* @retval ��
*/
void snakeMove(void)
{
	char disbuff[20];
	snake *nexthead;
	snake *p,*q;
	nexthead=(snake*)malloc(sizeof(snake));
	if (die == 0 && MODE==1)
	{
		if(status=='R')//������
		{
			nexthead->x = head->x+1;
			nexthead->y = head->y;
		}
		else if(status=='L')//������
		{ 
			nexthead->x = head->x-1;
			nexthead->y = head->y;
		}
		else if(status=='U')//������
		{
			nexthead->x = head->x;
			nexthead->y = head->y-1;
		}
		else if(status=='D')//������
		{
			nexthead->x = head->x;
			nexthead->y = head->y+1;
		}
		LCD_SetColors(CL_GREEN,CL_WHITE);
		if(nexthead->x==food1->x && nexthead->y==food1->y)//�Ե���ʳ��
		{
			nexthead->next=head;
			head=nexthead;
			p=head;//p������ͷ��������ӡ����
			while(p->next!=NULL)
			{
				ILI9341_DrawRectangle(p->x*6,p->y*6,6,6,1);
				p=p->next;
			}//�Ե���ʳ��ô���
			score=score+1;
			sprintf(disbuff,"Score:%d",score);//��ӡ�ɼ�
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
			creatFood();
		}
		else//û��ʳ��
		{
			nexthead->next=head;
			head=nexthead;
			p=head;//p������ͷ��������ӡ����
			while(p->next->next!=NULL)
			{
				ILI9341_DrawRectangle(p->x*6,p->y*6,6,6,1);
				p=p->next;
			}
			LCD_SetColors(CL_WHITE,CL_WHITE);
			ILI9341_DrawRectangle(p->x*6,p->y*6,6,6,1);
			free(p->next);
			p->next=NULL;
		}
	}

	
	judgeAlive(); //�жϴ�����
	if(die == 1)    //������˾�����
	{
		LCD_SetColors(CL_RED,CL_WHITE);
		LCD_SetFont(&Font24x32);
		ILI9341_DispString_EN_CH(12,120,"Game over");
		LCD_SetFont(&Font8x16);
		ILI9341_DispString_EN_CH(64,157,"������⴦��ʼ");
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);  //��ʹ���жϣ���������5������
		while(XPT2046_TouchDetect() != TOUCH_PRESSED);//������λ�����³�ʼ��
		p = q = head;
		while(p != NULL)   //�ͷ��ڴ�
		{
			p = q->next;
			free(q);
			q = p; 
		}
		free(p);	
		Palette_Init(6);  
		die = 0;
	}
	
	Delay(0xfffff);//���Ե������ƶ����ٶȣ������Ǻ��룬Խ���ٶ�Խ��
}


/**
* @brief judgeAlive �ж��ߵ�����
* @param  ��
* @retval ��
*/
void judgeAlive(void)
{
	snake *q;
	if(head->x==-1||head->y==-1||head->x==40||head->y==40)//����ǽ
		die=1;
	q=head->next;
	while(q!=NULL)   // �Ե��Լ�
	{
		if(q->x==head->x && head->y==q->y)
			die=1;
		q=q->next;
	};
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
* @brief  creatFood �������һ�����겢����ʳ��
* @param  ��
* @retval ��
*/
void creatFood(void)
{
	snake *food;//����һ��ʳ��
	snake *p;
  food=(snake*)malloc(sizeof(snake));
	srand(ADC_ConvertedValue);//����ADC�仯��������һ�����ӣ��ͻ�õ�û���ɵ�ʳ��
	food->x = rand()%39;
	food->y = rand()%39;
	p = head;//��p������
  while(p->next!=NULL)//���ʳ��������߱���
	{
	    if(food->x==p->x&&food->y==p->y)
			{
				free(food);
				creatFood();
			}
			p=p->next;
	}
	LCD_SetColors(CL_RED,CL_WHITE);
	ILI9341_DrawRectangle(food->x*6,food->y*6,6,6,1);
	food1=food;//food1������ǵ�����

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
	if(ptr->para==LEFT && status!='R')
		status='L';
	if(ptr->para==RIGHT && status!='L')
		status='R';
}




/* ------------------------------------------end of file---------------------------------------- */



