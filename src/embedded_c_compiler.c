#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils.h"

char* getDate(){
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	return strdup(asctime (timeinfo));
}


char* compile_embbeded_c(char* file){
	FILE *fp;

	if( ( fp = fopen(file, "r") ) == NULL){
		return -1;
	}

	char* buffer;
	char* tmp_buffer;
	char* ret;
	int file_size = get_file_size(file);
	int offset = 0;
	 
	 // allocating mem for file
	buffer = (char*) malloc (sizeof(char) * file_size);
	if (buffer == NULL) { fputs ("Memory error", stderr);  exit(2); }

	fread(buffer, sizeof(char), file_size, fp);

	if( ( ret = strstr(buffer, "<%=") ) == NULL) {
		printf("Missing nuggets\n");
	} else {
		int nugget_pos = file_size - strlen(ret);
		offset += nugget_pos + 3;
		
		char* result;

		// Create duplicate of ret pointer for token extraction
		char* tokens = strdup(ret);

		// <%= token + space
		strtok(tokens, " ");
		offset += 1;

		// function name token
		char* function = strtok(NULL, " ");

		if(strcmp("getDate()", function) == 0){
				result = getDate();
				offset += 9;
		} else {
			printf("Function not defined.\n");
			return;
		}

		// %> token + space
		char* closing_bracket = strtok(NULL, " ");
		offset += 3;

		// Slice buffer from the first occourence of '<%=' up to the next first occourence of '%>'
		char* embedded_code_line = slice(buffer, nugget_pos, offset, 1);

		if(strcmp("%>", closing_bracket) == 0){
		} else {
			printf("Missing '%>' on file.\n");
			return;
		}

		int read_bytes = nugget_pos + 1;

		int tot_siz = read_bytes + strlen(result) + strlen(slice(buffer, offset, file_size, 1));

		tmp_buffer = (char*) malloc (sizeof(char) * tot_siz);
		snprintf(tmp_buffer, tot_siz, "%s%s%s", slice(buffer, 0, strlen(buffer) - strlen(ret), 1), result, slice(buffer, offset, file_size, 1) );

		free(tokens);
		free(embedded_code_line);
		
		return tmp_buffer;
	}

}