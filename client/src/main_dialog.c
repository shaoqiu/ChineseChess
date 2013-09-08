#include <gtk/gtk.h>
#include <string.h>
#include <arpa/inet.h>
#include "ui.h"
#include "common.h"
#include "net.h"

enum
{
	ROOM_NUMBER, 
	PLAYER1, 
	PLAYER2,
	N_COLUMNS
};

static GtkWidget *main_dialog;
static GtkWidget *room_number_editor;
static GtkWidget *message_editor;
static GtkWidget *message_view;
static GtkWidget *list_view;
static GtkListStore *list_store;
static char user_name[MAXNAMELEN];

static void create_room_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("create room button clicked...\n");
	set_color (RED);
	struct message msg;
	msg.type = CREATE_ROOM;
	sprintf (msg.buff, "%s", user_name);
	send_message (&msg);
}

static void join_room (int room_number)
{
	set_color (BLACK);
	struct message msg;
	msg.type = JOIN_ROOM;
	sprintf (msg.buff, "%d %s", room_number , user_name);
	g_print ("%s\n" ,msg.buff);
	send_message (&msg);
}
static void join_room_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("join room button clicked...\n");
	int room_number;
	char *buf = gtk_entry_get_text ( (GtkEntry *)room_number_editor);
	sscanf(buf, "%d", &room_number);
	join_room (room_number);
}

static void quick_start_game_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("quick start game button clicked...\n");
	set_color (BLACK);
	struct message msg;
	msg.type = QUICK_START_GAME;
	sprintf (msg.buff, "%s", get_user_name ());
	send_message (&msg);
}

static void send_message_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("send message button clicked...\n");
	struct message msg;
	msg.type = HALL_MSG;
	sprintf (msg.buff, "%s:%s\n", get_user_name (), gtk_entry_get_text ((GtkEntry *)message_editor));
	send_message (&msg);
}

static void destroy (GtkWidget *widget, gpointer data)
{
	g_print ("destroy...\n");
	gtk_main_quit ();
}

static void tree_double_clicked (GtkTreeView *treeview, 
		GtkTreePath *path,
		GtkTreeViewColumn *col,
		gpointer data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	
	model = gtk_tree_view_get_model (treeview);
	if (gtk_tree_model_get_iter (model, &iter, path))
	{
		int room_number;
		gtk_tree_model_get (model, &iter, ROOM_NUMBER, &room_number, -1);
		join_room (room_number);
	}
}

static GtkWidget *create_list_view ()
{
	GtkWidget *sw;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

    sw= gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
			GTK_SHADOW_IN);

	list_store = gtk_list_store_new (N_COLUMNS,
			G_TYPE_INT,
			G_TYPE_STRING,
			G_TYPE_STRING);

	list_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (list_store));
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(list_view), TRUE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list_view), TRUE);

	renderer = gtk_cell_renderer_text_new ();

	column = gtk_tree_view_column_new_with_attributes ("          房间号           ",
			renderer, "text",
			ROOM_NUMBER, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list_view), column);

	column = gtk_tree_view_column_new_with_attributes ("           红方            ",
			renderer, "text",
			PLAYER1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list_view), column);

	column = gtk_tree_view_column_new_with_attributes ("           黒方            ",
			renderer, "text",
			PLAYER2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list_view), column);

	g_signal_connect (list_view, "row-activated", G_CALLBACK(tree_double_clicked), NULL);
	gtk_container_add (GTK_CONTAINER (sw), list_view);
	return sw;
}

void create_main_dialog ()
{
	GtkWidget *fixed, *button, *sw, *background,*label;

	main_dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (main_dialog, 850, 650);
	gtk_window_set_resizable (GTK_WINDOW (main_dialog), FALSE);
	g_signal_connect (main_dialog, "destroy", G_CALLBACK (destroy), NULL);
	gtk_window_set_position (GTK_WINDOW (main_dialog), GTK_WIN_POS_CENTER);

	fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (main_dialog), fixed);
	
	background = gtk_image_new_from_file ("photo/main_dialog_bg.png");
	gtk_fixed_put (GTK_FIXED (fixed), background, 0, 0);

	list_view = create_list_view ();
	gtk_widget_set_size_request (list_view, 425, 500);
	gtk_fixed_put (GTK_FIXED (fixed), list_view, 50, 50);

	label = gtk_label_new ("大厅信息");
	gtk_fixed_put (GTK_FIXED (fixed), label, 500, 50);

	message_view = gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (message_view), FALSE);
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request (sw, 300, 470);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (sw), message_view);
	gtk_fixed_put (GTK_FIXED (fixed), sw, 500, 80);

	button = gtk_button_new_with_label ("创建房间");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (create_room_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 50, 575);
	

	room_number_editor = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (room_number_editor), 6);
	gtk_fixed_put (GTK_FIXED (fixed), room_number_editor, 170, 575);

	button = gtk_button_new_with_label ("加入房间");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (join_room_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 254, 576);

	button = gtk_button_new_with_label ("快速开始游戏");
	g_signal_connect (G_OBJECT (button), "clicked", 
				G_CALLBACK (quick_start_game_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 370, 575);

	message_editor = gtk_entry_new ();
	gtk_widget_set_size_request (message_editor, 250, 10);
	gtk_fixed_put (GTK_FIXED (fixed), message_editor, 500, 575);

	button = gtk_button_new_with_label ("发送");
	g_signal_connect (G_OBJECT (button), "clicked", 
				G_CALLBACK (send_message_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 755, 575);
}

void set_user_name (const char *name)
{
	strcpy (user_name, name);
}

const char *get_user_name ()
{
	return user_name;
}

void add_room (struct room_info *room)
{
	g_print ("add room...\n");
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			ROOM_NUMBER, room->index,
			PLAYER1, room->player1,
			PLAYER2, room->player2,
			-1);
	gtk_widget_queue_draw ((GtkWidget *)main_dialog);
}

void del_room (int room_number)
{
	g_print ("delete room...\n");
	GtkTreeIter iter;
	int tnumber;
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter) == FALSE) return;
	do {
		gtk_tree_model_get (GTK_TREE_MODEL (list_store), &iter, ROOM_NUMBER, &tnumber, -1);
		if (room_number == tnumber)
		{
			gtk_list_store_remove (list_store, &iter);
			gtk_widget_queue_draw ((GtkWidget *)main_dialog);
			break;
		}

	}while (gtk_tree_model_iter_next(GTK_TREE_MODEL (list_store), &iter) == TRUE);
	gtk_widget_queue_draw ((GtkWidget *)main_dialog);
	g_print ("exit delete room...\n");
}

void add_hall_message (const char *message)
{
	GtkTextBuffer *text_buff = gtk_text_view_get_buffer ((GtkTextView *)message_view);
	GtkTextIter end;
   	gtk_text_buffer_get_end_iter (text_buff, &end);
	gtk_text_buffer_insert (text_buff, &end, message, -1);
}

void show_main_dialog ()
{
	gtk_window_set_title (GTK_WINDOW (main_dialog), get_user_name ());
	gtk_widget_queue_draw ((GtkWidget *)main_dialog);
	gtk_widget_show_all (main_dialog);
}

void hide_main_dialog ()
{
	gtk_widget_hide (main_dialog);
}

