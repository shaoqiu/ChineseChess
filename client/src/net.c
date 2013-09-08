#include "net.h"
#include "common.h"
#include "ui.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#define MAXQUE 100
#define MAXEVENTS 2

static int sock_fd;
static int epoll_fd;
/*  services[] 在services.c 中定义 */
extern  void (*services[MAXSERVICES]) (int fd, struct message *msg);

static const char *get_server_ip ()
{
	static char ip[64],buff[64];
	int fd = open ("ipconfig", O_RDONLY);
	if (fd == -1){
		perror ("open");
		exit (-1);
	}
	ssize_t len = read (fd, buff, 64);
	if (len == -1 || len == 0)
	{
		perror ("read");
		exit (-1);
	}
	sscanf (buff, "%s", ip);
	close (fd);
	return ip;
}
int connect_server ()
{
	struct sockaddr_in server_addr;

	if ((sock_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror ("socket");
		return -1;
	}

	bzero (&server_addr, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (PORT);
	if (inet_pton (AF_INET, get_server_ip (), &server_addr.sin_addr) <= 0)
	{
		perror ("inet_pton");
		return -1;
	}
	
	if (connect (sock_fd, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0 )
	{
		perror ("connect");
		return -1;
	}
	return 0;
}

void create_receive_thread ()
{
	epoll_fd = epoll_create (MAXEVENTS);
	if(epoll_fd <= 0)
	{
		perror ("epoll_create");
		exit (-1);
	}
	if (fcntl (sock_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror ("fcntl");
		exit (-1);
	}

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = sock_fd;

	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) < 0)
	{
		perror ("epoll_ctl");
		exit (-1);
	}
	g_thread_create (receive_thread, NULL, FALSE, NULL);
}

void* receive_thread (void *arg)
{
	g_print ("running receive thread...\n");

	struct epoll_event events[MAXEVENTS];
	int nfd,i;

	while (1)
	{
		nfd = epoll_wait (epoll_fd, events, MAXEVENTS, -1);
		if (nfd == -1 )
		{
			perror ("epoll_wait");
			continue;
		}

		for (i=0;i<nfd;i++)
		{
			on_receive (events[i].data.fd);
		}
	}
}

void on_receive (int fd)
{
	struct message msg;

	while (1)
	{
		ssize_t len = get_message (&msg);
		if (len < 0)
		{
			break;
		}
		else if (len == 0)
		{
			show_message_dialog (GTK_MESSAGE_ERROR, "服务器掉线了\n");
			exit (-1);
		}
		else
		{
			g_print ("on_recive: msgtype = %d\n",msg.type);
			gdk_threads_enter ();
			services[msg.type] (fd, &msg);
			gdk_threads_leave ();
		}
	}
}

/* 下面是经过封装的读写函数
 * 因为 read /write 读写时可能会出现错误
 * 或者被中断，或者数据未就绪，或者缓冲区已满等等
 * 得处理这些异常的情况 */
ssize_t readn (int fd, void *vptr, size_t n)
{
	ssize_t nleft;
	ssize_t nread;
	char *ptr;
	
	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nread = read (fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		}
		else if (nread == 0)
		{
			return 0;
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);
}

ssize_t writen (int fd, const void *vptr, size_t n)
{
	ssize_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *)vptr;
	nleft = n;
	while (nleft > 0)
	{
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
	return n;
}

ssize_t get_message (struct message *msg)
{
	printf ("get_message\n");
	ssize_t len = readn (sock_fd, msg, sizeof (struct message));
	msg->type = ntohl (msg->type);
	return len;
}
void send_message (struct message *msg)
{
	printf ("send_message\n");
	struct message tmp;
	memcpy (&tmp, msg, sizeof (tmp));
	tmp.type = htonl (msg->type);
	writen (sock_fd, &tmp, sizeof (struct message));
}
