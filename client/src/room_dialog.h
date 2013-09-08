#ifndef ROOM_DIALOG_H_INCLUDE
#define ROOM_DIALOG_H_INCLUDE

#include <gtk/gtk.h>

void create_room_dialog ();
void show_room_dialog ();
void hide_room_dialog ();
void init_room_dialog (const char *buff);
void opp_exit_room ();
void add_room_message (const char *buff);
void start_game ();
void game_win ();
void game_lose ();
void game_draw ();
void game_over ();
void set_color (int cl);
void set_room_number (int number);
void set_player1_name (const char *name);
void set_player2_name (const char *name);
void set_player1_state (int state);
void set_player2_state (int state);
void set_player1_state_label (const char *state);
void set_player2_state_label (const char *state);
void set_turn ();
void unset_turn ();
void add_step ();
void min_step ();
void set_regret_flag (gboolean f);
int  get_player1_state ();
int  get_player2_state ();
int  get_turn ();
int  get_color ();
int  get_step ();
gboolean can_regret ();
const char *get_room_number ();

#endif
