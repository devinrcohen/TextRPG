//
// Created by devinrcohen on 2/12/26.
//

#pragma once
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "krono.h"

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
    GameOverReason go_reason,
    bool player_valid
} Game;

/* lifecycle helpers */
static void game_reset_to_new_run(Game *g);

/* scene functions */
static GameState scene_title(Game *g);
static GameState scene_ask_name(Game *g);
static GameState scene_bedroom(Game *g);
static GameState scene_game_over(Game *g);
