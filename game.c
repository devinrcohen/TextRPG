//
// Created by devinrcohen on 2/12/26.
//

#include "game.h"

#include "menu.h"
#include <string.h>

#include "ui.h"

#define PLAYER (g->player)
#define WINDOWS (g->ui.windows)
#define PANELS (g->ui.panels)
/* lifecycle helpers */

static bool yes_no_menu(WINDOW*, int, int);
static void print_status(WINDOW*, Game*);
static void clear_inventory(Player*);

void game_reset_to_new_run(Game *g) {
    strcpy(PLAYER.name, "");
    PLAYER.health = 3;
    PLAYER.maxHealth = 10;
    PLAYER.popularity = 0;
    PLAYER.money = 0;
    for (int i=0; i < NUMBER_OF_CHARACTERS; ++i) {
        strcpy(g->characters[i].name, "");
        g->characters[i].met = false;
    }
    clear_inventory(&PLAYER);
}

/* scene functions */
GameState scene_title(Game* g) {
    WINDOW* main = WINDOWS[INTRO_INDEX];
    CURSOR_OFF;

    BLOCK(main, A_UNDERLINE | RED_ON_WHITE | A_REVERSE | A_BOLD,
        wPrintToCenter_Offsetf(main, -1, "Welcome to [GAME TITLE HERE]");
    )

    update_panels();
    doupdate();
    delay_ms(START_DELAY_MS);

    BLOCK(main, BLUE_ON_WHITE | A_REVERSE,
        wPrintToCenter_Offsetf(main, 1, "Press [Enter] to start");
        wPrintToCenter_Offsetf(main, 2, "Press [q] to exit");
    )

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
                return ST_QUIT_ALPHA;
            default:
                break;
        }
    }
}

GameState scene_panel_test(Game* g) {

}

GameState scene_ask_name(Game* g) {
    WINDOW* dialog = WINDOWS[DIALOG_INDEX];
    WINDOW* status = WINDOWS[STATUS_INDEX];
    PANEL* intro_pan = PANELS[INTRO_INDEX];
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
    wgetnstr(dialog, PLAYER.name, 20);
    CURSOR_OFF;
)
    noecho();
    wp_refresh(dialog, 0, 0);
    print_status(status, g);
    return ST_BEDROOM;
}

GameState scene_bedroom(Game* g) {
    WINDOW* main = WINDOWS[MAIN_INDEX];
    PANEL* intro_pan = PANELS[INTRO_INDEX];
    PANEL* main_pan = PANELS[MAIN_INDEX];

    bottom_panel(intro_pan);
    top_panel(main_pan);

    wp_refresh(main, 0, 0);

    wattron(main, WHITE_ON_BLACK);
    DialogBlock block1, block2;

    dialogStart(&block1, 1, 1);
    addDialogLine(&block1, 0,   0, (RED_ON_BLACK | A_BOLD),   NEXT,  "Mahm: ");
    addDialogLine(&block1, 16,  1, (WHITE_ON_BLACK),          WAIT, "\"%s, it's time to wake up!!\"", PLAYER.name);
    addDialogLine(&block1, 0,   0, (CYAN_ON_BLACK | A_BOLD),   NEXT,  "%s: ", PLAYER.name);
    addDialogLine(&block1, /*128*/64, 1, (WHITE_ON_BLACK),    NEXT,  "\"unnnhhhh...\"");
    mvwDialogTrickle(main, &block1);

    wgetch(main);

    bool go_to_school = false;
    while (!go_to_school) {
        wp_refresh(main, 0, 0);
        dialogStart(&block2, 1, 1);
        addDialogLine(&block2, 0,   0, (RED_ON_BLACK | A_BOLD), NEXT,  "Mahm: ");
        addDialogLine(&block2, 16,  1, (WHITE_ON_BLACK),         NEXT, "\"So %s... were you planning on going to school today?\"", PLAYER.name);
        mvwDialogTrickle(main, &block2);

        // Place menu under the dialog; adjust coordinates to taste
        go_to_school = yes_no_menu(main, 4, 2);   // y=4, x=2 inside border

        if (go_to_school) {
            break;
        }
        wp_refresh(main, 0, 0);

        DialogBlock block3;
        dialogStart(&block3, 1, 1);
        addDialogLine(&block3, 0,   0, (RED_ON_BLACK | A_BOLD), NEXT,  "Mahm: ");
        addDialogLine(&block3, 16,  1, (WHITE_ON_BLACK),          WAIT, "\"Um, I don't think so...\"");
        mvwDialogTrickle(main, &block3);
    }
    return ST_GET_DRESSED;
}

