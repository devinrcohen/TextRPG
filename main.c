#include <curses.h>
#include "ui.h"
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
    int rowsMax = game.ui.settings.yMax;
    int colsMax = game.ui.settings.xMax;

    float main_height_ratio = 0.75f;
    float main_width_ratio  = 0.75f;

    int rowsMain = main_height_ratio * rowsMax;
    int colsMain = main_width_ratio * colsMax;
    int y0main = 0;
    int x0main = colsMax - colsMain;

    float status_height_ratio = 1.0f;
    float status_width_ratio  = 0.25f;

    int rowsStatus = status_height_ratio * rowsMax;
    int colsStatus = status_width_ratio * colsMax;
    int y0status = 0;
    int x0status = 0;

    float dialog_height_ratio = 0.25f;
    float dialog_width_ratio  = 0.75f;

    int rowsDialog = dialog_height_ratio * rowsMax;
    int colsDialog = dialog_width_ratio * colsMax;
    int y0dialog = rowsMax - rowsDialog;
    int x0dialog = colsMax - colsDialog;

    WINDOW* win_intro  = newwin(rowsMax, colsMax, 0, 0);
    WINDOW* win_main   = newwin(rowsMain, colsMain, y0main, x0main);
    WINDOW* win_status = newwin(rowsStatus, colsStatus, y0status, x0status);
    WINDOW* win_dialog = newwin(rowsDialog+1, colsDialog, y0dialog-1, x0dialog);
    WINDOW* win_quit_alpha = newwin(rowsMax, colsMax, 0, 0);


    PANEL* pan_intro  = ui_add_panel(&game.ui, win_intro,  INTRO_INDEX);
    PANEL* pan_main   = ui_add_panel(&game.ui, win_main,   MAIN_INDEX);
    PANEL* pan_status = ui_add_panel(&game.ui, win_status, STATUS_INDEX);
    PANEL* pan_dialog = ui_add_panel(&game.ui, win_dialog, DIALOG_INDEX);
    PANEL* pan_quit_alpha = ui_add_panel(&game.ui, win_quit_alpha, QUIT_ALPHA_INDEX);

    box(win_main, 0, 0);
    box(win_status, 0, 0);
    box(win_dialog, 0, 0);

    top_panel(pan_intro);
    bottom_panel(pan_quit_alpha);
    update_panels();
    doupdate();

    GameState st = ST_TITLE;
    game_reset_to_new_run(&game);
    /* game loop */
    while (st != ST_QUIT) {
        switch (st) {
            case ST_TITLE:       st = scene_title(&game);        break; // --> ST_ASKNAME
            case ST_PANELTEST:   st = scene_paneltest(&game);    break;
            case ST_ASKNAME:     st = scene_askname(&game);      break; // --> ST_BEDROOM
            case ST_BEDROOM:     st = scene_bedroom(&game);      break; // --> ST_GETDRESSED
            case ST_GETDRESSED:  st = scene_getdressed(&game);   break; // --> ST_BUSRIDE
            case ST_BUSRIDE: { // -------------------------> _EMPTYSEAT ---------> ST_BUSRIDE_EXIT
                st = scene_busride(&game); //      `----> _LITTLEBUDDY ----'
                while (st != ST_BUSRIDE_EXIT) {
                    switch (st) {
                        case ST_BUSRIDE_EMPTYSEAT:      st = scene_busride_emptyseat(&game); break;   // ____
                        case ST_BUSRIDE_LITTLEBUDDY:    st = scene_busride_littlebuddy(&game); break; // ____\--> ST_BUSRIDE_EXIT
                        default: st = ST_BUSRIDE_EXIT;
                    }
                }
                st = ST_GAMEOVER; // will change to actual next scene
                break;
            }
            case ST_BUSRIDE_EMPTYSEAT:  st = scene_busride_emptyseat(&game);   break;
            case ST_GAMEOVER:   st = scene_gameover(&game);    break;
            case ST_QUITALPHA:  st = scene_quitalpha(&game);   break;
            default:             st = ST_QUIT;                   break;
        }
    }

    ui_shutdown();

    return 0;
}