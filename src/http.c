#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/tcp.h>

#include "connect.h"
#include "parser.h"
#include "http_request.h"
#include "errors.h"
#include "dirent.h"

#define BUFSIZ 256

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;

mime_map meme_types [] = {
    {".css", "text/css"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".ico", "image/x-icon"},
    {".js", "application/javascript"},
    {".pdf", "application/pdf"},
    {".mp4", "video/mp4"},
    {".png", "image/png"},
    {".svg", "image/svg+xml"},
    {".xml", "text/xml"},
    {NULL, NULL},
};

char *default_mime_type = "text/plain";

static const char* get_mime_type(char *filename){
    char *dot = strrchr(filename, '.');
    if(dot){ // strrchar Locate last occurrence of character in string
        mime_map *map = meme_types;
        while(map->extension){
            if(strcmp(map->extension, dot) == 0){
                return map->mime_type;
            }
            map++;
        }
    }
    return default_mime_type;
}

size_t get_file_size(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;   
}

char* build_html(char* path) {

	char* temp_content = strdup("<!DOCTYPE html>\n<html>\n<head>\n<title>HTTP Server</title>\n</head>\n<body>\n");
	char* footer_element = strdup("</body>\n</html>");
	int temp_content_length = strlen(temp_content);

	DIR *d;
	struct dirent *dir;

	d = opendir(path);

	// Walks the cursor up to 7 bytes to remove "./root/" from path pointer
	path += 7;

	// Dynamically inserts links to directories in the html content
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if(!strcmp(dir -> d_name, ".") || !strcmp(dir -> d_name, "..")){
				continue;
			}
		    	char link[snprintf(NULL, 0, "<a href=\"/%s\">%s/</a>\n", dir -> d_name, dir -> d_name)];

		    	sprintf(link, "<a href=\"%s/%s\">%s</a>\n", path, dir -> d_name, dir -> d_name);

		    	printf("Link: %s\n", link);

		    	temp_content_length += strlen(link);

		    	temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
		    	strncat(temp_content, link, strlen(link));

		}
		    	
	    closedir(d);
	}

	temp_content_length += strlen("</body>\n</html>");
	temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length);
	strncat(temp_content, footer_element, strlen(footer_element));

	return temp_content;
}

void render_directory(void *new_socket, char* path) {

	char* temp_content = strdup("<!DOCTYPE html>\n<html>\n<head>\n<title>HTTP Server</title>\n</head>\n<body>\n");
	char* footer_element = strdup("</body>\n</html>");
	int temp_content_length = strlen(temp_content);

	DIR *d;
	struct dirent *dir;

	d = opendir(path);

	char* abs_path = strdup(path);

	// Walks the cursor up to 6 bytes to remove "./root" from path pointer
	path += 6;

	// Dynamically inserts links to directories in the html content
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if(!strcmp(dir -> d_name, ".") || !strcmp(dir -> d_name, "..")){
				continue;
			}

			int dir_element_length = strlen(abs_path) + strlen(dir -> d_name) + 1;
			char* dir_element = (char*) malloc (sizeof(char) * dir_element_length);
			snprintf(dir_element, dir_element_length, "%s%s", abs_path, dir -> d_name);

			struct stat statbuf;
			int fd = open(dir_element, O_RDONLY);
			printf("Opening %s\n", dir_element);
			fstat(fd, &statbuf);

			if(S_ISDIR(statbuf.st_mode)){
				printf("DIR\n");
				char link[snprintf(NULL, 0, "<br><a href=\"%s%s/\">%s</a>\n", path, dir -> d_name, dir -> d_name)];
		    	sprintf(link, "<br><a href=\"%s%s/\">%s</a>\n", path, dir -> d_name, dir -> d_name);

		    	printf("Link: %s\n", link);

			    temp_content_length += strlen(link);
		    	temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
		    	strncat(temp_content, link, strlen(link));

		    	printf("PATH:%s\n", path);
			} else {
				printf("REGULAR FILE\n");
				char link[snprintf(NULL, 0, "<br><a href=\"%s%s\">%s</a>\n", path, dir -> d_name, dir -> d_name)];
		    	sprintf(link, "<br><a href=\"%s%s\">%s</a>\n", path, dir -> d_name, dir -> d_name);

		    	printf("Link: %s\n", link);

		    	temp_content_length += strlen(link);
	    		temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
	    		strncat(temp_content, link, strlen(link));

	    		printf("PATH:%s\n", path);
			}
		    //printf("Link: %s\n", link);

		   // temp_content_length += strlen(link);
	    	//temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
	    	//strncat(temp_content, link, strlen(link));

	    	//printf("PATH:%s\n", path);
		}
		    	
	    closedir(d);
	}

	temp_content_length += strlen("</body>\n</html>");
	temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
	strncat(temp_content, footer_element, strlen(footer_element));

	char *temp_header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";

	int html_content_length = strlen(temp_content);

	// Creates a header string with the size of temp_header length plus the string size of html content length
	char *header = (char*) malloc (sizeof(char) * (strlen(temp_header) + snprintf(NULL, 0, "%d", html_content_length) + 4));
	sprintf(header, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\r\n\r\n", html_content_length);

	// Creates response  with the necessary size. (header length  + content length)
	char* resp = (char*) malloc (sizeof(char) * (strlen(header) + html_content_length) + 1);

	// Concatenates header and html to response
	strcpy(resp, header);
	strcat(resp, temp_content);

	send_new(*(int*) new_socket, resp);

	free(header);
	free(temp_content);
	free(footer_element);
	free(resp);
}

