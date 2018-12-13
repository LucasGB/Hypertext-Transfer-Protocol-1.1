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
#include <time.h>

#include "connect.h"
#include "parser.h"
#include "http_request.h"
#include "errors.h"
#include "b64.h"
#include "dirent.h"
#include "utils.h"
#include "embedded_c_compiler.h"
#include "cJSON.h"

#define BUFSIZ 256

int n_requests = 0;
time_t start_t, end_t;
double diff_t;

void render_telemetria(void *client_socket, HTTP_REQUEST *req){
	printf("SENDING TELEMETRIA HTML\n");
	char* path = strdup("./res/templates/telemetria.html");

	char *file_type = get_mime_type(path);
	int file_size = get_file_size(path);
	printf("SIZE: %d\n", file_size);
	printf("%s\n", file_type);
	
	int header_size = strlen(file_type) + strlen(req -> cookie) + snprintf(NULL, 0, "%d", file_size) + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;
	char *header = (char*) malloc (sizeof(char) * header_size);
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\n%s\r\n\r\n", file_type, file_size, req -> cookie);
	
	send_new(*(int*) client_socket, header);
	
	int remain_data = file_size;
	int sent_bytes = 0;
	off_t offset = 0;
	int fd = open(path, O_RDONLY);
	// Force flush socket stream
	while(((sent_bytes = sendfile(*(int*) client_socket, fd, &offset, 256)) > 0) && remain_data > 0) {
		int flag = 1;
		setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		remain_data -= sent_bytes;
		flag = 0; 
		setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	}

	close(fd);
	free(header);
}

void render_directory(void *client_socket, HTTP_REQUEST *req){
	printf("Printing dir\n");
	printf("%s\n", req -> path);
	char* path = strdup("./res/templates/directory.html");

	char *file_type = get_mime_type(path);
	int file_size = get_file_size(path);
	printf("SIZE: %d\n", file_size);
	printf("%s\n", file_type);
	
	int header_size = strlen(file_type) + strlen(req -> cookie) + snprintf(NULL, 0, "%d", file_size) + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;
	char *header = (char*) malloc (sizeof(char) * header_size);
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\n%s\r\n\r\n", file_type, file_size, req -> cookie);
	
	send_new(*(int*) client_socket, header);
	
	int remain_data = file_size;
	int sent_bytes = 0;
	off_t offset = 0;
	int fd = open(path, O_RDONLY);
	// Force flush socket stream
	while(((sent_bytes = sendfile(*(int*) client_socket, fd, &offset, 256)) > 0) && remain_data > 0) {
		int flag = 1;
		setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		remain_data -= sent_bytes;
		flag = 0; 
		setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	}

	close(fd);
	free(header);
}

void render_directory_old(void *client_socket, HTTP_REQUEST *req) {

	char* temp_content = strdup("<!DOCTYPE html>\n<html>\n<head>\n<title>HTTP Server</title>\n</head>\n<body>\n");
	char* footer_element = strdup("</body>\n</html>");
	int temp_content_length = strlen(temp_content);

	DIR *d;
	struct dirent *dir;

	d = opendir(req -> path);

	char* abs_path = strdup(req -> path);

	// Walks the cursor up to 6 bytes to remove "./root" from path pointer
	char* path = strdup(req -> path);
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

			printf("DIR ELEMENT: %s\n", dir_element);

			struct stat statbuf;
			int fd = open(dir_element, O_RDONLY);
			fstat(fd, &statbuf);

			if(S_ISDIR(statbuf.st_mode)){

				char link[snprintf(NULL, 0, "<br><a href=\"%s%s/\">%s</a>\n", path, dir -> d_name, dir -> d_name)];
		    	sprintf(link, "<br><a href=\"%s%s/\">%s</a>\n", path, dir -> d_name, dir -> d_name);

			    temp_content_length += strlen(link);
		    	temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
		    	strncat(temp_content, link, strlen(link));
			} else {
				char link[snprintf(NULL, 0, "<br><a href=\"%s%s\">%s</a>\n", path, dir -> d_name, dir -> d_name)];
		    	sprintf(link, "<br><a href=\"%s%s\">%s</a>\n", path, dir -> d_name, dir -> d_name);

		    	temp_content_length += strlen(link);
	    		temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
	    		strncat(temp_content, link, strlen(link));
			}
		}
		    	
	    closedir(d);
	}

	temp_content_length += strlen("</body>\n</html>");
	temp_content = (char*) realloc (temp_content, sizeof(char) * temp_content_length + 1);
	strncat(temp_content, footer_element, strlen(footer_element));

	int html_content_length = strlen(temp_content);

	int header_size = strlen("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: \r\n\r\n") + strlen(req -> cookie) + snprintf(NULL, 0, "%d", html_content_length) + 1;

	// Creates a header string with the size of temp_header length plus the string size of html content length
	char *header = (char*) malloc (sizeof(char) * header_size);
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\n%s\r\n\r\n", html_content_length, req -> cookie);

	// Creates response  with the necessary size. (header length  + content length)
	char* resp = (char*) malloc (sizeof(char) * (strlen(header) + html_content_length) + 1);

	// Concatenates header and html to response
	strcpy(resp, header);
	strcat(resp, temp_content);

	send_new(*(int*) client_socket, resp);

	free(header);
	free(temp_content);
	free(footer_element);
	free(resp);
}

