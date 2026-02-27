//
// Created by devinrcohen on 2/12/26.
//


#include <string.h>

#include "game.h"
#include "ui.h"
#include "menu.h"


/* lifecycle helpers */
static bool yes_no_menu(WINDOW*, int, int);
static void print_status(WINDOW*, Game*);
static void clear_inventory(Player*);
static GameState poll_end_scene(WINDOW*, Game*, GameState);

void game_reset_to_new_run(Game *g) {
    strcpy(g->player.name, "");
    g->player.health = 3;
    g->player.maxHealth = 10;
    g->player.fear = 0;
    g->player.popularity = 0;
    g->player.money = 0;
    for (int i=0; i < NUMBER_OF_CHARACTERS; ++i) {
        strcpy(g->characters[i].name, "");
        g->characters[i].met = false;
    }
    clear_inventory(&g->player);
}

/* scene functions */
GameState scene_title(Game* g) {
    WINDOW* main = g->ui.windows[INTRO_INDEX];
    CURSOR_OFF;

    BLOCK(main, A_UNDERLINE | RED_ON_WHITE | A_REVERSE | A_BOLD,
        wPrintToCenter_Offsetf(main, -1, "Welcome to [GAME TITLE HERE]");
    )

    update_panels();
    doupdate();
    delay_ms(START_DELAY_MS);

#ifdef ALLOW_SAVE_GAME
    BLOCK(main, BLUE_ON_WHITE | A_REVERSE,
        wPrintToCenter_Offsetf(main, 1, "Press [Enter] to start");
        wPrintToCenter_Offsetf(main, 2, "Press [L] to load save");
        wPrintToCenter_Offsetf(main, 3, "Press [Q] to exit");
    )
#else
    BLOCK(main, BLUE_ON_WHITE | A_REVERSE,
        wPrintToCenter_Offsetf(main, 1, "Press [Enter] to start");
        wPrintToCenter_Offsetf(main, 2, "Press [Q] to exit");
    )
#endif


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
                return ST_ASKNAME;
            case KEY_ESC_CUSTOM:
                update_panels();
                doupdate();
                return ST_PANELTEST;
            case 'q':
            case 'Q':
                update_panels();
                doupdate();
                return ST_QUITALPHA;
#ifdef ALLOW_SAVE_GAME
            case 'l':
            case 'L':
                load_game(g, "savefile.json");
                bottom_panel(g->ui.panels[INTRO_INDEX]);
                update_panels();
                doupdate();
                return g->state;
#endif
            default:
                break;
        }
    }
}

GameState scene_askname(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    PANEL* intro_pan = g->ui.panels[INTRO_INDEX];
    bottom_panel(intro_pan);
    update_panels();
    doupdate();

BLOCK(dialog, RED_ON_BLACK | A_BOLD,
    mvwprintw(dialog, 1, 1, "Enter your name: ");
    wmove(dialog, 2, 1);
    echo();
    CURSOR_STRONG;
)
BLOCK(dialog, WHITE_ON_BLACK,
    wgetnstr(dialog, g->player.name, 20);
    CURSOR_OFF;
)
    noecho();
    wp_refresh(dialog, 0, 0);
    print_status(status, g);
    return ST_BEDROOM;
}

