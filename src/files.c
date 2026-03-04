#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *
read_file(char *filepath) {
  FILE *file = fopen(filepath, "r");

  if (file == NULL) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  int len = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(sizeof(char) * (len + 1));
  char cur = fgetc(file);
  int idx = 0;

  while (cur != EOF) {
    buffer[idx] = cur;
    cur = fgetc(file);
    idx++;
  }

  buffer[idx] = '\0';
  fclose(file);

  return buffer;
}

bool
write_file(char *filepath, const char *content, size_t length) {
  FILE *file = fopen(filepath, "w");

  if (file == NULL) {
    return false;
  }

  int str_written = fwrite(content, sizeof(char), length, file);
  fclose(file);
  return str_written == length;
}

bool
directory_exists(const char *path) {
  struct stat buffer;
  return (stat(path, &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

char *
get_task_file_path(char *result) {
  char *home_dir = getenv("HOME");
  if (home_dir == NULL) {
    fprintf(stderr, "Could not resolve $HOME environment variable.\n");
    return NULL;
  }
  int buffer_size = strlen(home_dir) + strlen("/.config/task_chain.json") + 1;
  char *path_task_file = malloc(buffer_size);

  int config_buffer_size = strlen(home_dir) + strlen("/.config") + 1;
  char config_buffer[config_buffer_size] = {};
  int bytes_written = sprintf(config_buffer, "%s%s", home_dir, "/.config");

  if (directory_exists(config_buffer)) {
    int bytes_written =
        sprintf(result, "%s%s", home_dir, "/.config/task_chain.json");
    return result;
  } else {
    fprintf(stderr, ".config directory does not exist, very sus.\n");
    return NULL;
  }
}
