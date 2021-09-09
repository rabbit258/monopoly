#include<stdio.h>
#include<stdio.h>//�����ַ����봿���ݵ�ת��
#include<graphics.h>//����ͼ�����
#include<mmsystem.h>//�������bgm
#include<time.h>//��������
#pragma comment(lib,"winmm.lib")

void cover(void);//һ�����������ķ���
void initdata(void);//��ʼ������
void initmap(void);//������ͼ
void bgm(void);//��������,�������������л�����
void players(void);//�����ҵ����ݺʹ�����ҵ�Բ
void turn(void);//�жϻغϿ�ʼ�����
void tip(void);//���������������
void playermove(void);//�ƶ�
void begin(void);//��㽱��
void land(void);//���⺯��
void jail(void);//�����ж�
void fate(void);//�������
void judger(void);//���ֺ���Ϸ�����ж�
int dice(void);//����

enum color { blue, red, yellow, green };
char colors[4][20] = { {"��ң�blue"},{"��ң�red"},{"��ң�yellow"},{"��ң�green"} };
char suitation[3][20] = { {"״̬������"} ,{"״̬��������"},{"״̬������"} };

struct {
	enum color playcolor;//�����ĸ����
	int money;
	char char_money[10];//������������ַ���
	int state;//��ǰ״̬
	int propnum;//��������
	int x, y;//����
	int islucky;//����˥���ж�
	int isunluky;
	int ishum;//�ж��Ƿ����������
}play[4];

struct {
	int owner;//-1�������˹���
	int prince;//�������
	int level;//���صȼ�
	int rent;//���
	int x1, x2, y1, y2;//��������꣬�Ա���
}lands[8];

int step, judge=3, turnend = 1,record,num_hum=1;//һЩ��Ҫ��ȫ������,

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
			judge++;//��̭�������
		judge %= 4;//��ֹԽ��
	}
}




void cover(void)
{
	
	initgraph(800, 800);
	mciSendString("open start.mp3",0, 0, 0);
	mciSendString("play start.mp3", 0, 0, 0);
	HWND hwnd = GetHWnd();//���
	SetWindowText(hwnd, "����");
	int x, y;
	IMAGE img;
	MOUSEMSG msg;
	char tip[] = "������⴦��ʼ��Ϸ";
	int width = textwidth(tip);//���ھ�����ʾ
	loadimage(&img, "./cover.png");
	setbkmode(TRANSPARENT);
	for (x = 0, y = 800;y >= 0; y -= 1)
	{
		cleardevice();
		putimage(x, y, &img);
		Sleep(2);
	}//ʵ�ַ�����ƶ�

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
		Sleep(5);//��ʾ����
	}//��ʾ�û�������⴦��ʼ��Ϸ
	mciSendString("close start.mp3", 0, 0, 0);
}
void initdata(void)
{
	int index;//�޸�num���޸������������
	for (index = 0;index < num_hum;index++)
		play[index].ishum = 1;
	for (index = 0;index < 4;index++)
	{
		play[index].playcolor = (enum color)index;//c++����ֱ����int����ֵ����ֵ
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
		}//ͳһ�������

	}
	//��������ĸ�ֵ,�Ѻ���
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
	//���س�ʼ�۸���������ĸ�ֵ
	for (int count = 0, i = 1;count < 8;count++)//i�����ж�
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
	switch (num_out)//����̭����Ϊ�ж�ֵ
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
	char cast[] = { "Ͷ����" };
	int height, width;

	setbkcolor(WHITE);
	cleardevice();
	loadimage(&img, "./map.png");
	putimage(0, 200, &img);
	height = (100 - textheight(cast)) / 2;
	width = (200 - textwidth(cast)) / 2;

	setcolor(RED);
	rectangle(300, 450, 500, 550);
	outtextxy(300 + width, 450 + height, cast);//���ӵİ�ť
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
		outtextxy(index * 200, 80, "���н��:");
		sprintf(play[index].char_money, "%d", play[index].money);
		outtextxy(index * 200 + 80, 80, play[index].char_money);
		outtextxy(index * 200, 140, suitation[play[index].state]);//������֣���״̬�ͳ�ʼλ��
		if (play[index].state != 2)
			solidcircle(play[index].x, play[index].y, 15);//������ҵ�Բ
	}
	//����ӵ���ߵ��ֿ���ʾ
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
			judge %= 4;//��ֹԽ��
		}
		else
			turnend = 1;
	}
	
	while (play[judge].state == 1)
	{
		play[judge].state = 0;
		judge++;
		judge %= 4;//��ֹԽ��
	}
}
void tip(void)
{

	while (play[judge].ishum && record==step)//��ʾ������ҵ��
	{

		MOUSEMSG msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN && msg.x > 300 && msg.x < 500 && msg.y>450 && msg.y < 550)
		{
			setfillcolor(BLUE);
			solidrectangle(300, 450, 500, 550);
			break;//�������
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
			else if (play[judge].y < 800 && play[judge].y > 304 && play[judge].x < 132)//��ͼ�ĵȷ�Ȼ���ж��ƶ�
				play[judge].y -= 95;
			step--;
			Sleep(500);//��ֹ�ƶ�����
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
			outtextxy(judge * 200, 180, "��㽱��+500");
			Sleep(500);
		}
}
int dice(void)
{
	int num = 0;
	srand((unsigned int)time(NULL));
	return num = (rand() + 13 * step) % 5 + 1;//�������4�����ͬʱ���ӵ�����ͬ�����
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
	int  num, index = judge;//һ��ʼ˼·���ˣ���������һ��һ���滻��
	if (turnend)
	{
		if (step == 0)//��Ϊ��ͼ���û�ͼ���������е㲻����������ֻ���뵽�����ְ취���ж��ˣ�ʵ����Ҳ���Է����ж�
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
				break;//�������ֲ�ͬ��ҵ�����
			}
			switch (num)
			{
			case 1:
				play[index].money += 2000;
				outtextxy(index * 200, 180, "����֮��+2000");
				Sleep(500);
				break;
			case 2:
				play[index].money -= 2000;
				outtextxy(index * 200, 180, "��ù��-2000");
				Sleep(500);
				break;
			case 3:
				outtextxy(index * 200, 180, "������");
				play[index].islucky = 1;
				Sleep(500);
				break;
			case 4:
				outtextxy(index * 200, 180, "˥����");
				play[index].isunluky = 1;
				Sleep(500);//��ֹ��Ϣһ������
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
		HWND hwnd = GetHWnd();//���
		if (count_humout == num_hum)//�������ȫ����̭��Ϸ����
		{
	 		MessageBox(hwnd, "���Ʋ���", "��Ϸʧ��", MB_OK);
			closegraph();
			exit(0);//��������
		}
		else if (count_cptout == 4 - num_hum)
		{
			MessageBox(hwnd, "����̭�����еĵ���", "��Ϸʤ��", MB_OK);
			closegraph();
			exit(0);//��������
		}
	}
}
void land(void)
{
	HWND hwnd = GetHWnd();//���
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
					goto end;//����Ǯ����������
				else if (play[judge].ishum)
				{
					if (IDCANCEL == MessageBox(hwnd, "�Ƿ��������", "��ʾ", MB_OKCANCEL))
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
						if (IDCANCEL == MessageBox(hwnd, "�Ƿ�����������", "��ʾ", MB_OKCANCEL))
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