GameState scene_bedroom(Game* g) {
    WINDOW* main = g->ui.windows[MAIN_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    PANEL* intro_pan = g->ui.panels[INTRO_INDEX];
    PANEL* main_pan = g->ui.panels[MAIN_INDEX];

    bottom_panel(intro_pan);
    top_panel(main_pan);

    wp_refresh(main, 0, 0);
    print_status(status, g);

    wattron(main, WHITE_ON_BLACK);
    DialogBlock block1, block2, block3, block4;

    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 0,   0, (RED_ON_BLACK | A_BOLD),   NEXT,  "Mahm: ");
    addDialogLine(&block1, 16,  1, (WHITE_ON_BLACK),          WAIT, "\"%s, it's time to wake up!!\"", g->player.name);
    addDialogLine(&block1, 0,   0, (CYAN_ON_BLACK | A_BOLD),   NEXT,  "%s: ", g->player.name);
    addDialogLine(&block1, /*128*/64, 1, (WHITE_ON_BLACK),    NEXT,  "\"unnnhhhh...\"");
    mvwDialogTrickle(main, &block1);

    wgetch(main);

    bool go_to_school = false;
    while (!go_to_school) {
        wp_refresh(main, 0, 0);
        dialogStart(&block2, 1, 1);
        addDialogLine(&block2, 0,   0, (RED_ON_BLACK | A_BOLD), NEXT,  "Mahm: ");
        addDialogLine(&block2, 16,  1, (WHITE_ON_BLACK),         NEXT, "\"So %s... were you planning on going to school today?\"", g->player.name);
        mvwDialogTrickle(main, &block2);

        // Place menu under the dialog; adjust coordinates to taste
        go_to_school = yes_no_menu(main, 4, 2);   // y=4, x=2 inside border

        if (go_to_school) {
            break;
        }
        wp_refresh(main, 0, 0);

        dialogStart(&block3, 1, 1);
        addDialogLine(&block3, 0,   0, (RED_ON_BLACK | A_BOLD), NEXT,  "Mahm: ");
        addDialogLine(&block3, 16,  1, (WHITE_ON_BLACK),          NEXT, "\"Um, I don't think so...\"");
        mvwDialogTrickle(main, &block3);
    }

    wp_refresh(main, 0, 0);
    dialogStart(&block4, 1, 1);
    addDialogLine(&block4, 0,   0, (RED_ON_BLACK | A_BOLD), NEXT,  "Mahm: ");
    addDialogLine(&block4, 16,  1, (WHITE_ON_BLACK),          NEXT, "\"Well, get a move on then!\"");
    mvwDialogTrickle(main, &block4);

    return poll_end_scene(main, g, ST_GETDRESSED);
}

GameState scene_getdressed(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    print_status(status, g);

    DialogBlock block1, block2, block3, block4;

    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 16, 1, (RED_ON_BLACK | A_BOLD), WAIT, ">You look at your watch and panic... the bus will arrive in just 10 minutes!!");
    addDialogLine(&block1, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">You shower quickly but before you walk out the door,");
    addDialogLine(&block1, 16, 2, (RED_ON_BLACK | A_BOLD), WAIT, " you realize that your hair is a mess!");
    mvwDialogTrickle(main, &block1);

    dialogStart(&block2, 1, 1);
    addDialogLine(&block2, 0, 0, (CYAN_ON_BLACK | A_BOLD), NEXT, ">> Style your hair?"); // change to NEXT after testing dialog
    mvwDialogTrickle(dialog, &block2);

    bool style_hair = yes_no_menu(dialog, 2, 1);   // y=2, x=1 inside border
    wp_refresh(dialog, 0, 0);
    wp_refresh(main, 0, 0);

    if (style_hair) {
        dialogStart(&block3, 1, 1);
        addDialogLine(&block3, 16, 1, (RED_ON_BLACK | A_BOLD), WAIT, ">You carefully put pomade in your hair. Nice!");
        addDialogLine(&block3, 0, 0, (CYAN_ON_BLACK | A_BOLD), NEXT, "+3 popularity points gained!");
        mvwDialogTrickle(main, &block3);

        g->player.popularity += 3;
        wp_refresh(dialog, 0, 0);
        delay_ms(50);
        print_status(status, g);

        dialogStart(&block4, 4, 1);
        addDialogLine(&block4, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">You didn't have time to eat,");
        addDialogLine(&block4, 16, 0, (RED_ON_BLACK | A_BOLD), WAIT, " but at least you made the bus lookin' dope.");
        mvwDialogTrickle(main, &block4);
    } else {
        dialogStart(&block3, 1, 1);
        addDialogLine(&block3, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">You grab a delicious granola bar");
        addDialogLine(&block3, 16, 0, (RED_ON_BLACK | A_BOLD), WAIT, " and chomp on it as you run out the door.");
        mvwDialogTrickle(main, &block3);

        g->player.health += 7;
        wp_refresh(dialog, 0, 0);
        delay_ms(50);
        print_status(status, g);

        dialogStart(&block4, 4, 1);
        addDialogLine(&block4, 0, 2, (CYAN_ON_BLACK | A_BOLD), WAIT, "Hunger curbed.");
        addDialogLine(&block4, 16, 0, (RED_ON_BLACK | A_BOLD), NEXT, ">You get on the bus with a full stomach and clear head.");
        mvwDialogTrickle(main, &block4);
    }

    return poll_end_scene(main, g, ST_BUSRIDE);
}

