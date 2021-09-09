#include<stdio.h>
#include<stdio.h>//用于字符串与纯数据的转化
#include<graphics.h>//基本图像输出
#include<mmsystem.h>//用于输出bgm
#include<time.h>//用于骰子
#pragma comment(lib,"winmm.lib")

void cover(void);//一个“华丽”的封面
void initdata(void);//初始化数据
void initmap(void);//框架与地图
void bgm(void);//背景音乐,根据人数减少切换音乐
void players(void);//输出玩家的数据和代表玩家的圆
void turn(void);//判断回合开始与结束
void tip(void);//用于提醒玩家输入
void playermove(void);//移动
void begin(void);//起点奖励
void land(void);//地租函数
void jail(void);//坐牢判定
void fate(void);//随机命运
void judger(void);//出局和游戏结束判定
int dice(void);//骰子

enum color { blue, red, yellow, green };
char colors[4][20] = { {"玩家：blue"},{"玩家：red"},{"玩家：yellow"},{"玩家：green"} };
char suitation[3][20] = { {"状态：正常"} ,{"状态：被逮捕"},{"状态：出局"} };

struct {
	enum color playcolor;//代表四个玩家
	int money;
	char char_money[10];//用于输出金额的字符串
	int state;//当前状态
	int propnum;//道具数量
	int x, y;//坐标
	int islucky;//财神衰神判断
	int isunluky;
	int ishum;//判断是否是人类玩家
}play[4];

struct {
	int owner;//-1代表无人购买
	int prince;//购买费用
	int level;//土地等级
	int rent;//租费
	int x1, x2, y1, y2;//代表的坐标，以便标记
}lands[8];

int step, judge=3, turnend = 1,record,num_hum=1;//一些必要的全局数据,

int main(void)
{
	cover();
	initdata();
	
	while (1)
	{
		BeginBatchDraw();
		bgm();
		initmap();
		players();
		EndBatchDraw();
		if (play[judge].state!=2)
		{
			turn();
			tip();
			playermove();
			land();
			begin();
			fate();
			jail();
			judger();
		}
		else
			judge++;//淘汰出局玩家
		judge %= 4;//防止越界
	}
}




void cover(void)
{
	
	initgraph(800, 800);
	mciSendString("open start.mp3",0, 0, 0);
	mciSendString("play start.mp3", 0, 0, 0);
	HWND hwnd = GetHWnd();//句柄
	SetWindowText(hwnd, "大富翁");
	int x, y;
	IMAGE img;
	MOUSEMSG msg;
	char tip[] = "左键任意处开始游戏";
	int width = textwidth(tip);//用于居中显示
	loadimage(&img, "./cover.png");
	setbkmode(TRANSPARENT);
	for (x = 0, y = 800;y >= 0; y -= 1)
	{
		cleardevice();
		putimage(x, y, &img);
		Sleep(2);
	}//实现封面的移动

	while (1)
	{
		if (MouseHit())
		{
			msg = GetMouseMsg();
			if (msg.uMsg == WM_LBUTTONDOWN)
				break;
		}
		settextcolor(RGB(201, 162, 70));
		outtextxy((800 - width) / 2, 650, tip);
		Sleep(10);
		settextcolor(WHITE);
		outtextxy((800 - width) / 2, 650, tip);
		Sleep(5);//提示跳动
	}//提示用户左键任意处开始游戏
	mciSendString("close start.mp3", 0, 0, 0);
}
void initdata(void)
{
	int index;//修改num来修改人类玩家数量
	for (index = 0;index < num_hum;index++)
		play[index].ishum = 1;
	for (index = 0;index < 4;index++)
	{
		play[index].playcolor = (enum color)index;//c++不能直接用int类型值来赋值
		play[index].money = 10000;
		if (index < 2)
		{
			play[index].x = 30 + index * 60;
			play[index].y = 230;
		}
		else
		{
			play[index].x = 30 + (index - 2) * 60;
			play[index].y = 280;
		}//统一放在起点

	}
	//土地坐标的赋值,已忽略
	lands[0].x1 = 264;
	lands[0].y1 = 204;
	lands[0].x2 = 398;
	lands[0].y2 = 303;
	lands[1].x1 = 520;
	lands[1].y1 = 204;
	lands[1].x2 = 665;
	lands[1].y2 = 304;
	lands[2].x1 = 669;
	lands[2].y1 = 400;
	lands[2].x2 = 797;
	lands[2].y2 = 497;
	lands[3].x1 = 668;
	lands[3].y1 = 588;
	lands[3].x2 = 798;
	lands[3].y2 = 678;
	lands[4].x1 = 396;
	lands[4].y1 = 679;
	lands[4].x2 = 519;
	lands[4].y2 = 800;
	lands[5].x1 = 133;
	lands[5].y1 = 679;
	lands[5].x2 = 265;
	lands[5].y2 = 800;
	lands[6].x1 = 0;
	lands[6].y1 = 500;
	lands[6].x2 = 132;
	lands[6].y2 = 586;
	lands[7].x1 = 0;
	lands[7].y1 = 305;
	lands[7].x2 = 132;
	lands[7].y2 = 400;
	//土地初始价格及其他杂项的赋值
	for (int count = 0, i = 1;count < 8;count++)//i用于判断
	{
		lands[count].owner = -1;
		lands[count].prince = ((i) ? 1000 : 2000);
		i =!i;
	}
}

