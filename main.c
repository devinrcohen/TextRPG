#include <stdio.h>
#include <string.h>
#include <ncurses/curses.h>

int main(void) {
    initscr();
    raw();
    int row, col;
    getmaxyx(stdscr, row, col);
    char* str1 = "Welcome to [GAME TITLE HERE]";
    mvaddstr(row/2, (col-strlen(str1))/2, str1);

    refresh();
    getch();
    endwin();
    return 0;
}