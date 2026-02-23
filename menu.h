//
// Created by devinrcohen on 2/23/26.
//

#ifndef TEXTRPG_MENU_H
#define TEXTRPG_MENU_H

#include <curses.h>
#include <panel.h>

/*typedef struct {
    int y;
    int x;
} point;*/

typedef struct {
    char **options;
    int num_options;
    int selected;
    bool in_focus;
} menu;

typedef struct {
    int idx;
    char* str;
} OptionSelected;

menu* initmenu(const char* const options[]);
void mvwprint_menu(const menu*, WINDOW*, int, int);
void delete_menu(menu*);
OptionSelected poll_menu(WINDOW*, menu*);

#endif //TEXTRPG_MENU_H