#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <ncurses.h>

#include "config.h"
extern Configuration config;

#define PANEL_HEIGHT 6
#define PANEL_WIDTH 35

#define PANEL_MAX_CHILDREN 4

typedef struct Panel {
    struct Panel** children;
    uint8_t numChildren;

    WINDOW* window;

    uint8_t width;
    uint8_t height;

    void (*update)(struct Panel*, uint16_t);
    void (*draw)(struct Panel*);
    void (*quit)(struct Panel*);
} Panel;

Panel* panelCreate(Panel* parent, uint8_t y, uint8_t x, uint8_t height, uint8_t width);
void panelDestroy(Panel* panel);

void panelInit(Panel* panel, uint8_t height, uint8_t width);
void panelQuit(Panel* panel);

void panelAddChild(Panel* panel, Panel* child);
void panelDrawChildren(Panel* panel);
void panelUpdateChildren(Panel* panel, uint16_t refreshInterval);

void drawPanelBorder(Panel* panel, const char* title);

#endif