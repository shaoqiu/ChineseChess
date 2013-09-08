#include "chessbord.h"
#include "room_dialog.h"
#include "rules.h"
#include "ui.h"
#include "net.h"
#include "common.h"
#include "chessman.h"
#include "sound.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

static GdkPixbuf *background;
static GdkPixbuf *da;
static GdkPixbuf *chessman[32];
static GtkWidget *frame;
static GtkWidget *event_box; 
static int back_width, back_height;
static int chessbord[10][9];
static int pre_chessbord[10][9];
static int pre_x = -1, pre_y = -1;

char *paths[] = 
{	
"photo/OO.GIF",

"photo/RK.GIF",
"photo/RKS.GIF",
"photo/RA.GIF",
"photo/RAS.GIF",
"photo/RB.GIF",
"photo/RBS.GIF",
"photo/RN.GIF",
"photo/RNS.GIF",
"photo/RR.GIF",
"photo/RRS.GIF",
"photo/RC.GIF",
"photo/RCS.GIF",
"photo/RP.GIF",
"photo/RPS.GIF",

"photo/BK.GIF",
"photo/BKS.GIF",
"photo/BA.GIF",
"photo/BAS.GIF",
"photo/BB.GIF",
"photo/BBS.GIF",
"photo/BN.GIF",
"photo/BNS.GIF",
"photo/BR.GIF",
"photo/BRS.GIF",
"photo/BC.GIF",
"photo/BCS.GIF",
"photo/BP.GIF",
"photo/BPS.GIF"
};

