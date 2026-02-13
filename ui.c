//
// Created by devinrcohen on 2/11/26.
//

#include "ui.h"

void mvaddstr_pt(const point yx, const char* str) {
    mvaddstr(yx.y, yx.x, str);
}

void mvaddstr_center(const point yx, const char* str) {
    point midpoint = getMidStart(yx, str);
    mvaddstr_pt(midpoint, str);
}

point getMidStart(const point midpt, const char* str) {
    int y = midpt.y;
    int xMid = midpt.x;
    int xStart = xMid - (int)strlen(str)/2;
    point new = {y, xStart};
    return new;
}

/**
 * Prints a left-aligned string half-way between top and bottom of terminal window
 *
 * @param win pointer to window on which to place string
 * @param col column number
 * @param str string to place
 */
void wPrintToCenterX(WINDOW* win, int col, const char* str) {
    int yMax = getmaxy(win);
    mvwaddstr(win, yMax/2, col, str); // not centered
}

/**
 * Prints a center-aligned string at on window at row
 *
 * @param win pointer to window on which to place string
 * @param row row number
 * @param str string to place
 *
 */

void wPrintToCenterY(WINDOW* win, int row, const char* str) {
    int xMax = getmaxx(win);
    int len = (int)strlen(str);
    mvwaddstr(win, row, (xMax-len)/2, str);
}

/**
 * Prints a center-aligned string vertically offset from middle of screen
 *
 * @param win pointer to window on which to place string
 * @param offset vertical offset from center (negative to move up, positive to move down)
 * @param str string to place
 */
void wPrintToCenter_Offset(WINDOW* win, int offset, const char* str) {
    int yMax = getmaxy(win);
    wPrintToCenterY(win, yMax/2 + offset, str);
}

void wPrintToCenter(WINDOW* win, const char* str) {
    wPrintToCenter_Offset(win, 0, str);
}

/**
 * Prints a left-aligned string on window at specified row
 *
 * @param win pointer to window on which to place string
 * @param row row number
 * @param str string to place
 *
 */
void wPrintLeft(WINDOW* win, int row, const char* str) {
    mvwaddstr(win, row, 0, str);
}

/**
 * Prints a right-aligned string on window at specified row
 *
 * @param win pointer to window on which to place string
 * @param row row number
 * @param str string to place
 *
 */
void wPrintRight(WINDOW* win, int row, const char* str) {
    int xMax = getmaxx(win);
    int len = strlen(str);
    mvwaddstr(win, row, xMax-len, str); // justify right
}

void delay_ms(uint64_t ms) {
    krono_sleep_ns(1000000*ms);
}

void ui_init() {
    initscr();
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_RED);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
    }
    cbreak();
    keypad(stdscr, TRUE);
}

void ui_clear() {

}

void ui_shutdown() {
    getch();
    endwin();
}