void bgm(void)
{
	int num_out, index;
	for (index = num_out = 0;index < 4;index++)
		if (play[index].state == 2)
			num_out++;
	switch (num_out)//以淘汰人数为判断值
	{
	case 0:
		mciSendString("open ./cover.mp3 ", 0, 0, 0);
		mciSendString("play ./cover.mp3 repeat", 0, 0, 0);
		break;
	case 1:
		mciSendString("close ./cover.mp3", 0, 0, 0);
		mciSendString("open ./Bonetrousle.mp3 ", 0, 0, 0);
		mciSendString("play ./Bonetrousle.mp3 repeat", 0, 0, 0);
		break;
	case 2:
		mciSendString("close ./Bonetrousle.mp3", 0, 0, 0);
		mciSendString("open ./MEGALOVANIA.mp3 ", 0, 0, 0);
		mciSendString("play ./MEGALOVANIA.mp3 repeart", 0, 0, 0);
		break;
	}

}
void initmap(void)
{

	IMAGE img;
	char cast[] = { "投骰子" };
	int height, width;

	setbkcolor(WHITE);
	cleardevice();
	loadimage(&img, "./map.png");
	putimage(0, 200, &img);
	height = (100 - textheight(cast)) / 2;
	width = (200 - textwidth(cast)) / 2;

	setcolor(RED);
	rectangle(300, 450, 500, 550);
	outtextxy(300 + width, 450 + height, cast);//骰子的按钮
}

