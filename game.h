//
// Created by devinrcohen on 2/12/26.
//

#pragma once

#define INTRO_INDEX   0
#define MAIN_INDEX    1
#define STATUS_INDEX  2
#define DIALOG_INDEX  3

#define TOTAL_ITEMS 100
#define ITEM_NAME_LENGTH 20
#define CHARACTER_NAME_LENGTH 20

#include "ui.h"

typedef struct {
    char name[ITEM_NAME_LENGTH];
    int quantity;
} Item;

typedef struct {
    char name[CHARACTER_NAME_LENGTH];
    int popularity;
    int health;
    int maxHealth;
    int money;
    Item inventory[TOTAL_ITEMS];
} Player;



typedef enum {
    ST_TITLE,
    ST_PANEL_TEST,
    ST_ASK_NAME,
    ST_BEDROOM,
    ST_GET_DRESSED,
    ST_GAME_OVER,
    ST_QUIT
} GameState;

typedef enum {
    GO_NONE,
    GO_MISSED_BUS
} GameOverReason;

typedef struct {
    UI ui;
    Player player;
    //Settings settings;
    GameOverReason go_reason;
    bool player_valid;
} Game;

/* lifecycle helpers */
void game_reset_to_new_run(Game*);

/* scene functions */
GameState scene_title(Game*);
GameState scene_panel_test(Game*);
GameState scene_ask_name(Game*);
GameState scene_bedroom(Game*);
GameState scene_game_over(Game*);
