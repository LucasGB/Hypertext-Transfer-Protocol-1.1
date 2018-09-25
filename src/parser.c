#include <regex.h>  
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "parser.h"      


ssize_t parse(char *request_buffer, HTTP_REQUEST *req){
	printf("---------------- Inicio do HEADER -------------\n");
	printf("%s", request_buffer);
	printf("---------------- Fim do HEADER -------------\n");

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

		char* aux = strdup(strtok(NULL, " "));

		req -> path = (char*) malloc (sizeof(char) * (strlen("./root") + strlen(aux)) + 1);
		snprintf(req -> path, strlen(aux) + strlen("./root") + 1, "./root%s", aux);

		char* version = strdup(strtok(NULL, " "));
		
		//printf("Method: %s\n", req -> method);
		printf("PARSER Path: %s\n", req -> path);
		//printf("Version: %s\n", version);
		//printf("First line: %d\n", strlen(first_line));
				
		if(strcmp(version, "HTTP/1.1")){
			printf("INCORRECT VERSION.\n");
		}

		free(first_line);
		free(aux);
	}



}