void send_dir_json(void *client_socket, HTTP_REQUEST *req){
	DIR *d;
	struct dirent *dir;
	printf("CREATING JSON\n");
	
	char *out;
   	cJSON *root, *directories, *directory, *files, *file;

   	/* create root node and array */
   	root = cJSON_CreateObject();
   	directories = cJSON_CreateArray();
   	files = cJSON_CreateArray();

   	/* add cars array to root */
   	cJSON_AddItemToObject(root, "directories", directories);
   	cJSON_AddItemToObject(root, "files", files);

	char* prefix = "./root";

	int path_length = snprintf(NULL, 0, "./root%s", req -> query_string) + 1;
	printf("%d\n", path_length);
	char* path = (char*) malloc (sizeof(char) * path_length);
	snprintf(path, path_length, "./root%s", req -> query_string);


	d = opendir(path);
	printf("OPENED DIR\n");

	char* abs_path = strdup(req -> query_string);
	printf("ABS PATH: %s\n", abs_path);

	printf("DIR PATH %s\n", path);

	// Dynamically inserts links to directories in the html content
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if(!strcmp(dir -> d_name, ".") || !strcmp(dir -> d_name, "..") || 
				!strcmp(dir -> d_name, "3rdpartylicenses.txt") || !strcmp(dir -> d_name, "favicon.ico") || 
				!strcmp(dir -> d_name, "main.5943c4b105b85d7298ee.js") || !strcmp(dir -> d_name, "polyfills.20ab2d163684112c2aba.js") || 
				!strcmp(dir -> d_name, "runtime.ec2944dd8b20ec099bf3.js") || !strcmp(dir -> d_name, "styles.3ff695c00d717f2d2a11.css")){
				continue;
			}
			printf("%s\n", dir -> d_name);
			
			int dir_element_length = strlen(path) + strlen(dir -> d_name) + 1;
			char* dir_element = (char*) malloc (sizeof(char) * dir_element_length);
			snprintf(dir_element, dir_element_length, "%s%s", path, dir -> d_name);

			printf("DIR ELEMENT: %s\n", dir_element);

			struct stat statbuf;
			int fd = open(dir_element, O_RDONLY);
			fstat(fd, &statbuf);
			char date[10];
			strftime(date, 10, "%d-%m-%y", localtime(&(statbuf.st_ctime)));
			printf("%s\n", date);
			off_t size = statbuf.st_size;

			if(S_ISDIR(statbuf.st_mode)){

				/* add directory content array */
				cJSON_AddItemToArray(directories, directory = cJSON_CreateObject());
				cJSON_AddItemToObject(directory, "name", cJSON_CreateString(dir -> d_name));
				cJSON_AddItemToObject(directory, "modified", cJSON_CreateString(date));
				cJSON_AddItemToObject(directory, "size", cJSON_CreateString("--"));
				

			} else {
				/* add file contentarray */
				cJSON_AddItemToArray(files, file = cJSON_CreateObject());
				cJSON_AddItemToObject(file, "name", cJSON_CreateString(dir -> d_name));
				cJSON_AddItemToObject(file, "modified", cJSON_CreateString(date));
				cJSON_AddItemToObject(file, "size", cJSON_CreateNumber(size));

			}
			
		}
		    	
	    closedir(d);
	}

	/* print everything */
	out = cJSON_Print(root);
	printf("%s\n", out);
	printf("LEN OUT: %d\n", strlen(out));

	int header_size = strlen("HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: \r\n\r\n") + strlen(req -> cookie) + snprintf(NULL, 0, "%d", strlen(out)) + 1;
	printf("SIZE HEADER: %d\n", header_size);

	// Creates a header string with the size of temp_header length plus the string size of html content length
	char *header = (char*) malloc (sizeof(char) * header_size);
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: %d\n%s\r\n\r\n", strlen(out), req -> cookie);

	// Creates response  with the necessary size. (header length  + content length)
	char* resp = (char*) malloc (sizeof(char) * (strlen(header) + strlen(out)));

	// Concatenates header and html to response
	strcpy(resp, header);
	strcat(resp, out);

	send_new(*(int*) client_socket, resp);

   	/* free all objects under root and root itself */
   	//cJSON_Delete(root);

	//free(header);
	//free(resp);
}

