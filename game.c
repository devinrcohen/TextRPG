//
// Created by devinrcohen on 2/12/26.
//

#include "game.h"

#include <string.h>

#include "ui.h"

#define PLAYER (g->player)
/* lifecycle helpers */
void game_reset_to_new_run(Game *g) {
    strcpy(g->player.name, "");
    g->player.health = 3;
    g->player.maxHealth = 10;
    g->player.popularity = 0;
    g->player.money = 0;
}

/* scene functions */
GameState scene_title(Game* g) {
    WINDOW* main = g->ui.windows[INTRO_INDEX];
    wattron(main, A_UNDERLINE | COLOR_PAIR(3) | A_BOLD);
    CURSOR_OFF;

    wPrintToCenter_Offsetf(main, -1, "Welcome to [GAME TITLE HERE]");
    wattroff(main, A_UNDERLINE);
    update_panels();
    doupdate();
    delay_ms(START_DELAY_MS);

    wPrintToCenter_Offsetf(main, 1, "Press [Enter] to start");
    wPrintToCenter_Offsetf(main, 2, "Press [q] to exit");
    wattroff(main, A_BOLD);
    // refresh();
    update_panels();
    doupdate();
    /* get key-presses */
    while (1) {
        int ch = wgetch(main);
        switch (ch) {
            case '\n':
            case '\r':
            case KEY_ENTER:
                update_panels();
                doupdate();
                return ST_ASK_NAME;
            case KEY_ESC_CUSTOM:
                update_panels();
                doupdate();
                return ST_PANEL_TEST;
            case 'q':
                update_panels();
                doupdate();
                return ST_QUIT;
            default:
                break;
        }
    }
}

GameState scene_panel_test(Game* g) {

}

GameState scene_ask_name(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    PANEL* intro_pan = g->ui.panels[INTRO_INDEX];
    bottom_panel(intro_pan);
    update_panels();
    doupdate();

    wattron(dialog, COLOR_PAIR(1));
    mvwprintw(dialog, 1, 1, "Enter your name: ");
    wmove(dialog, 2, 1);

    echo();
    CURSOR_STRONG;
    wgetnstr(dialog, PLAYER.name, 20);
    CURSOR_OFF;
    wattroff(dialog, COLOR_PAIR(1));
    noecho();
    wp_refresh(dialog, 0, 0);

    print_status(status, g);
    return ST_BEDROOM;
}

GameState scene_bedroom(Game* g) {
    WINDOW* main = g->ui.windows[MAIN_INDEX];
    PANEL* intro_pan = g->ui.panels[INTRO_INDEX];
    PANEL* main_pan = g->ui.panels[MAIN_INDEX];

    bottom_panel(intro_pan);
    top_panel(main_pan);

    wp_refresh(main, 0, 0);

    wattron(main, COLOR_PAIR(0));
    DialogBlock block1, block2;

    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 0,   0, (COLOR_PAIR(1) | A_BOLD), NEXT,  "Mahm: ");
    addDialogLine(&block1, 16,  1, (COLOR_PAIR(2)),          WAIT, "\"%s, it's time to wake up!!\"", g->player.name);
    addDialogLine(&block1, 0,   0, (COLOR_PAIR(1) | A_BOLD), NEXT,  "%s: ", g->player.name);
    addDialogLine(&block1, /*128*/64, 1, (COLOR_PAIR(2)),          NEXT,  "\"unnnhhhh...\"");
    mvwDialogTrickle(main, &block1);

    wgetch(main);
    wp_refresh(main, 0, 0);

    dialogStart(&block2, 1, 1);
    addDialogLine(&block2, 0,   0, (COLOR_PAIR(1) | A_BOLD), NEXT,  "Mahm: ");
    addDialogLine(&block2, 16,  1, (COLOR_PAIR(2)),          NEXT, "\"So... were you planning on going to school today?\"", g->player.name);
    mvwDialogTrickle(main, &block2);

    /* END OF SCENE DIALOG */
    wgetch(main);
    wp_refresh(main, 0, 0);

    return ST_QUIT;
}

GameState scene_game_over(Game* g) {
    return ST_QUIT;
}

void print_status(WINDOW* win, Game* g) {
    wp_refresh(win, 0, 0);
    mvwprintw(win, 1, 1, "%s", PLAYER.name);
    mvwprintw(win, 2, 1, "Cash: $%d", PLAYER.money);
    mvwprintw(win, 3, 1, "Fullness: %d/%d", PLAYER.health, PLAYER.maxHealth);
    mvwprintw(win, 4, 1, "Popularity: %d", PLAYER.popularity);
}