#include "ui.h"
#include "common.h"
#include "net.h"
#include <string.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

GtkWidget *register_dialog;
static GtkWidget *name_editor;
static GtkWidget *password_editor;
static GtkWidget *re_password_editor;
static GtkWidget *email_editor;

static void register_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("register button clicked...\n");
	struct message msg;

	if (strcmp ( gtk_entry_get_text ((GtkEntry *) password_editor), 
				 gtk_entry_get_text ((GtkEntry *) re_password_editor)
				 ))
	{
		show_message_dialog (GTK_MESSAGE_ERROR, "两次输入的密码不相同\n");
		return ;	
	}
	msg.type = REGISTER;
	sprintf (msg.buff, "%s %s %s", 
			gtk_entry_get_text ( (GtkEntry *)name_editor),
			gtk_entry_get_text ( (GtkEntry *)password_editor),
			gtk_entry_get_text ( (GtkEntry *)email_editor));
	send_message (&msg);
}

static void cancel_button_clicked (GtkWidget *widget, gpointer data)
{
	g_print ("cancel button clicked...\n");
	hide_register_dialog ();
	show_login_dialog ();
}

static GtkWidget *create_fixed ()
{
	GtkWidget *fixed, *button, *label, *background;
	fixed = gtk_fixed_new ();
	gtk_widget_set_size_request (fixed, 380, 350);

	background = gtk_image_new_from_file ("photo/register_dialog.jpg");
	gtk_fixed_put (GTK_FIXED (fixed), background, 0, 0);

	label = gtk_label_new ("账号");
	gtk_fixed_put (GTK_FIXED (fixed), label, 90, 100);
	name_editor = gtk_entry_new ();
	gtk_fixed_put (GTK_FIXED (fixed), name_editor, 140, 95);

	label = gtk_label_new ("密码");
	gtk_fixed_put (GTK_FIXED (fixed), label, 90, 140);
	password_editor = gtk_entry_new ();
	gtk_entry_set_invisible_char (GTK_ENTRY (password_editor), '*');
	gtk_entry_set_visibility (GTK_ENTRY (password_editor), FALSE);
	gtk_fixed_put (GTK_FIXED (fixed), password_editor, 140, 135);

	label = gtk_label_new ("确认密码");
	gtk_fixed_put (GTK_FIXED (fixed), label, 60, 180);
	re_password_editor = gtk_entry_new ();
	gtk_entry_set_invisible_char (GTK_ENTRY (re_password_editor), '*');
	gtk_entry_set_visibility (GTK_ENTRY (re_password_editor), FALSE);
	gtk_fixed_put (GTK_FIXED (fixed), re_password_editor, 140, 175);

	label = gtk_label_new ("邮箱");
	gtk_fixed_put (GTK_FIXED (fixed), label, 90, 220);
	email_editor = gtk_entry_new ();
	gtk_fixed_put (GTK_FIXED (fixed),email_editor, 140, 215);

	button = gtk_button_new_with_label ("注册");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (register_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 140, 250);

	button = gtk_button_new_with_label ("取消");
	g_signal_connect (G_OBJECT (button), "clicked",
				G_CALLBACK (cancel_button_clicked), NULL);
	gtk_fixed_put (GTK_FIXED (fixed), button, 250, 250);

	return fixed;
}

static gboolean button_press (GtkWidget *widget, GdkEventButton *event, gint data)
{
	if (event->type == GDK_BUTTON_PRESS)
	{
		if (event->button == 1)
		{
			gtk_window_begin_move_drag (GTK_WINDOW (gtk_widget_get_toplevel (widget)), 
													event->button, event->x_root, event->y_root, event->time);

		}
	}
	return TRUE;
}


void create_register_dialog ()
{
	GtkWidget *window, *fixed;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "注册");
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK);

	fixed = create_fixed ();
	gtk_container_add (GTK_CONTAINER (window), fixed);
	
	g_signal_connect (G_OBJECT (window), "button-press-event",
				G_CALLBACK (button_press), NULL);

	register_dialog = window;
}


void show_register_dialog ()
{
	create_register_dialog ();
	gtk_widget_show_all ( (GtkWidget *)register_dialog);
}
void hide_register_dialog ()
{
	gtk_widget_destroy (register_dialog);
}

