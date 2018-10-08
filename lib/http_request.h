#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

typedef struct {
	char* method;
	char* path;
	char* cookie;
	//off_t offset;
	//size_t end;
} HTTP_REQUEST; 

#endif