GameState scene_busride(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);
    DialogBlock block1, block2, block3;

    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">Luckily, your bus stop is only the second, so you ");
    addDialogLine(&block1, 16, 2, (RED_ON_BLACK | A_BOLD), WAIT, " have a good pick of seats. You walk slowly toward the back.");
    addDialogLine(&block1, 16, 0, (RED_ON_BLACK | A_BOLD), NEXT, ">Hmm, where to sit");
    addDialogLine(&block1, 128, 1, (RED_ON_BLACK | A_BOLD), WAIT, ".....");
    addDialogLine(&block1, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">There's an empty seat on the third row.");
    mvwDialogTrickle(main, &block1);

    dialogStart(&block2, 1, 1);
    addDialogLine(&block2, 0, 0, (CYAN_ON_BLACK | A_BOLD), NEXT, ">> Take the empty seat?"); // change to NEXT after testing dialog
    mvwDialogTrickle(dialog, &block2);
    if (yes_no_menu(dialog, 2, 1)) return ST_BUSRIDE_EMPTYSEAT;

    // keep moving
    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);

    dialogStart(&block3, 1, 1);
    addDialogLine(&block3, 16, 1, (RED_ON_BLACK | A_BOLD), WAIT, ">You keep walking and suddenly see your little buddy snoozing in the back.");
    addDialogLine(&block3, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, " You decide to sit down next to him. What is your little buddy's name, again?");
    mvwDialogTrickle(main, &block3);
    BLOCK(dialog, CYAN_ON_BLACK | A_BOLD,
        mvwprintw(dialog, 1, 1, ">> Buddy's name: ");
        wmove(dialog, 2, 1);
        echo();
        CURSOR_STRONG;
    )
    BLOCK(dialog, WHITE_ON_BLACK,
        wgetnstr(dialog, g->characters[BUDDY].name, 20);
        CURSOR_OFF;
    )
    noecho();
    g->characters[BUDDY].met = true;

    wp_refresh(dialog, 0, 0);
    print_status(status, g);
    return poll_end_scene(main, g, ST_BUSRIDE_LITTLEBUDDY);
}

GameState scene_busride_emptyseat(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];
    //PANEL* dialog_pan = g->ui.panels[DIALOG_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);
    DialogBlock block1;

    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">The bus hums along for about five minutes until you reach the next stop.");
    addDialogLine(&block1, 16, 2, (RED_ON_BLACK | A_BOLD), WAIT, " The door creaks open and on walks a girl with glasses and a ponytail.");
    addDialogLine(&block1, 0, 0, (CYAN_ON_BLACK | A_BOLD), NEXT, "???: ");
    addDialogLine(&block1, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, "\"Excuse me, may I sit here?\" she asks sweetly.");
    mvwDialogTrickle(main, &block1);

    const char *options[] = {"Beat it, dork!", "Sure. What's your name?", NULL};
    menu* new_menu = initmenu(options);
    curs_set(0);
    OptionSelected girl_answer = poll_menu(dialog, new_menu, 1, 1);
    wp_refresh(main, 0, 0);
    delete_menu(new_menu);
    wp_refresh(dialog, 0, 0);

    /* Interpret results here */
    if (girl_answer.idx == 0) { // Beat it, dork!
        g->characters[GLASSES_GIRL] = (Character) { "???", FALSE };
        DialogBlock block2;
        dialogStart(&block2, 1, 1);
        addDialogLine(&block2, 0, 0, (CYAN_ON_BLACK | A_BOLD), NEXT, "???: ");
        addDialogLine(&block2, 16, 1, (RED_ON_BLACK | A_BOLD), WAIT, "*sniff sniff* get bent, prick!");
        addDialogLine(&block2, 16, 1, (RED_ON_BLACK | A_BOLD), WAIT, ">You made a girl cry. What's wrong with you?");
        mvwDialogTrickle(main, &block2);

        delay_ms(50);
        DialogBlock block3;
        dialogStart(&block3, 5, 1);
        addDialogLine(&block3, 0, 1, (CYAN_ON_BLACK | A_BOLD), NEXT, "-3 popularity points");
        addDialogLine(&block3, 0, 1, (CYAN_ON_BLACK | A_BOLD), NEXT, "+3 fear points");
        mvwDialogTrickle(main, &block3);
        g->player.popularity -= 3;
        g->player.fear += 3;

        print_status(status, g);
        update_panels();
        doupdate();

        wgetch(main);
        wp_refresh(main, 0, 0);

        DialogBlock block4;
        dialogStart(&block4, 1, 1);
        addDialogLine(&block4, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">The poor girl moves on and a smelly fat kid takes the seat.");
        addDialogLine(&block4, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, ">He falls asleep and drools all over your shoulder, while");
        addDialogLine(&block4, 16, 1, (RED_ON_BLACK | A_BOLD), NEXT, " your eyes start to water at his stench.");
        mvwDialogTrickle(main, &block4);

        update_panels();
        doupdate();
    } else {

    }

    return poll_end_scene(main, g, ST_QUITALPHA);
}

