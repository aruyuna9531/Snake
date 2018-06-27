#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>
#include<time.h>

#define region_x 30
#define region_y 30		//蛇的活动范围

static int Score=0;
static int straight=0;			//方向，0右 1下 2左 3上
static char gameRegion[region_x][region_y];	//游戏区域
static int food_x,food_y;		//食物的位置
int gameover=0;		//gameover信号量，为1则不能操作

void printScreen();

void putfood(){
	int x,y;
	srand(time(NULL));
	do{
		x=rand()%region_x;
		y=rand()%region_y;	//随机生成食物坐标
	}while(gameRegion[x][y]=='*');	//位置没被蛇身占据时结束
	gameRegion[x][y]='0';	//以字符0代表食物
	food_x=x;
	food_y=y;
}

void regionInitial(){
	int i,j;
	for(i=0;i<region_x;i++){
		for(j=0;j<region_y;j++){
			gameRegion[i][j]=' ';	//置空白
		}
	}
	for(i=3;i<7;i++){				//画蛇，以字符*代表蛇身
		gameRegion[3][i]='*';
	}
	putfood();						//放食物，初始化结束
}

class pos{
public:
	int x;
	int y;
	pos *pre;
	pos *next;
	pos(int xx,int yy){
	x=xx;
	y=yy;
	pre=NULL;
	next=NULL;
	}
};

class snake{
private:
	pos *head, *tail;
public:
	void createSnake(){
		head=new pos(3,6);
		head->next=new pos(3,5);
		head->next->pre=head;
		head->next->next=new pos(3,4);
		head->next->next->pre=head->next;
		head->next->next->next=new pos(3,3);
		head->next->next->next->pre=head->next->next;
		tail=head->next->next->next;
	}
	void destroySnake(){
		pos *tmp=head;
		while(head!=NULL){
			tmp=head;
			head=head->next;
			delete tmp;
			tmp=NULL;
		}
	}
	void printSnake(){
		pos *s=head;
		while(s!=NULL){
			printf("(%d,%d)",s->x,s->y);
			s=s->next;
		}
		printf("\n");
		s=tail;
		while(s!=NULL){
			printf("(%d,%d)",s->x,s->y);
			s=s->pre;
		}
		printf("\n");
	}
	int moveForward(){		//前进，通常返回0，gameover返回1
		int eatFood=0;
		switch(straight){
		case 0:if(gameRegion[head->x][head->y+1]=='0')eatFood=1;break;
		case 1:if(gameRegion[head->x+1][head->y]=='0')eatFood=1;break;
		case 2:if(gameRegion[head->x][head->y-1]=='0')eatFood=1;break;
		case 3:if(gameRegion[head->x-1][head->y]=='0')eatFood=1;break;
		}
		int x=0,y=0;
		if(eatFood==0){					//没吃
			//去尾
			pos *tmp=tail;
			gameRegion[tmp->x][tmp->y]=' ';	//清空该位置
			tail=tail->pre;
			tail->next=NULL;
			//加头
			switch(straight){
			case 0:
				x=head->x;
				y=head->y+1;
				break;
			case 1:
				x=head->x+1;
				y=head->y;
				break;
			case 2:
				x=head->x;
				y=head->y-1;
				break;
			case 3:
				x=head->x-1;
				y=head->y;
				break;
			}
			if(x>=region_x || y>=region_y || x<0 || y<0 || gameRegion[x][y]=='*'){
				gameover=1;
				return 1;
			}
			tmp->x=x;
			tmp->y=y;
			tmp->next=head;
			head->pre=tmp;
			head=tmp;
			tmp->pre=NULL;
			gameRegion[head->x][head->y]='*';
		}
		else{
			//吃了食物，加头
			int new_x,new_y;
			switch(straight){
			case 0:
				new_x=head->x;
				new_y=head->y+1;
				break;
			case 1:
				new_x=head->x+1;
				new_y=head->y;
				break;
			case 2:
				new_x=head->x;
				new_y=head->y-1;
				break;
			case 3:
				new_x=head->x-1;
				new_y=head->y;
				break;
			}
			head->pre=new pos(new_x,new_y);
			head->pre->next=head;
			head=head->pre;
			gameRegion[head->x][head->y]='*';
			Score++;
			putfood();
		}
		printScreen();
		return 0;
	}
};

snake s;

void printScreen(){			//画版面
	int i,j;
	system("cls");						//清除屏幕
	printf("Score\t\n",Score);		//第一行写分数
	for(i=0;i<region_y+2;i++)printf("#");	//以#号代表边界
	printf("\n");
	for(i=0;i<region_x;i++){
		printf("#");
		for(j=0;j<region_y;j++)printf("%c",gameRegion[i][j]);
		printf("#\n");
	}										//画正体
	for(i=0;i<region_y+2;i++)printf("#");	//底部边界
	printf("\n");
}

int difficulty(){
	printf("选择游戏难度：\n");
	printf("1.Easy（蛇移动极慢）\n");
	printf("2.Normal（蛇移动速度中等）\n");
	printf("3.Hard（蛇移动较快）\n");
	printf("4.Expert（蛇移动极快）\n");
	printf("5.Master（蛇一直在动）\n");
	int t;
	scanf("%d",&t);
	switch(t){
	case 1:return 1000;
	case 2:return 500;
	case 3:return 250;
	case 4:return 100;
	case 5:return 20;
	}
}

//利用多线程表示游戏进展（接收按键转换蛇方向/蛇向前移动

DWORD WINAPI ChangeDirect(LPVOID lpParamter)
{
	while(gameover==0){
		int c=getch();
		switch(c){
		case 'w':if(straight!=1)straight=3;break;
		case 'a':if(straight!=0)straight=2;break;
		case 's':if(straight!=3)straight=1;break;
		case 'd':if(straight!=2)straight=0;break;
		}
		//printScreen();
	}
    return 0L;
}

int main()
{
	int diff=difficulty();
	s.createSnake();
	regionInitial();
    HANDLE hThread = CreateThread(NULL, 0, ChangeDirect, NULL, 0, NULL);
    CloseHandle(hThread);
    while(gameover==0 && s.moveForward()==0){
		Sleep(diff);
	}
	printf("GAME OVER，按ESC退出\n");
	s.destroySnake();
	int exit;
	do{
		exit=getch();
	}while(exit!=27);
    return 0;
}
