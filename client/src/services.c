#include "services.h"
#include "net.h"
#include "chessbord.h"
#include "ui.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

void (*services[MAXSERVICES]) (int fd, struct message *msg);

static int accept_request (const char *request)
{
	GtkWidget *dialog, *content_area, *label;
	dialog = gtk_dialog_new_with_buttons ("opp's request",
			NULL,
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_YES,
			GTK_RESPONSE_ACCEPT,
			GTK_STOCK_NO,
			GTK_RESPONSE_REJECT,
			NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	label = gtk_label_new (request);
	gtk_container_add (GTK_CONTAINER (content_area), label);
	gtk_widget_show_all (dialog);

	int result = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	if (result == GTK_RESPONSE_ACCEPT)
		return 1;
	else return 0;
}
void on_register (int fd, struct message *msg)
{
	printf ("on_register\n");
	printf ("msg.type = %d\n", msg->type);
	printf ("msg.buff = %d\n", msg->buff[0]);

	if (msg->buff[0] == 0)
	{
		show_message_dialog (GTK_MESSAGE_INFO, "注册成功！");
		hide_register_dialog ();
		show_login_dialog ();
	}
	else
	{
		show_message_dialog (GTK_MESSAGE_ERROR,  "该用户名已被注册，请选择其他用户");
	}
}
void on_login (int fd, struct message *msg)
{
	printf ("on_login\n");
	printf ("msg.type = %d\n", msg->type);
	printf ("msg.buff = %d\n", msg->buff[0]);
	if (msg->buff[0] == 0)
	{
		hide_login_dialog ();
		create_main_dialog ();
		show_main_dialog ();
		msg->type = GET_ROOM_INFO;
		send_message (msg);
		//play_sound ("sound/bg.mp3",0);
	}
	else if (msg->buff[0] == 1)
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "密码错误!");
	}
	else
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "用户不存在!");
	}
}
void on_forget_password (int fd, struct message *msg)
{
	printf ("on_forget_password\n");
}
void on_exit_game (int fd, struct message *msg)
{
	printf ("on_exit_game\n");
}
void on_get_room_info (int fd, struct message *msg)
{
	printf ("on_get_room_info\n");
	struct room_info room;
	memcpy (&room, msg->buff, sizeof (room));
	add_room (&room);
}
void on_create_room (int fd, struct message *msg)
{
	printf ("on_create_room\n");
	if (msg->buff[0])
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "没有空闲的房间!\n");
		return ;
	}
	hide_main_dialog ();
	show_room_dialog ();
	init_room_dialog (msg->buff+1);
}
void on_join_room (int fd, struct message *msg)
{
	printf ("on_join_room\n");
	if (msg->buff[0] == 2)
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "房间不存在");
		return ;
	}
	if (msg->buff[0] == 1)
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "房间人满");
		return ;
	}
	hide_main_dialog ();
	show_room_dialog ();
	init_room_dialog (msg->buff+1);
}
void on_opp_join_room (int fd, struct message *msg)
{
	printf ("on_opp_join_room\n");
	init_room_dialog (msg->buff);
}
void on_update_room (int fd, struct message *msg)
{
	printf ("on_update_room\n");
	struct room_info troom;
	memcpy (&troom, msg->buff, sizeof (troom));
	del_room (troom.index);
	if (troom.state1 != EMPTY || troom.state2 != EMPTY)
		add_room (&troom);
}
void on_ready (int fd, struct message *msg)
{
	printf ("on_ready\n");
	if (get_color () == RED)
	{
		set_player2_state (START);
	}
	else 
	{
		set_player1_state (START);
	}
}
void on_move (int fd, struct message *msg)
{
	printf ("on_move\n");
	int number, src_x, src_y, dst_x, dst_y;
	sscanf (msg->buff, "%d %d %d %d %d", &number, &src_x, &src_y, &dst_x, &dst_y);
	opp_move_chessman (src_x, src_y, dst_x, dst_y);
}
void on_regret (int fd, struct message *msg)
{
	printf ("on_regret\n");

	msg->type = REGRET_FEEDBACK;
	if (accept_request ("对方请求悔棋\n是否同意"))
	{
		sprintf (msg->buff, "%s 1", get_room_number ());
		/*  同意悔棋了，所以要把棋盘恢复到上一步的局面 */
		unset_turn ();
		restore_chessbord ();
		set_regret_flag (FALSE);
		min_step ();
	}
	else
	{
		sprintf (msg->buff, "%s 0", get_room_number ());
	}
	send_message (msg);
}
void on_regret_feedback (int fd, struct message *msg)
{
	printf ("on_regret_feedback\n");
	int result;
	char number[32];
	sscanf (msg->buff, "%s %d", number, &result);
	if (result == 1)
	{
		/*  对方同意悔棋，把棋盘恢复到上一步的局面 */
		set_turn ();
		restore_chessbord ();
		min_step ();
	}
	else 
		show_message_dialog (GTK_MESSAGE_ERROR, "对方拒绝您的请求");
}
void on_draw_game (int fd, struct message *msg)
{
	printf ("on_draw_game\n");
	msg->type = DRAW_GAME_FEEDBACK;
	if (accept_request ("对方请求和棋\n是否同意"))
	{
		sprintf (msg->buff, "%s 1", get_room_number ());
		/*  同意和棋 */
		game_draw ();
	}
	else
	{
		sprintf (msg->buff, "%s 0", get_room_number ());
	}
	send_message (msg);
}
void on_draw_game_feedback (int fd, struct message *msg)
{
	printf ("on_draw_game_feedback \n");
	int result;
	char number[32];
	sscanf (msg->buff, "%s %d", number, &result);
	if (result == 1)
	{
		/*  对方同意和棋 */
		game_draw ();
	}
	else 
		show_message_dialog (GTK_MESSAGE_ERROR, "对方拒绝您的请求");
}
void on_give_up (int fd, struct message *msg)
{
	printf ("on_give_up\n");
	game_win ();
}
void on_exit_room (int fd, struct message *msg)
{
	printf ("on_exit_room\n");
	if (get_color () == BLACK)
	{
		struct message msg;
		msg.type = EXIT_ROOM;
		sprintf (msg.buff, "%s", get_room_number ());
		send_message (&msg);

		show_message_dialog (GTK_MESSAGE_ERROR, "游戏已被房主解散");
		hide_room_dialog ();
		show_main_dialog ();
	}
	else
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "对方已经退出房间");
		opp_exit_room ();
	}
}
void on_hall_msg (int fd, struct message *msg)
{
	printf ("on_hall_msg\n");
	g_print ("message = %s", msg->buff);
	add_hall_message (msg->buff);
}
void on_room_msg (int fd, struct message *msg)
{
	printf ("on_room_msg\n");
	g_print ("message = %s", msg->buff);
	char *ptr = msg->buff;
	/*  去掉房间号 */
	while (*ptr != ' ') ptr++;
	ptr++;
	add_room_message (ptr);
}
void on_update_personal_info (int fd, struct message *msg)
{
	printf ("on_update_personal_info\n");
}
void on_quick_start_game (int fd, struct message *msg)
{
	printf ("on_quick_start_game\n");
	if (msg->buff[0] == 0)
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "系统找不到匹配的房间");
		return;
	}
	hide_main_dialog ();
	show_room_dialog ();
	init_room_dialog (msg->buff+1);
	start_game ();
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
	services[OPP_JOIN_ROOM] = on_opp_join_room;
	services[UPDATE_ROOM] = on_update_room;
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
