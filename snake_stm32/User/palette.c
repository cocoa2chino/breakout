/**
  ******************************************************************************
  * @file    palette.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   触摸画板应用函数
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32  F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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


#define DISPLAY_X_MAX 240     //游戏主界面显示区域x坐标最大值
#define DISPLAY_Y_MAX 240     //游戏主界面显示区域y坐标最大值
#define WALL_WIDTH    5       //墙厚度
#define BOARD_LENGTH  40      //挡板长度
#define BOARD_WIDTH   3       //挡板厚度
#define BLOCK_LENGTH  10      //砖块长度
#define BLOCK_WIDTH   8       //砖块宽度

block *blk;                                             //砖块链表头指针
uint16_t row = 2;                                   //生成砖块行数
int pixel[40][40];    //状态数组,存储像素信息
extern __IO uint16_t ADC_ConvertedValue;

board *brd;                                             //挡板指针
ball *bal;                                              //小球指针
/*按钮结构体数组*/
Touch_Button button[BUTTON_NUM];
snake *head;//头指针
snake *food1;//头指针
uint8_t status = 'L';
uint8_t score;
uint8_t die = 0; //die == 0说明活着
uint8_t MODE = 1; // MODE == 1说明游戏正在进行

static void Draw_Mode_Button(void *btn);
static void Draw_Direction_Button(void *btn);

static void Command_Control_Direction(void *btn);
static void Command_Change_Mode(void *btn);  



//延时函数
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

/**
* @brief  Palette_Init 游戏初始化
* @param  无
* @retval 无
*/
void Palette_Init(uint8_t LCD_Mode)
{
  
  uint8_t i;
	
	ADCx_Init();
	ILI9341_GramScan ( LCD_Mode );
	score = 0;
	status = 'L';
	
  /* 整屏清为白色 */
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
  /* 描绘按钮 */
  for(i=0;i<BUTTON_NUM;i++)
  {
    button[i].draw_btn(&button[i]);
  }

    

}



/**
* @brief  Palette_Init1 游戏初始化
* @param  无
* @retval 无
*/
void Palette_Init1(uint8_t LCD_Mode)
{

  uint8_t i;

	ADCx_Init();
	ILI9341_GramScan ( LCD_Mode );
	score = 0;
	status = 'L';

  /* 整屏清为白色 */
	LCD_SetBackColor(CL_WHITE);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	

	Touch_Button_Init();  
  Square_Init();
	Snake_Init();

  TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);
  /* 描绘按钮 */
  for(i=0;i<BUTTON_NUM;i++)
  {
    button[i].draw_btn(&button[i]);
  }
	creatFood();

}



/**
* @brief  Snake_Init 蛇身初始化，给定一个长度4
* @param  无
* @retval 无
*/
void Snake_Init(void)//
{
	int i;
	snake *tail;//尾指针
	tail =( snake*)malloc(sizeof(snake));//第一个节点/头结点
	tail->x = 30;//2的倍数，因为方块的长是两个单位
	tail->y = 10;//1个单位
	tail->next=NULL;
	for(i=1;i<=4;i++)//尾插法
	{
		head=(snake*)malloc(sizeof(snake));//申请一个节点
		head->next=tail;//连接成链
		head->x = 30-i;//下一个节点的位置
		head->y = 10;
		tail=head;
	} 
	//遍历打印出来
	while(tail->next!=NULL)
	{
		LCD_SetColors(CL_GREEN,CL_WHITE);
		ILI9341_DrawRectangle(tail->x*6,tail->y*6,6,6,1);
		tail = tail->next;
	}
}



