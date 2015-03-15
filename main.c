////////////////////////////////////////////  
//五子棋                                  //
//Author:Bread                            //      
//Date: 2015.1.5                          //
//Update: 2015.3.13                       //
////////////////////////////////////////////  
//定义：无子为0，黑子为1，白子为-1.
/*规则：棋盘大小为N*N。黑棋先手，落子无悔。（pvr时机器人先执黑）
当A方存在连续（包括斜向）的五个棋子时，A方胜利，此局结束。
下局由另一方执黑。
无子：■
白子：●
黑子：○
*/
/*格点价值：
9.己方能连成五子或以上 5 0/5 1/5 2 c
8.对方能连成五子或以上 5 0/5 1/5 2 -c
7.己方能连成四子（无阻碍）4 0 c
  己方能连成一个三子（无）与一个四子（一） 3 0 4 1 c
  己方能连成两个四子（一阻碍）4 1 4 1 c
6.对方能连成四子（无阻碍）4 0 -c
  对方能连成两个三子（无阻碍） 3 0 3 0 -c
  己方能连成两个三子（无阻碍） 3 0 3 0 c
  对方能连成一个三子（无）与一个四子（一） 3 0 4 1 -c
  对方能连成两个四子（一阻碍）4 1 4 1 -c
5.己方能连成四子（一阻碍）4 1 c
  己方能连成三子（无阻碍）3 0 c
4.对方能连成四子（一阻碍）4 1 -c
  对方能连成三子（无阻碍）3 0 -c
3.对方能连成三子（一阻碍）3 1 -c
  己方能连成三子（一阻碍）3 1 c
  己方能连成二子（无阻碍）2 0 c
  对方能连成二子（无阻碍）2 0 -c
2.其它
1.[1->4] 8 [any]
Ps.0 代表已有棋子，不能下。
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define size 15

typedef struct Cb {
	int Value;
	int Colour;
} Chessboard;
typedef struct location {
	int x;
	int y;
} location;
typedef struct {
	int num;
	int block[2];
} Case;
void getcase(Chessboard[size][size], Case[4], location p, int);//输入棋盘 Case容器 坐标 颜色 扫描并得到四个情况
void printChessboard(Chessboard[size][size]);
int searchcase(Case[4], int, int);// 搜寻满足 num block 的情况个数 
int ValueOfLocation(Chessboard[size][size], location, int);//输入棋盘 坐标 和颜色，返回此点的优先级。
location PrimaryRobot(Chessboard[size][size], int);//输入棋盘和机器人颜色，返回坐标。
location SmartRobot(Chessboard[size][size], int, int);//输入棋盘、机器人颜色以及样本量，返回坐标。
int PlayChessSimulation(Chessboard[size][size], location, int);//输入棋盘,初始落子点和开局颜色，返回胜利颜色。

int main() {
	Chessboard Cb[size][size];
	location p;
	int i, j;
	int PlayerColour = 1, Now = 1, flag = 1, Value = 0, Step = 0;
	for(i = 0; i < size; ++i) {
		for(j = 0; j < size; ++j) {
			Cb[i][j].Colour = 0;
			Cb[i][j].Value = 0;
		}
	}
	scanf("%d", &PlayerColour);//input 1 or -1
	if(PlayerColour*PlayerColour != 1) {
		flag = 0;
		printf("Error! : 000\n");
	}
	while(flag) {
		printChessboard(Cb);
		Start:
		if(PlayerColour == Now) {
			scanf("%d %d", &p.x, &p.y);//得到坐标
			--p.x;
			--p.y;
		}
		else {
			//得到坐标
			if(Step > 4) {
				p = SmartRobot(Cb, Now, Step*20);
			}
			else {
				p = PrimaryRobot(Cb, Now);
			}
		}
		if(!(p.x >= 0 && p.y >= 0 && p.x < size && p.y < size) || Cb[p.x][p.y].Colour) { //检查坐标合法性
			printf("Error! : 001\n");
			goto Start;
		}
		Value = ValueOfLocation(Cb, p, Now); //得到坐标价值
		Cb[p.x][p.y].Colour = Now;//落子
		++Step;//计步
		if(Value == 9 || Step == size*size) { //此方获胜 || 平局
			printChessboard(Cb);
			printf("Game over！ Player：%d Winner：%d\n", PlayerColour, (Step == size * size) ? 0 : (Step % 2) ? 1 : -1);
			break;
		}
		Now *= -1;
	}
	return 0;
}

void getcase(Chessboard Cb[size][size], Case allcase[4], location p, int Colour) {
	int i, flag = 1, e[4][2] = { 0 };
	for(i = 0; i < 4; ++i) { //初始化情况
		allcase[i].num = 1;//
		allcase[i].block[0] = 0;
		allcase[i].block[1] = 0;
	}
	for(i = 1; i < 5; ++i) {
		////////case0////////
		if(!e[0][0] && p.x - i >= 0 && Cb[p.x - i][p.y].Colour == Colour) { // 行 向上计数
			++allcase[0].num;
		}
		else {
			if(!e[0][0] && (p.x - i < 0 || Cb[p.x - i][p.y].Colour == -Colour)) { // 第一次进入且越界或有障碍
				allcase[0].block[0] = 1;//上障碍存在
			}
			e[0][0] = 1;
		}
		if(!e[0][1] && p.x + i < size && Cb[p.x + i][p.y].Colour == Colour) {// 行 向下计数
			++allcase[0].num;
		}
		else {
			if(!e[0][1] && (p.x + i >= size || Cb[p.x + i][p.y].Colour == -Colour)) { // 第一次进入且越界或有障碍
				allcase[0].block[1] = 1;//下障碍存在
			}
			e[0][1] = 1;
		}
		////////case1////////
		if(!e[1][0] && p.y - i >= 0 && Cb[p.x][p.y - i].Colour == Colour) { // 列 向左计数
			++allcase[1].num;
		}
		else {
			if(!e[1][0] && (p.y - i < 0 || Cb[p.x][p.y - i].Colour == -Colour)) { // 第一次进入且越界或有障碍
				allcase[1].block[0] = 1;//左障碍存在
			}
			e[1][0] = 1;
		}
		if(!e[1][1] && p.y + i < size && Cb[p.x][p.y + i].Colour == Colour) { // 列 向右计数
			++allcase[1].num;
		}
		else {
			if(!e[1][1] && (p.y + i >= size || Cb[p.x][p.y + i].Colour == -Colour)) { // 第一次进入且越界或有障碍
				allcase[1].block[1] = 1;//右障碍存在
			}
			e[1][1] = 1;
		}
		////////case2////////
		if(!e[2][0] && p.x - i >= 0 && p.y + i < size && Cb[p.x - i][p.y + i].Colour == Colour) { // 斜 向上计数
			++allcase[2].num;
		}
		else {
			if(!e[2][0] && (p.x - i < 0 || p.y + i >= size || Cb[p.x - i][p.y + i].Colour == -Colour)) { // 第一次进入且越界或有障碍
				allcase[2].block[0] = 1;//上障碍存在
			}
			e[2][0] = 1;
		}
		if(!e[2][1] && p.x + i < size && p.y - i >= 0 && Cb[p.x + i][p.y - i].Colour == Colour) { // 斜 向下计数
			++allcase[2].num;
		}
		else {
			if(!e[2][1] && (p.x + i >= size || p.y - i < 0 || Cb[p.x + i][p.y - i].Colour == -Colour)) { // 第一次进入且越界或有障碍
				allcase[2].block[1] = 1;//下障碍存在
			}
			e[2][1] = 1;
		}
		////////case3////////
		if(!e[3][0] && p.x - i >= 0 && p.y - i >= 0 && Cb[p.x - i][p.y - i].Colour == Colour) { // 反斜 向上计数
			++allcase[3].num;
		}
		else {
			if(!e[3][0] && (p.x - i < 0 || p.y - i < 0 || Cb[p.x - i][p.y - i].Colour == -Colour)) {
				allcase[3].block[0] = 1;//上障碍存在
			}
			e[3][0] = 1;
		}
		if(!e[3][1] && p.x + i < size && p.y + i < size && Cb[p.x + i][p.y + i].Colour == Colour) { // 反斜 向下计数
			++allcase[3].num;
		}
		else {
			if(!e[3][1] && (p.x + i >= size || p.y + i >= size || Cb[p.x + i][p.y + i].Colour == -Colour)) {
				allcase[3].block[1] = 1;//下障碍存在
			}
			e[3][1] = 1;
		}
	}
}
void printChessboard(Chessboard Cb[size][size]) {
	int i, j;
	for(i = 0; i <= size; ++i) {
		printf("%2d", i);
	}
	printf("\n");
	for(i = 0; i < size; ++i) {
		printf("%2d", i + 1);
		for(j = 0; j < size; ++j) {
			if(Cb[i][j].Colour) {
				if(Cb[i][j].Colour == 1) {
					printf("○");
				}
				else {
					printf("●");
				}
			}
			else {
				printf("■");
			}
		}
		printf("%-2d\n", i + 1);
	}
	for(i = 0; i <= size; ++i) {
		printf("%2d", i);
	}
	printf("\n");
}
int searchcase(Case allcase[4], int num, int block) {
	int i, flag = 0;
	if(num == 5) {
		for(i = 0; i < 4; ++i) {
			if(allcase[i].num >= 5) {
				++flag;
			}
		}
	}
	else {
		for(i = 0; i < 4; ++i) {
			if(allcase[i].num == num) {
				switch(block) {
				case 0:
					if(!(allcase[i].block[0] + allcase[i].block[1])) {
						++flag;
					}
					break;
				case 1:
					if((allcase[i].block[0] + allcase[i].block[1]) == 1) {
						++flag;
					}
					break;
				case 2:
					if(allcase[i].block[0] && allcase[i].block[1]) {
						++flag;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	
	return flag;
}
int ValueOfLocation(Chessboard Cb[size][size], location p, int Colour) {
	int i, fullblock = 1;
	Case allcase[4], againstcase[4];
	Cb[p.x][p.y].Value = 0;
	if(!Cb[p.x][p.y].Colour) {
		/*getcase*/
		getcase(Cb, allcase, p, Colour); //己方情况
		getcase(Cb, againstcase, p, -Colour); //对手情况
		for(i = 0; i < 4; ++i) { //是否完全被阻碍
			fullblock *= allcase[i].block[0] * allcase[i].block[1];
		}
		if(searchcase(allcase, 5, 0)) { // 5(->9) 0->2
			Cb[p.x][p.y].Value = 9;
		}
		else if(searchcase(againstcase, 5, 0)) {
			Cb[p.x][p.y].Value = 8;
		}
		else if(searchcase(allcase, 4, 0) || (searchcase(allcase, 4, 1) && searchcase(allcase, 3, 0)) || searchcase(allcase, 4, 1) >= 2) {
			Cb[p.x][p.y].Value = 7;
		}
		else if(searchcase(allcase, 3, 0) >= 2 || searchcase(againstcase, 4, 0)) {
			Cb[p.x][p.y].Value = 6;
		}
		else if(searchcase(againstcase, 3, 0) >= 2 || (searchcase(againstcase, 3, 0) && searchcase(againstcase, 4, 1))) {
			Cb[p.x][p.y].Value = 6;
		}
		else if(searchcase(againstcase, 4, 1) >= 2) {
			Cb[p.x][p.y].Value = 6;
		}
		else if(searchcase(allcase, 4, 1) || searchcase(allcase, 3, 0)) {
			Cb[p.x][p.y].Value = 5;
		}
		else if(searchcase(againstcase, 4, 1) || searchcase(againstcase, 3, 0)) {
			Cb[p.x][p.y].Value = 4;
		}
		else if(searchcase(againstcase, 3, 1) || searchcase(allcase, 3, 1)) {
			Cb[p.x][p.y].Value = 3;
		}
		else if(searchcase(againstcase, 2, 0) || searchcase(allcase, 2, 0)) {
			Cb[p.x][p.y].Value = 3;
		}
		else if(fullblock) {
			Cb[p.x][p.y].Value = 1;
		}
		else {
			Cb[p.x][p.y].Value = 2;
		}
	}
	return Cb[p.x][p.y].Value;
}
location PrimaryRobot(Chessboard Cb[size][size], int Colour) {
	int i, j, max = 1, sum = 0, num;
	location p, Choice[size*size];;
	srand((unsigned)time(NULL));
	for(i = 0; i < size; ++i) {
		for(j = 0; j < size; ++j) {
			p.x = i;
			p.y = j;	
			if(ValueOfLocation(Cb, p, Colour) == max) {
				Choice[sum] = p;
				++sum;
			}
			else if(Cb[p.x][p.y].Value > max) {
				Choice[0] = p;
				sum = 1;
				max = Cb[p.x][p.y].Value;
			}
		}
	}
	num = rand() % sum;
	return Choice[num];
}
location SmartRobot(Chessboard Cb[size][size], int Colour,int SMLT) {
	int i, j, max = 1, index = 0, sum = 0, num[size*size] = { 0 };
	location p, Choice[size*size];
	srand((unsigned)time(NULL));
	for(i = 0; i < size; ++i) {
		for(j = 0; j < size; ++j) {
			p.x = i;
			p.y = j;
			if(ValueOfLocation(Cb, p, Colour) == max) {
				Choice[sum] = p;
				++sum;
			}
			else if(Cb[p.x][p.y].Value > max) {
				Choice[0] = p;
				sum = 1;
				max = Cb[p.x][p.y].Value;
			}
		}
	}
	
	for(i = 0; i < sum; ++i) { //进行模拟对弈
		for(j = 0; j < SMLT; ++j) {
			if(PlayChessSimulation(Cb, Choice[i], Colour) == Colour) {
				++num[i];
			}
		}
	}
	max = -1;
	for(i = 0; i < sum; ++i) {
		if(num[i] > max) {
			max = num[i];
			index = i;
		}
	}
	return Choice[index];
}
int PlayChessSimulation(Chessboard DefaultCb[size][size], location p, int DefaultColour) {
	Chessboard Cb[size][size];
	int i, j;
	int Now = -DefaultColour, flag = 1, Value = 0, Step = 1;
	for(i = 0; i < size; ++i) { //初始化为起始棋局
		for(j = 0; j < size; ++j) {
			Cb[i][j].Colour = DefaultCb[i][j].Colour;
			if(Cb[i][j].Colour) {
				++Step;
			}
		}
	}
	Cb[p.x][p.y].Colour = DefaultColour;//落下起始子
	while(flag) {
		if(Value == 9 || Step == size*size) { //此方获胜 || 平局
			//printChessboard(Cb);
			//printf("Game over:%d %d\n", PlayerColour, (Step == size * size) ? 0 : (Step % 2) ? 1 : -1);// 输出玩家颜色，以及获胜颜色。
			break;
		}
		//printChessboard(Cb);
		if(DefaultColour == Now) {
			p = PrimaryRobot(Cb, Now);//得到坐标
		}
		else {
			p = PrimaryRobot(Cb, Now);//得到坐标
			//= SmartRobot(Cb, Now);
		}
		Value = ValueOfLocation(Cb, p, Now); //得到坐标价值
		Cb[p.x][p.y].Colour = Now;//落子
		++Step;//计步
		Now *= -1;
	}
	return (Step == size * size) ? 0 : (Step % 2) ? 1 : -1;
}
