//
// Created by devinrcohen on 2/11/26.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

/**
 * Initiates dialog block pointer block_name to length 0
 *
 * @param block dialog block pointer
 * @param row starting row
 * @param col starting column
 */
void dialogStart(DialogBlock* block, int row, int col) {
    block->lastLine = 0;
    block->pStart.y = row;
    block->pStart.x = col;
}

/**
 *
 * @param block dialog block pointer
 * @param delay_ms delay between each character, in milliseconds
 * @param skipLine number of lines to skip. 0 to stay on same line
 * @param msk mask for text formatting
 * @param stop WAIT for user to hit enter, NEXT to move to next line of dialog
 * @param fmt formatted string
 * @param ... format args
 */
void addDialogLine (DialogBlock* block, uint64_t delay_ms, int skipLine, MASK msk, bool stop, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(block->lines[block->lastLine].str, sizeof(block->lines[block->lastLine].str), fmt, ap);
    va_end(ap);
    block->lines[block->lastLine].delay_ms = delay_ms;
    block->lines[block->lastLine].skipLine = skipLine;
    block->lines[block->lastLine].stop = stop;
    block->lines[block->lastLine].mask = msk;
    block->lastLine += 1;
}

point mvwTrickle(WINDOW* win, int row, int col, uint64_t trickle_ms, const char* fmt, ...) {
    va_list ap;
    char buf[MAX_LENGTH];
    int maxlen = sizeof(buf);
    va_start(ap, fmt);
    vsnprintf(buf, maxlen, fmt, ap);
    va_end(ap);
    int len = strlen(buf);
    int last_col = col;
    for (int i = 0; i < len; ++i, ++last_col) {
        mvwaddch(win, row, col+i, buf[i]);
        delay_ms(trickle_ms);
        update_panels();
        doupdate();
    }
    point last_point = {row, last_col};
    return last_point;
}

point mvwCenterOffsetTrickle(WINDOW* win, int offset, uint64_t trickle_ms, const char* fmt, ...) {
    va_list ap;
    int yMax, xMax;
    getmaxyx(win, yMax, xMax);
    char buf[512];
    int maxlen = sizeof(buf);
    va_start(ap, fmt);
    vsnprintf(buf, maxlen, fmt, ap);
    int len = strlen(buf);
    point last_point = mvwTrickle(win, yMax/2+offset, (xMax-len)/2, trickle_ms, "%s", buf);
    va_end(ap);
    return last_point;
}

void mvwDialogTrickle(WINDOW* win, DialogBlock* block) {
    point currentStartPoint = block->pStart;
    for (int i=0; i < block->lastLine; ++i) {
        wattron(win, block->lines[i].mask);
        currentStartPoint = mvwTrickle(win, currentStartPoint.y, currentStartPoint.x, block->lines[i].delay_ms, "%s", block->lines[i].str);
        wattroff(win, block->lines[i].mask);
        if (block->lines[i].skipLine > 0) {
            currentStartPoint.y += block->lines[i].skipLine;
            currentStartPoint.x = block->pStart.x;
        }
        if (block->lines[i].stop) {
            getch();
        }
    }
}

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

static void vwPrintToCenterY(WINDOW* win, int row, const char* fmt, va_list ap) {
    int xMax = getmaxx(win);
    char buf[512];
    vsnprintf(buf, sizeof(buf), fmt, ap);

    int len = (int)strlen(buf);
    int col = (xMax - len)/2;
    mvwprintw(win, row, col, "%s", buf);
}

/**
 *
 * @param win pointer to window on which to place string
 * @param row row at which to place string on y-axis
 * @param fmt string to place
 * @param ...
 */
void wPrintToCenterYf(WINDOW* win, int row, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwPrintToCenterY(win, row, fmt, ap);
    va_end(ap);
}

void wPrintToCenterYf_trickle(WINDOW* win, int row, uint64_t trickle_ms, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    //vwPrintToCenterY(win, row, fmt, ap);
    va_end(ap);
}

/**
 * Prints a center-aligned string vertically offset from middle of screen
 *
 * @param win pointer to window on which to place string
 * @param offset vertical offset from center (negative to move up, positive to move down)
 * @param fmt string to place
 * @param ...
 */
void wPrintToCenter_Offsetf(WINDOW* win, int offset, const char* fmt, ...) {
    int yMax = getmaxy(win);

    va_list ap;
    va_start(ap, fmt);
    vwPrintToCenterY(win, yMax/2 + offset, fmt, ap);
    va_end(ap);
}

/**
 *
 * @param win pointer to window on which to place string
 * @param fmt
 * @param ...
 */
void wPrintToCenterf(WINDOW* win, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwPrintToCenterY(win, getmaxy(win)/2, fmt, ap);
    va_end(ap);
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

void ui_init(UI* ui) {
    initscr();
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    ui->settings.yMax = yMax;
    ui->settings.xMax = xMax;

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
        init_pair(6, COLOR_RED, COLOR_WHITE);
        init_pair(7, COLOR_BLUE, COLOR_WHITE);
    }
    cbreak();
    noecho();
}

PANEL* ui_add_panel(UI* ui, WINDOW* win, int index) {
    ui->windows[index] = win;
    ui->panels[index] = new_panel(win);
    return ui->panels[index];
}

void ui_clear() {

}

void ui_shutdown() {
    endwin();
}

TextEdit* add_textField(WINDOW* win, point location, int width, int capacity) {
    TextEdit te = {calloc(capacity+1, sizeof(char)), capacity, 0, width, win, location};
    return &te;
}

void edit_textedit(TextEdit* te) {
    WINDOW* win = te->win;
    const int cap = te->capacity;
    const int len = strlen(te->text);
    const char ch = wgetch(win);
    while (1) {
        if (len == cap) {
            break;
        }
        switch (ch) {
            case '\n':
            case '\r':
            case KEY_ENTER:
                break;
            case KEY_BACKSPACE:
                te->text[len] = '\0';
                break;

            default:
                te->text[len] = ch;
                break;
        }
    }
}

void delete_textedit(TextEdit* te) {
    free(te->text);
}

void wp_refresh(WINDOW* win, char v, char h) {
    wclear(win);
    box(win, v, h);
    update_panels();
    doupdate();
}

void wp_delete(WINDOW* win) {
    wclear(win);
    delwin(win);
    update_panels();
    doupdate();
}

SelectBox new_selectbox(WINDOW* win, UI* ui, point location, const char *option1, const char *option2) {
    SelectBox box;
    strcpy(box.options[0], option1);
    strcpy(box.options[1], option2);
    box.location = location;
    box.win = win;
    box.selected = 0;
    box.visible = false;
    return box;
}

void render_selectbox(SelectBox* box) {

}