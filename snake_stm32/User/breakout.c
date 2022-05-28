#include "breakout.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include <stdlib.h>
#include <stdio.h>
#include "bsp_adc.h"
#include "bsp_TiMbase.h"   

extern __IO uint16_t ADC_ConvertedValue;
/*按钮结构体数组*/
Touch_Button button[BUTTON_NUM];
uint8_t status = 'S';
uint8_t score;
uint8_t die = 0;    //die == 0说明活着
uint8_t MODE = 1;   //MODE == 1说明游戏正在进行

static uint16_t pixel[DISPLAY_Y_MAX][DISPLAY_X_MAX];    //状态数组,存储像素信息
block *blk;                                             //砖块链表头指针
uint16_t blk_row = 2;                                   //生成砖块行数
board *brd;                                             //挡板指针
ball *bal;                                              //小球指针

static void Draw_Mode_Button(void *btn);
static void Draw_Direction_Button(void *btn);

static void Command_Control_Direction(void *btn);
static void Command_Change_Mode(void *btn);  

void Delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
} 

/**
* @brief  Palette_Init 游戏初始化
* @param  LCD_Mode LCD扫描模式,该程序中使用模式6
* @retval --
*/
void Palette_Init(uint8_t LCD_Mode)
{
    uint8_t i;
	
	ADCx_Init();
	ILI9341_GramScan(LCD_Mode);
	score = 0;
	status = 'S';
    for(uint16_t j = 0; j < DISPLAY_Y_MAX; j++)
    {
        for(uint16_t k = 0; k < DISPLAY_X_MAX; k++)
        {
            pixel[j][k] = 0;
        }
    }
	
    /* 整屏清为白色 */
	LCD_SetBackColor(CL_WHITE);
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);	
	
	Touch_Button_Init();  
    Square_Init();
	
    TIM_ITConfig(BASIC_TIM, TIM_IT_Update, ENABLE);
    /* 描绘按钮 */
    for(i = 0; i < BUTTON_NUM; i++)
    {
        button[i].draw_btn(&button[i]);
    }

    /* 游戏内画面初始化和绘制 */
    GenWall();
    GenBlock(blk, blk_row);
    DrawBlock(blk);
    BoardInit(brd);
    BoardDraw(brd);
    BallInit(bal);
    BallDraw(bal);
}

/**
* @brief  Touch_Button_Init 初始化外框游戏区域
* @param  --
* @retval --
*/
void Square_Init(void)
{
	LCD_SetColors(CL_BLUE,CL_WHITE);
	ILI9341_DrawLine(0,240,240,240);
	//绘制显示分数的格子
	LCD_SetColors(CL_WHITE, CL_WHITE);
    ILI9341_DrawRectangle(BUTTON_START_X + COLOR_BLOCK_WIDTH * 2, 320 - COLOR_BLOCK_HEIGHT * 2, COLOR_BLOCK_WIDTH, COLOR_BLOCK_HEIGHT, 1);
	LCD_SetColors(CL_BLACK, CL_WHITE);
	LCD_SetFont(&Font8x16);
    ILI9341_DispString_EN_CH(162, 253, "Score:0");
	LCD_SetColors(CL_BLUE4,CL_WHITE);
    ILI9341_DrawRectangle(BUTTON_START_X + COLOR_BLOCK_WIDTH * 2, 320 - COLOR_BLOCK_HEIGHT * 2, COLOR_BLOCK_WIDTH, COLOR_BLOCK_HEIGHT, 0);
}

