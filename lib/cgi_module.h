#ifndef CGI_MODULE_H
#define CGI_MODULE_H

#include "http_request.h"

pid_t popen2(const char *command, int *infp, int *outfp);
char* call_cgi(HTTP_REQUEST* req);

#endif