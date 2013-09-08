#include <gtk/gtk.h>
#include <string.h>
#include <arpa/inet.h>
#include "common.h"
#include "ui.h"
#include "net.h"
#include "sound.h"

static GtkWidget *room_dialog;
static GtkWidget *room_number_label;
static GtkWidget *player1_name_label;
static GtkWidget *player2_name_label;
static GtkWidget *player1_state_label;
static GtkWidget *player2_state_label;
static GtkWidget *message_view;
static GtkWidget *message_editor;
static int turn;
static int color;
static int step;
static gboolean regret_flag;
static struct room_info room;
static const char *states[]={" ", " ","准备中"};

void start_game ()
{
	play_sound ("sound/start.wav",1);
	g_print ("enter start game...\n");
	if (get_player1_state() == START && get_player2_state() == START) return ;

	init_chessbord ();
	step = 0;
	if (get_color() == RED)
	{
		set_player1_state (START);
		set_turn ();
	}
	else
	{
		set_player2_state (START);
		unset_turn ();
	}
	gtk_widget_queue_draw ((GtkWidget *)room_dialog);

	struct message msg;
	msg.type = READY;
	sprintf (msg.buff, "%s", get_room_number ()); 
	send_message (&msg);

	g_print ("exit start game...\n");
}
static void start_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("start button clicked...\n");
	start_game ();
	g_print ("exit start button clicked...\n");
}

void add_step ()
{
	step++;
}
void min_step ()
{
	step--;
}
int get_step ()
{
	return step;
}
gboolean can_regret ()
{
	return regret_flag;
}
void set_regret_flag (gboolean f)
{
	regret_flag = f;
}
static void regret_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("regret button clicked...\n");
	if (get_step () == 0) return;
	if (can_regret () == FALSE) return;
	if (get_player1_state() != START || get_player2_state() != START) return ;
	if (get_turn ()) return;

	struct message msg;
	msg.type = REGRET;
	strcpy (msg.buff, get_room_number ());
	send_message (&msg);
}

static void draw_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("draw button clicked...\n");
	if (get_player1_state() != START || get_player2_state() != START) return ;

	struct message msg;
	msg.type = DRAW_GAME;
	strcpy (msg.buff, get_room_number ());
	send_message (&msg);
}

static void give_up_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("give up button clicked...\n");
	if (get_player1_state() != START || get_player2_state() != START) return ;

	struct message msg;
	msg.type = GIVE_UP;
	strcpy (msg.buff, get_room_number ());
	send_message (&msg);
	game_lose ();
}

static void send_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("send button clicked...\n");
	struct message msg;
	msg.type = ROOM_MSG;
	char str[256];
	sprintf (str, "%s:%s\n", get_user_name (), gtk_entry_get_text ((GtkEntry *) message_editor));
	add_room_message (str);
	sprintf (msg.buff, "%s %s", get_room_number (), str);
	send_message (&msg);
}

/*  退出房间 */
static void destroy (GtkWidget *widget, gpointer data)
{
	g_print ("destroy...\n");
	struct message msg;
	sprintf (msg.buff, "%s", get_room_number ());
	if (get_player1_state () == START && get_player2_state () == START)
	{
		msg.type = GIVE_UP;
		send_message (&msg);
	}
	msg.type = EXIT_ROOM;
	send_message (&msg);
	hide_room_dialog ();
	show_main_dialog ();
}

void init_room_dialog (const char *buff)
{
	/*  color 在点击创建房间或加入房间时已经被设置 */
	g_print ("init room dialog...\n");

	memcpy (&room, buff, sizeof (room)); 
	init_chessbord ();

	set_room_number (room.index);
	set_player1_name (room.player1);
	set_player2_name (room.player2);
	set_player1_state_label (" ");
	set_player2_state_label (" ");

	gtk_widget_queue_draw ((GtkWidget *)room_dialog);
}

