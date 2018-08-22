#include "connect.h"

int init_server(int* listen_fd){
	struct sockaddr_in server_address;

	(*listen_fd) = socket(AF_INET, SOCK_STREAM, 0);

	if(listen_fd == -1){
		fprintf(stderr, "Error creating socket -> %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT_NUMBER);

	if(bind((*listen_fd), (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		fprintf(stderr, "Error on bind --> %s", strerror(errno));
        exit(EXIT_FAILURE);
	}

	if((listen((*listen_fd), 5)) == -1) {
		fprintf(stderr, "Error on listen --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Started server on %s:%d\n", SERVER_ADDRESS, PORT_NUMBER);

	return 1;
}