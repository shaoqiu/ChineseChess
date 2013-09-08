#ifndef NET_H_INCLUDE 
#define NET_H_INCLUDE 

#include "common.h"
#include <unistd.h>

ssize_t readn (int fd, void *ptr, size_t size);
ssize_t writen (int fd, void *ptr, size_t size);
ssize_t get_message (int fd, struct message *msg);
ssize_t send_message (int fd, struct message *msg);

int add_fd_to_epoll (int socket_fd);
void remove_fd_from_epoll (int socket_fd);
void init_listen_socket();
void on_accept ();

#endif
