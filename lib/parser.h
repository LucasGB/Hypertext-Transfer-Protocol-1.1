#ifndef PARSER_H
#define PARSER_H

#include "http_request.h"

#define HTTP_regex "HTTP/1.1"
#define GET_regex "GET "

ssize_t parse(char* request_buffer, HTTP_REQUEST *req);

void print_header(char *request_buffer);

#endif