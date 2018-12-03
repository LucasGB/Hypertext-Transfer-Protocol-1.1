#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "cgi_module.h"
#include "http_request.h"

#define READ 0
#define WRITE 1

pid_t popen2(const char *command, int *infp, int *outfp) {

	/* Define file descriptors for stdin and stdout */
	int p_stdin[2], p_stdout[2];
	
	/* Define process id variable for our child process */
	pid_t child_pid;

	/*
	* To create a simple pipe with C, we make use of the pipe() system call.
	* It takes a single argument, which is an array of two integers, and if successful,
	* the array will contain two new file descriptors to be used for the pipeline.
	*/
	if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
		return -1;

	/* Once we established the pipeline, we then fork our child process */
	child_pid = fork();
	
	/* Return the negative value if the creation of a child process was unsucessful */
	if (child_pid < 0)
		return child_pid;

	/*
	* If the parent wants to receive data from the child, it should close fd1, and the child should close fd0.
	* If the parent wants to send data to the child, it should close fd0, and the child should close fd1.
	* Since descriptors are shared between the parent and child, we should always be sure to close the end 
	* of pipe we aren't concerned with. On a technical note, the EOF will never be returned if the
	* unnecessary ends of the pipe are not explicitly closed. 
	*/
	else if (child_pid == 0) {
		/* If this is the child process, then we close stdin[1] */
		close(p_stdin[WRITE]);
		/* Standard input will be a copy of the pipe's standard file descriptor input */
		dup2(p_stdin[READ], READ);

		/* Close stdout[0] */
		close(p_stdout[READ]);
		
		/* Standard output will be a copy of the pipe's standard file descriptor output */
		dup2(p_stdout[WRITE], WRITE);

		execl("/bin/sh", "sh", "-c", command, NULL);
		perror("execl");
		exit(1);
	}

	if (infp == NULL)
		close(p_stdin[WRITE]);
	else
		*infp = p_stdin[WRITE];
	
	if (outfp == NULL)
		close(p_stdout[READ]);
	else
		*outfp = p_stdout[READ];
	
	return child_pid;
}

char* call_cgi(HTTP_REQUEST* req) {

	/* Find the resource name */
	char *resource = strrchr(req -> path, '/');
	++resource;

	int infp, outfp;
	char buf[128];

	/* Calculates the size of command string */
	int command_length = snprintf(NULL, 0, "%s %s %s %s", req -> path, req -> params[0] -> value, req -> params[1] -> value, req -> params[2] -> value) + 1;
	char command[command_length];
	snprintf(command, command_length, "%s %s %s %s", req -> path, req -> params[0] -> value, req -> params[1] -> value, req -> params[2] -> value);
				
	if (popen2(command, &infp, &outfp) <= 0) {
		printf("Unable to exec %s\n", command);
		exit(1);
	}

	memset(buf, 0x0, sizeof(buf));
	//write(infp, "Z\n", 2);
	//close(infp);
				
	read(outfp, buf, 128);

	int html_length = snprintf(NULL, 0, "<!DOCTYPE html><html><head><title>Calculadora de idade</title></head><body><div id=\"texto\">%s</div><script type=\"text/javascript\"></script></body></html>", buf) + 1;
	char *html = (char*) malloc (sizeof(char) * html_length);
	snprintf(html, html_length, "<!DOCTYPE html><html><head><title>Calculadora de idade</title></head><body><div id=\"texto\">%s</div><script type=\"text/javascript\"></script></body></html>", buf);

	return html;
}