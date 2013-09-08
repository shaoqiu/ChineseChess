#ifndef MAIN_DIALOG_H_INCLUDE
#define MAIN_DIALOG_H_INCLUDE

#include "common.h"

void create_main_dialog ();
void show_main_dialog ();
void hide_main_dialog ();

void set_user_name (const char *name);
const char *get_user_name ();

void add_room (struct room_info *room);
void del_room (int index);
void add_hall_message (const char *buff);

#endif