/**
* @brief snakeMove 蛇移动过程函数，核心部分
* @param  无
* @retval 无
*/
void snakeMove(void)
{
	char disbuff[20];
	snake *nexthead;
	snake *p,*q;
	nexthead=(snake*)malloc(sizeof(snake));
	if (die == 0 && MODE==1)
	{
		if(status=='R')//向右走
		{
			nexthead->x = head->x+1;
			nexthead->y = head->y;
		}
		else if(status=='L')//向左走
		{ 
			nexthead->x = head->x-1;
			nexthead->y = head->y;
		}
		else if(status=='U')//向上走
		{
			nexthead->x = head->x;
			nexthead->y = head->y-1;
		}
		else if(status=='D')//向下走
		{
			nexthead->x = head->x;
			nexthead->y = head->y+1;
		}
		LCD_SetColors(CL_GREEN,CL_WHITE);
		if(nexthead->x==food1->x && nexthead->y==food1->y)//吃掉了食物
		{
			nexthead->next=head;
			head=nexthead;
			p=head;//p用来从头遍历，打印方块
			while(p->next!=NULL)
			{
				ILI9341_DrawRectangle(p->x*6,p->y*6,6,6,1);
				p=p->next;
			}//吃掉了食物得创造
			score=score+1;
			sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
			creatFood();
		}
		else//没有食物
		{
			nexthead->next=head;
			head=nexthead;
			p=head;//p用来从头遍历，打印方块
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

	
	judgeAlive(); //判断存活情况
	if(die == 1)    //如果死了就重启
	{
		LCD_SetColors(CL_RED,CL_WHITE);
		LCD_SetFont(&Font24x32);
		ILI9341_DispString_EN_CH(12,120,"Game over");
		LCD_SetFont(&Font8x16);
		ILI9341_DispString_EN_CH(64,157,"点击任意处开始");
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);  //不使能中断，即不是能5个按键
		while(XPT2046_TouchDetect() != TOUCH_PRESSED);//按任意位置重新初始化
		p = q = head;
		while(p != NULL)   //释放内存
		{
			p = q->next;
			free(q);
			q = p; 
		}
		free(p);	
		Palette_Init1(6);  
		die = 0;
	}
	
	Delay(0xfffff);//可以调节蛇移动的速度，里面是毫秒，越大速度越慢
}




/**
* @brief judgeAlive 判断蛇的死活
* @param  无
* @retval 无
*/
void judgeAlive(void)
{
	snake *q;
	if(head->x==-1||head->y==-1||head->x==40||head->y==40)//碰到墙
		die=1;
	q=head->next;
	while(q!=NULL)   // 吃到自己
	{
		if(q->x==head->x && head->y==q->y)
			die=1;
		q=q->next;
	};
}

/**
* @brief  creatFood 随机生成一个坐标并绘制食物
* @param  无
* @retval 无
*/
void creatFood(void)
{
	snake *food;//创造一个食物
	snake *p;
  food=(snake*)malloc(sizeof(snake));
	srand(ADC_ConvertedValue);//随着ADC变化，产生不一样种子，就会得到没规律的食物
	food->x = rand()%39;
	food->y = rand()%39;
	p = head;//用p来遍历
  while(p->next!=NULL)//解决食物出现在蛇本身
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
	food1=food;//food1用来标记的作用

}


/**
* @brief  GenWall 生成左/右/上方墙壁
* @param  --
* @retval --
*/
void GenWall()
{
	int i;
	//LCD显示
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
* @brief  GenBlock 初始化砖块
* @param  --
* @retval --
*/
void GenBlock()
{
	int i,j;
    //初始化第一个砖块
    for(i = 1; i <= row; i++)
    {
        for(j = 1; j < 39; j++)
        {
            pixel[j][i] = 1;
        }
   }
}



/**
* @brief  DrawBlock 砖块绘制
* @param  --
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
* @brief  DelBlock 砖块消除
* @param  x 横坐标
* @param  y 纵坐标
* @retval --
*/
void DelBlock(int x,int y)
{
    ILI9341_Clear(x *6, y*6, 6,6);
    pixel[x][y] = 0;
}



/**
* @brief  BallInit 初始化小球
* @param  --
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
* @brief  BallRestart 失败之后重新开始,小球在当前挡板中间生成
* @param  --
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
* @brief  BallDraw 小球绘制
* @param  --
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
	/* 游戏区域清为白色 */
	LCD_SetBackColor(CL_WHITE);
	//ILI9341_Clear(6, 6, DISPLAY_X_MAX-12, DISPLAY_Y_MAX-6);
	/* 重新显示新的画面 */
	//GenWall();
	//DrawBlock();
	//BoardDraw();
	//BallDraw();
    Delay(0xfffff);
	
}


/**
* @brief  BallMove 小球移动过程函数
* @param  --
* @retval --
*/
void BallMove()
{
    char disbuff[20];
    uint16_t x = bal->pos_x;
    uint16_t y = bal->pos_y;
    if(!MODE)   return;     /*游戏暂停,小球位置不变化*/
    ILI9341_Clear(bal->pos_x *6, bal->pos_y*6, 6,6);   
    switch(bal->direct)
    {
    //当前朝右上
    case 0:
        if(x+1 >= 39)     /*碰到右边墙壁*/
        {
            bal->direct = 2;
            //再调用一次,保证一个时刻内球会移动
            BallMove();
        }
        else if(y-1 <= 0)                /*碰到上面墙壁*/
        {
            bal->direct = 1;
            BallMove();
        }
        else if(pixel[x+1][y])                    /*碰到右边砖块*/
        {
            bal->direct = 2;
            DelBlock(x + 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y-1])                    /*碰到上面砖块*/
        {
            bal->direct = 1;
            DelBlock( x, y - 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x+1][y-1])                    /*碰到右上面砖块*/
        {
            bal->direct = 3;
            DelBlock( x+1, y - 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
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
    //当前朝右下
    case 1:
        if(x + 1 >= 39)                 /*碰到右边墙壁*/
        {
            bal->direct = 3;
            BallMove();
        }
        else if(y + 1 >= 39)           /*碰到下面挡板*/
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
        else if(pixel[x+1][y])                                /*碰到右边砖块*/
        {
            bal->direct = 3;
            DelBlock( x + 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y+1])                                /*碰到下面砖块*/
        {
            bal->direct = 0;
            DelBlock( x, y + 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x+1][y+1])                    /*碰到右下面砖块*/
        {
            bal->direct = 2;
            DelBlock( x+1, y +1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
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
    //当前朝左上
    case 2:
        if(x - 1 <=0)                     /*碰到左边墙壁*/
        {
            bal->direct = 0;
            //再调用一次,保证一个时刻内球会移动
            BallMove();
        }
        else if(y - 1 <= 0)                /*碰到上面墙壁*/
        {
            bal->direct = 3;
            BallMove();
        }
        else if(pixel[x - 1][y])                    /*碰到左边砖块*/
        {
            bal->direct = 0;
            DelBlock( x - 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y-1])                    /*碰到上面砖块*/
        {
            bal->direct = 3;
            DelBlock( x, y - 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x-1][y-1])                    /*碰到左上面砖块*/
        {
            bal->direct = 1;
            DelBlock( x-1, y -1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
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
    //当前朝左下
    case 3:
        if(x - 1 <=0)                                 /*碰到左边墙壁*/
        {
            bal->direct = 1;
            BallMove();
        }
       else if(y + 1 >= 39)           /*碰到下面挡板*/
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
        else if(pixel[x-1][y])                                /*碰到左边砖块*/
        {
            bal->direct = 1;
            DelBlock( x - 1, y);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x][y+1])                                /*碰到下面砖块*/
        {
            bal->direct = 2;
            DelBlock( x, y + 1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
			LCD_SetFont(&Font8x16);
			LCD_SetColors(CL_BLACK,CL_WHITE);
			ILI9341_DispString_EN_CH(162,253,disbuff);
            BallMove();
        }
        else if(pixel[x-1][y+1])                    /*碰到左下面砖块*/
        {
            bal->direct = 2;
            DelBlock( x-1, y +1);
            score++;
            sprintf(disbuff,"Score:%d",score);//打印成绩
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


/**
* @brief  BoardInit 挡板初始化
* @param  --
* @retval --
*/
void BoardInit(void)
{
    brd = (board*)malloc(sizeof(struct Board));
    brd->pos_x = 18;
    brd->pos_y = 39;
    brd->speed = 3;
}

/**
* @brief  BoardMove 挡板移动
* @param  --
* @retval --
*/
void BoardMove()
{
    
    if(MODE && status == 'L')                                               /*游戏正在进行且左方向键被按下*/
    {
        if(brd->pos_x > 1)                      /*没有碰到左侧墙壁*/
        {
            LCD_SetColors(CL_GREEN,CL_WHITE);
            ILI9341_Clear(brd->pos_x *6+48, brd->pos_y*6, 6,6);
            brd->pos_x = brd->pos_x - 1;
            ILI9341_DrawRectangle(brd->pos_x *6, brd->pos_y*6, 6, 6, 1);
        }
    }
    else if(MODE && status == 'R')                                          /*游戏正在进行且右方向键被按下*/
    {
        if(brd->pos_x + 10 < 40)      /*没有碰到右侧墙壁*/
        {
            LCD_SetColors(CL_GREEN,CL_WHITE);
            ILI9341_Clear(brd->pos_x *6, brd->pos_y*6, 6,6);
            brd->pos_x = brd->pos_x + 1;
            ILI9341_DrawRectangle(brd->pos_x *6+48, brd->pos_y*6, 6, 6, 1);
        }
    }

    if(die == 1)    //如果死了就重启
	{
		LCD_SetColors(CL_RED,CL_WHITE);
		LCD_SetFont(&Font24x32);
		ILI9341_DispString_EN_CH(12,120,"Game over");
		LCD_SetFont(&Font8x16);
		ILI9341_DispString_EN_CH(64,157,"点击任意处开始");
		TIM_ITConfig(BASIC_TIM,TIM_IT_Update,DISABLE);  //不使能中断，即不是能5个按键
		while(XPT2046_TouchDetect() != TOUCH_PRESSED);//按任意位置重新初始化
		Palette_Init(6);  
		die = 0;
	}
}

/**
* @brief  BoardDraw 挡板绘制
* @param  --
* @retval --
*/
void BoardDraw()
{
    int i=0;
	//LCD显示
    ILI9341_DrawRectangle(brd->pos_x *6, brd->pos_y*6, 48, 6, 1);
	for(i=0;i<8;i++){
		pixel[brd->pos_x+i][brd->pos_y] = 1;
	}
    //更新状态数组
}


/**
* @brief  Touch_Button_Init 初始化外框游戏区域
* @param  无
* @retval 无
*/
void Square_Init(void)
{
	LCD_SetColors(CL_BLUE,CL_WHITE);
	ILI9341_DrawLine(0,240,240,240);
	//绘制显示分数的格子
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
* @brief  Touch_Button_Init 初始化按钮参数
* @param  无
* @retval 无
*/
void Touch_Button_Init(void)
{
  /*方向键*/
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
//下面是开始或暂停键  
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
* @brief  Touch_Button_Down 按键被按下时调用的函数，由触摸屏调用
* @param  x 触摸位置的x坐标
* @param  y 触摸位置的y坐标
* @retval 无
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
  uint8_t i;
  for(i=0;i<BUTTON_NUM;i++)
  {
    
    /* 触摸到了按钮 */
    if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x )
    {
      if(button[i].touch_flag == 0)     /*原本的状态为没有按下，则更新状态*/
      {
      button[i].touch_flag = 1;         /* 记录按下标志 */
      
      button[i].draw_btn(&button[i]);  /*重绘按钮*/
      }        
      
    }
    else if(button[i].touch_flag == 1) /* 触摸移出了按键的范围且之前有按下按钮 */
    {
      button[i].touch_flag = 0;         /* 清除按下标志，判断为误操作*/
      
      button[i].draw_btn(&button[i]);   /*重绘按钮*/
    }

  }

}

/**
* @brief  Touch_Button_Up 按键被释放时调用的函数，由触摸屏调用
* @param  x 触摸最后释放时的x坐标
* @param  y 触摸最后释放时的y坐标
* @retval 无
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
   uint8_t i; 
   for(i=0;i<BUTTON_NUM;i++)
   {
     
     
     /* 触笔在按钮区域释放 */
      if((x<button[i].end_x && x>button[i].start_x && y<button[i].end_y && y>button[i].start_y))
      {        
        button[i].touch_flag = 0;       /*释放触摸标志*/
        
        button[i].btn_command(&button[i]);  /*执行按键的功能命令*/
				
        button[i].draw_btn(&button[i]); /*重绘按钮*/
        break;
      }
    }  

}

/**
* @brief  Draw_Direction_Button 方向键绘制
* @param  &button[i]
* @retval 无
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
  else  /*按键按下*/
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

  /*根据按键方向写字*/
	switch(ptr->para)
  {
    		
		case LEFT:      
				LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"左");
      
      break;   
      
    case DOWN:
        LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"下");
    
      break;
        
    case RIGHT:
				LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"右");
 
      break;
    
		case UP:
        LCD_SetColors(CL_BLACK,RGB);
        ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16)/2,
//																ptr->start_y+15,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"上");
    
      break;
		
  }
  
}

/**
* @brief  Draw_Mode_Button 模式键绘制
* @param  &button[i]
* @retval 无
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
  else  /*按键按下*/
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
																"暂停");
	}
	else
	{
		ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16*2)/2,
																ptr->start_y+ ((ptr->end_y - ptr->start_y-16)/2),	
																"继续");
	}
}

/**
* @brief  Command_Change_Mode 控制游戏的暂停与继续
* @param  btn Touch_Button 类型的按键参数
* @retval 无
*/
static void Command_Change_Mode(void *btn)
{
	MODE = !MODE;
}

/**
* @brief  Command_Control_Direction 运动方向功能执行函数
* @param  btn Touch_Button 类型的按键参数
* @retval 无
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



