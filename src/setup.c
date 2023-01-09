#include "setup.h"

#include "display.h"

#define CURSOR_REFRESH 0
static uint8_t cursor = 0;

#define NUM_REFRESH_INTERVALS 4
const uint16_t refreshIntervals[NUM_REFRESH_INTERVALS] = {
     250,
     500,
    1000,
    2000
};
static int8_t refreshIntervalIndex = 0;

void drawSetup(WINDOW* win)
{
    char buffer[SETUP_WIN_WIDTH - 1];

    drawTitledWindow(win, "Setup", SETUP_WIN_WIDTH);

    mvwaddstr(win, 1, 3, "Refresh interval:");
    drawSlider(win, 1, 21, NUM_REFRESH_INTERVALS, refreshIntervalIndex);
    sprintf(buffer, "%3.1fs", refreshIntervals[refreshIntervalIndex] / 1000.0f);
    mvwaddstr(win, 1, 29, buffer);

    //Draw cursor
    wattrset(win, A_BOLD);
    mvwaddch(win, cursor + 1, 1, '>');
    wattrset(win, 0);

    wrefresh(win);
}

void moveSetupCursorLR(bool left)
{
    switch(cursor)
    {
        case CURSOR_REFRESH:
        {
            if(left)
            {
                refreshIntervalIndex--;
                if(refreshIntervalIndex < 0)
                {
                    refreshIntervalIndex = NUM_REFRESH_INTERVALS - 1;
                }
            }
            else
            {
                refreshIntervalIndex++;
                if(refreshIntervalIndex >= NUM_REFRESH_INTERVALS)
                {
                    refreshIntervalIndex = 0;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

uint16_t getRefreshInterval()
{
    return refreshIntervals[refreshIntervalIndex];
}