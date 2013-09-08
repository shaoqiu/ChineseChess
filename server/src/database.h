#ifndef DATABASE_H_INCLUDE
#define DATABASE_H_INCLUDE 

#include "common.h"

void connect_database ();
void add_user (struct user_info *user);
void change_user_info (struct user_info *user);
struct user_info *get_user_info (const char *name);

#endif
