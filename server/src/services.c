#include "services.h"
#include "net.h"
#include "database.h"
#include "common.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


extern int listen_socket;
extern int epoll_fd;

void (*services[MAXSERVICES]) (int fd, struct message *msg);
static struct socket_list user_list_head;
static struct room_info rooms[MAXROOM];
static int max_room = 0;

static void list_add (int fd)
{
	printf ("enter list add...\n");

	struct socket_list *new_user = (struct socket_list *) malloc (sizeof (struct socket_list));
	if (new_user == NULL)
	{
		perror ("malloc");
		exit (-1);
	}
	new_user->socket_fd = fd;

	/*  头插法 */
	struct socket_list *tmp = user_list_head.next;
	user_list_head.next = new_user;
	new_user->next = tmp;

	printf ("exit list add...\n");

}
static void list_del (int fd)
{
	printf ("enter list delete...\n");

	struct socket_list *ptr = &user_list_head;
	while (ptr->next != NULL )
	{
		if (ptr->next->socket_fd == fd)
		{
			struct socket_list *tmp = ptr->next;
			ptr->next = tmp->next;
			free (tmp);
			break;
		}
		ptr = ptr->next;
	}

	printf ("exit list delete ...\n");
}

static void user_online (int fd)
{
	printf ("enter user online ...\n");

	list_add (fd);

	printf ("exit user online ...\n");
}

static void user_offline (int fd)
{
	printf ("enter user offline ...\n");

	list_del (fd);
	remove_fd_from_epoll (fd);
	close (fd);

	/*  若是客户端意外终止，则有可能还没有退出房间，所以要此要检查一下 */
	struct message msg;
	msg.type = EXIT_ROOM;
	int i;
	for (i = 0; i <= max_room; i++)
	{
		if (rooms[i].socket1 == fd || rooms[i].socket2 == fd)
		{
			sprintf (msg.buff, "%d", i);
			on_exit_room (fd, &msg);
		}
		break;
	}

	printf ("exit user offline...\n");
}

static void send_to_opp (int fd, struct message *msg)
{
	printf ("enter send to opp...\n");

	int index;
	sscanf (msg->buff, "%d", &index);

	if (fd == rooms[index].socket1 && rooms[index].socket2 != 0)
	{
		send_message (rooms[index].socket2, msg);
	}
	else if (fd == rooms[index].socket2 && rooms[index].socket1 != 0)
	{
		send_message (rooms[index].socket1, msg);
	}
	printf ("exit send to opp...\n");
}
static void send_to_online_user (struct message *msg)
{
	printf ("sned to online user...\n");

	struct message tmp;

	struct socket_list *ptr = user_list_head.next;
	while (ptr != NULL)
	{
		memcpy (&tmp, msg, sizeof (tmp));
		send_message (ptr->socket_fd, msg);
		ptr = ptr->next;
	}

	printf ("exit send to online user...\n");
}

static void on_receive (int socket_fd)
{
	printf ("enter on receive...\n");

	struct message msg;
	
	while (1)
	{
		ssize_t len = get_message (socket_fd, &msg);
		if (len < 0)
		{
			break;
		}
		/*  用户掉线或者主动退出游戏 */
		else if (len == 0)
		{
			user_offline (socket_fd);
			break;
		}
		else 
		{
			services[msg.type] (socket_fd, &msg);
		}
	}

	printf ("exit on receive...\n");
}
void start_services ()
{
	init_services ();
	bzero (&user_list_head, sizeof (struct socket_list));
	bzero (rooms, sizeof (rooms));

	int i;
	for (i = 0; i < MAXROOM; i++)
		rooms[i].index = i;

	printf ("running services...\n");

	struct epoll_event events[MAXEVENTS];
	int nfds;
	while (1)
	{
		nfds = epoll_wait (epoll_fd, events, MAXEVENTS, -1);
		if (nfds == -1)
		{
			perror ("epoll_wait");
			continue;
		}

		for (i = 0; i < nfds; ++i)
		{
			if (events[i].data.fd == listen_socket)
				on_accept ();
			else
				on_receive (events[i].data.fd);
		}

	}
}

