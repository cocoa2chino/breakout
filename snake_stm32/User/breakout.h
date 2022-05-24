#ifndef _BREAKOUT_H
#define _BREAKOUT_H

#include "stm32f10x.h"
#include "./lcd/bsp_ili9341_lcd.h"
 

#define COLOR_BLOCK_WIDTH   80
#define COLOR_BLOCK_HEIGHT  39

#define BUTTON_NUM 5

#if 1     //按钮栏在左边
  #define BUTTON_START_X    0
  #define PALETTE_START_X   (COLOR_BLOCK_WIDTH*2+1)
  #define PALETTE_END_X     LCD_X_LENGTH

#else     //按钮栏在右边,(存在触摸按键时也会的bug仅用于测试触摸屏左边界)
  #define BUTTON_START_X      (LCD_X_LENGTH-2*COLOR_BLOCK_WIDTH)
  #define PALETTE_START_X   	0
  #define PALETTE_END_X     	(LCD_X_LENGTH-2*COLOR_BLOCK_WIDTH)

#endif



/*
	LCD 颜色代码,CL_是Color的简写
	16Bit由高位至低位, RRRR RGGG GGGB BBBB

	下面的RGB 宏将24位的RGB值转换为16位格式
	启动windows的画笔程序,点击编辑颜色,选择自定义颜色,可以获得的RGB值

	推荐使用迷你取色器软件获得你看到的界面颜色
*/
#if LCD_RGB_888
  /*RGB888颜色转换*/
  #define RGB(R,G,B)  ((R<< 16) | (G << 8) | (B))	/* 将8位R,G,B转化为 24位RGB888格式 */

#else 
  /*RGB565 颜色转换*/
  #define RGB(R,G,B)    (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* 将8位R,G,B转化为 16位RGB565格式 */
  #define RGB565_R(x)   ((x >> 8) & 0xF8)
  #define RGB565_G(x)   ((x >> 3) & 0xFC)
  #define RGB565_B(x)   ((x << 3) & 0xF8)

#endif

#define DISPLAY_X_MAX 240     //游戏主界面显示区域x坐标最大值
#define DISPLAY_Y_MAX 240     //游戏主界面显示区域y坐标最大值
#define WALL_WIDTH    5       //墙厚度
#define BOARD_LENGTH  40      //挡板长度
#define BOARD_WIDTH   3       //挡板厚度
#define BOLCK_LENGTH  10      //砖块长度
#define BLOCK_WIDTH   6       //砖块宽度

enum
{
  CL_WHITE    = RGB(255,255,255),	/* 白色 */
	CL_BLACK    = RGB(  0,  0,  0),	/* 黑色 */
	CL_RED      = RGB(255,	0,  0),	/* 红色 */
	CL_GREEN    = RGB(  0,255,  0),	/* 绿色 */
	CL_BLUE     = RGB(  0,	0,255),	/* 蓝色 */
	CL_YELLOW   = RGB(255,255,  0),	/* 黄色 */

	CL_GREY     = RGB( 98,  98,  98), 	/* 深灰色 */
	CL_GREY1		= RGB(150, 150, 150), 	/* 浅灰色 */
	CL_GREY2		= RGB(180, 180, 180), 	/* 浅灰色 */
	CL_GREY3		= RGB(200, 200, 200), 	/* 最浅灰色 */
	CL_GREY4		= RGB(230, 230, 230), 	/* 最浅灰色 */

	CL_BUTTON_GREY	= RGB(195, 195, 195), /* WINDOWS 按钮表面灰色 */

	CL_MAGENTA      = RGB(255,   0, 255),	/* 红紫色,洋红色 */
	CL_CYAN         = RGB(  0, 255, 255),	/* 蓝绿色，青色 */

	CL_BLUE1        = RGB(  0,  0, 240),		/* 深蓝色 */
	CL_BLUE2        = RGB(  0,  0, 128),		/* 深蓝色 */
	CL_BLUE3        = RGB( 68, 68, 255),		/* 浅蓝色1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* 浅蓝色1 */

	/* UI 界面 Windows控件常用色 */
	CL_BTN_FACE		  = RGB(236, 233, 216),	/* 按钮表面颜色(灰) */
	CL_BOX_BORDER1	= RGB(172, 168, 153),	/* 分组框主线颜色 */
	CL_BOX_BORDER2	= RGB(255, 255, 255),	/* 分组框阴影线颜色 */
                                                                                                                                                                                                              
	CL_MASK			    = 0x7FFF	/* RGB565颜色掩码,用于文字背景透明 */
};

typedef struct 
{
  uint16_t start_x;   //按键的x起始坐标  
  uint16_t start_y;   //按键的y起始坐标
  uint16_t end_x;     //按键的x结束坐标 
  uint16_t end_y;     //按键的y结束坐标
  uint16_t para;      //颜色按钮中表示选择的颜色,笔迹形状按钮中表示选择的画刷
  uint8_t touch_flag; //按键按下的标志
    
  void (*draw_btn)(void * btn);     //按键描绘函数
  void (*btn_command)(void * btn);  //按键功能执行函数,例如切换颜色、画刷
}Touch_Button;


/*按钮参数列表*/
typedef enum 
{
  LEFT = 1,   //单像素线

  DOWN,       //2像素线
  
  RIGHT,      //4像素线
  
  UP,         //6像素
}SHAPE;

//单个砖块信息
typedef struct Block
{
  uint16_t pos_x;    //扫描方向下起始点x坐标
  uint16_t pos_y;    //扫描方向下起始点y坐标
}block;

//挡板信息
typedef struct Board
{
  uint16_t pos_x;    //中点x坐标
  uint16_t pos_y;    //中点y坐标
  uint16_t speed;    //移动速度
}board;

//小球信息
typedef struct Ball
{
  uint16_t pos_x;    //圆心x坐标
  uint16_t pos_y;    //圆心y坐标
  uint16_t radius;   //圆半径
  /*
    运动方向(45°)
    0:右上
    1:右下
    2:左上
    3:左下
  */
  uint16_t direct;
  uint16_t speed;    //运动速度,下一时刻位置(pos_x +/- speed, pos_y +/- speed)
}ball;

void Delay(__IO uint32_t nCount);
void Palette_Init(uint8_t LCD_Mode);

void Square_Init(void);

void Touch_Button_Init(void);
void Touch_Button_Down(uint16_t x,uint16_t y);
void Touch_Button_Up(uint16_t x,uint16_t y);

void GenWall();

block *GenBlock(uint16_t pos_x, uint16_t pos_y);
void DelBlock(block *blk);

void BoardInit(board *brd);
void BoardMove(board *brd);
void BoardDraw(board *brd);

void BallInit(ball *bal);
void BallRestart(ball *bal, uint16_t pos_x);
void BallMove(ball *bal);

#endif //_PALETTE_H