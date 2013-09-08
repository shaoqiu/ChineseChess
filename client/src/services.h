#ifndef SERVICES_H_INCLUDE
#define SERVICES_H_INCLUDE

#include "common.h"

void init_services ();
void on_register (int fd, struct message *msg);
void on_login (int fd, struct message *msg);
void on_forget_password (int fd, struct message *msg);
void on_exit_game (int fd, struct message *msg);
void on_get_room_info (int fd, struct message *msg);
void on_create_room (int fd, struct message *msg);
void on_join_room (int fd, struct message *msg);
void on_opp_join_room (int fd, struct message *msg);
void on_update_room (int fd, struct message *msg);
void on_ready (int fd, struct message *msg);
void on_move (int fd, struct message *msg);
void on_regret (int fd, struct message *msg);
void on_regret_feedback (int fd, struct message *msg);
void on_draw_game (int fd, struct message *msg);
void on_draw_game_feedback (int fd, struct message *msg);
void on_give_up (int fd, struct message *msg);
void on_exit_room (int fd, struct message *msg);
void on_hall_msg (int fd, struct message *msg);
void on_room_msg (int fd, struct message *msg);
void on_update_personal_info (int fd, struct message *msg);

#endif