/**
* @brief  Touch_Button_Init 初始化按钮参数
* @param  --
* @retval --
*/
void Touch_Button_Init(void)
{
    /*方向键*/
    button[0].start_x = BUTTON_START_X;
    button[0].start_y = 320 - COLOR_BLOCK_HEIGHT;
    button[0].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
    button[0].end_y = 320;
    button[0].para = LEFT;
    button[0].touch_flag = 0;  
    button[0].draw_btn = Draw_Direction_Button;   
    button[0].btn_command = Command_Control_Direction ;  
  
    button[1].start_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
    button[1].start_y = 320 - COLOR_BLOCK_HEIGHT;
    button[1].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH * 2 ;
    button[1].end_y = 320;
    button[1].para = DOWN;
    button[1].touch_flag = 0;  
    button[1].draw_btn = Draw_Direction_Button;
    button[1].btn_command = Command_Control_Direction ;
  
    button[2].start_x = BUTTON_START_X + COLOR_BLOCK_WIDTH * 2;
    button[2].start_y = 320 - COLOR_BLOCK_HEIGHT;
    button[2].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH * 3;
    button[2].end_y = 320;
    button[2].para = RIGHT;
    button[2].touch_flag = 0;
    button[2].draw_btn = Draw_Direction_Button;
    button[2].btn_command = Command_Control_Direction ;
 
    button[3].start_x = BUTTON_START_X + COLOR_BLOCK_WIDTH;
    button[3].start_y = 320 - COLOR_BLOCK_HEIGHT * 2;
    button[3].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH * 2;
    button[3].end_y = 320 - COLOR_BLOCK_HEIGHT;
    button[3].para = UP;
    button[3].touch_flag = 0;  
    button[3].draw_btn = Draw_Direction_Button;
    button[3].btn_command = Command_Control_Direction ;
    //下面是开始或暂停键  
    button[4].start_x = BUTTON_START_X;
    button[4].start_y = 320 - COLOR_BLOCK_HEIGHT * 2;
    button[4].end_x = BUTTON_START_X + COLOR_BLOCK_WIDTH ;
    button[4].end_y = 320 - COLOR_BLOCK_HEIGHT;
    button[4].para = 0;
    button[4].touch_flag = 0;  
    button[4].draw_btn = Draw_Mode_Button ;
    button[4].btn_command = Command_Change_Mode;
}

/**
* @brief  Touch_Button_Down 按键被按下时调用的函数,由触摸屏调用
* @param  x 触摸位置的x坐标
* @param  y 触摸位置的y坐标
* @retval --
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
    uint8_t i;
    for(i = 0; i < BUTTON_NUM; i++)
    {
        /* 触摸到了按钮 */
        if(x <= button[i].end_x && y <= button[i].end_y && y >= button[i].start_y && x >= button[i].start_x)
        {
            if(button[i].para != 0)                 /*按下按键的持续时间内,更新status;如按住左方向键,这期间status='L',挡板向左*/
            {
                button[i].btn_command(&button[i]);
            }
            if(button[i].touch_flag == 0)           /*原本的状态为没有按下，则更新状态*/
            {
                button[i].touch_flag = 1;           /* 记录按下标志 */

                button[i].draw_btn(&button[i]);     /*重绘按钮*/
            }        
        }
        else if(button[i].touch_flag == 1)          /* 触摸移出了按键的范围且之前有按下按钮 */
        {
            button[i].touch_flag = 0;               /* 清除按下标志，判断为误操作*/
      
            button[i].draw_btn(&button[i]);         /*重绘按钮*/
        }
    }
}

/**
* @brief  Touch_Button_Up 按键被释放时调用的函数,由触摸屏调用
* @param  x 触摸最后释放时的x坐标
* @param  y 触摸最后释放时的y坐标
* @retval --
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
    uint8_t i; 
    for(i = 0; i < BUTTON_NUM; i++)
    {
        /* 触笔在按钮区域释放 */
        if(x < button[i].end_x && x > button[i].start_x && y < button[i].end_y && y > button[i].start_y)
        {        
            button[i].touch_flag = 0;               /*释放触摸标志*/

            if(button[i].para == 0)                 /*松开的是开始/暂停键*/
            {
                button[i].btn_command(&button[i]);  /*执行按键的功能命令*/
            }
            else                                    /*松开的是方向键,将status更新为'S',即STOP,挡板不移动*/
            {
                status = 'S';
            }
				
            button[i].draw_btn(&button[i]);         /*重绘按钮*/
            break;
        }
    }  
}

