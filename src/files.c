#include "files.h"
#include <stdlib.h>
#include <stdio.h>

char *read_file(char *filepath) {
  FILE *file = fopen(filepath, "r");

  if (file == NULL) {return NULL;}

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
