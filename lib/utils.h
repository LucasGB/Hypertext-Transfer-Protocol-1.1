#ifndef UTILS_H
#define UTILS_H

size_t get_file_size(const char* filename);
char *slice(char *string, int start, int stop, int step);
pid_t popen2(const char *command, int *infp, int *outfp);
char* get_mime_type(char *filename);
char* get_file_extention(char *filename);

#endif