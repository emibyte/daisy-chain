#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

bool
is_integer(const char *str);
void
strnconcat(char ch, size_t n, char *result, size_t result_size);
int
get_digit_count(int n);

#endif
