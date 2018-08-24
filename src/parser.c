#include <regex.h>  
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "parser.h"      


ssize_t parse(char *request_buffer, HTTP_REQUEST *req){
	printf("%s\n", request_buffer);
	
	char *ret = strstr(request_buffer, "\r\n\r\n");

	if(ret == NULL){
		printf("Error 413.");
	} else {
		int pos = ret - request_buffer;
		printf("%d\n", pos);
	}

	//ret = strstr(request_buffer, "")

	//char *eol = strchr(request_buffer, '\n');
	char *eol;

	fgets(eol, 512, request_buffer);
	
	printf("%s\n", eol);
	char *method;
	char *path;
	char *version;

	sscanf(eol, "%s %s", method, path);

	printf("%s\n", method);
	printf("%s\n", path);
	//printf("%s\n", version);

}