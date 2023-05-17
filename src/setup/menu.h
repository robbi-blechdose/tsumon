#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>
#include <ncurses.h>

#include "../config.h"

typedef struct MenuTree {
    bool selected;
    int8_t cursor;

    WINDOW* win;

    void (*moveCursorLR)(bool, struct MenuTree*);
    void (*moveCursorUD)(bool, struct MenuTree*);
    void (*enterCursor)(struct MenuTree*);
    void (*draw)(struct MenuTree*);

    uint8_t numChildren;
    struct MenuTree** children;
    struct MenuTree* parent;
} MenuTree;

void createMenuTree(MenuTree* mt);
void insertMenuTree(MenuTree* mt, MenuTree* child);

void drawMenuTree(MenuTree* mt);

void destroyMenuTree(MenuTree* mt);

#endif