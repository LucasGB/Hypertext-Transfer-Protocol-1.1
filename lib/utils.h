#ifndef UTILS_H
#define UTILS_H

size_t get_file_size(const char* filename);
char *slice(char *string, int start, int stop, int step);
pid_t popen2(const char *command, int *infp, int *outfp);

#endif