void serve_file(void *new_socket, char* path){
	char *file_type = get_mime_type(path);
	int file_size = get_file_size(path);

	int header_size = strlen(file_type) + snprintf(NULL, 0, "%d", file_size) + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;

	char *header = (char*) malloc (sizeof(char) * header_size);
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\r\n\r\n", file_type, file_size);

	send_new(*(int*) new_socket, header);

	int remain_data = file_size;
	int sent_bytes = 0;
	off_t offset = 0;
	int fd = open(path, O_RDONLY);

	// Force flush socket stream
	while(((sent_bytes = sendfile(*(int*) new_socket, fd, &offset, 256)) > 0) && remain_data > 0){
		int flag = 1; 
		setsockopt(*(int*) new_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		remain_data -= sent_bytes;
		flag = 0; 
		setsockopt(*(int*) new_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	}

	close(fd);
	free(header);
}

void serve_chunked_file(void *new_socket, char* path){
	char *file_type = get_mime_type(path);

	// READ THIS LINE AGAIN, SLOWLY, AND FIX IT
	char *header = (char*) malloc (sizeof(char) * (strlen("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ") + snprintf(NULL, 0, "%d", strlen(file_type)) + 4));
	sprintf(header, "HTTP/1.1 200 OK\nContent-Type: %s\nTransfer-Enconding: chunked\r\n\r\n", file_type);


	off_t offset = 0;
	int remain_data = 0;
	int sent_bytes = 0;

	struct stat file_stat;
	int fd = open(path, O_RDONLY);
	fstat(fd, &file_stat);

    remain_data = file_stat.st_size;
    /* Sending file data */
    send_new(*(int*) new_socket, header);

    //while (((sent_bytes = sendbigfile(*(int*) new_socket, fd, &offset, 256)) > 0) && (remain_data > 0)) {
    //            remain_data -= sent_bytes;
  //              fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
//        }

        while (((sent_bytes = sendfile(*(int*) new_socket, fd, &offset, 256)) > 0) && (remain_data > 0)) {
                remain_data -= sent_bytes;
                fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
        }
        int chunk_size = snprintf(NULL, 0, "0\r\n");
            char chunk[chunk_size];
            sprintf(chunk, "0\r\n");
            int b = write(*(int*) new_socket, chunk, strlen(chunk));
            printf("HEAD: %d\n", b);
}

void request_handler(void *new_socket) {
	HTTP_REQUEST* req = (HTTP_REQUEST*) malloc (sizeof(HTTP_REQUEST));
	int fd;
	int sent_bytes = 0;
	int remain_data;
	int* offset;
	ssize_t len;
	char request_buffer[1024];

	// Receives request from client
	if(recv(*(int*) new_socket, request_buffer, sizeof(request_buffer), MSG_PEEK) == -1) {
		fprintf(stderr, "Error to receive request from client --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	parse(request_buffer, req);

	if(!strcmp(req -> method, "GET")){

		struct stat statbuf;

		if((fd = open(req -> path, O_RDONLY, 0)) <= 0){
			error_404(new_socket);	
            fprintf(stderr, "Error opening file --> %s", strerror(errno));

		} else {
			fstat(fd, &statbuf);
			if(S_ISREG(statbuf.st_mode)){
				printf("Is a regular file.\n");

				serve_file(new_socket, req -> path);
				
			} else if(S_ISDIR(statbuf.st_mode)){
				render_directory(new_socket, req -> path);
			}
		}

		free(req);

		close(*(int*) new_socket);
	}
}

void send_new(int fd, char *msg) {
	if(send(fd, msg, strlen(msg), 0) == -1) {
		printf("Error in send\n");
	}
}

int main(int argc, char* argv[]) {

	//	char request_buffer[BUFFER_SIZE];
	int *new_socket;

	// Socket file descriptors
	int listen_fd, conn_fd;

	// Client structures
	struct sockaddr_in client_address;
	socklen_t client = sizeof(client_address);
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

	int port;
	if(argc == 2){
		port = atoi(argv[1]);
		server_address.sin_port = htons(argv[1]);
	} else {
		port = PORT_NUMBER;
		server_address.sin_port = htons(PORT_NUMBER);
	}

	if(bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		fprintf(stderr, "Error on bind --> %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}

	// Listen a maximum of 1 connections
	if((listen(listen_fd, 2)) == -1) {
		fprintf(stderr, "Error on listen --> %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Started server on %s:%d\n", SERVER_ADDRESS, port);

	while(1){
		// Accepts incoming connections from clients
		if((conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client)) == -1) {
			fprintf(stderr, "Error on accept --> %s", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			//pthread_t sniffer_thread;
			new_socket = malloc (sizeof(int*));
			*new_socket = conn_fd;

			request_handler(new_socket);

			//if(pthread_create(&sniffer_thread, NULL, request_handler, (void*) new_socket) < 0){
			//	fprintf(stderr, "Could not create thread --> %s", strerror(errno));
			//	exit(EXIT_FAILURE);
			//}
		}

	}

	return 0;
}