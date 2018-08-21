#include "connect.h"

int init_connection(char *hostname, char *port, struct addrinfo **res) {
    int sockfd;
	struct sockaddr_in addr;
	struct hostent * host_ent;
	char * hostAddr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(9090;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {  
		printf("Error creating socket!\n");  
		exit(1);  
	}
	printf("Socket created...\n");

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		printf("Error binding socket to port!\n");  
		exit(1);
	}

	printf("Binding done...\n");

	return sockfd;
}