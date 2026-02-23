//
// Created by devinrcohen on 2/11/26.
//

#pragma once

#define START_DELAY_MS  500
#define MAX_LINES       20
#define MAX_LENGTH      100
#define MAX_NUM_WINDOWS 5

#define CURSOR_OFF      (curs_set(0))
#define CURSOR_NORMAL   (curs_set(1))
#define CURSOR_STRONG   (curs_set(2))

/* more keys */
#define KEY_ESC_CUSTOM 27

#define RED_ON_BLACK    (COLOR_PAIR(1))
#define WHITE_ON_BLACK  (COLOR_PAIR(2))
#define BLUE_ON_BLACK   (COLOR_PAIR(3))
#define YELLOW_ON_BLACK (COLOR_PAIR(4))
#define CYAN_ON_BLACK   (COLOR_PAIR(5))
#define RED_ON_WHITE    (COLOR_PAIR(6))
#define BLUE_ON_WHITE   (COLOR_PAIR(7))

#define BLOCK(window, format, code) \
    wattron(window, format); \
    code  \
    wattroff(window, format);

#include <stdarg.h>
#include <curses.h>
#include <panel.h>
#include "krono.h"

typedef uint64_t MASK;

enum stop {
    WAIT = TRUE,
    NEXT = FALSE
};

typedef struct {
    int y; // row
    int x; // column
} point;

typedef struct {
    /*WINDOW* window;*/
    int yMax;
    int xMax;
} Settings;

typedef struct {
    uint64_t delay_ms;
    int skipLine;
    char str[MAX_LENGTH];
    bool stop;
    MASK mask;
} DialogLine;

typedef struct {
    point pStart;
    DialogLine lines[MAX_LINES];
    int lastLine;
} DialogBlock;

/**
 * Book-keeping for windows and their associated panels
 * The number of windows/panels does NOT include stdscr
 */
typedef struct {
    WINDOW*   windows[MAX_NUM_WINDOWS];
    PANEL*    panels [MAX_NUM_WINDOWS];
    int       num_panels;
    Settings  settings;
    //int active;   // optional: which panel has focus
} UI;

enum EditStatus {
    EDIT_BREAK = 0,
    EDIT_CONTINUE,
    EDIT_FULL
};

typedef struct {
    char *text;
    int capacity;
    int lenchars;
    int width;
    WINDOW* win;
    point location;
} TextEdit;

typedef struct {
    char options[2][30];
    int selected;
    WINDOW* win;
    point location;
    bool visible;
} SelectBox;

SelectBox new_selectbox(WINDOW*, UI*, point, const char*, const char*);
void wp_refresh(WINDOW*, char, char);
void wp_delete(WINDOW*);
TextEdit* add_textfield(WINDOW*, point, int, int);
void delete_textfield(TextEdit*);
void edit_textedit(TextEdit*);
int addch_to_textfield(TextEdit*, char);
void dialogStart(DialogBlock*, int, int);
void addDialogLine (DialogBlock*, uint64_t, int, MASK, bool, const char*, ...);
void mvwDialogTrickle(WINDOW*, DialogBlock*);
point mvwTrickle(WINDOW*, int, int, uint64_t, const char*, ...);
point mvwCenterOffsetTrickle(WINDOW*, int, uint64_t, const char*, ...);
void mvaddstr_pt(const point, const char*);
void mvaddstr_center(const point, const char*);
point getMidStart(const point, const char*);
void wPrintToCenterX(WINDOW*, int, const char*);
void wPrintToCenterYf(WINDOW*, int, const char*, ...);
void wPrintToCenterYf_trickle(WINDOW*, int, uint64_t, const char*, ...);
void wPrintToCenter_Offsetf(WINDOW*, int, const char*, ...);
void wPrintToCenterf(WINDOW*, const char*, ...);
void wPrintLeft(WINDOW*, int, const char*);
void wPrintRight(WINDOW*, int, const char*);
void delay_ms(uint64_t);
void ui_init(UI*);
PANEL* ui_add_panel(UI*, WINDOW*, int);
void ui_clear();
void ui_shutdown();