/**
* @brief  Draw_Direction_Button 方向键绘制
* @param  btn 按钮指针的引用
* @retval --
*/
static void Draw_Direction_Button(void *btn)
{
    Touch_Button *ptr = (Touch_Button *)btn;
    uint16_t RGB;

	if(ptr->touch_flag == 0)
    {
	    LCD_SetColors(CL_BUTTON_GREY,CL_WHITE);
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x, ptr->end_y - ptr->start_y, 1);
        RGB = CL_BUTTON_GREY;
    }
    else  /*按键按下*/
    {
		LCD_SetColors(CL_WHITE,CL_WHITE);
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x, ptr->end_y - ptr->start_y, 1);
		RGB = CL_WHITE;
    } 
    LCD_SetColors(CL_BLUE4,CL_WHITE);
    ILI9341_DrawRectangle(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x, ptr->end_y - ptr->start_y, 0);
     
    LCD_SetColors(CL_BLACK,RGB);
	LCD_SetFont(&Font8x16);
    /*根据按键方向写字*/
	switch(ptr->para)
    {
    	case LEFT:      
			LCD_SetColors(CL_BLACK,RGB);
            ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16) / 2,/*ptr->start_y+15,*/ptr->start_y + ((ptr->end_y - ptr->start_y - 16) / 2), "左");
            break;   
        case DOWN:
            LCD_SetColors(CL_BLACK,RGB);
            ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16) / 2,/*ptr->start_y+15,*/ptr->start_y + ((ptr->end_y - ptr->start_y - 16) / 2), "下");
            break;
        case RIGHT:
			LCD_SetColors(CL_BLACK,RGB);
            ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16) / 2,/*ptr->start_y+15,*/ptr->start_y + ((ptr->end_y - ptr->start_y - 16) / 2),	"右");
            break;
		case UP:
            LCD_SetColors(CL_BLACK,RGB);
            ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16) / 2,/*ptr->start_y+15,*/ptr->start_y + ((ptr->end_y - ptr->start_y - 16) / 2), "上");
            break;
	}
}

/**
* @brief  Draw_Mode_Button 模式键绘制
* @param  btn 按键指针的引用
* @retval --
*/
static void Draw_Mode_Button(void *btn)
{
    Touch_Button *ptr = (Touch_Button *)btn;
    uint16_t RGB;

	if(ptr->touch_flag == 0)
    {
	    LCD_SetColors(CL_BUTTON_GREY, CL_WHITE);
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x, ptr->end_y - ptr->start_y, 1);
        RGB = CL_BUTTON_GREY;
    }
    else  /*按键按下*/
    {
		LCD_SetColors(CL_WHITE, CL_WHITE);
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x, ptr->end_y - ptr->start_y, 1);
		RGB = CL_WHITE;
    } 
    LCD_SetColors(CL_BLUE4,CL_WHITE);
    ILI9341_DrawRectangle(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x, ptr->end_y - ptr->start_y, 0);
    LCD_SetColors(CL_BLACK, RGB);
	LCD_SetFont(&Font8x16);
	if(MODE)
	{
		ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16 * 2) / 2, ptr->start_y + ((ptr->end_y - ptr->start_y - 16) / 2), "暂停");
	}
	else
	{
		ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16 * 2) / 2, ptr->start_y + ((ptr->end_y - ptr->start_y - 16) / 2), "继续");
	}
}

/**
* @brief  Command_Change_Mode 控制游戏的暂停与继续
* @param  btn 按键指针的引用
* @retval 无
*/
static void Command_Change_Mode(void *btn)
{
    MODE = !MODE;
}

/**
* @brief  Command_Control_Direction 运动方向功能执行函数
* @param  btn 按键指针的引用
* @retval 无
*/
static void Command_Control_Direction(void *btn)
{
    Touch_Button *ptr = (Touch_Button *)btn;

	if(ptr->para == UP)    
        status='U';
	if(ptr->para == DOWN)
	    status='D';
	if(ptr->para == LEFT)
	    status='L';
	if(ptr->para == RIGHT)
	    status='R';
}

/**
* @brief  GenWall 生成左/右/上方墙壁
* @param  --
* @retval --
*/
void GenWall()
{
    //LCD显示
    ILI9341_DrawRectangle(0, 0, WALL_WIDTH, DISPLAY_Y_MAX, 1);
    ILI9341_DrawRectangle(DISPLAY_X_MAX - WALL_WIDTH, 0, WALL_WIDTH, DISPLAY_Y_MAX, 1);
    ILI9341_DrawRectangle(WALL_WIDTH, 0, DISPLAY_X_MAX - 2 * WALL_WIDTH, WALL_WIDTH, 1);
    //更新状态数组
    for (uint16_t i = 0; i < WALL_WIDTH; i++)
    {
        for (uint16_t j = 0; j < DISPLAY_X_MAX; j++)
        {
        pixel[i][j] = 1;
        }
    }
    for (uint16_t m = WALL_WIDTH; m < DISPLAY_Y_MAX; m++)
    {
        for (uint16_t n1 = 0; n1 < WALL_WIDTH; n1++)
        {
            pixel[m][n1] = 1;
        }
        for (uint16_t n2 = DISPLAY_X_MAX - WALL_WIDTH; n2 < DISPLAY_X_MAX; n2++)
        {
            pixel[m][n2] = 1;
        }
    }
}

