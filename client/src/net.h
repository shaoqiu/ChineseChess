#ifndef NET_H_INCLUDE
#define NET_H_INCLUDE

#include <unistd.h>
#include "common.h"

int connect_server ();
void create_receive_thread (); 
void* receive_thread (void *arg);
void on_receive (int fd);
ssize_t readn (int fd, void *vptr, size_t n);
ssize_t writen (int fd, const void *vptr, size_t n);
void send_message (struct message *msg);
ssize_t get_message (struct message *msg);

#endif
