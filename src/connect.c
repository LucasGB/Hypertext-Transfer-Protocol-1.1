#include "connect.h"


int init_connection(char *hostname, char *port, struct addrinfo **res) {
    int sockfd;
	struct sockaddr_in addr;
	struct hostent * host_ent;
	char * hostAddr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(9090);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {  
		printf("Error creating socket!\n");  
		exit(1);  
	}
	printf("Socket created...\n");

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		printf("Error binding socket to port!\n");  
		exit(1);
	}

	printf("Binding done...\n");

	return sockfd;
}

int connection(int fd) {
	char request[500], resource[500], *ptr;		
	int fd1, length;

	if(recv_new(fd, request) == 0) {
		printf("Recieve failed\n");
	}
	// Checking for a valid browser request
	ptr = strstr(request," HTTP/");
	if(ptr == NULL) {
		printf("NOT HTTP!!\n");
	} else {
		*ptr = 0;
		ptr = NULL;
	
		if(strncmp(request, "GET ", 4) == 0)	{
			ptr = request + 4;
		}
		if(strncmp(request, "HEAD ", 5) == 0)
			ptr = request + 5;
		if(ptr == NULL) {
			printf("Unknown Request !!! \n");
		} else {
			if(ptr[strlen(ptr) - 1] == '/' ) {
				strcat(ptr,"index.html");
			}
			strcpy(resource, ROOT);
			strcat(resource, ptr);
			fd1 = open(resource, O_RDONLY,0);
			printf("Opening \"%s\"\n",resource);
			if(fd1 == -1) {
				printf("404 File not found Error\n");
				send_new(fd,"HTTP/1.0 404 Not Found\r\n");
				send_new(fd,"Server : Lucas/Private\r\n\r\n");
				send_new(fd,"<html><head><title>404 not found! :( </head></title>");
				send_new(fd,"<body><h1>Url not found</h1><br><p>Sorry, the url you were searching for was not found on this server.</p><br><br><br><h1>Lucas's Server</h1></body></html>");
			} else {
				printf("200 OK!!!\n");
				send_new(fd,"HTTP/1.0 200 OK\r\n");
				send_new(fd,"Server : Lucas/Private\r\n\r\n");
			
				if( ptr == request+4 ) // if it is a get request
				{
					if( (length = get_file_size(fd1)) == -1 )
						printf("Error getting size \n");
					if( (ptr = (char *)malloc(length) ) == NULL )
						printf("Error allocating memory!!\n");
					read(fd1,ptr,length);
			
					if(send(fd,ptr,length,0) == -1) {
						printf("Send err!\n");
					}
					free(ptr);
				}
			}
			close(fd);
		}
	}
	shutdown(fd,SHUT_RDWR);
}			

int get_file_size(int fd)
{
	struct stat stat_struct;
	if(fstat(fd, &stat_struct) == -1)
		return(1);
	return (int)stat_struct.st_size;
}
void send_new(int fd,char *msg)
{
	int len = strlen(msg);
	if(send(fd,msg,len,0) == -1)
	{
		printf("Error in send\n");
	}
}
int recv_new(int fd,char *buffer)		
{
	#define EOL "\r\n"
	#define EOL_SIZE 2
	char *p = buffer;			
	int eol_matched=0;			
	while(recv(fd,p,1,0)!=0)		
	{
		if( *p == EOL[eol_matched])
		{	
			++eol_matched;		
			if(eol_matched==EOL_SIZE)	
			{
				*(p+1-EOL_SIZE) = '\0';	
				return(strlen(buffer));	
			}
		}
		else
		{
			eol_matched=0;			
		}
		p++;					
	}
	return(0);
}

int check_errors(){

}