void opp_exit_room ()
{
	g_print ("opp exit room...\n");
	init_chessbord ();
	room.player2[0] = 0;
	room.state2 = EMPTY;
	room.state1 = ENTER;
	set_player2_name (room.player2);

	gtk_widget_queue_draw ((GtkWidget *)room_dialog);
}
void create_room_dialog ()
{
	GtkWidget *fixed, *button, *sw, *background, *label, *chessbord;

	room_dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (room_dialog, 946, 627);
	gtk_window_set_resizable (GTK_WINDOW (room_dialog), FALSE);
	g_signal_connect (room_dialog, "destroy", G_CALLBACK (destroy), NULL);
	gtk_window_set_position (GTK_WINDOW (room_dialog), GTK_WIN_POS_CENTER);

	fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (room_dialog), fixed);
	
	background = gtk_image_new_from_file ("photo/room_dialog_bg.jpg");
	gtk_fixed_put (GTK_FIXED (fixed), background, 0, 0);

	background = gtk_image_new_from_file ("photo/woodbg.jpg");
	gtk_fixed_put (GTK_FIXED (fixed), background, 590, 25);

	chessbord = create_chessbord ();
	gtk_fixed_put (GTK_FIXED (fixed), chessbord, 50, 25);

	label = gtk_label_new ("");
	gtk_label_set_markup (GTK_LABEL (label),
			"<span foreground='blue' font_desc='20'>房间:</span>");
	gtk_fixed_put (GTK_FIXED (fixed), label, 596, 25);
	room_number_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed), room_number_label, 670, 25);

	label = gtk_label_new ("");
	gtk_label_set_markup (GTK_LABEL (label),
			"<span foreground='red' font_desc='20'>红方:</span>");
	gtk_fixed_put (GTK_FIXED (fixed), label, 596, 75);
	player1_name_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed), player1_name_label, 670, 75);

	label = gtk_label_new ("黑方");
	gtk_label_set_markup (GTK_LABEL (label),
			"<span foreground='black' font_desc='20'>黑方:</span>");
	gtk_fixed_put (GTK_FIXED (fixed), label, 596, 125);
	player2_name_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed),  player2_name_label, 670, 125);

	player1_state_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed),  player1_state_label, 770, 75);
	player2_state_label = gtk_label_new ("");
	gtk_fixed_put (GTK_FIXED (fixed),  player2_state_label, 770, 125);

	button = gtk_button_new_with_label ("悔棋");
	g_signal_connect (button, "clicked", G_CALLBACK (regret_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 596, 175);


	button = gtk_button_new_with_label ("求和");
	g_signal_connect (button, "clicked", G_CALLBACK (draw_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 660, 175);

	button = gtk_button_new_with_label ("认输");
	g_signal_connect (button, "clicked", G_CALLBACK (give_up_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 724, 175);

	button = gtk_button_new_with_label ("开始");
	g_signal_connect (button, "clicked", G_CALLBACK (start_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 788, 175);

	button = gtk_button_new_with_label ("发送");
	g_signal_connect (button, "clicked", G_CALLBACK (send_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 800, 560);

	GdkRGBA color;
	color.red = 0.99; color.green = 0.95; color.blue = 0.75; color.alpha = 0.5;
	message_view = gtk_text_view_new ();
	gtk_widget_override_background_color (message_view,GTK_STATE_FLAG_NORMAL, &color);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (message_view), FALSE);
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request (sw, 250, 320);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (sw), message_view);
	gtk_fixed_put (GTK_FIXED (fixed), sw, 596, 230);

	message_editor = gtk_entry_new ();
	gtk_widget_set_size_request (message_editor, 200, 10);
	gtk_fixed_put (GTK_FIXED (fixed), message_editor, 596, 560);

}

void set_turn ()
{
	g_print ("set turn ...\n");
	turn = 1;
}
void unset_turn ()
{
	g_print ("unset turn ...\n");
	turn = 0;
}
int get_turn ()
{
	g_print ("get turn ...\n");
	return turn;
}

void set_room_number (int number)
{
	char attr[128];
	sprintf (attr, "<span foreground='blue' font_desc='20'>%d</span>", number);
	gtk_label_set_markup (GTK_LABEL (room_number_label), attr);
}
void set_player1_name (const char *name)
{
	char attr[128];
	sprintf (attr, "<span foreground='red' font_desc='20'>%s</span>", name);
	gtk_label_set_markup (GTK_LABEL (player1_name_label), attr);
}
void set_player2_name (const char *name)
{
	char attr[128];
	sprintf (attr, "<span foreground='black' font_desc='20'>%s</span>", name);
	gtk_label_set_markup (GTK_LABEL (player2_name_label), attr);
}
void set_player1_state (int state)
{
	room.state1 = state;
	set_player1_state_label (states[state]);
	if (get_player1_state () == START && get_player2_state () == START)
	{
		set_player1_state_label ("进行中");
		set_player2_state_label ("进行中");
	}
}
void set_player2_state (int state)
{
	room.state2 = state;
	set_player2_state_label (states[state]);
	if (get_player1_state () == START && get_player2_state () == START)
	{
		set_player1_state_label ("进行中");
		set_player2_state_label ("进行中");
	}
}
int  get_player1_state ()
{
	return room.state1;
}
int  get_player2_state ()
{
	return room.state2;
}
void set_player1_state_label (const char *state)
{
	char attr[128];
	sprintf (attr, "<span foreground='red' font_desc='20'>%s</span>", state);
	gtk_label_set_markup (GTK_LABEL (player1_state_label), attr);
}
void set_player2_state_label (const char *state)
{
	char attr[128];
	sprintf (attr, "<span foreground='red' font_desc='20'>%s</span>",state);
	gtk_label_set_markup (GTK_LABEL (player2_state_label), attr);
}
const char *get_room_number ()
{
	return gtk_label_get_text ((GtkLabel *)room_number_label);
}
void add_room_message (const char *buff)
{
	GtkTextBuffer *text_buff = gtk_text_view_get_buffer ((GtkTextView *)message_view);
	GtkTextIter end;
   	gtk_text_buffer_get_end_iter (text_buff, &end);
	gtk_text_buffer_insert (text_buff, &end, buff, -1);
}
void show_room_dialog ()
{
	create_room_dialog ();
	gtk_window_set_title (GTK_WINDOW (room_dialog), get_user_name ());
	gtk_widget_queue_draw ((GtkWidget *)room_dialog);
	gtk_widget_show_all (room_dialog);
}

void hide_room_dialog ()
{
	gtk_widget_destroy (room_dialog);
}
void game_over ()
{
	set_player1_state (ENTER);
	set_player2_state (ENTER);
}
void game_win ()
{
	play_sound ("sound/win.wav",1);
	game_over ();
}
void game_lose ()
{
	play_sound ("sound/loss.wav",1);
	game_over ();
}
void game_draw ()
{
	play_sound ("sound/win.wav",1);
	game_over ();
}
void set_color (int cl)
{
	color = cl;
}
int get_color ()
{
	return color;
}