void on_register (int fd, struct message *msg)
{
	printf ("enter on register...\n");

	struct user_info user;
	memset (&user, 0, sizeof(user));
	sscanf (msg->buff, "%s %s %s", user.name, user.password, user.email);

	if ( get_user_info (user.name) == NULL)
	{
		add_user(&user);
		msg->buff[0] = 0;
	}
	else
	{
		/* 用户名已经存在，发送一个错误信息反馈给客户端
		 * */
		msg->buff[0] = 1;
	}
	send_message (fd, msg);

	printf ("exit on register...\n");
}
void on_login (int fd, struct message *msg)
{
	printf ("enter on login...\n");

	struct user_info user, *info;
	memset(&user, 0, sizeof(user));
	sscanf (msg->buff, "%s %s", user.name, user.password);
	
	/* get_user_base_info 返回的是该函数的内部静态变量的内存，
	 * 不需要释放返回的内存
	 * */
	info = get_user_info (user.name);
	if (info != NULL)
	{
		if (strcmp (info->password, user.password) == 0)
		{
			/* login success
			 * 把新登录的用户加入在线用户列表
			 * 发送登录成功反馈
			 * */
			user_online (fd);
			msg->buff[0] = 0;
			memcpy (&msg->buff[1], info, sizeof (struct user_info));

		}
		else
		{
			/*  密码错误 */
			msg->buff[0] = 1;
		}
	}
	else
	{
		/* 用户不存在 */
		msg->buff[0] = 2;
	}
	send_message (fd, msg);

	printf ("exit on login...\n");
}
void on_forget_password (int fd, struct message *msg)
{
	printf ("on_forget_password\n");
}
void on_exit_game (int fd, struct message *msg)
{
	printf ("on_exit_game\n");
	user_offline (fd);
}
void on_get_room_info (int fd, struct message *msg)
{
	printf ("on_get_room_info\n");
	int i;
	for (i = 0; i < MAXROOM; i++)
	{
		if (rooms[i].state1 | rooms[i].state2) //若房间有人在
		{
			memcpy (msg->buff, &rooms[i], sizeof (struct room_info));
			msg->type = GET_ROOM_INFO;
			send_message (fd, msg);
		}
	}
}
void on_create_room (int fd, struct message *msg)
{
	printf ("on_create_room\n");
	int i;
	for (i = 0; i < MAXROOM; i++)
		if (rooms[i].state1 == 0 && rooms[i].state2 == 0)
			break;

	if (i == MAXROOM)
	{
		msg->buff[0] = 1;
		send_message (fd, msg);
		return;
	}

	if (i > max_room) max_room = i;
	rooms[i].state1 = 1;
	rooms[i].socket1 = fd;
	sscanf (msg->buff, "%s", rooms[i].name1);

	msg->buff[0] = 0;
	memcpy (msg->buff+1, &rooms[i], sizeof (struct room_info));
	send_message (fd, msg);

	msg->type = GET_ROOM_INFO;
	memcpy (msg->buff, &rooms[i], sizeof (struct room_info));
	send_to_online_user (msg);
}
void on_join_room (int fd, struct message *msg)
{
	printf ("on_join_room\n");
	int index;
	char name[MAXNAMELEN];

	sscanf (msg->buff, "%d %s", &index,name);
	if (rooms[index].state1 == 0)
	{
		/*  房间不存在 */
		msg->type = OPP_JOIN_ROOM;
		msg->buff[0] = 2;
		send_message (rooms[index].socket1, msg);
		return ;
	}
	if (rooms[index].state2 == 0)
	{
		rooms[index].socket2 = fd;
		rooms[index].state2 = ENTER;
		sprintf (rooms[index].name2, "%s",name);
		msg->buff[0] = 0;
		memcpy (msg->buff+1, &rooms[index], sizeof (rooms[index]));
		send_message (fd, msg);

		memcpy (msg->buff, &rooms[index], sizeof (rooms[index]));

		msg->type = OPP_JOIN_ROOM;
		send_message (rooms[index].socket1, msg);

		msg->type = UPDATE_ROOM;
		send_to_online_user (msg);
	}
	else 
	{
		/*  房间人满 */
		msg->buff[0] = 1;
		send_message (fd, msg);
	}
}
void on_ready (int fd, struct message *msg)
{
	printf ("on_ready\n");
	int index;
	sscanf (msg->buff, "%d", &index);
	if (rooms[index].socket1 == fd)
		rooms[index].state1 = START;
	else rooms[index].state2 = START;
	send_to_opp (fd, msg);
}
void on_move (int fd, struct message *msg)
{
	printf ("on_move\n");
	send_to_opp (fd, msg);
}
void on_regret (int fd, struct message *msg)
{
	printf ("on_regret\n");
	send_to_opp (fd, msg);
}
void on_regret_feedback (int fd, struct message *msg)
{
	printf ("on_regret_feedback\n");
	send_to_opp (fd, msg);
}
void on_draw_game (int fd, struct message *msg)
{
	printf ("on_draw_game\n");
	send_to_opp (fd, msg);
}
void on_draw_game_feedback (int fd, struct message *msg)
{
	printf ("on_draw_game_feedback\n");
	send_to_opp (fd, msg);
}
void on_give_up (int fd, struct message *msg)
{
	printf ("on_give_up\n");
	send_to_opp (fd, msg);
}

