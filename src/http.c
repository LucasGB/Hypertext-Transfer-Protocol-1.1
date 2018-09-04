#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>


#include "connect.h"
#include "parser.h"
#include "http_request.h"


void request_handler(void *new_socket){
	HTTP_REQUEST* req = malloc (sizeof(HTTP_REQUEST));

	char request_buffer[BUFFER_SIZE];

	// Receives request from client
	if(recv(*(int*) new_socket, request_buffer, sizeof(request_buffer), 0) == -1) {
		fprintf(stderr, "Error to receive request from client --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	parse(request_buffer, &req);

	printf("%s\n", req -> method);

	if(!strcmp(req -> method, "GET")){
		printf("GET REQUEST\n");
	}
}

int main(int argc, char* argv[]){

//	char request_buffer[BUFFER_SIZE];

	int *new_socket;

	// Socket file descriptors
	int listen_fd, conn_fd;

	// Client structures
	struct sockaddr_in client_address;
	socklen_t client;
	char ipAddress[INET_ADDRSTRLEN];
	
	// Initializes the server
	//init_server(&listen_fd);

	struct sockaddr_in server_address;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(listen_fd == -1){
		fprintf(stderr, "Error creating socket -> %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT_NUMBER);

	if(bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		fprintf(stderr, "Error on bind --> %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}

	// Listen a maximum of 1 connections
	if((listen(listen_fd, 2)) == -1) {
		fprintf(stderr, "Error on listen --> %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Started server on %s:%d\n", SERVER_ADDRESS, PORT_NUMBER);

	client = sizeof(client_address);

	while(1){
		// Accepts incoming connections from clients
		if((conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client)) == -1) {
			fprintf(stderr, "Error on accept --> %s", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			printf("Client connected.\n");

			pthread_t sniffer_thread;
			new_socket = malloc (sizeof(int*));
			*new_socket = conn_fd;

			if(pthread_create(&sniffer_thread, NULL, request_handler, (void*) new_socket) < 0){
				fprintf(stderr, "Could not create thread --> %s", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

	}

	return 0;
}