GameState scene_get_dressed(Game* g) {
    WINDOW* dialog = WINDOWS[DIALOG_INDEX];
    WINDOW* status = WINDOWS[STATUS_INDEX];
    WINDOW* main = WINDOWS[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);

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

        PLAYER.popularity += 3;
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

        PLAYER.health += 7;
        wp_refresh(dialog, 0, 0);
        delay_ms(50);
        print_status(status, g);

        dialogStart(&block4, 4, 1);
        addDialogLine(&block4, 0, 2, (CYAN_ON_BLACK | A_BOLD), WAIT, "Hunger curbed.");
        addDialogLine(&block4, 16, 0, (RED_ON_BLACK | A_BOLD), WAIT, ">You get on the bus with a full stomach and clear head.");
        mvwDialogTrickle(main, &block4);
    }

    return ST_BUS_RIDE;
}

GameState scene_bus_ride(Game* g) {
    WINDOW* dialog = WINDOWS[DIALOG_INDEX];
    WINDOW* status = WINDOWS[STATUS_INDEX];
    WINDOW* main = WINDOWS[MAIN_INDEX];

    wp_refresh(main, 0, 0);
    wp_refresh(dialog, 0, 0);
    wp_refresh(status, 0, 0);

    print_status(status, g);
    DialogBlock block1, block2, block3, block4;

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
    if (yes_no_menu(dialog, 2, 1)) return ST_EMPTY_SEAT;

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

    return ST_QUIT_ALPHA;
}

GameState scene_empty_seat(Game* g) {

    return ST_QUIT_ALPHA;
}

GameState scene_game_over(Game* g) {
    return ST_QUIT_ALPHA;
}

GameState scene_quit_alpha(Game* g) {
    top_panel(g->ui.panels[QUIT_ALPHA_INDEX]);
    update_panels();
    doupdate();
    WINDOW* outro = WINDOWS[QUIT_ALPHA_INDEX];

    CURSOR_OFF;

    BLOCK(outro,  CYAN_ON_BLACK | A_BOLD,
        wPrintToCenter_Offsetf(outro, -2, "Your game ends here, %s.", PLAYER.name);
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
    mvwprintw(win, 1, 1, "%s", PLAYER.name);
)
    mvwprintw(win, 2, 1, "Cash: $%d", PLAYER.money);
    mvwprintw(win, 3, 1, "Fullness: %d/%d", PLAYER.health, PLAYER.maxHealth);
    mvwprintw(win, 4, 1, "Popularity: %d", PLAYER.popularity);
    mvwprintw(win, 6, 1, "Inventory");
    mvwprintw(win, 7, 1, "=========");
    for (int i = 0; i < TOTAL_ITEMS/10; i++) {
        if (strcmp("", PLAYER.inventory[i].name))
            mvwprintw(win, 8+i, 1, "%s:\t%dx", PLAYER.inventory[i].name, PLAYER.inventory[i].quantity);
    }
}

// returns true for Yes, false for No
static bool yes_no_menu(WINDOW* parent, int y, int x) {
    // Menu items must be NULL-terminated array
    const char *options[] = {"Yes", "No", NULL};
    WINDOW* menu_win = derwin(parent, 4, 5, y, x);
    menu* new_menu = initmenu(options);

    curs_set(0);
    //keypad(parent, TRUE);
    OptionSelected picked = poll_menu(menu_win, new_menu);
    update_panels();
    doupdate();
    delete_menu(new_menu);

    if (picked.idx == 0) return TRUE;
    return FALSE;
}

static void clear_inventory(Player *p) {
    for (int i = 0; i < TOTAL_ITEMS; ++i) {
        strcpy(p->inventory[i].name, "");
        p->inventory[i].quantity = 0;
    }
}