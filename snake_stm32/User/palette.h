#ifndef _PALETTE_H
#define _PALETTE_H

#include "stm32f10x.h"
#include "./lcd/bsp_ili9341_lcd.h"
 

#define COLOR_BLOCK_WIDTH   80
#define COLOR_BLOCK_HEIGHT  39

#define BUTTON_NUM 5
#define PALETTE_START_Y   0
#define PALETTE_END_Y     LCD_Y_LENGTH

#if 1     //��ť�������?
  #define BUTTON_START_X     0
  #define PALETTE_START_X   (COLOR_BLOCK_WIDTH*2+1)
  #define PALETTE_END_X     LCD_X_LENGTH

#else     //��ť�����ұߣ�(���ڴ�������ʱҲ���bug�����ڲ��Դ�������߽�?)
  #define BUTTON_START_X      (LCD_X_LENGTH-2*COLOR_BLOCK_WIDTH)
  #define PALETTE_START_X   	0
  #define PALETTE_END_X     	(LCD_X_LENGTH-2*COLOR_BLOCK_WIDTH)

#endif



/*
	LCD ��ɫ���룬CL_��Color�ļ�д
	16Bit�ɸ�λ����λ�� RRRR RGGG GGGB BBBB

	�����RGB �꽫24λ��RGBֵת��Ϊ16λ��ʽ��
	����windows�Ļ��ʳ��򣬵����?��ɫ��ѡ���Զ�����ɫ�����Ի�õ�RGBֵ��

	�Ƽ�ʹ������ȡɫ����������㿴���Ľ������?��
*/
#if LCD_RGB_888
/*RGB888��ɫת��*/
#define RGB(R,G,B)	( (R<< 16) | (G << 8) | (B))	/* ��8λR,G,Bת��Ϊ 24λRGB888��ʽ */

#else 
/*RGB565 ��ɫת��*/
#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
#define RGB565_R(x)  ((x >> 8) & 0xF8)
#define RGB565_G(x)  ((x >> 3) & 0xFC)
#define RGB565_B(x)  ((x << 3) & 0xF8)

#endif


enum
{
	CL_WHITE    = RGB(255,255,255),	/* ��ɫ */
	CL_BLACK    = RGB(  0,  0,  0),	/* ��ɫ */
	CL_RED      = RGB(255,	0,  0),	/* ��ɫ */
	CL_GREEN    = RGB(  0,255,  0),	/* ��ɫ */
	CL_BLUE     = RGB(  0,	0,255),	/* ��ɫ */
	CL_YELLOW   = RGB(255,255,  0),	/* ��ɫ */

	CL_GREY    = RGB( 98, 98, 98), 	/* ����? */
	CL_GREY1		= RGB( 150, 150, 150), 	/* ǳ��ɫ */
	CL_GREY2		= RGB( 180, 180, 180), 	/* ǳ��ɫ */
	CL_GREY3		= RGB( 200, 200, 200), 	/* ��ǳ��ɫ */
	CL_GREY4		= RGB( 230, 230, 230), 	/* ��ǳ��ɫ */

	CL_BUTTON_GREY	= RGB( 195, 195, 195), /* WINDOWS ��ť������? */

	CL_MAGENTA      = RGB(255, 0, 255),	/* ����ɫ������? */
	CL_CYAN         = RGB( 0, 255, 255),	/* ����ɫ����ɫ */

	CL_BLUE1        = RGB(  0,  0, 240),		/* ����ɫ */
	CL_BLUE2        = RGB(  0,  0, 128),		/* ����ɫ */
	CL_BLUE3        = RGB(  68, 68, 255),		/* ǳ��ɫ1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* ǳ��ɫ1 */

	/* UI ���� Windows�ؼ�����ɫ */
	CL_BTN_FACE		  = RGB(236, 233, 216),	/* ��ť������ɫ(��) */
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* ������������? */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* �������Ӱ�����? */
                                                                                                                                                                                                              
	CL_MASK			    = 0x7FFF	/* RGB565��ɫ���룬�������ֱ���͸�� */
};

typedef struct 
{
  uint16_t start_x;   //������x��ʼ����  
  uint16_t start_y;   //������y��ʼ����
  uint16_t end_x;     //������x�������� 
  uint16_t end_y;     //������y��������
  uint32_t para;      //��ɫ��ť�б�ʾѡ������?���ʼ���״��ť�б�ʾѡ��Ļ��?
  uint8_t touch_flag; //�������µı�־
    
  void (*draw_btn)(void * btn);     //�������?��
  void (*btn_command)(void * btn);  //��������ִ�к����������л���ɫ����ˢ
 
}Touch_Button;


/*��ť�����б�*/
typedef enum 
{
  LEFT = 0,   //��������
  
  DOWN,  //2������
  
  RIGHT,  //4������
  
	UP,  //6����
	
}SHAPE;


typedef struct Snake//�൱����һ���ڵ�
{
	int x;//������
	int y;//������
  struct Snake *next;
}snake;


typedef struct Block
{
  int pos_x;    //�?描方向下起�?�点x坐标
  int pos_y;    //�?描方向下起�?�点y坐标

  //双链表形�?
  struct Block *prev;
  struct Block *next;
}block;

//挡板信息
typedef struct Board
{
  uint16_t pos_x;    //�?点x坐标
  uint16_t pos_y;    //�?点y坐标
  uint16_t speed;    //移动速度
}board;

//小球信息
typedef struct Ball
{
  uint16_t pos_x;    //圆心x坐标
  uint16_t pos_y;    //圆心y坐标
  uint16_t radius;   //圆半�?
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
void Palette_Init1(uint8_t LCD_Mode);
void creatFood(void);
void snakeMove(void);
void judgeAlive(void);
void Square_Init(void);
void Snake_Init(void);
void GenWall(void);

void GenBlock(void);
void DrawBlock(void);
void DelBlock(int x,int y);


void BoardInit(void);
void BoardMove(void);
void BoardDraw(void);
void Play(void);

void BallInit(void);
void BallRestart(void);
void BallMove(void);
void BallDraw(void);

void Touch_Button_Init(void);
void Touch_Button_Down(uint16_t x,uint16_t y);
void Touch_Button_Up(uint16_t x,uint16_t y);


#endif //_PALETTE_H

