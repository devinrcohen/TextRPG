#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <panel.h>

#include "ui.h"
#include "krono.h"
#include "game.h"

int main(void) {
    Game game;
    game_reset_to_new_run(&game);

    GameState st = ST_TITLE;
    ui_init();

    /* game loop */
    while (st != ST_QUIT) {
        switch (st) {
            case ST_TITLE:      st = scene_title(&game);     break;
            case ST_ASK_NAME:   st = scene_ask_name(&game);  break;
            case ST_BEDROOM:    st = scene_bedroom(&game);   break;
            case ST_GAME_OVER:  st = scene_game_over(&game); break;
            default:            st = ST_QUIT;                break;
        }
    }

    /*old stuff, for reference
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    attron(A_UNDERLINE | COLOR_PAIR(3) | A_BOLD);
    wPrintToCenter_Offset(stdscr, -1, "Welcome to [GAME TITLE HERE]");
    attroff(A_UNDERLINE);
    refresh(); // must flush before delay
    delay_ms(1500);

    wPrintToCenter_Offset(stdscr, 1, "Press [Enter] to continue");
    attroff(A_UNDERLINE | A_BOLD);
    refresh();

    attron(COLOR_PAIR(1));
    mvaddstr(yMax-2, 0, "Enter your name: ");

    move(yMax-1, 0);
    char name[20];

    CURSOR_STRONG
    getstr(name);
    CURSOR_OFF

    attron(COLOR_PAIR(4));
    wPrintToCenterY(stdscr, 1, name);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(1)); // Blue-on-black
    wPrintToCenterX(stdscr, 1, name);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2)); // Red-on-black
    wPrintLeft(stdscr, yMax*3/4, name);
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    wPrintRight(stdscr, yMax*1/4, name);
    attroff(COLOR_PAIR(3));
    refresh();*/

    ui_shutdown();

    return 0;
}