GameState scene_busride_emptyseat_girlconvo(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);

    /* scene stuff goes here */

    return poll_end_scene(main, g, ST_QUITALPHA);
}

GameState scene_busride_littlebuddy(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);

    /* scene stuff goes here */

    return poll_end_scene(main, g, ST_BUSRIDE_LITTLEBUDDY_BUDDYCONVO);
}

GameState scene_busride_littlebuddy_buddyconvo(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);

    /* scene stuff goes here */

    return poll_end_scene(main, g, ST_QUITALPHA);
}

GameState scene_gameover(Game* g) {
    WINDOW* dialog = g->ui.windows[DIALOG_INDEX];
    WINDOW* status = g->ui.windows[STATUS_INDEX];
    WINDOW* main = g->ui.windows[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);

    /* scene stuff goes here */

    return poll_end_scene(main, g, ST_QUITALPHA);
}

GameState scene_quitalpha(Game* g) {
    top_panel(g->ui.panels[QUIT_ALPHA_INDEX]);
    update_panels();
    doupdate();
    WINDOW* outro = g->ui.windows[QUIT_ALPHA_INDEX];

    CURSOR_OFF;

    BLOCK(outro,  CYAN_ON_BLACK | A_BOLD,
        wPrintToCenter_Offsetf(outro, -2, "Your game ends here, %s.", g->player.name);
        wPrintToCenter_Offsetf(outro, -1, "This game is in active development.");
        wPrintToCenter_Offsetf(outro, 0, "For inquiries, contact me at:");
    )

    BLOCK(outro, CYAN_ON_BLACK | A_UNDERLINE,
        wPrintToCenter_Offsetf(outro, 1, "devin.r.cohen@protonmail.com");
    )

    BLOCK(outro,  CYAN_ON_BLACK | A_BOLD,
        wPrintToCenter_Offsetf(outro, 2, "Thank you for playing!");
        if (g->characters[BUDDY].met)
            wPrintToCenter_Offsetf(outro, 5, "%s yells \"bye bye!\"", g->characters[BUDDY].name);
    )
    wgetch(outro);
    return ST_QUIT;
}

static void print_status(WINDOW* win, Game* g) {
    wp_refresh(win, 0, 0);
BLOCK(win, CYAN_ON_BLACK | A_BOLD,
    mvwprintw(win, 1, 1, "%s", g->player.name);
)
    mvwprintw(win, 2, 1, "Cash: $%d", g->player.money);
    mvwprintw(win, 3, 1, "Fullness: %d/%d", g->player.health, g->player.maxHealth);
    mvwprintw(win, 4, 1, "Popularity: %d", g->player.popularity);
    mvwprintw(win, 5, 1, "Fear: %d", g->player.fear);
    mvwprintw(win, 7, 1, "Inventory");
    mvwprintw(win, 8, 1, "=========");
    for (int i = 0; i < TOTAL_ITEMS/10; i++) {
        if (strcmp("", g->player.inventory[i].name)) {
            mvwprintw(win, 9+i, 1, "%s:\t%dx", g->player.inventory[i].name, g->player.inventory[i].quantity);
        }
    }
}

// returns true for Yes, false for No
static bool yes_no_menu(WINDOW* parent, int y, int x) {
    // Menu items must be NULL-terminated array
    const char *options[] = {"Yes", "No", NULL};
    WINDOW* menu_win = derwin(parent, 4, 5, y, x);
    menu* new_menu = initmenu(options);

    curs_set(0);
    OptionSelected picked = poll_menu(menu_win, new_menu, 1, 1);
    update_panels();
    doupdate();
    delete_menu(new_menu);
    wp_delete(menu_win);

    if (picked.idx == 0) return TRUE;
    return FALSE;
}

static void clear_inventory(Player *p) {
    for (int i = 0; i < TOTAL_ITEMS; ++i) {
        strcpy(p->inventory[i].name, "");
        p->inventory[i].quantity = 0;
    }
}

