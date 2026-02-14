//
// Created by devinrcohen on 2/11/26.
//

#pragma once

#define START_DELAY_MS 500

/** DIALOG_START
 * Initiates dialog block pointer block_name to length 0
 * @param block_name
 */
#define DIALOG_START(block_name) do {\
(block_name).lastLine = 0;\
} while (0)

/**
 * Variadic macro for creating a line of dialog. Automatically tracks the number of lines,
 * so no end macro is needed. Lines must be added in chronological order
 *
 * @param block_name dialog block to modify
 * @param line_num dialog line number
 * @param delay_qty trickle delay (ms)
 * @param stop_val WAIT for user to hit enter, NEXT to move to next line of dialog
 * @param skip_line_qty number of lines to skip. 0 to stay on same line
 * @param fmt formatted string and args
 */
#define DIALOG_LINE(block_name, delay_qty, skip_line_qty, stop_val, fmt, ...) do {\
snprintf((block_name).lines[(block_name).lastLine].str, sizeof((block_name).lines[(block_name).lastLine].str),\
(fmt) __VA_OPT__(,) __VA_ARGS__);\
(block_name).lines[((block_name).lastLine)].delay_ms = (delay_qty);\
(block_name).lines[((block_name).lastLine)].skipLine = (skip_line_qty);\
(block_name).lines[((block_name).lastLine)].stop = (stop_val);\
(block_name).lastLine += 1;\
} while (0)

#define MAX_LINES 20
#define MAX_LENGTH 100

#include <curses.h>
#include "krono.h"

#define CURSOR_OFF curs_set(0);
#define CURSOR_NORMAL curs_set(1);
#define CURSOR_STRONG curs_set(2);

enum stop {
    WAIT=TRUE,
    NEXT=FALSE
};

typedef struct {
    int y; // row
    int x; // column
} point;

typedef struct {
    WINDOW* mainWindow;
    int yMax;
    int xMax;
} Settings;

typedef struct {
    uint64_t delay_ms;
    int skipLine;
    char str[MAX_LENGTH];
    bool stop;
} DialogLine;

typedef struct {
    point pStart;
    DialogLine lines[MAX_LINES];
    int lastLine;
} DialogBlock;

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
void ui_init(Settings*);
void ui_clear();
void ui_shutdown();
