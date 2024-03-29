#include "display.h"

#include <string.h>

void ncursesSetup(void)
{
    initscr();
    noecho();
    curs_set(0);
    //Set up colors
    start_color();
    init_pair(C_WB, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_GB, COLOR_GREEN, COLOR_BLACK);
    init_pair(C_YB, COLOR_YELLOW, COLOR_BLACK);
    init_pair(C_RB, COLOR_RED, COLOR_BLACK);
    init_pair(C_BC, COLOR_BLACK, COLOR_CYAN);
}

void drawBar(WINDOW* win, uint8_t y, uint8_t x, float value)
{
    mvwaddch(win, y, x, '[');
    mvwaddch(win, y, x + 21, ']');

    //Set color
    if(value < THRESHOLD_GREEN)
    {
        wcolor_set(win, C_GB, 0);
    }
    else if(value < THRESHOLD_YELLOW)
    {
        wcolor_set(win, C_YB, 0);
    }
    else
    {
        wcolor_set(win, C_RB, 0);
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
    wcolor_set(win, C_WB, 0);
}

void drawBarWithPercentage(WINDOW* win, uint8_t y, uint8_t x, float value)
{
    drawBar(win, y, x, value);
    #define PERCENT_BUFFER_SIZE 7
    char buffer[PERCENT_BUFFER_SIZE];
    snprintf(buffer, PERCENT_BUFFER_SIZE, "%5.1f%%", value);
    mvwaddstr(win, y, x + 22, buffer);
}

void drawTitledBarWithPercentage(WINDOW* win, uint8_t y, uint8_t x, float value, const char* title)
{
    mvwaddstr(win, y, x, title);
    drawBarWithPercentage(win, y, x + strlen(title) + 1, value);
}

void drawTitledWindow(WINDOW* win, const char* title, uint8_t width)
{
    wcolor_set(win, C_WB, 0);
    box(win, 0, 0);
    mvwaddstr(win, 0, width / 2 - strlen(title) / 2, title);
}

void drawSlider(WINDOW* win, uint8_t y, uint8_t x, uint8_t length, uint8_t current)
{
    mvwaddch(win, y, x, '<');
    for(uint8_t i = 0; i < length; i++)
    {
        mvwaddch(win, y, x + 1 + i, '=');
        if(i == current)
        {
            mvwaddch(win, y, x + 1 + i, '#');
        }
    }
    mvwaddch(win, y, x + 1 + length, '>');
}

void drawStringCondBold(WINDOW* win, uint8_t y, uint8_t x, const char* str, bool bold)
{
    if(bold)
    {
        wattrset(win, A_BOLD);
        mvwaddstr(win, y, x, str);
        wattrset(win, 0);
    }
    else
    {
        mvwaddstr(win, y, x, str);
    }
}