void init_chessbord ()
{
	bzero (chessbord, sizeof(chessbord));
	if (get_color () == RED)
	{
		chessbord[9-0][0] = RR;
		chessbord[9-0][1] = RN;
		chessbord[9-0][2] = RB;
		chessbord[9-0][3] = RA;
		chessbord[9-0][4] = RK;
		chessbord[9-0][5] = RA;
		chessbord[9-0][6] = RB;
		chessbord[9-0][7] = RN;
		chessbord[9-0][8] = RR;
		chessbord[9-2][1] = RC;
		chessbord[9-2][7] = RC;
		chessbord[9-3][0] = RP;
		chessbord[9-3][2] = RP;
		chessbord[9-3][4] = RP;
		chessbord[9-3][6] = RP;
		chessbord[9-3][8] = RP;

		chessbord[9-9][0] = BR;
		chessbord[9-9][1] = BN;
		chessbord[9-9][2] = BB;
		chessbord[9-9][3] = BA;
		chessbord[9-9][4] = BK;
		chessbord[9-9][5] = BA;
		chessbord[9-9][6] = BB;
		chessbord[9-9][7] = BN;
		chessbord[9-9][8] = BR;
		chessbord[9-7][1] = BC;
		chessbord[9-7][7] = BC;
		chessbord[9-6][0] = BP;
		chessbord[9-6][2] = BP;
		chessbord[9-6][4] = BP;
		chessbord[9-6][6] = BP;
		chessbord[9-6][8] = BP;
	}
	else
	{
		chessbord[0][0] = RR;
		chessbord[0][1] = RN;
		chessbord[0][2] = RB;
		chessbord[0][3] = RA;
		chessbord[0][4] = RK;
		chessbord[0][5] = RA;
		chessbord[0][6] = RB;
		chessbord[0][7] = RN;
		chessbord[0][8] = RR;
		chessbord[2][1] = RC;
		chessbord[2][7] = RC;
		chessbord[3][0] = RP;
		chessbord[3][2] = RP;
		chessbord[3][4] = RP;
		chessbord[3][6] = RP;
		chessbord[3][8] = RP;

		chessbord[9][0] = BR;
		chessbord[9][1] = BN;
		chessbord[9][2] = BB;
		chessbord[9][3] = BA;
		chessbord[9][4] = BK;
		chessbord[9][5] = BA;
		chessbord[9][6] = BB;
		chessbord[9][7] = BN;
		chessbord[9][8] = BR;
		chessbord[7][1] = BC;
		chessbord[7][7] = BC;
		chessbord[6][0] = BP;
		chessbord[6][2] = BP;
		chessbord[6][4] = BP;
		chessbord[6][6] = BP;
		chessbord[6][8] = BP;
	}
}
static void load_image ()
{
	g_print ("enter load background\n");
	background = gdk_pixbuf_new_from_file ("photo/WOOD.JPG", NULL);
	if (background == NULL )
	{
		g_print ("load background error\n");
		exit (-1);
	}
	int i;
	for (i=0;i<29;i++)
	{
		g_print ("load image %s\n", paths[i]);
		chessman[i] = gdk_pixbuf_new_from_file (paths[i], NULL);
		if (chessman[i] == NULL)
		{
			g_print ("load_image wrong\n");
			exit (-1);
		}
	}
	g_print ("exit load background\n");
}
void restore_chessbord ()
{
	memcpy (chessbord, pre_chessbord, sizeof (chessbord));
	gtk_widget_queue_draw (frame);
}
void opp_move_chessman (int src_x, int src_y, int dst_x, int dst_y)
{
	play_sound ("sound/move.wav",1);
	src_y = 9 - src_y; 
	dst_y = 9 - dst_y;
	int destination = chessbord[dst_y][dst_x];
	memcpy (pre_chessbord, chessbord, sizeof (chessbord));
	chessbord[dst_y][dst_x] = chessbord[src_y][src_x];
	chessbord[src_y][src_x] = OO;
	set_turn ();
	add_step ();
	gtk_widget_queue_draw (frame);
	if ( (get_color () == RED && destination == RK) || (get_color () == BLACK && destination == BK) )
	{
		game_lose ();
	}
}
void move_chessman (int src_x, int src_y, int dst_x, int dst_y)
{
	play_sound ("sound/move.wav",1);
	set_regret_flag (TRUE);
	int destination = chessbord[dst_y][dst_x];
	memcpy (pre_chessbord, chessbord, sizeof (chessbord));
	chessbord[dst_y][dst_x] = chessbord[src_y][src_x];
	chessbord[src_y][src_x] = OO;
	unset_turn ();
	add_step ();
	gtk_widget_queue_draw (frame);
	struct message msg;
	msg.type = MOVE;
	sprintf (msg.buff, "%s %d %d %d %d", get_room_number (), src_x, src_y, dst_x, dst_y);
	send_message (&msg);
	gtk_widget_queue_draw (frame);
	if ( (get_color () == BLACK && destination == RK) || (get_color () == RED && destination == BK) )
	{
		game_win ();
	}
}
void select_chessman (int x, int y)
{
	chessbord[y][x] += 1;
	play_sound ("sound/select.wav",1);
}
void unselect_chessman (int x, int y)
{
	if (x == -1 || y == -1) return;
	chessbord[y][x] -= 1;
}
void do_nothing ()
{
}
int get_color_type (int chessman)
{
	if (chessman >= RK && chessman <= RPS)
		return RED;
	else if (chessman >= BK && chessman <= BPS)
		return BLACK;
	else  return BLANK;
}
static void clicked_chessman (int x, int y)
{
	g_print ("clicked chessman...\n");
	if (get_player1_state () != START || get_player2_state () != START) return;
	if (!get_turn ()) return ;
	if (pre_x == x && pre_y == y) return ;

	int type = get_color_type (chessbord[y][x]);
	if (type == get_color ())
	{
		select_chessman (x, y);
		unselect_chessman (pre_x, pre_y);
		pre_x = x;
		pre_y = y;
	}
	else if (pre_x != -1 )
	{
		if (can_move (chessbord, pre_x, pre_y, x, y))
		{
			unselect_chessman (pre_x, pre_y);
			move_chessman (pre_x, pre_y, x, y);
			pre_x = pre_y = -1;
		}
	}
	gtk_widget_queue_draw (frame);
	g_print ("exit clicked chessman...\n");

}
static gboolean image_click (GtkWidget *event_box,
			GdkEventButton *event,
			gpointer data)
{
	clicked_chessman ( (int)event->x / 57, (int)event->y / 57);
	return TRUE;
}
static void draw_chessbord ()
{
	int i,j;
	gdk_pixbuf_copy_area (background, 0, 0, back_width, back_height, da, 0, 0);

	for (i=0;i<10;i++) for(j=0;j<9;j++)
	{
		if (chessbord[i][j] == 0)
			continue;
		gdk_pixbuf_composite (chessman[chessbord[i][j]],
						da,
						57*j, 57*i,
						57, 57,
						57*j, 57*i,
						1, 1,
						GDK_INTERP_NEAREST,
						255);
	}
}
static gboolean draw_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
	draw_chessbord ();	
    gdk_cairo_set_source_pixbuf (cr, da, 0, 0);
	cairo_paint (cr);

	return TRUE;
}


GtkWidget *create_chessbord ()
{
	g_print ("enter create_chessbord\n");

	load_image ();
	init_chessbord ();

	back_width = gdk_pixbuf_get_width (background);
	back_height = gdk_pixbuf_get_height (background);

	da = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, back_width, back_height);

	frame = gtk_drawing_area_new ();
	event_box = gtk_event_box_new ();
	gtk_container_add (GTK_CONTAINER (event_box), frame);

	gtk_widget_set_size_request (frame, back_width, back_height);

	g_signal_connect (frame, "draw",
			  G_CALLBACK (draw_cb), NULL);

	g_signal_connect (G_OBJECT (event_box),
				"button_press_event",
				G_CALLBACK (image_click),
				NULL);

	gtk_widget_show_all (frame);

	g_print ("exit create_chessbord\n");
	return event_box;
}
