#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <ncurses.h>

#define SETUP_WIN_HEIGHT 5
#define SETUP_WIN_WIDTH 60

#define CONFIG_NAME "spmon.conf"

void drawSetup(WINDOW* win);

void moveSetupCursorLR(bool left);

uint16_t getRefreshInterval();

uint8_t saveConfig();
uint8_t loadConfig();

#endif