#include "net.h"
#include "common.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int listen_socket;
int epoll_fd;
ssize_t get_message (int fd, struct message *msg)
{
	ssize_t len = readn (fd, msg, sizeof (struct message));
	msg->type = ntohl (msg->type);
	return len;
}
ssize_t send_message (int fd, struct message *msg)
{
	struct message tmp;
	memcpy (&tmp, msg, sizeof (tmp));
	tmp.type = htonl (msg->type);
	return writen (fd, &tmp, sizeof (struct message));
}

ssize_t readn (int fd, void *vptr, size_t size)
{
	ssize_t nleft;
	ssize_t nread;
	char *ptr;
	
	ptr = (char *)vptr;
	nleft = size;
	while (nleft > 0)
	{
		printf ("read...\n");
		if ((nread = read (fd, ptr, nleft)) < 0)
		{
			return -1;//没有数据
		}
		else if (nread == 0)
		{
			return 0; //断开连接
		}
		nleft -= nread;
		ptr += nread;
	}
	return (size - nleft);
}
ssize_t writen (int fd, void *vptr, size_t size)
{
	ssize_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *)vptr;
	nleft = size;
	while (nleft > 0)
	{
		printf ("write...\n");
		if ((nwritten = write (fd, ptr, nleft)) <= 0)
		 {
			 if (errno == EINTR)
				 nwritten = 0;
			 else
				 return -1;
		 }
		 nleft -= nwritten;
		 ptr += nwritten;
	}
	return size;
}
void remove_fd_from_epoll (int socket_fd)
{
	struct epoll_event event;
	if (epoll_ctl (epoll_fd, EPOLL_CTL_DEL, socket_fd, &event) < 0)
	{
		perror ("remove_fd");
	}
}

int add_fd_to_epoll (int nfd)
{
	if (fcntl (nfd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror ("fcntl");
		close (nfd);
		return -1;
	}

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = nfd;

	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, nfd, &event) < 0)
	{
		perror ("epoll_ctl");
		close (nfd);
		return -1;
	}
	return 0;

}
void on_accept()
{

	struct sockaddr_in client_addr;
	socklen_t len = sizeof (client_addr);
	while (1)
	{
		int nfd = accept (listen_socket, (struct sockaddr*)&client_addr, &len);
		if (nfd <= 0)
		{
			perror ("accept");
			return ;
		}

		add_fd_to_epoll (nfd);

		printf ("new connection\n");
	}
}

void init_listen_socket()
{
	listen_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (listen_socket < 0)
	{
		perror ("socket");
		exit (-1);
	}

	struct sockaddr_in server_addr;
	bzero (&server_addr, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (PORT);
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	if (bind (listen_socket, (struct sockaddr*)&server_addr, sizeof (server_addr)) < 0)
	{
		perror ("bind");
		exit (-1);
	}

	if (listen (listen_socket, MAXQUE) < 0)
	{
		perror ("listen");
		exit (-1);
	}

	epoll_fd = epoll_create (MAXEVENTS);
	if(epoll_fd <= 0)
	{
		perror ("epoll_create");
		exit(1);
	}
	add_fd_to_epoll (listen_socket);
}

