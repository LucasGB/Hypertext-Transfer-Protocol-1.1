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
#include "dirent.h"


char* build_html(char* path) {

	char* html_version = strdup("<!DOCTYPE html>\n<html>\n");
	char* head_element = strdup("<head>\n<title>HTTP Server</title>\n</head>\n");
	char* body_element = NULL;
	int body_element_length = 0;
	char* footer_element = strdup("</body>\n</html>");

	char* temp_content = strdup("<!DOCTYPE html>\n<html>\n<head>\n<title>HTTP Server</title>\n</head>\n<body>\n");
	char* content;
	int temp_content_length = strlen(temp_content);

	DIR *d;
	struct dirent *dir;

	d = opendir(path);

	// Walks the cursor up to 7 bytes to remove "./root/" from path pointer
	path += 7;

	// Dynamically inserts links to directories in the html content
	if (d) {
		while ((dir = readdir(d)) != NULL) {
		    	char link[snprintf(NULL, 0, "<a href=\"/%s\">%s</a>\n", dir -> d_name, dir -> d_name)];

		    	sprintf(link, "<a href=\"%s/%s\">%s</a>\n", path, dir -> d_name, dir -> d_name);

		    	printf("Link: %s\n", link);

		    	body_element_length += strlen(link);

		    	char* temp_body = NULL;

		    	if(body_element) temp_body = strdup(body_element);

		    	free(body_element);
		    	
		    	body_element = (char*) malloc (sizeof(body_element_length));

		    	if(temp_body){
		    		snprintf(body_element, "%s%s", temp_body, link);
		    	} else {
		    		snprintf(body_element, "%s", link);
		    	}
		    	printf("Body Link: %s\n", body_element);
	    }
	    closedir(d);
	}

	//temp_content_length += strlen("</body>\n</html>");
	//temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length);
	//strcat(temp_content, "</body>\n</html>");
	//snprintf(temp_content, sizeof(temp_content), "%s</body>\n</html>", temp_content);
	//strncat(temp_content, "</body>\n</html>", temp_content_length);

	//printf("%s\n", temp_content);
	int html_content_length = (strlen(html_version) + strlen(head_element) + strlen(body_element) + strlen(footer_element));

	char* html_content = (char*) malloc (sizeof(char) * html_content_length);
	
	snprintf(html_content, html_content_length, "%s\n%s%s%s", html_version, head_element, body_element, footer_element);

	printf("HTML: %s\n", html_content);
	
	return html_content;
}

void build_response(void *new_socket, char* path) {
	char *html_content = build_html(path);
	char *temp_header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";

	int html_content_length = strlen(html_content);

	// Creates a header string with the size of temp_header length plus the string size of html content length
	char *header = (char*) malloc (sizeof(char) * (strlen(temp_header) + snprintf(NULL, 0, "%d", html_content_length) + 4));
	sprintf(header, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\r\n\r\n", html_content_length);

	// Creates response  with the necessary size. (header length  + content length)
	char* resp = (char*) malloc (sizeof(char) * (strlen(header) + html_content_length));

	// Concatenates header and html to response
	strcpy(resp, header);
	strcat(resp, html_content);

	send_new(*(int*) new_socket, resp);

	free(header);
	free(resp);
}

void request_handler(void *new_socket) {
	HTTP_REQUEST* req = malloc (sizeof(HTTP_REQUEST));

	char request_buffer[BUFFER_SIZE];

	// Receives request from client
	if(recv(*(int*) new_socket, request_buffer, sizeof(request_buffer), MSG_PEEK) == -1) {
		fprintf(stderr, "Error to receive request from client --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	parse(request_buffer, req);

	if(!strcmp(req -> method, "GET")){

		build_response(new_socket, req -> path);

		free(req);
		close(*(int*) new_socket);

		printf("Socket closed.\n");
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

	while(1){
		// Accepts incoming connections from clients
		if((conn_fd = accept(listen_fd, (struct sockaddr *) &client_address, &client)) == -1) {
			fprintf(stderr, "Error on accept --> %s", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			printf("Client connected.\n");

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