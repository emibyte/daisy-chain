#ifndef FILES_H
#define FILES_H

#include <stdbool.h>
#include <stdlib.h>

char *
read_file(char *filepath);
bool
write_file(char *filepath, const char *content, size_t length);
char *
get_task_file_path(char *result);

#endif
