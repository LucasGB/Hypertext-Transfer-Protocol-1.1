#include <stdlib.h>
#include <stdio.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/stat.h>
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"time.h"
#include"dirent.h"
#include"netdb.h"

#include"connect.h"

int main(int argc, char *argv[]){
	int comm_fd, new_fd;
	int err;
	struct addrinfo *res, *p, hints;
	struct sockaddr_storage their_addr;

	socklen_t addr_size;
	int yes = 1;
	char ip[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	printf("Starting server on port: 9090");

	if( (err = getaddrinfo(NULL, PORT, &hints, &res) ) == -1) {
		printf("Err in getaddrinfo: %s\n", gai_strerror(err));
		return 1;
	}

	for(p = res; p != NULL; p = p -> ai_next){
		if( (comm_fd = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol) ) == -1) {
			printf("Socket error!\n");
			continue;
		}

		if(setsockopt(comm_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			printf("Setsockopt error\n");
			return 1;
		}

		if(bind(comm_fd, p -> ai_addr, p -> ai_addrlen) == -1){
			printf("Binding error\n");
			close(comm_fd);
			continue;
		}
		//break;
	}

	if(listen(comm_fd, 15) == -1) {
		printf("Error in listen\n");
		return 1;
	}

	while(1) {
	
		char y;	
		addr_size = sizeof(their_addr);
		if((new_fd = accept(comm_fd, (struct sockaddr *) &their_addr, &addr_size) ) == -1) {
			printf("Error in accept!\n");
			return(1);
		}
		for(p = res; p != NULL; p = p -> ai_next) {
			void *addr;
			
			if(p->ai_family == AF_INET) {
				struct sockaddr_in *ip;
				ip = (struct sockaddr_in *) p -> ai_addr;
				addr = &(ip->sin_addr);
			}

			if(p->ai_family == AF_INET6) {
				struct sockaddr_in6 *ip;
				ip = (struct sockaddr_in6 *) p -> ai_addr;
				addr = &(ip->sin6_addr);
			}
			
			inet_ntop(p -> ai_family, addr, ip, sizeof(ip));
			printf("Got connection from %s\n",ip);
		}
		connection(new_fd);
	}
	freeaddrinfo(res);
	close(new_fd);
	close(comm_fd);
	return(0);
}