static GameState poll_end_scene(WINDOW* win, Game* g, GameState next_state) {
    int maxRows = getmaxy(win);
    wattron(win, A_BOLD);
    mvwprintw(win, maxRows-2, 1, "[Enter] to continue...");
    wattroff(win, A_BOLD);
    while (1) {
        int ch = wgetch(win);
        switch (ch) {
            case '\n':
            case '\r':
            case KEY_ENTER:
                /*update_panels();
                doupdate();*/
                return next_state;
            case KEY_ESC_CUSTOM:
            case 'q':
            case 'Q':
                /*update_panels();
                doupdate();*/
                return ST_QUITALPHA;
#ifdef ALLOW_SAVE_GAME
            case 'l':
            case 'L':
                load_game(g, "savefile.json");
                bottom_panel(g->ui.panels[INTRO_INDEX]);
                update_panels();
                doupdate();
                return g->state;
            case 's':
            case 'S':
            case KEY_F(1):
                g->state = next_state;
                save_game(g, "savefile.json");
#endif
            default:
                break;
        }
    }
}

#ifdef ALLOW_SAVE_GAME
void save_game(Game* g, const char* filename) {
    FILE* savefile = NULL;
    cJSON *save = NULL;
    cJSON *character_array = NULL;
    char *out_string = NULL;

    GameState state = g->state;
    Player player = g->player;
    Character *characters = g->characters;
    bool player_valid = g->player_valid;

    save = cJSON_CreateObject();
    cJSON_AddItemToObject(save, "name", cJSON_CreateString(player.name));
    cJSON_AddNumberToObject(save, "popularity", player.popularity);
    cJSON_AddNumberToObject(save, "fear", player.fear);
    cJSON_AddNumberToObject(save, "health", player.health);
    cJSON_AddNumberToObject(save, "maxHealth", player.maxHealth);
    cJSON_AddNumberToObject(save, "money", player.money);
    cJSON_AddNumberToObject(save, "state", state);
    cJSON_AddBoolToObject(save, "valid", player_valid);

    character_array = cJSON_AddArrayToObject(save, "characters");
    for (int i = 0; i < NUMBER_OF_CHARACTERS; ++i) {
        cJSON_AddItemToArray(character_array, cJSON_CreateString(characters[i].name));
    }

    if ((savefile = fopen(filename, "w")) != NULL && (out_string = cJSON_Print(save)) != NULL) {
        fprintf(savefile, "%s", out_string);
        fclose(savefile);
    } else {
        fprintf(stderr, "Failed to save.");
    }
}

void load_game(Game* g, const char* filename) {
    FILE* jsonfile = NULL; /*fopen(filename, "r");*/
    const cJSON *name = NULL;
    const cJSON *popularity = NULL;
    const cJSON *fear = NULL;
    const cJSON *health = NULL;
    const cJSON *maxHealth = NULL;
    const cJSON *money = NULL;
    const cJSON *characters = NULL;
    const cJSON *state = NULL;
    const cJSON *valid = NULL;

    if ((jsonfile = fopen(filename, "r")) != NULL) {
        char in_string[MAX_SAVE_FILE_LENGTH];
        //fgets(in_string, MAX_SAVE_FILE_LENGTH, jsonfile);
        fread(in_string, sizeof(char), MAX_SAVE_FILE_LENGTH, jsonfile);
        cJSON *savefile_json = cJSON_Parse(in_string);
        if (savefile_json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                // wPrintToCenterf(g->ui.windows[INTRO_INDEX], "Error before: %s\n", error_ptr);
                fprintf(stderr, "Error before: %s\n", error_ptr);
            }
        }
        name = cJSON_GetObjectItem(savefile_json, "name");
        popularity = cJSON_GetObjectItemCaseSensitive(savefile_json, "popularity");
        fear = cJSON_GetObjectItemCaseSensitive(savefile_json, "fear");
        health = cJSON_GetObjectItemCaseSensitive(savefile_json, "health");
        maxHealth = cJSON_GetObjectItemCaseSensitive(savefile_json, "maxHealth");
        money = cJSON_GetObjectItemCaseSensitive(savefile_json, "money");
        state = cJSON_GetObjectItemCaseSensitive(savefile_json, "state");
        valid = cJSON_GetObjectItemCaseSensitive(savefile_json, "valid");
        /* get characters array */
        strcpy(g->player.name, name->valuestring);
        g->player.popularity = popularity->valueint;
        g->player.fear = fear->valueint;
        g->player.health = health->valueint;
        g->player.maxHealth = maxHealth->valueint;
        g->player.money = money->valueint;
        g->player_valid = (bool)valid->valueint;
        g->state = state->valueint;

        fclose(jsonfile);
    } else {
        g->state = ST_ASKNAME;
    }
}
#endif