void players(void)
{
	int index = 0;
	char level_show[2];
	for (index = 0;index < 4;index++)
	{
		switch (index)
		{
		case blue:
			settextcolor(BLUE);
			setfillcolor(BLUE);
			break;
		case red:
			settextcolor(RED);
			setfillcolor(RED);
			break;
		case yellow:
			settextcolor(YELLOW);
			setfillcolor(YELLOW);
			break;
		case green:
			settextcolor(GREEN);
			setfillcolor(GREEN);
			break;
		}
		outtextxy(index * 200, 20, colors[index]);
		outtextxy(index * 200, 80, "持有金额:");
		sprintf(play[index].char_money, "%d", play[index].money);
		outtextxy(index * 200 + 80, 80, play[index].char_money);
		outtextxy(index * 200, 140, suitation[play[index].state]);//输出名字，金额，状态和初始位置
		if (play[index].state != 2)
			solidcircle(play[index].x, play[index].y, 15);//代表玩家的圆
	}
	//土地拥有者的轮框显示
	for (index = 0;index < 8;index++)
	{
		if (lands[index].owner >= 0)
		{
			switch (lands[index].owner)
			{
			case blue:
				settextcolor(BLUE);
				setlinecolor(BLUE);
				break;
			case red:
				settextcolor(RED);
				setlinecolor(RED);
				break;
			case yellow:
				settextcolor(YELLOW);
				setlinecolor(YELLOW);
				break;
			case green:
				settextcolor(GREEN);
				setlinecolor(GREEN);
				break;
			}
			rectangle(lands[index].x1+5, lands[index].y1+5, lands[index].x2-5, lands[index].y2-5);
			sprintf(level_show, "%d", lands[index].level);
			outtextxy(lands[index].x1 + 5, lands[index].y1 + 5, level_show);
		}
	}
}
void turn(void)
{
	if (!step)
	{
		if (turnend)
		{
			step = dice();
			record = step;
			turnend = 0;
			judge++;
			judge %= 4;//防止越界
		}
		else
			turnend = 1;
	}
	
	while (play[judge].state == 1)
	{
		play[judge].state = 0;
		judge++;
		judge %= 4;//防止越界
	}
}
void tip(void)
{

	while (play[judge].ishum && record==step)//提示人类玩家点击
	{

		MOUSEMSG msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN && msg.x > 300 && msg.x < 500 && msg.y>450 && msg.y < 550)
		{
			setfillcolor(BLUE);
			solidrectangle(300, 450, 500, 550);
			break;//点击反馈
		}

	}
}
void playermove(void)
{
	switch (play[judge].state)
	{
	case 0:
		if (step)
		{
			if (play[judge].x > 0 && play[judge].x < 668 && play[judge].y < 308)
				play[judge].x += 135;
			else if (play[judge].x > 668 && play[judge].y > 200 && play[judge].y < 680)
				play[judge].y += 95;
			else if (play[judge].x < 800 && play[judge].x>132 && play[judge].y > 680)
				play[judge].x -= 135;
			else if (play[judge].y < 800 && play[judge].y > 304 && play[judge].x < 132)//地图四等分然后判断移动
				play[judge].y -= 95;
			step--;
			Sleep(500);//防止移动过快
		}
		break;


	case 1:
		play[judge].state = 0;
		break;
	case 2:
		break;
	}
}
void begin(void)
{
	if (turnend)
		if (play[judge].x > 0 && play[judge].x < 133 && play[judge].y>200 && play[judge].y < 308)
		{
			play[judge].money += 500;
			switch (judge)
			{
			case blue:
				settextcolor(BLUE);
				break;
			case red:
				settextcolor(RED);
				break;
			case yellow:
				settextcolor(YELLOW);
				break;
			case green:
				settextcolor(GREEN);
				break;
			}
			outtextxy(judge * 200, 180, "起点奖励+500");
			Sleep(500);
		}
}
int dice(void)
{
	int num = 0;
	srand((unsigned int)time(NULL));
	return num = (rand() + 13 * step) % 5 + 1;//避免出现4个玩家同时骰子点数相同的情况
}
void jail(void)
{
	if (turnend)
	{
		if (step == 0 && play[judge].x > 666 && play[judge].x < 800 && play[judge].y>200 && play[judge].y < 303)
		{
			play[judge].state = 1;
			play[judge].x -= 675;
			play[judge].y += 475;
		}
		if (step == 0 && play[judge].x > 0 && play[judge].x < 128 && play[judge].y>678 && play[judge].y < 800)
		{
			play[judge].state = 1;
		}
	}
}
void fate(void)
{
	int  num, index = judge;//一开始思路错了，这里懒得一个一个替换了
	if (turnend)
	{
		if (step == 0)//因为地图是用画图画出来的有点不规则所以我只能想到用这种办法来判断了，实际上也可以分类判断
		{
			if ((play[index].x > 135 && play[index].x < 267 && play[index].y>200 && play[index].y < 303) ||
				(play[index].x > 397 && play[index].x < 517 && play[index].y>200 && play[index].y < 305) ||
				(play[index].x > 666 && play[index].x < 800 && play[index].y>307 && play[index].y < 400) ||
				(play[index].x > 667 && play[index].x < 800 && play[index].y> 500 && play[index].y < 589) ||
				(play[index].x > 520 && play[index].x < 669 && play[index].y>680 && play[index].y < 800) ||
				(play[index].x > 265 && play[index].x < 399 && play[index].y>680 && play[index].y < 800) ||
				(play[index].x > 0 && play[index].x < 132 && play[index].y> 589 && play[index].y < 681) ||
				(play[index].x > 0 && play[index].x < 132 && play[index].y> 404 && play[index].y < 497))
				num = rand() % 4 + 1;
			else
				num = 0;

			switch (index)
			{
			case blue:
				settextcolor(BLUE);
				break;
			case red:
				settextcolor(RED);
				break;
			case yellow:
				settextcolor(YELLOW);
				break;
			case green:
				settextcolor(GREEN);
				break;//用于区分不同玩家的命运
			}
			switch (num)
			{
			case 1:
				play[index].money += 2000;
				outtextxy(index * 200, 180, "意外之财+2000");
				Sleep(500);
				break;
			case 2:
				play[index].money -= 2000;
				outtextxy(index * 200, 180, "倒霉蛋-2000");
				Sleep(500);
				break;
			case 3:
				outtextxy(index * 200, 180, "财神保佑");
				play[index].islucky = 1;
				Sleep(500);
				break;
			case 4:
				outtextxy(index * 200, 180, "衰神附体");
				play[index].isunluky = 1;
				Sleep(500);//防止信息一闪而过
				break;
			}
		}
	}
}
void judger(void)
{
	int num,count_humout,count_cptout;
	if (turnend)
	{
		if (play[judge].money <= 0)
			play[judge].state = 2;
		for (num = count_humout =count_cptout= 0;num < 4;num++)
		{
			if (play[num].money <= 0 && play[num].ishum)
				count_humout++;
			else if (play[num].money <= 0)
				count_cptout++;
		}
		HWND hwnd = GetHWnd();//句柄
		if (count_humout == num_hum)//人类玩家全部淘汰游戏结束
		{
	 		MessageBox(hwnd, "你破产了", "游戏失败", MB_OK);
			closegraph();
			exit(0);//结束程序
		}
		else if (count_cptout == 4 - num_hum)
		{
			MessageBox(hwnd, "你淘汰了所有的电脑", "游戏胜利", MB_OK);
			closegraph();
			exit(0);//结束程序
		}
	}
}
void land(void)
{
	HWND hwnd = GetHWnd();//句柄
	int index, find = 0;
	if (turnend)
	{
		for (index = 0;index < 8;index++)
		{
			if (play[judge].x > lands[index].x1 && play[judge].x < lands[index].x2 && play[judge].y > lands[index].y1 && play[judge].y < lands[index].y2)
			{
				find = 1;
				break;
			}
		}
		if (find)
			if (lands[index].owner == -1)
			{
				if (play[judge].money <= lands[index].prince)
					goto end;//电脑钱不够不会买
				else if (play[judge].ishum)
				{
					if (IDCANCEL == MessageBox(hwnd, "是否购买该土地", "提示", MB_OKCANCEL))
						goto end;
				}
				play[judge].money -= lands[index].prince;
				lands[index].owner = judge;
				lands[index].level = 1;
				lands[index].rent = lands[index].prince * lands[index].level;

			}
			else if (judge == lands[index].owner)
			{			
				if ((play[judge].money <= lands[index].rent/2))
					goto end;
				if (lands[index].level <= 5)
				{
					if (play[judge].ishum)
					{
						if (IDCANCEL == MessageBox(hwnd, "是否升级该土地", "提示", MB_OKCANCEL))
							goto end;
					}
					lands[index].level++;
					play[judge].money -= lands[index].rent / 2;
				}
	        }
			else 
			{
				play[judge].money -= lands[index].rent * (play[judge].isunluky + 1) / (play[judge].islucky + 1);
				play[lands[index].owner].money+= lands[index].rent * (play[judge].isunluky + 1) / (play[judge].islucky + 1);
			}
	end:;
	}
}
