#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "http_request.h"
#include "dirent.h"
#include "utils.h"

void error_401(void* client_socket){
	char* response = strdup("HTTP/1.1 401 Unauthorized\nWWW-Authenticate: Basic realm=\"Whatever\"\r\n\r\n");

	if(send(*(int*) client_socket, response, strlen(response), 0) == -1) {
		printf("Error in send\n");
	}

	printf("sent resp\n");
	free(response);
}

void error_404(void* client_socket, HTTP_REQUEST *req) {
	printf("SENDING 404 HTML\n");
	char* path = strdup("./res/templates/404/index.html");

	char *file_type = get_mime_type(path);
	int file_size = get_file_size(path);
	printf("SIZE: %d\n", file_size);
	printf("%s\n", file_type);
	
	int header_size = strlen(file_type) + strlen(req -> cookie) + snprintf(NULL, 0, "%d", file_size) + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;
	printf("CALC HEADER SIZE\n");
	char *header = (char*) malloc (sizeof(char) * header_size);
	printf("MALLOC HEADER\n");
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\n%s\r\n\r\n", file_type, file_size, req -> cookie);
	printf("%s\n", header);
	
	send_new(*(int*) client_socket, header);
	
	int remain_data = file_size;
	int sent_bytes = 0;
	off_t offset = 0;
	//int fd = open(path, O_RDONLY);
	int fd;
	if((fd = open(path, O_RDONLY, 0)) <= 0){
		printf("COULDNT OPEN FD SEND\n");
	}

	// Force flush socket stream
	while(((sent_bytes = sendfile(*(int*) client_socket, fd, &offset, 256)) > 0) && remain_data > 0) {
		int flag = 1;
		setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		remain_data -= sent_bytes;
		flag = 0; 
		setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	}

	printf("sent stuff\n");

	close(fd);
	free(header);
}