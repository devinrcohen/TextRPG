//
// Created by devinrcohen on 2/11/26.
//

#pragma once
#include <curses.h>
#include <string.h>
#include <stdarg.h>
#include "krono.h"

#define CURSOR_OFF curs_set(0);
#define CURSOR_NORMAL curs_set(1);
#define CURSOR_STRONG curs_set(2);

typedef struct {
    int y; // row
    int x; // column
} point;

typedef struct {
    WINDOW* mainWindow;
    int yMax;
    int xMax;
} Settings;

int mvw_trickle(WINDOW*, int, int, uint64_t, const char*, ...);
void mvaddstr_pt(const point, const char*);
void mvaddstr_center(const point, const char*);
point getMidStart(const point, const char*);
void wPrintToCenterX(WINDOW*, int, const char*);
void wPrintToCenterYf(WINDOW*, int, const char*, ...);
void wPrintToCenter_Offsetf(WINDOW*, int, const char*, ...);
void wPrintToCenterf(WINDOW*, const char*, ...);
void wPrintLeft(WINDOW*, int, const char*);
void wPrintRight(WINDOW*, int, const char*);
void delay_ms(uint64_t);
void ui_init(Settings*);
void ui_clear();
void ui_shutdown();
