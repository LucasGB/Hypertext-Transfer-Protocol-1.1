#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

typedef struct {
	char* key;
	char* value;
} param;

typedef struct {
	char* method;
	char* path;
	char* cookie;
	char* authorization;
	char* query_string;

	param** params;

	//off_t offset;
	//size_t end;
} HTTP_REQUEST; 

#endif