void send_telemetria_json(void *client_socket, HTTP_REQUEST *req){
	time(&end_t);

	diff_t = difftime(end_t, start_t);

	char *out;
 	cJSON *root, *up_time, *requests;

	root = cJSON_CreateObject();
  	cJSON_AddItemToObject(root, "up_time", cJSON_CreateNumber(diff_t));
  	cJSON_AddItemToObject(root, "n_requests", cJSON_CreateNumber(n_requests/ 2 + 1));

  	out = cJSON_Print(root);
	printf("%s\n", out);

	int header_size = strlen("HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: \r\n\r\n") + strlen(req -> cookie) + snprintf(NULL, 0, "%d", strlen(out)) + 1;
	printf("SIZE HEADER: %d\n", header_size);

	// Creates a header string with the size of temp_header length plus the string size of html content length
	char *header = (char*) malloc (sizeof(char) * header_size);
	snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: %d\n%s\r\n\r\n", strlen(out), req -> cookie);

	// Creates response  with the necessary size. (header length  + content length)
	char* resp = (char*) malloc (sizeof(char) * (strlen(header) + strlen(out)));

	// Concatenates header and html to response
	strcpy(resp, header);
	strcat(resp, out);

	send_new(*(int*) client_socket, resp);
}

void serve_file(void *client_socket, HTTP_REQUEST *req){

	char *dot = strrchr(req -> path, '.');
	if(dot){
		printf("FOUND DOT\n");
	} else{
		printf("NO DOT\n");
	}

	char* header;
	int header_size;
	char *file_type;
	int file_size;

	if(strcmp(".dyn", dot) == 0) {
		char* html_body = compile_embbeded_c(req -> path);
		int html_body_length = strlen(html_body);

		header_size = strlen("text/html") + strlen(req -> cookie) + html_body_length + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;
		header = (char*) malloc (sizeof(char) * header_size);
		snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\n%s\r\n\r\n", html_body_length, req -> cookie);

		char* resp = (char*) malloc (sizeof(char) * (header_size + html_body_length) );
		snprintf(resp, header_size + html_body_length, "%s%s", header, html_body);
		
		send_new(*(int*) client_socket, resp);
	}
	else {
		file_type = get_mime_type(req -> path);
		file_size = get_file_size(req -> path);

		header_size = strlen(file_type) + strlen(req -> cookie) + snprintf(NULL, 0, "%d", file_size) + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;

		header = (char*) malloc (sizeof(char) * header_size);
		snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\n%s\r\n\r\n", file_type, file_size, req -> cookie);

		send_new(*(int*) client_socket, header);

		int remain_data = file_size;
		int sent_bytes = 0;
		off_t offset = 0;
		int fd = open(req -> path, O_RDONLY);

		// Force flush socket stream
		while(((sent_bytes = sendfile(*(int*) client_socket, fd, &offset, 256)) > 0) && remain_data > 0){
			int flag = 1;
			setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
			remain_data -= sent_bytes;
			flag = 0; 
			setsockopt(*(int*) client_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		}

		close(fd);
	}
	
	free(header);
}

int authenticate(void *client_socket, HTTP_REQUEST* req){
	char* dec;
	if(req -> authorization == NULL){
		error_401(client_socket);
		
		return 0;
	} else {
		dec = b64_decode(req -> authorization, strlen(req -> authorization));	

		char *login = strdup(strtok(dec, ":"));
		char* password = strdup(strtok(NULL, ":"));

		if(!strcmp(login, "admin") && !strcmp(password, "admin")){
			return 1;
		} else {
			error_401(client_socket);
		}
	}
}

void request_handler(void *client_socket) {
	HTTP_REQUEST* req = (HTTP_REQUEST*) malloc (sizeof(HTTP_REQUEST));

	n_requests += 1;
	int fd;
	int sent_bytes = 0;
	int remain_data;
	int* offset;
	ssize_t len;
	char request_buffer[1024];

	for (;;) {
	  ssize_t bytes_received = recv ( *(int*) client_socket , request_buffer , sizeof(request_buffer), 0 ) ;
	  if ( bytes_received == -1 ) {
	    return 0 ;
	  }

	  *(request_buffer+bytes_received) = '\0';

	  if ( bytes_received == 0 )
	    return 0;

	  break;
	}

	parse(request_buffer, req);

	int auth = authenticate(client_socket, req);

	if(auth == 1){
		if(!strcmp(req -> method, "GET")){

			struct stat statbuf;

			printf("PATH: %s\n", req -> path);

			if((fd = open(req -> path, O_RDONLY, 0)) <= 0){
				printf("COULDNT OPEN FD\n");
				char* ret = strstr(req -> path, "virtual");
				printf("RET %s\n", ret);
				if(ret != NULL) {
					if(strcmp("virtual/dir.json", ret) == 0){
						send_dir_json(client_socket, req);
					}
					else if(strcmp("virtual/telemetria.html", ret) == 0){
						render_telemetria(client_socket, req);
					}
					else if(strcmp("virtual/telemetria.json", ret) == 0){
						send_telemetria_json(client_socket, req);
					} else {
						error_404(client_socket, req);	
					}
				} else {
					error_404(client_socket, req);
				}
	            fprintf(stderr, "Error opening file --> %s", strerror(errno));
			} else {
				fstat(fd, &statbuf);

				/* If the regular file is executable */
				if(S_ISREG(statbuf.st_mode) && statbuf.st_mode & 0111) {
					printf("CGI REQUESTED !!!\n");

					char *html_body = call_cgi(req);
					int html_body_length = strlen(html_body);

					int header_size = strlen("text/html") + strlen(req -> cookie) + html_body_length + strlen("HTTP/1.1 200 OK\nContent-Type: \nContent-Length: \r\n\r\n") + 1;
					char* header = (char*) malloc (sizeof(char) * header_size);
					snprintf(header, header_size, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\n%s\r\n\r\n", html_body_length, req -> cookie);

					char* resp = (char*) malloc (sizeof(char) * (header_size + html_body_length) );
					snprintf(resp, header_size + html_body_length, "%s%s", header, html_body);

					printf("RESPONSE: %s\n", resp);
					
					send_new(*(int*) client_socket, resp);

					free(html_body);
					free(header);
					free(resp);
					close(fd);
				} 
				/* If it's a non-executable regular file */
				else if(S_ISREG(statbuf.st_mode)){

					if(req -> query_string){
						printf("\nQSTRING: %s\n", req -> query_string);
					} else {
						serve_file(client_socket, req);
					}
				/* If it's a directory */
				} else if(S_ISDIR(statbuf.st_mode)){
					//if(string_ends_with(req -> path, "/") == 0){
						//error_301(client_socket, req);
					//}
					render_directory(client_socket, req);
				}
			}

			//free(req);
		}

	}

	free(req -> query_string);
	
	free(req);
	close(*(int*) client_socket);
}

void send_new(int fd, char *msg) {
	if(send(fd, msg, strlen(msg), 0) == -1) {
		printf("Error in send\n");
	}
}

int main(int argc, char* argv[]) {

	time(&start_t);
	//	char request_buffer[BUFFER_SIZE];
	int *client_socket;

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
			pthread_t sniffer_thread;
			
			client_socket = malloc (sizeof(int*));
			*client_socket = conn_fd;

			request_handler(client_socket);

			if(pthread_create(&sniffer_thread, NULL, request_handler, (void*) client_socket) < 0){
				fprintf(stderr, "Could not create thread --> %s", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

	}

	return 0;
}