/**
* @brief  GenBlock 初始化砖块
* @param  blk 砖块链表指针引用
* @param  row 生成砖块行数
* @retval --
*/
void GenBlock(block *blk, uint16_t row)
{
    block *tmp;

    //初始化第一个砖块
    blk = (block*)malloc(sizeof(struct Block));
    blk->pos_x = WALL_WIDTH;
    blk->pos_y = WALL_WIDTH;
    blk->prev = NULL;
    blk->next = NULL;

    //根据要生成的砖块行数和砖块长度,初始化后续砖块
    tmp = blk;
    for(uint16_t i = 0; i < row; i++)
    {
        for(uint16_t j = 0; WALL_WIDTH + BLOCK_LENGTH * j < DISPLAY_X_MAX - WALL_WIDTH; j++)
        {
            if(i == 0 && j == 0)
                continue;
            tmp->next = (block*)malloc(sizeof(struct Block));
            tmp->next->prev = tmp;
            tmp = tmp->next;
            tmp->pos_x = WALL_WIDTH + BLOCK_LENGTH * j;
            tmp->pos_y = WALL_WIDTH + BLOCK_WIDTH * i;
            tmp->next = NULL;
        }
    }
}

/**
* @brief  DrawBlock 砖块绘制
* @param  blk 砖块链表头指针引用
* @retval --
*/
void DrawBlock(block *blk)
{
    block *tmp = blk;
    while(tmp->next != NULL)    /*遍历链表*/
    {
        //LCD显示
        ILI9341_DrawRectangle(tmp->pos_x, tmp->pos_y, BLOCK_LENGTH - 2, BLOCK_WIDTH - 2, 1);
        //更新状态数组
        for(uint16_t i = tmp->pos_y; i < tmp->pos_y + BLOCK_WIDTH; i++)
        {
            for(uint16_t j = tmp->pos_x; j < tmp->pos_x + BLOCK_LENGTH; j++)
            {
                pixel[i][j] = 1;
            }
        }
        tmp = tmp->next;
    }
}

/**
* @brief  FindBlock 根据x,y坐标找到对应砖块并消除
* @param  blk 砖块链表头指针引用
* @param  pos_x x坐标
* @param  pos_y y坐标
* @retval
*/
void FindBlock(block *blk, uint16_t pos_x, uint16_t pos_y)
{
    block *tmp = blk;
    while(tmp->next != NULL)    /*遍历链表*/
    {
        if(tmp->pos_x <= pos_x && tmp->pos_x + BLOCK_LENGTH >= pos_x && tmp->pos_y <= pos_y && tmp->pos_y + BLOCK_WIDTH >= pos_y)
        {
            DelBlock(tmp);
            return;
        }
        tmp = tmp->next;
    }
}

/**
* @brief  DelBlock 砖块消除
* @param  blki 砖块指针引用
* @retval --
*/
void DelBlock(block *blki)
{
    if(blki->prev == NULL)              /*删除的是链表第一个*/
    {
        if(blki->next == NULL)          /*只有一个砖块,将头指针置为空*/
        {
            UpdateDelBlock(blk);
            free(blk);
            blk = NULL;
            return;
        }
        else                            /*把头指针设为第二个砖块*/
        {
            blk = blk->next;
            blk->prev = NULL;
        }
    }
    else if(blki->next == NULL)         /*删除的是链表最后一个*/
    {
        blki->prev->next = NULL;
    }
    else
    {
        blki->prev->next = blki->next;
        blki->next->prev = blki->prev;
    }
    UpdateDelBlock(blki);
    free(blki);
}

