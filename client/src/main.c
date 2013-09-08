#include "net.h"
#include "services.h"
#include "common.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	/*----- CSS ----------- */
	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;
	provider = gtk_css_provider_new ();
  	display = gdk_display_get_default ();
  	screen = gdk_display_get_default_screen (display);
  	gtk_style_context_add_provider_for_screen (screen,
                       GTK_STYLE_PROVIDER(provider),
                       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  	gsize bytes_written, bytes_read;
  	const gchar* home = "style.css";   
  	GError *error = 0;
   	gtk_css_provider_load_from_path (provider,
                                    g_filename_to_utf8(home, strlen(home), &bytes_read, &bytes_written, &error),
                                    NULL);
  	g_object_unref (provider);
/*----------------------------------------------------------------------------------------------------------------------*/
	if (!g_thread_supported())
		g_thread_init (NULL);

	/*  很多关于GTK 多线程的文章都没有下面这个调用，结果是各种错误啊 */
	gdk_threads_init ();

	if (connect_server () == -1)
	{
		fprintf (stderr, "connect server failed\n");
		exit (-1);
	}

	init_services ();
	create_receive_thread ();
	show_login_dialog ();

	/*  gdk_threads_enter , gdk_threads_leave 加锁，解锁操作
	 *  所在对UI 进行操作的代码都要进行加锁，解锁操作
	 *  否则就会有意想不到的错误了 */
	gdk_threads_enter ();
	gtk_main ();
	gdk_threads_leave ();
	return 0;
}
