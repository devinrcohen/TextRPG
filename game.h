//
// Created by devinrcohen on 2/12/26.
//

#pragma once

#define TOTAL_ITEMS 100
#define ITEM_NAME_LENGTH 20
#define CHARACTER_NAME_LENGTH 20
#define NUMBER_OF_CHARACTERS 20

#include "ui.h"

enum PanelIndices {
    INTRO_INDEX=0,
    MAIN_INDEX,
    STATUS_INDEX,
    DIALOG_INDEX,
    QUIT_ALPHA_INDEX
};

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

enum CharacterTitle {
    BUDDY=0, // use these as indices to get characters
    LADY_FRIEND,
    BULLY_KID,
    BUS_DRIVER,
    SCIENCE_TEACHER,
    MATH_TEACHER,
    ENGLISH_TEACHER,
    FRENCH_TEACHER,
    PRINCIPAL
};

typedef struct {
    char name[CHARACTER_NAME_LENGTH];
    bool met;
} Character;

typedef enum {
    ST_TITLE,
    ST_PANELTEST,
    ST_ASKNAME,
    ST_BEDROOM,
    ST_GETDRESSED,
    ST_BUSRIDE,
        ST_BUSRIDE_START,
        ST_BUSRIDE_EMPTYSEAT,
        ST_BUSRIDE_LITTLEBUDDY,
        ST_BUSRIDE_EXIT,
    ST_GAMEOVER,
    ST_QUIT,
    ST_QUITALPHA
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
    Character characters[NUMBER_OF_CHARACTERS];
} Game;

/* lifecycle helpers */
void game_reset_to_new_run(Game*);

/* scene functions */
GameState scene_title(Game*);
GameState scene_paneltest(Game*);
GameState scene_askname(Game*);
GameState scene_bedroom(Game*);
GameState scene_getdressed(Game*);

/* Bus Ride Scenes */
GameState scene_busride(Game*);
GameState scene_busride_emptyseat(Game*);
GameState scene_busride_littlebuddy(Game*);
/* END Bus Ride Scenes */

GameState scene_gameover(Game*);
GameState scene_quitalpha(Game*);
