//
// Created by devinrcohen on 2/12/26.
//

#pragma once

#include "ui.h"
#include "game.h"

typedef struct {
    char name[64];
    int popularity;
    int fullness;
} Player;

typedef enum {
    ST_TITLE,
    ST_ASK_NAME,
    ST_BEDROOM,
    ST_GAME_OVER,
    ST_QUIT
} GameState;

typedef enum {
    GO_NONE,
    GO_MISSED_BUS
} GameOverReason;

typedef struct {
    Player player;
    Settings settings;
    GameOverReason go_reason;
    bool player_valid;
} Game;

/* lifecycle helpers */
void game_reset_to_new_run(Game*);

/* scene functions */
GameState scene_title(Game*);
GameState scene_ask_name(Game*);
GameState scene_bedroom(Game*);
GameState scene_game_over(Game*);
