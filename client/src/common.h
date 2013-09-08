#ifndef COMMON_H_INCLUDE
#define COMMON_H_INCLUDE

#define PORT 1501
#define MAXSERVICES 32
#define MAXBUFFSIZE 512
#define MAXNAMELEN 32
#define MAXEMAILLEN 32
#define MAXPASSWORDLEN 32

#define BLANK 0
#define RED  1
#define BLACK 2

/*  玩家状态 */
#define EMPTY 0
#define ENTER 1
#define START 2


/*  message type  */
#define REGISTER 0
#define LOGIN 1
#define FORGET_PASSWORD 2
#define EXIT_GAME 3
#define GET_ROOM_INFO 4
#define CREATE_ROOM 5
#define JOIN_ROOM 6
#define OPP_JOIN_ROOM 7
#define UPDATE_ROOM 8
#define READY 9
#define MOVE 10
#define REGRET 11
#define REGRET_FEEDBACK 12
#define DRAW_GAME 13
#define DRAW_GAME_FEEDBACK 14
#define GIVE_UP 15
#define EXIT_ROOM 16
#define HALL_MSG 17
#define ROOM_MSG 18
#define UPDATE_PERSONAL_INFO 19
#define WATCH_GAME 20
#define QUICK_START_GAME 21


struct message
{
	int type;
	char buff[MAXBUFFSIZE];
};

struct user_info
{
	char name[MAXNAMELEN];
	char password[MAXPASSWORDLEN];
	char email[MAXEMAILLEN];
};

struct room_info
{
	int index;
	int state1;
	int state2;
	int socket1;
	int socket2;
	char player1[MAXNAMELEN];
	char player2[MAXNAMELEN];
	char password[MAXPASSWORDLEN];
};


#endif
