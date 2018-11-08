#include <regex.h>  
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "parser.h"  
#include "errors.h"    

void print_header(char *request_buffer){
	printf("---------------- Inicio do HEADER -------------\n");
	printf("%s", request_buffer);
	printf("---------------- Fim do HEADER -------------\n");
}

void parse_query_string(HTTP_REQUEST *req, char* url){
	char *q_mark = strrchr(url, '?');
	req -> query_string = NULL;
	if(q_mark){
		printf("FOUND QUESTION MARk\n");
		// Remove '?' from the query string
		req -> query_string = strdup(++q_mark);
	}
}

ssize_t parse(char *request_buffer, HTTP_REQUEST *req){
	print_header(request_buffer);

	// Search for CRLF in request buffer
	char *ret = strstr(request_buffer, "\r\n\r\n");

	// If the request is too long, throw error
	if(ret == NULL){
		printf("Error 413. Request Entity Too Large.");
	} else {
		// Calculates the request header length
		int request_length = strlen(request_buffer);

		// Calculates the first line length from the request
		int first_line_length = request_length - strlen(strstr(request_buffer, "\n"));

		// Properly allocates memory to store content from the first line
		char *first_line = (char*) malloc (first_line_length * sizeof(char));

		// Copies the first line from the request buffer to the char pointer
		//memcpy(first_line, request_buffer, first_line_length);
		snprintf(first_line, first_line_length, "%s", request_buffer);

		// Constructs the HTTP_REQUEST struct
		req -> method = strdup(strtok(first_line, " "));

		char* url = strdup(strtok(NULL, " "));
		printf("url\n");

		parse_query_string(req, url);
		
		int query_string_length = 0;
		if(req -> query_string) {
			query_string_length = strlen(req -> query_string) + 1;
			printf("QUERY STRING: %s\n", req -> query_string);
		}

		printf("query\n");
		printf("%d\n", query_string_length);
		printf("len query\n");
		printf("%d\n", strlen(url));
		printf("lenurl\n");

		req -> path = (char*) malloc (sizeof(char) * (strlen("./root") + strlen(url) - query_string_length) + 1);
		snprintf(req -> path, strlen(url) + strlen("./root") - query_string_length + 1, "./root%s", url);
		printf("path\n");
		char* version = strdup(strtok(NULL, " "));

		char* auth = strstr(request_buffer, "Authorization: Basic ");
		if(auth == NULL){
			req -> authorization = NULL;
		} else {
			strtok(auth, " ");
			strtok(NULL, " ");
			req -> authorization  = strdup(strtok(NULL, " "));
		}
		printf("auth\n");


		char* cookie = strstr(request_buffer, "Cookie");
		if(cookie == NULL){
			req -> cookie = strdup("Set-Cookie: cookie-count=1\n");
		} else {
			strtok(cookie, "=");

			int cookie_count = atoi(strdup(strtok(NULL, "="))) + 1;

			// Why did I write this?
			//free(req -> cookie);

			int cookie_header_size = snprintf(NULL, 0, "Set-Cookie: cookie-count=%d\n", cookie_count) + 1;

			req -> cookie = (char*) malloc (sizeof(char) * cookie_header_size);

			snprintf(req -> cookie, cookie_header_size, "Set-Cookie: cookie-count=%d\n", cookie_count);
		}

		printf("cookie\n");

						
		if(strcmp(version, "HTTP/1.1")){
			printf("INCORRECT VERSION.\n");
		}

		//printf("%s\n", req -> query_string);
		//printf("%s\n", req -> path);
		//printf("%s\n", url);

		free(first_line);
		free(url);
	}

}