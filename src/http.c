#include <stdlib.h>
#include <stdio.h>
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"time.h"
#include"dirent.h"
#include"netdb.h"

#include"connect.h"

int main(int argc, char *argv[]){
	DIR * dirptr;
	FILE * fileptr;
	time_t timenow;
	struct tm * timeinfo;
	time (&timenow);
	timeinfo = localtime(&timenow);

	char * header, * request, * path, * newpath, * host;
	char * dir, * temp;
	int port, sockfd, connfd;
	char get[3], http[9];
	char filepath[BUF_SIZE];
	char http_not_found[] = "HTTP/1.0 404 Not Found\n";
	char http_ok[] = "HTTP/1.0 200 OK\n";
	char buffer[BUF_SIZE];
	char * contentType;

	if (argc != 4) {
		printf("usage: [host] [directory] [portnumber]\n");
		exit(1);
	}

	header = (char*) malloc (BUF_SIZE * sizeof(char));
	request = (char*) malloc (BUF_SIZE * sizeof(char));
	path = (char*) malloc (BUF_SIZE * sizeof(char));
	newpath = (char*) malloc (BUF_SIZE * sizeof(char));

	host = argv[1];
	dir = argv[2];
	port = atoi(argv[3]);

	if ((dirptr = opendir(dir)) == NULL) {
		printf("Directory Not Found!\n");
		exit(1);
	}
}