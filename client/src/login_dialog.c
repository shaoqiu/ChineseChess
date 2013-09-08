#include <gtk/gtk.h>
#include <string.h>
#include <arpa/inet.h>
#include "common.h"
#include "net.h"
#include "ui.h"

GtkWidget *login_dialog;
static GtkWidget  *name_editor, *password_editor;
static void login_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("login button clicked...\n");
	struct message msg;
	msg.type = LOGIN;
	sprintf (msg.buff, "%s %s", 
			gtk_entry_get_text ( (GtkEntry *)name_editor),
			gtk_entry_get_text ( (GtkEntry *)password_editor));

	send_message (&msg);
	set_user_name (gtk_entry_get_text ( (GtkEntry *)name_editor));
}

static void register_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("register button clicked...\n");
	hide_login_dialog ();
	show_register_dialog ();
}

static void exit_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("exit button clicked...\n");
	gtk_main_quit ();
}
static GtkWidget *create_fixed ()
{
	GtkWidget *fixed, *button, *label, *background;
	fixed = gtk_fixed_new ();
	gtk_widget_set_size_request (fixed, 380, 250);

	background = gtk_image_new_from_file ("photo/logon_dialog.jpg");
	gtk_fixed_put (GTK_FIXED (fixed), background, 0, 0);

	label = gtk_label_new ("账号");
	gtk_fixed_put (GTK_FIXED (fixed), label, 100, 100);
	name_editor = gtk_entry_new ();
	gtk_fixed_put (GTK_FIXED (fixed), name_editor, 140, 95);

	label = gtk_label_new ("密码");
	gtk_fixed_put (GTK_FIXED (fixed), label, 100, 140);
	password_editor = gtk_entry_new ();
	gtk_entry_set_invisible_char (GTK_ENTRY (password_editor), '*');
	gtk_entry_set_visibility (GTK_ENTRY (password_editor), FALSE);
	gtk_fixed_put (GTK_FIXED (fixed), password_editor, 140, 135);

	button = gtk_button_new_with_label ("登录");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (login_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 100, 180);

	button = gtk_button_new_with_label ("注册");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (register_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 175, 180);

	button = gtk_button_new_with_label ("退出");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (exit_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 250, 180);

	return fixed;
}
static gboolean button_press (GtkWidget *widget, GdkEventButton *event, gint data)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		if (event->button == 1)
		{
			gtk_window_begin_move_drag (GTK_WINDOW (gtk_widget_get_toplevel (widget)), event->button, event->x_root, event->y_root, event->time);

		}
	}
	return TRUE;
}
void create_login_dialog ()
{
	GtkWidget *window, *fixed;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "登录");
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK);

	fixed = create_fixed ();
	gtk_container_add (GTK_CONTAINER (window), fixed);
	
	g_signal_connect (G_OBJECT (window), "button-press-event",
				G_CALLBACK (button_press), NULL);

	gtk_widget_show_all (window);
	login_dialog = window;
}
void show_login_dialog ()
{
	create_login_dialog ();
	gtk_widget_show_all ( (GtkWidget *)login_dialog);
}
void hide_login_dialog ()
{
	gtk_widget_destroy (login_dialog);
}

