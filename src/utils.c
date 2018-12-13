#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;

mime_map meme_types [] = {
    {".css", "text/css"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".dyn", "text/html"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".ico", "image/x-icon"},
    {".js", "application/javascript"},
    {".pdf", "application/pdf"},
    {".mp4", "video/mp4"},
    {".png", "image/png"},
    {".svg", "image/svg+xml"},
    {".xml", "text/xml"},
    {".o", "text/html"},
    {".json", "application/json"},
    {NULL, NULL},
};

char *default_mime_type = "text/plain";

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

char* get_mime_type(char *filename){
    char *dot = strrchr(filename, '.');
    if(dot){ // strrchar Locate last occurrence of character in string
        mime_map *map = meme_types;
        while(map->extension){
            if(strcmp(map->extension, dot) == 0){
                return map->mime_type;
            }
            map++;
        }
    }
    return default_mime_type;
}

char* get_file_extention(char *filename){
    char *dot = strrchr(filename, '.');
    if(dot){ // strrchar Locate last occurrence of character in string
        mime_map *map = meme_types;
        while(map->extension){
            if(strcmp(map->extension, dot) == 0){
                return map->extension;
            }
            map++;
        }
    }
    return default_mime_type;
}

int string_ends_with(char * str, char * suffix) {
  int str_len = strlen(str);
  int suffix_len = strlen(suffix);

  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len-suffix_len), suffix));
}