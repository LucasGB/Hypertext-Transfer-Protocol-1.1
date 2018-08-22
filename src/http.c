#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "connect.h"
#include "url_parser.h"

#define BUFFER_SIZE 512

int main(int argc, char* argv[]){

	char buffer[BUFFER_SIZE];

	pid_t childpid;

	// Socket file descriptors
	int listen_fd, conn_fd;

	// Client structures
	struct sockaddr_in client_address;
	socklen_t client;
	 char ipAddress[INET_ADDRSTRLEN];
	
	// Initializes the server
	init_server(&listen_fd);

	client = sizeof(client_address);

	// Accepts incoming connections from clients
	if((conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client)) == -1) {
		fprintf(stderr, "Error on accept --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	} else {
		printf("Client connected.\n");

		// Receives request from client
		if(recv(conn_fd, buffer, sizeof(buffer), 0) == -1) {
			fprintf(stderr, "Error to receive request from client --> %s", strerror(errno));
			exit(EXIT_FAILURE);
		}

		inet_ntop(AF_INET, &client_address.sin_addr, ipAddress, sizeof(ipAddress)); //convert binary-ip from client to "networkToPresentable" string
    	printf("Request from %s:%i\n", ipAddress, ntohs(client_address.sin_port));
    	printf("Message: %s\n", buffer);

    	printf("%d\n", match(buffer));



	}

}