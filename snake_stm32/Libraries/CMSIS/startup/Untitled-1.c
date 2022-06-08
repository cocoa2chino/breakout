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
		Palette_Init(6);  
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
