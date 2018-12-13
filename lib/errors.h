#ifndef ERRORS_H
#define ERRORS_H

void error_301(void* client_socket, HTTP_REQUEST *req);
void error_401(void* client_socket);
void error_404(void* client_socket, HTTP_REQUEST *req);

#endif