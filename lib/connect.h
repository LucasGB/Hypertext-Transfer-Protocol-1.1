#ifndef CONNECT_H
#define CONNECT_H

#include <netdb.h>
#include <unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

//#include <buffer.h>

#define RECV_SIZE 1024
#define BUF_SIZE  RECV_SIZE + 1
#define PORT "9090"
#define ROOT "/home/suporte"

int init_connection(char *hostname, char *port, struct addrinfo **res);
int make_connection(struct addrinfo *res);
int make_request(int sockfd, char *hostname, char *request_path);
int check_errors();

int connection(int fd);
int get_file_size(int fd);
void send_new(int fd,char *msg);
int recv_new(int fd,char *buffer);

//int fetch_response(int sockfd, Buffer **response, int recv_size);

#endif