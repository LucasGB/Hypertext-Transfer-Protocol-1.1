#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>


size_t get_file_size(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;   
}

char *slice(char *string, int start, int stop, int step) {
    size_t string_length = ceil(fabs((stop - start) / (float) step));

    char *new_string = malloc(string_length + 1);

    for (int i = start, j = 0; j < string_length; i += step, j++) {
        new_string[j] = string[i];
    }

    new_string[string_length] = '\0';

    return new_string;
}