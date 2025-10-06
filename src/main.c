#include <curses.h>

int main(void) {
  initscr();
  addstr("welcome to daisy chain\n");
  refresh();
  endwin();

  return 0;
}
