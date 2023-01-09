#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>

#define C_WB 0
#define C_GB 1
#define C_YB 2
#define C_RB 3
#define C_BC 4

void ncursesSetup(void);

#define THRESHOLD_GREEN 50
#define THRESHOLD_YELLOW 75

/**
 * Draws a 20-character long bar (total width is 22 characters)
 * The bar is colored depending on the threshold values
 * @param win Window to draw to
 * @param y Y position
 * @param x X position
 * @param value Value for the bar to display (0-100)
 **/
void drawBar(WINDOW* win, uint8_t y, uint8_t x, float value);

/**
 * Draws a 20-character bar with the percentage as text next to it (total width is 28 characters)
 * Uses drawBar() internally
 * @param win Window to draw to
 * @param y Y position
 * @param x X position
 * @param value Value to display (0-100)
 **/
void drawBarWithPercentage(WINDOW* win, uint8_t y, uint8_t x, float value);

/**
 * Draws a box with a centered title for a window
 * @param win Window to draw to
 * @param title Title string
 * @param width Width of the window
 **/
void drawTitledWindow(WINDOW* win, const char* title, uint8_t width);

/**
 * Draws a slider with the specified length and a marker at the current position
 * @param win Window to draw to
 * @param y Y position
 * @param x X start position
 * @param length Length of the slider (+2 for start/end marks)
 * @param current Current position of the slider. If > length, no marker is drawn
 **/
void drawSlider(WINDOW* win, uint8_t y, uint8_t x, uint8_t length, uint8_t current);

#endif