#include "munit/munit.h"
#include <stdio.h>

int main(void) {
  printf("this is the test file\n.");
  munit_assert_int(1, ==, 1);
  return 0;
}
