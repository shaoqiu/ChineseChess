#ifndef CHESSBORD_H_INCLUDE
#define CHESSBORD_H_INCLUDE

#include <gtk/gtk.h>

GtkWidget *create_chessbord ();
void init_chessbord ();
void move_chessman (int src_x, int src_y, int dst_x, int dst_y);
void opp_move_chessman (int src_x, int src_y, int dst_x, int dst_y);
void restore_chessbord ();

#endif
