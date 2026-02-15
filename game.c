//
// Created by devinrcohen on 2/12/26.
//

#include "game.h"
#include "ui.h"
/* lifecycle helpers */
void game_reset_to_new_run(Game *g) {

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
    PANEL* intro_pan = g->ui.panels[INTRO_INDEX];
    bottom_panel(intro_pan);
    update_panels();
    doupdate();

    wclear(dialog);
    wattron(dialog, COLOR_PAIR(1));
    mvwprintw(dialog, 0, 0, "Enter your name: ");
    wmove(dialog, 1, 0);

    echo();
    CURSOR_STRONG;
    wgetnstr(dialog, g->player.name, 20);
    CURSOR_OFF;
    wattroff(dialog, COLOR_PAIR(1));
    noecho();

    update_panels();
    doupdate();
    return ST_BEDROOM;
}

GameState scene_bedroom(Game* g) {
    WINDOW* main = g->ui.windows[MAIN_INDEX];
    PANEL* intro_pan = g->ui.panels[INTRO_INDEX];
    PANEL* main_pan = g->ui.panels[MAIN_INDEX];
    bottom_panel(intro_pan);
    top_panel(main_pan);
    update_panels();
    doupdate();

    wclear(main);
    wattron(main, COLOR_PAIR(0));
    DialogBlock block1;
    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 0,   0, (COLOR_PAIR(1) | A_BOLD), NEXT,  "Mahm: ");
    addDialogLine(&block1, 16,  1, (COLOR_PAIR(2)),          WAIT, "\"%s, it's time to wake up!!\"", g->player.name);
    addDialogLine(&block1, 0,   0, (COLOR_PAIR(1) | A_BOLD), NEXT,  "%s: ", g->player.name);
    addDialogLine(&block1, 128, 1, (COLOR_PAIR(2)),          NEXT,  "\"unnnhhhh...\"");

    mvwDialogTrickle(main, &block1);
    update_panels();
    doupdate();
    wgetch(main);
    wclear(main);

    DialogBlock block2;
    dialogStart(&block2, 1, 1);

    addDialogLine(&block2, 0,   0, (COLOR_PAIR(1) | A_BOLD), NEXT,  "Mahm: ");
    addDialogLine(&block2, 16,  1, (COLOR_PAIR(2)),          NEXT, "\"So... were you planning on going to school today?\"", g->player.name);

    mvwDialogTrickle(main, &block2);
    update_panels();
    doupdate();
    wgetch(main);
    wclear(main);

    return ST_QUIT;
}

GameState scene_game_over(Game* g) {
    return ST_QUIT;
}