static void exit_room (int fd, struct message *msg)
{
	printf ("enter exit room...\n");

	int index;
	sscanf (msg->buff, "%d", &index);
	if (fd == rooms[index].socket1)
	{
		rooms[index].socket1 = 0;
		rooms[index].state1 = 0;
		rooms[index].name1[0] = 0;
	}
	else
	{
		rooms[index].socket2 = 0;
		rooms[index].state2 = 0;
		rooms[index].name2[0] = 0;
	}

	printf ("exit exit room...\n");

}
void on_exit_room (int fd, struct message *msg)
{
	printf ("on_exit_room\n");
	int index ;
	sscanf (msg->buff, "%d", &index);

	send_to_opp (fd, msg);
	exit_room (fd, msg);

	msg->type = UPDATE_ROOM;
	memcpy (msg->buff, &rooms[index], sizeof (rooms[index]));
	send_to_online_user (msg);
}
void on_hall_msg (int fd, struct message *msg)
{
	printf ("on_hall_msg\n");
	send_to_online_user (msg);
}
void on_room_msg (int fd, struct message *msg)
{
	printf ("on_room_msg\n");
	send_to_opp (fd, msg);
}
void on_update_personal_info (int fd, struct message *msg)
{
	printf ("on_update_personal_info\n");
	struct user_info user;
	memcpy (&user, msg->buff, sizeof (user));
	change_user_info (&user);
	send_message (fd, msg);
}
void on_quick_start_game (int fd, struct message *msg)
{
	printf ("on_quick_start_game\n");
	int i;
	for (i = 0; i <= max_room; i++)
	{
		if (rooms[i].state1 == START && rooms[i].state2 == EMPTY)
		{
			rooms[i].socket2 = fd;
			rooms[i].state2 = ENTER;
			sprintf (rooms[i].name2, "%s", msg->buff);

			msg->buff[0] = 1;
			memcpy (msg->buff+1, &rooms[i], sizeof (rooms[i]));
			send_message (fd, msg);

			memcpy (msg->buff, &rooms[i], sizeof (rooms[i]));
			msg->type = OPP_JOIN_ROOM;
			send_message (rooms[i].socket1, msg);

			msg->type = UPDATE_ROOM;
			send_to_online_user (msg);
			return;
		}
	}
	msg->buff[0] = 0;
	send_message (fd, msg);
}
void init_services ()
{
	services[REGISTER] = on_register;
	services[LOGIN] = on_login;
	services[FORGET_PASSWORD] = on_forget_password;
	services[EXIT_GAME] = on_exit_game;
	services[GET_ROOM_INFO] = on_get_room_info;
	services[CREATE_ROOM] = on_create_room;
	services[JOIN_ROOM] = on_join_room;
	services[READY] = on_ready;
	services[MOVE] = on_move;
	services[REGRET] = on_regret;
	services[REGRET_FEEDBACK] = on_regret_feedback;
	services[DRAW_GAME] = on_draw_game;
	services[DRAW_GAME_FEEDBACK] = on_draw_game_feedback;
	services[GIVE_UP] = on_give_up;
	services[EXIT_ROOM] = on_exit_room;
	services[HALL_MSG] = on_hall_msg;
	services[ROOM_MSG] = on_room_msg;
	services[UPDATE_PERSONAL_INFO] = on_update_personal_info;
	services[QUICK_START_GAME] = on_quick_start_game;
}

