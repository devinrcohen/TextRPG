//
// Created by devinrcohen on 2/23/26.
//



#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "ui.h"

static char* xstrdup(const char *s) {
    size_t n = strlen(s);
    char *p = (char*)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}

menu* initmenu(const char* const options[]) {
    int n = 0;
    while (options[n] != NULL) {
        ++n;
    }
    menu* m = (menu*)calloc(n,sizeof(*m));
    if (!m) return NULL;

    m->options = (char**)calloc((size_t)n + 1, sizeof(char*));
    if (!m->options) {
        free(m);
        return NULL;
    }

    for (int i = 0; i < n; ++i) {
        m->options[i] = xstrdup(options[i]);
        if (!m->options[i]) {
            //clean up partial allocation
            for (int j=0; j < i; ++j) free (m->options[j]);
            free(m->options);
            free(m);
            return NULL;
        }
    }
    m->options[n] = NULL; // store sentinel
    m->num_options = n;
    m->selected = 0;
    m->in_focus = false;
    return m;
}

void mvwprintnc(WINDOW* win, char c, int y, int x, int maxLength) {
    for (int i = 0; i < maxLength; ++i) {
        mvwaddch(win, y, x+i, c);
    }
}

void mvwprint_menu(const menu* men, WINDOW* win, int y, int x) {
    int maxLength = 0;
    for (int i = 0; i < men->num_options; ++i) {
        int len = (int)strlen(men->options[i]);
        maxLength = len > maxLength ? len : maxLength;
    }
    for (int i = 0; i < men->num_options; ++i) {
        int len = (int)strlen(men->options[i]);
        if ((men->selected == i) && men->in_focus) {
            uint64_t fmt_mask = /*COLOR_PAIR(1) | */A_REVERSE;
            wattron(win, fmt_mask);
            mvwprintw(win, y+i, x, "%s", men->options[i]);
            if (len < maxLength) mvwprintnc(win, ' ', y+i, x+len, maxLength-strlen(men->options[i]));
            wattroff(win, fmt_mask);
        } else {
            mvwprintw(win, y+i, x, "%s", men->options[i]);
            if (len < maxLength) mvwprintnc(win, ' ', y+i, x+len, maxLength-strlen(men->options[i]));
        }
    }
}

void delete_menu(menu* men) {
    if (!men) return;
    if (men->options) {
        for (int i = 0; i < men->num_options; ++i) {
            free(men->options[i]); // save since we strdup'd
        }
        free(men->options);
    }
    free(men);
}

OptionSelected poll_menu(WINDOW* win, menu* men, int y, int x) {
    // Drive menu with win input
    OptionSelected option_selected;
    keypad(win, TRUE);
    curs_set(0);
    men->in_focus = TRUE;

    mvwprint_menu(men, win, y, x);
    wrefresh(win);
    update_panels();
    doupdate();
    do {
        int ch = wgetch(win);
        switch (ch) {
            case KEY_LEFT:
            case KEY_UP:
                if (men->selected > 0) men->selected--;
                break;
            case KEY_RIGHT:
            case KEY_DOWN:
                if (men->selected < men->num_options-1) men->selected++;
                break;
            case '\r':
            case '\n':
            case KEY_ENTER: {
                men->in_focus = FALSE;
                keypad(win, FALSE);
                mvwprint_menu(men, win, y, x);
                option_selected.idx = men->selected;
                option_selected.str = men->options[men->selected];
                return option_selected;
            }
        }
        wrefresh(win);
        mvwprint_menu(men, win, y, x);
        update_panels();
        doupdate();
    } while (1);
}