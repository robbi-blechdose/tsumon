#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <ncurses.h>

typedef enum {
    P_CPU,
    P_RAM,
    P_GPU,
    P_NETWORK,
    P_FETCH
} PanelType;

#define PANEL_HEIGHT 6
#define PANEL_WIDTH 35

typedef struct {
    PanelType type;
    void* data;

    WINDOW* window;
    uint8_t x;
    uint8_t y;
} Panel;

#define NUM_PANEL_TYPES 5
extern const char* panelNames[NUM_PANEL_TYPES];

extern uint8_t numPanels;
extern Panel* panels;

uint8_t initPanel(Panel* panel);
void updatePanel(Panel* panel);
void drawPanelSettings(WINDOW* win, Panel* panel);
void quitPanel(Panel* panel);

#endif