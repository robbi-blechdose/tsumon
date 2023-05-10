#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <ncurses.h>

#define PANEL_HEIGHT 6
#define PANEL_WIDTH 35

typedef struct Panel {
    WINDOW* window;

    uint8_t width;
    uint8_t height;

    void (*update)(struct Panel*, uint16_t);
    void (*draw)(struct Panel*);
} Panel;

void initPanelBase(Panel* panel, uint8_t height, uint8_t width);

//void drawPanelSettings(WINDOW* win, Panel* panel);
void quitPanel(Panel* panel);

void addEntryToHistory(uint8_t* history, uint8_t size, uint8_t newValue);

#endif