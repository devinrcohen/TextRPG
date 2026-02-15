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