/**
* @brief  UpdateDelBlock 砖块消除中封装的更新状态数组函数
* @param  blki 砖块指针引用
* @retval --
*/
void UpdateDelBlock(block *blki)
{
    for(uint16_t i = blki->pos_y; i < blki->pos_y + BLOCK_WIDTH; i++)
    {
        for(uint16_t j = blki->pos_x; j < blki->pos_x + BLOCK_LENGTH; j++)
        {
            pixel[i][j] = 0;
        }
    }
}

/**
* @brief  BoardInit 初始化挡板
* @param  brd 挡板指针引用
* @retval --
*/
void BoardInit(board *brd)
{
    brd = (board*)malloc(sizeof(struct Board));

    brd->pos_x = DISPLAY_X_MAX / 2;
    brd->pos_y = DISPLAY_Y_MAX - BOARD_WIDTH;
    brd->speed = 3;
}

/**
* @brief  BoardMove 挡板移动
* @param  brd 挡板指针引用
* @retval --
*/
void BoardMove(board *brd)
{
    if(MODE && status == 'L')                                               /*游戏正在进行且左方向键被按下*/
    {
        if(brd->pos_x - BOARD_LENGTH / 2 > WALL_WIDTH)                      /*没有碰到左侧墙壁*/
        {
            brd->pos_x = brd->pos_x - brd->speed;
        }
    }
    else if(MODE && status == 'R')                                          /*游戏正在进行且右方向键被按下*/
    {
        if(brd->pos_x + BOARD_LENGTH / 2 < DISPLAY_X_MAX - WALL_WIDTH)      /*没有碰到右侧墙壁*/
        {
            brd->pos_x = brd->pos_x + brd->speed;
        }
    }
}

/**
* @brief  BoardDraw 挡板绘制
* @param  brd 挡板指针引用
* @retval --
*/
void BoardDraw(board *brd)
{
    //LCD显示
    ILI9341_DrawRectangle(brd->pos_x - BOARD_LENGTH / 2, brd->pos_y, BOARD_LENGTH, BOARD_WIDTH, 1);
    //更新状态数组
    for(uint16_t i = brd->pos_y; i < brd->pos_y + BOARD_WIDTH; i++)
    {
        for(uint16_t j = brd->pos_x - BOARD_LENGTH / 2; j < brd->pos_x + BOARD_LENGTH / 2; j++)
        {
            pixel[i][j] = 1;
        }
    }
}

/**
* @brief  BallInit 初始化小球
* @param  bal 小球指针引用
* @retval --
*/
void BallInit(ball *bal)
{
    bal =	(ball*)malloc(sizeof(struct Ball));

    bal->pos_x = DISPLAY_X_MAX / 2;
    bal->radius = 3;
    bal->pos_y = DISPLAY_Y_MAX - BOARD_WIDTH - bal->radius;
    bal->direct = 0;
    bal->speed = 5;

    return bal;
}

/**
* @brief  BallRestart 失败之后重新开始,小球在当前挡板中间生成
* @param  bal 小球指针引用
* @param  pos_x 挡板中点x坐标
* @retval --
*/
void BallRestart(ball *bal, uint16_t pos_x)
{
    bal->pos_x = pos_x;
    bal->radius = 3;
    bal->pos_y = DISPLAY_Y_MAX - BOARD_WIDTH - bal->radius;
    bal->direct = 0;
    bal->speed = 5;
}

