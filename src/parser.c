#include <regex.h>  
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "parser.h"      


ssize_t parse(char *request_buffer, HTTP_REQUEST *req){
	printf("%s\n", request_buffer);

	char *ret = strstr(request_buffer, "\r\n\r\n");

	if(ret == NULL){
		printf("Error 413. Request Entity Too Large.");
	} else {
		int request_length = strlen(request_buffer);
		int first_line_length = request_length - strlen(strstr(request_buffer, "\n"));

		char *first_line = (char*) malloc (first_line_length * sizeof(char));

		memcpy(first_line, request_buffer, first_line_length);

		req -> method = strdup(strtok(first_line, " "));
		req -> path = strdup(strtok(NULL, " "));

		if(strcmp(strtok(NULL, " "), "HTTP/1.1\r")){
			printf("INCORRECT VERSION.\n");
		}

		printf("ASNdhiasd\n");
	}

	printf("Saiu");

}