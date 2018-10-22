#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils.h"

void getDate(){
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	//snprintf(timestamp, )
	printf ( "Current local time and date: %s", asctime (timeinfo) );

}

void compile_embbeded_c(char* file, char* html_body){
	FILE *fp;

	if( ( fp = fopen(file, "r") ) == NULL){
		return -1;
	}

	char buffer[512];
	char* ret;
	//char* compiled_html = strdup("");

	int read_bytes = get_file_size(file) - strlen(ret);

	printf("SAIUDBHIASB %d\n", read_bytes);


	while(fgets(buffer, 512, fp) != NULL){
		printf("%d\n", fp);
		//compiled_html = realloc (sizeof(char) * );
		if( (ret = strstr(buffer, "<%=") ) != NULL){
			printf("%d\n", strlen(ret));


			strtok(ret, " ");

			char* function = strdup(strtok(NULL, " "));
			
			if(strcmp("getDate()", function) == 0){
				getDate();
			}


			if(strcmp("%>", strdup(strtok(NULL, " "))) == 0){
				printf("ok\n");
			} else {
				printf("Missing '%>' on file.\n");
				return;
			}

			


		}

	}
	

	//printf("%s\n", ret);
}