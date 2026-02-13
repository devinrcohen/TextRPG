//
// Created by devinrcohen on 2/12/26.
//

#include "game.h"
/* lifecycle helpers */
void game_reset_to_new_run(Game *g) {

}

/* scene functions */
GameState scene_title(Game* g) {
    clear();
    attron(A_UNDERLINE | COLOR_PAIR(3) | A_BOLD);
    CURSOR_OFF
    wPrintToCenter_Offsetf(stdscr, -1, "Welcome to [GAME TITLE HERE]");
    attroff(A_UNDERLINE);
    refresh(); // must flush before delay
    delay_ms(1500);

    wPrintToCenter_Offsetf(stdscr, 1, "Press [Enter] to start");
    wPrintToCenter_Offsetf(stdscr, 2, "Press [Esc] to exit");
    attroff(A_BOLD);
    refresh();

    /* get key-presses */
    int ch = getch();
    if (ch == KEY_ENTER) {
        refresh();
        return ST_ASK_NAME;
    }
    return ST_ASK_NAME;
}

GameState scene_ask_name(Game* g) {
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(0, 0, "Enter your name: ");

    move(1, 0);
    CURSOR_STRONG
    getstr(g->player.name);
    CURSOR_OFF
    attroff(COLOR_PAIR(1));

    refresh();
    return ST_BEDROOM;
}

GameState scene_bedroom(Game* g) {
    clear();
    attron(COLOR_PAIR(0));
    int next_col = mvw_trickle(g->settings.mainWindow, 0, 0, 16, "Mom: \"%s, it's time to wake up!!\"", g->player.name);

    move(0, next_col);
    CURSOR_NORMAL
    getch();

    next_col = mvw_trickle(g->settings.mainWindow, 1, 0, 16, "%s: \"unnnnhhhhh wazup?\" ", g->player.name);
    int ch = getch();
    if (ch == KEY_ENTER) {
        refresh();
        return ST_ASK_NAME;
    }
    refresh();
    return ST_QUIT;
}

GameState scene_game_over(Game* g) {
    return ST_QUIT;
}