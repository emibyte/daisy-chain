#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

bool
is_integer(const char *str) {
  if (str == NULL || *str == '\0')
    return 0;

  int idx = 0;
  for (; str[idx] != '\0'; idx++) {
    if (!isdigit(str[idx])) {
      return false;
    }
  }
  return (idx > 0);
}

void
strnconcat(char ch, size_t n, char *result, size_t result_size) {
  assert(n < result_size);
  int i = 0;
  for (; i < n; i++) {
    result[i] = ch;
  }

  result[i] = '\0';
}

int
get_digit_count(int n) {
  int count = 0;
  if (n == 0)
    return 1;

  for (; n != 0;) {
    n /= 10;
    count++;
  }

  return count;
}

bool
startswith(const char *str, char ch) {
  return str[0] == ch;
}

bool
startswithsubstring(const char *str, const char *substr) {
  int len_sub = strlen(substr);
  int len = strlen(str);
  if (len_sub > len)
    return false;

  for (int i = 0; i < len_sub; i++) {
    if (str[i] != substr[i]) {
      return false;
    }
  }
  return true;
}
