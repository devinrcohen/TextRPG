#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <panel.h>

#include "ui.h"
#include "krono.h"
#include "game.h"

int main(void) {
    Game game;
    game.settings.mainWindow = stdscr;
    game_reset_to_new_run(&game);

    GameState st = ST_TITLE;
    ui_init(&game.settings);

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

    ui_shutdown();

    return 0;
}