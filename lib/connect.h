#ifndef CONNECT_H
#define CONNECT_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
//#include <buffer.h>

//#define RECV_SIZE 1024
//#define BUFFER_SIZE  RECV_SIZE + 1


#define SERVER_ADDRESS "127.0.0.1"
#define PORT_NUMBER 9090

//#define ROOT "/home/suporte"


struct Client{
	int *conn_fd;
	struct sockaddr_in* address;
};


int init_server(int* listen_fd);

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
//void request_handler(void *new_socket);

int make_connection(struct addrinfo *res);
int make_request(int sockfd, char *hostname, char *request_path);
int check_errors();

int connection(int fd);
void send_new(int fd,char *msg);
int recv_new(int fd,char *buffer);

//int fetch_response(int sockfd, Buffer **response, int recv_size);

#endif