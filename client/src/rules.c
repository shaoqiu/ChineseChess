#include "rules.h"
#include "chessman.h"
#include <stdlib.h>
#include <gtk/gtk.h>

static int line (int chessbord[10][9], int src_x, int src_y, int dst_x, int dst_y)
{

	int i,low,hig,count=0;

	if (src_x == dst_x)
	{
		if (src_y < dst_y) { low = src_y; hig = dst_y; }
		else { low = dst_y; hig = src_y; }
		for (i = low+1; i < hig; i++)
			if (chessbord[i][src_x] != OO)
				count++;
		return count;
	}
	else if (src_y == dst_y)
	{
		if (src_x < dst_x) { low = src_x; hig = dst_x; }
		else { low = dst_x; hig = src_x; }
		for (i = low+1; i < hig; i++)
			if (chessbord[src_y][i] != OO)
				count++;
		return count;
	}
	return -1;
}
int can_move (int chessbord[10][9], int src_x, int src_y, int dst_x, int dst_y)
{
	int i, j;
	switch (chessbord[src_y][src_x] - 1)
	{
			/*  che */
		case RR:
		case BR:
			if (line (chessbord, src_x, src_y, dst_x, dst_y) != 0)
				return 0;
			break;
			/*  ma */
		case RN:
		case BN:
			//马走日:
			if(!(
				(abs(dst_y-src_y)==1&&abs(dst_x-src_x)==2) ||
				(abs(dst_y-src_y)==2&&abs(dst_x-src_x)==1)
				))
				return 0;
			
			//找马脚:
			if		(dst_y-src_y==2){i=src_y+1;j=src_x;}
			else if	(src_y-dst_y==2){i=src_y-1;j=src_x;}
			else if	(dst_x-src_x==2){i=src_y;j=src_x+1;}
			else if	(src_x-dst_x==2){i=src_y;j=src_x-1;}
			
			//绊马脚:
			if(chessbord[i][j]!=OO)return 0;
			break;
			/*  xiang */
		case RB:
		case BB:
			//相不能过河
			if (dst_y < 5) return 0;
			//相走田:
			if(abs(src_y-dst_y)!=2||abs(src_x-dst_x)!=2)return 0;
			//相心:
			if(chessbord[(src_y+dst_y)/2][(src_x+dst_x)/2]!=OO)return 0;
			break;
			/*  shi */
		case RA:
		case BA:
			//目标点不在九宫内
			if (dst_y < 7 || dst_x < 3 || dst_x > 5) return 0;
			//士走斜线一步:
			if(!(abs(src_x-dst_x) == 1 && abs(dst_y-src_y) == 1))return 0;	
			break;
			/*  shuai */
		case RK:
		case BK:
			//目标点不在九宫内
			if (dst_y < 7 || dst_x < 3 || dst_x > 5)
			{
				//照将
				if (chessbord[dst_y][dst_x] == RK || chessbord[dst_y][dst_x] == BK)
				{
					if (line (chessbord, src_x, src_y, dst_x, dst_y) == 0)
						return 1;
					else return 0;
				}
				else return 0;
			}
			//将帅只走一步直线:
			if(abs(src_x-dst_x)+abs(dst_y-src_y)>1)return 0;
			break;
			/*  pao */
		case RC:
		case BC:
			//不吃子时
			if (chessbord[dst_y][dst_x] == OO)
			{
				if (line(chessbord, src_x,src_y,dst_x,dst_y) == 0)
					return 1;
				else return 0;
			}
			//吃子时
			else
			{
				if (line(chessbord, src_x,src_y,dst_x,dst_y) == 1)
					return 1;
				else return 0;
			}
			break;
			/*  bing */
		case RP:
		case BP:
			//不能往后走
			if (dst_y <= src_y)
			{
				//过河前
				if (src_y > 4)
				{
					if (dst_x == src_x && (src_y - dst_y) == 1)
						return 1;
					else return 0;
				}
				else//过河后
				{
					if ((src_y - dst_y + abs(src_x -dst_x)) == 1)
						return 1;
					else return 0;
				}
			}
			else return 0;
			break;
	}
	return 1;
}
