#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <ncurses.h>

#define SETUP_WIN_HEIGHT 5
#define SETUP_WIN_WIDTH 60

void drawSetup(WINDOW* win);

void moveSetupCursorLR(bool left);

uint16_t getRefreshInterval();

#endif