#include "display.h"

#include <string.h>

void ncursesSetup()
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    //Set up colors
    start_color();
    init_pair(C_WB, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_GB, COLOR_GREEN, COLOR_BLACK);
    init_pair(C_YB, COLOR_YELLOW, COLOR_BLACK);
    init_pair(C_RB, COLOR_RED, COLOR_BLACK);
}

void drawBar(WINDOW* win, uint8_t y, uint8_t x, float value)
{
    mvwaddch(win, y, x, '[');
    mvwaddch(win, y, x + 21, ']');

    //Set color
    if(value < THRESHOLD_GREEN)
    {
        wcolor_set(win, 1, 0);
    }
    else if(value < THRESHOLD_YELLOW)
    {
        wcolor_set(win, 2, 0);
    }
    else
    {
        wcolor_set(win, 3, 0);
    }

    //Draw bar
    for(uint8_t i = 0; i < 20; i++)
    {
        if(i < value / 5)
        {
            mvwaddch(win, y, x + 1 + i, '|');
        }
        else
        {
            mvwaddch(win, y, x + 1 + i, ' ');
        }
    }

    //Reset color
    wcolor_set(win, 0, 0);
}

void drawBarWithPercentage(WINDOW* win, uint8_t y, uint8_t x, float value)
{
    drawBar(win, y, x, value);
    char buffer[7];
    sprintf(buffer, "%5.1f%%", value);
    mvwaddstr(win, y, x + 22, buffer);
}

void drawTitledWindow(WINDOW* win, const char* title, uint8_t width)
{
    wcolor_set(win, 0, 0);
    box(win, 0, 0);
    mvwaddstr(win, 0, width / 2 - strlen(title) / 2, title);
}