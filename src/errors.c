#include <string.h>

void error_404(void* client_socket){
	char* temp_header = strdup("HTTP/1.1 Not Found\nContent-Type: text/html\nContent-Length: \r\n\r\n");
	char* html_body = strdup("<!DOCTYPE html>\n<head>    \n<title>We've got some trouble | 404 - Resource not found</title>\n</head>\n<body>    \n<div class=\"cover\"><h1>Resource not found <small>Error 404</small></h1><p class=\"lead\">The requested resource could not be found but may be available again in the future.</p></div>    \n<footer><p>Technical Contact: <a href=\"mailto:x@example.com\">x@example.com</a></p></footer>\n</body>\n</html>");

	int html_body_length = strlen(html_body);
	int header_length = strlen(temp_header) + snprintf(NULL, 0, "%d", strlen(html_body_length)) + 1;
	
	char* response = (char*) malloc (sizeof(char) * (header_length + html_body_length));
	snprintf(response, header_length + html_body_length, "HTTP/1.1 Not Found\nContent-Type: text/html\nContent-Length: %d\r\n\r\n%s", html_body_length, html_body);

	if(send(*(int*) client_socket, response, strlen(response), 0) == -1) {
		printf("Error in send\n");
	}

	free(response);
	free(html_body);
	free(temp_header);
}