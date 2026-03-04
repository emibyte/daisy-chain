#include <ncurses.h>
#include <string.h>

// NOTE: this doesnt work, just a barebones structure for an ncurses version of a tui i might build
int
main(void) {
  int row, col;
  char str[80];
  // NOTE: initialize a stdscr
  initscr();
  cbreak();
  noecho();

  // NOTE: set up the screen
  getmaxyx(stdscr, row, col);
  move(row / 2, col / 2);
  getstr(str);
  mvprintw(row / 2, (col - strlen(str)) / 2, "%s", str);

  // NOTE: display the screen that was set up
  refresh();
  getch();

  endwin();

  return 0;
}
