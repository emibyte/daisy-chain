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
