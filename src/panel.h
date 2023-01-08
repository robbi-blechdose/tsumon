#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <ncurses.h>

typedef enum {
    P_CPU,
    P_RAM,
    P_GPU,
    P_NETWORK
} PanelType;

#define PANEL_HEIGHT 5
#define PANEL_WIDTH 30

typedef struct {
    PanelType type;
    char title[PANEL_WIDTH - 1];
    void* data;

    WINDOW* window;
    uint8_t x;
    uint8_t y;
} Panel;

void updatePanel(Panel* panel);
void quitPanel(Panel* panel);

#endif