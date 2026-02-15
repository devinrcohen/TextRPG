#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <panel.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ui.h"
#include "krono.h"
#include "game.h"


int main(void) {
    Game game;
    game_reset_to_new_run(&game);
    ui_init(&game.ui);
/*
 *________________
 *|st|  main      |
 *|at|            |
 *|us|            |
 *|  |____________|
 *|__|___dialog___|
 */
    int rowsMain = game.ui.settings.yMax;
    int colsMain = game.ui.settings.xMax;

    float intro_height_ratio = 1.0f;
    float intro_width_ratio  = 1.0f;

    float main_height_ratio = 0.75f;
    float main_width_ratio  = 0.75f;

    float status_height_ratio = 1.0f;
    float status_width_ratio  = 0.75f;

    float dialog_height_ratio = 0.25f;
    float dialog_width_ratio  = 0.75f;

    WINDOW* win_intro  = newwin(rowsMain, colsMain, 0, 0);
    WINDOW* win_main   = newwin((3*rowsMain/4), (3*colsMain/4), 0, colsMain/4);
    WINDOW* win_status = newwin(rowsMain, colsMain/4, 0, 0);
    WINDOW* win_dialog = newwin(rowsMain/4+1, (3*colsMain)/4, (3*rowsMain)/4, colsMain/4);


    PANEL* pan_intro  = ui_add_panel(&game.ui, win_intro,  INTRO_INDEX);
    PANEL* pan_main   = ui_add_panel(&game.ui, win_main,   MAIN_INDEX);
    PANEL* pan_status = ui_add_panel(&game.ui, win_status, STATUS_INDEX);
    PANEL* pan_dialog = ui_add_panel(&game.ui, win_dialog, DIALOG_INDEX);


    box(win_main, '*', '/');
    box(win_status, '|', '-');
    box(win_dialog, '\\', '~');

    top_panel(pan_intro);
    update_panels();
    doupdate();

    GameState st = ST_TITLE;

    /* game loop */
    while (st != ST_QUIT) {
        switch (st) {
            case ST_TITLE:      st = scene_title(&game);      break;
            case ST_PANEL_TEST: st = scene_panel_test(&game); break;
            case ST_ASK_NAME:   st = scene_ask_name(&game);   break;
            case ST_BEDROOM:    st = scene_bedroom(&game);    break;
            case ST_GAME_OVER:  st = scene_game_over(&game);  break;
            default:            st = ST_QUIT;                 break;
        }
    }

    ui_shutdown();

    return 0;
}