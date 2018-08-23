#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "connect.h"
#include "url_parser.h"


typedef struct {
	int *conn_fd;
	struct sockaddr_in* address;
} Client;



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
	init_server(&listen_fd);

	client = sizeof(client_address);

	// Ignore SIGPIPE signal when the browser cancels the request, so that the whole process won't be killed.
	//signal(SIGPIPE, SIG_IGN);

	while(1){
		// Accepts incoming connections from clients
		if((conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client)) == -1) {
			fprintf(stderr, "Error on accept --> %s", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			printf("Client connected.\n");

			pthread_t sniffer_thread;
			Client client = (Client) malloc (sizeof(Client));
			client -> conn_fd = conn_fd;
			client -> address = &client_address;

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