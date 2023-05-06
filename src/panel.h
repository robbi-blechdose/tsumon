#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <ncurses.h>

#define PANEL_HEIGHT 6
#define PANEL_WIDTH 35

//TODO: add height (since it can be different)
//TODO: add name? (or move it to individual panels)
typedef struct Panel {
    WINDOW* window;

    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;

    void (*update)(struct Panel*, uint16_t);
    void (*draw)(struct Panel*);
} Panel;

uint8_t initPanel(Panel* panel);
void updatePanel(Panel* panel, uint16_t refreshInterval);
void drawPanelSettings(WINDOW* win, Panel* panel);
void quitPanel(Panel* panel);

void addEntryToHistory(uint8_t* history, uint8_t size, uint8_t newValue);

#endif