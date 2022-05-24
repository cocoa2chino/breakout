#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

void gotoxy(int x, int y);
void gotoxy(int x, int y)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x+1,y});
}

int main(){

    while (1)
    {
        gotoxy(40,40);
        if(GetKeyState(65)<0)printf("a");//读取输入
        if(GetKeyState(68)<0)printf("d");
        //gotoxy(x,y)用于将光标移动至x,y处，配合printf进行打印
        //实际stm32板上用ILI9341_DrawRectangle(x*6,y*6,6,6,1);函数实现
        //板上资源为40*40每个方格为6*6像素，c中实现时应当也为40*40
        if(GetKeyState(VK_SPACE)<0)exit(0);	
    }
    getch();//获取键盘输入
}
