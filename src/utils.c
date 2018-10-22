#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

size_t get_file_size(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;   
}