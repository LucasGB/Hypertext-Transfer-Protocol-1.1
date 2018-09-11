#include <regex.h>  
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "parser.h"      


ssize_t parse(char *request_buffer, HTTP_REQUEST *req){
	printf("%s\n", request_buffer);

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
		memcpy(first_line, request_buffer, first_line_length);

		// Constructs the HTTP_REQUEST struct
		req -> method = strdup(strtok(first_line, " "));

		char* root = strdup("./root");
		char* aux = strdup(strtok(NULL, " "));

		printf("AUX: %s\n", aux);

		req -> path = (char*) malloc (sizeof(char) * (strlen(root) + strlen(aux)));
		strcpy(req -> path, root);
		strcat(req -> path, aux);
		
		if(strcmp(strtok(NULL, " "), "HTTP/1.1\r")){
			printf("INCORRECT VERSION.\n");
		}
	}



}