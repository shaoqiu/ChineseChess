#include "ui.h"
#include <gtk/gtk.h>

void show_message_dialog (int type, const char *str)
{
	g_print ("show_message_dialog\n");
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (NULL,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			type,
			GTK_BUTTONS_CLOSE,
			str);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
