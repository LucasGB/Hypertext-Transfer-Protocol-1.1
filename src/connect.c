#include "connect.h"
#include "url_parser.h"

int init_server(int* listen_fd){
	struct sockaddr_in server_address;

	(*listen_fd) = socket(AF_INET, SOCK_STREAM, 0);

	if((*listen_fd) == -1){
		fprintf(stderr, "Error creating socket -> %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT_NUMBER);

	if(bind((*listen_fd), (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		fprintf(stderr, "Error on bind --> %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}

	// Listen a maximum of 1 connections
	if((listen((*listen_fd), 1)) == -1) {
		fprintf(stderr, "Error on listen --> %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Started server on %s:%d\n", SERVER_ADDRESS, PORT_NUMBER);

	return 1;
}

void request_handler(void *new_socket){
	char request_buffer[BUFFER_SIZE];

	int conn_fd = *(int*) new_socket;
		// Receives request from client
	if(recv(conn_fd, request_buffer, sizeof(request_buffer), 0) == -1) {
		fprintf(stderr, "Error to receive request from client --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("%s\n", request_buffer);
}

void process(int conn_fd, struct sockaddr_in *client_address){

}