/**
* @brief  BallMove 小球移动过程函数
* @param  bal 小球指针引用
* @retval --
*/
void BallMove(ball *bal)
{
    if(!MODE)   return;     /*游戏暂停,小球位置不变化*/

    uint16_t x = bal->pos_x;
    uint16_t y = bal->pos_y;
    uint16_t r = bal->radius;
    switch(bal->direct)
    {
    //当前朝右上
    case 0:
        if(x + r >= DISPLAY_X_MAX - WALL_WIDTH)     /*碰到右边墙壁*/
        {
            bal->direct = 2;
            //再调用一次,保证一个时刻内球会移动
            BallMove(bal);
        }
        else if(y - r <= WALL_WIDTH)                /*碰到上面墙壁*/
        {
            bal->direct = 1;
            BallMove(bal);
        }
        else if(pixel[y][x + r])                    /*碰到右边砖块*/
        {
            bal->direct = 2;
            FindBlock(blk, x + r, y);
            score++;
            BallMove(bal);
        }
        else if(pixel[y - r][x])                    /*碰到上面砖块*/
        {
            bal->direct = 1;
            FindBlock(blk, x, y - r);
            score++;
            BallMove(bal);
        }
        else
        {
            bal->pos_x = bal->pos_x + bal->speed;
            bal->pos_y = bal->pos_y - bal->speed;
        }
        break;
    //当前朝右下
    case 1:
        if(x + r >= DISPLAY_X_MAX - WALL_WIDTH)                 /*碰到右边墙壁*/
        {
            bal->direct = 3;
            BallMove(bal);
        }
        else if(y + r >= DISPLAY_Y_MAX - BOARD_WIDTH)           /*碰到下面挡板*/
        {
            if(pixel[y + r][x])
            {
                bal->direct = 0;
                BallMove(bal);
            }
            else
            {
                /*本次失败,相关处理*/
            }
        }
        else if(pixel[y][x + r])                                /*碰到右边砖块*/
        {
            bal->direct = 3;
            FindBlock(blk, x + r, y);
            score++;
            BallMove(bal);
        }
        else if(pixel[y + r][x])                                /*碰到下面砖块*/
        {
            bal->direct = 0;
            FindBlock(blk, x, y + r);
            score++;
            BallMove(bal);
        }
        else
        {
            bal->pos_x = bal->pos_x + bal->speed;
            bal->pos_y = bal->pos_y + bal->speed;
        }
        break;
    //当前朝左上
    case 2:
        if(x - r <= WALL_WIDTH)                     /*碰到左边墙壁*/
        {
            bal->direct = 0;
            //再调用一次,保证一个时刻内球会移动
            BallMove(bal);
        }
        else if(y - r <= WALL_WIDTH)                /*碰到上面墙壁*/
        {
            bal->direct = 3;
            BallMove(bal);
        }
        else if(pixel[y][x - r])                    /*碰到左边砖块*/
        {
            bal->direct = 0;
            FindBlock(blk, x - r, y);
            score++;
            BallMove(bal);
        }
        else if(pixel[y - r][x])                    /*碰到上面砖块*/
        {
            bal->direct = 3;
            FindBlock(blk, x, y - r);
            score++;
            BallMove(bal);
        }
        else
        {
            bal->pos_x = bal->pos_x - bal->speed;
            bal->pos_y = bal->pos_y - bal->speed;
        }
        break;
    //当前朝左下
    case 3:
        if(x - r <= WALL_WIDTH)                                 /*碰到左边墙壁*/
        {
            bal->direct = 1;
            BallMove(bal);
        }
        else if(y + r >= DISPLAY_Y_MAX - BOARD_WIDTH)           /*碰到下面挡板*/
        {
            if(pixel[y + r][x])
            {
                bal->direct = 2;
                BallMove(bal);
            }
            else
            {
                /*本次失败,相关处理*/
            }
        }
        else if(pixel[y][x - r])                                /*碰到左边砖块*/
        {
            bal->direct = 1;
            FindBlock(blk, x - r, y);
            score++;
            BallMove(bal);
        }
        else if(pixel[y + r][x])                                /*碰到下面砖块*/
        {
            bal->direct = 2;
            FindBlock(blk, x, y + r);
            score++;
            BallMove(bal);
        }
        else
        {
            bal->pos_x = bal->pos_x - bal->speed;
            bal->pos_y = bal->pos_y + bal->speed;
        }
        break;
    }
}

/**
* @brief  BallDraw 小球绘制
* @param  bal 小球指针引用
* @retval --
*/
void BallDraw(ball *bal)
{
    ILI9341_DrawCircle(bal->pos_x, bal->pos_y, bal->radius, 1);
}

/**
* @brief  Play 游戏运行函数
* @param  --
* @retval --
*/
void Play()
{
    BoardMove(brd);
    BallMove(bal);

    /* 游戏区域清为白色 */
	LCD_SetBackColor(CL_WHITE);
    ILI9341_Clear(0, 0, DISPLAY_X_MAX, DISPLAY_Y_MAX);
    /* 重新显示新的画面 */
    GenWall();
    DrawBlock(blk);
    BoardDraw(brd);
    BallDraw(bal);
}
/* ------------------------------------------end of file---------------------------------------- */