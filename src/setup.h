#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <ncurses.h>
#include "config.h"
#include "panel.h"

#define SETUP_WIN_HEIGHT 12
#define SETUP_WIN_WIDTH  60

#define SETUP_EDIT_WIN_HEIGHT 3
#define SETUP_EDIT_WIN_WIDTH 30

extern uint8_t numPanels;

void initSetup(WINDOW* win);

void drawSetup(Configuration* config);

void moveCursor(int8_t* cursor, bool dec, int8_t max);

void moveSetupCursorLR(bool left, Configuration* config);
void moveSetupCursorUD(bool up, Configuration* config);
void enterSetupCursor(Configuration* config);
void cancelSetupCursor();

bool canExitSetup();

#endif