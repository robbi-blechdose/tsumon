#include "general_setup.h"

#include "../display.h"
#include "../setup.h"

void drawRefreshInterval(MenuTree* menu)
{
    char buffer[SETUP_WIN_WIDTH - 1];
    if(menu->selected)
    {
        wattrset(menu->win, A_BOLD);
        mvwaddstr(menu->win, 1, 3, "Refresh interval:");
        wattrset(menu->win, 0);
    }
    else
    {
        mvwaddstr(menu->win, 1, 3, "Refresh interval:");
    }
    drawSlider(menu->win, 1, 21, NUM_REFRESH_INTERVALS, config.refreshIntervalIndex);
    sprintf(buffer, "%3.1fs", refreshIntervals[config.refreshIntervalIndex] / 1000.0f);
    mvwaddstr(menu->win, 1, 29, buffer);
}

void moveRefreshIntervalCursorLR(bool left, MenuTree*)
{
    moveCursor(&config.refreshIntervalIndex, left, NUM_REFRESH_INTERVALS - 1);
}

void initRefreshIntervalMenu(MenuTree* menu)
{
    menu->draw = &drawRefreshInterval;
    menu->moveCursorLR = &moveRefreshIntervalCursorLR;
}

void drawWidthLimit(MenuTree* menu)
{
    char buffer[SETUP_WIN_WIDTH - 1];
    if(menu->selected)
    {
        wattrset(menu->win, A_BOLD);
        mvwaddstr(menu->win, 3, 3, "Width Limit:");
        wattrset(menu->win, 0);
    }
    else
    {
        mvwaddstr(menu->win, 3, 3, "Width Limit:");
    }
    if(config.widthLimit)
    {
        sprintf(buffer, "%d  ", config.widthLimit);
        mvwaddstr(menu->win, 3, 16, buffer);
    }
    else
    {
        mvwaddstr(menu->win, 3, 16, "Off");
    }
}

void moveWidthLimitCursorLR(bool left, MenuTree*)
{
    moveCursor(&config.widthLimit, left, 8); //TODO: replace limit number with a more sensible constant
}

void initWidthLimitMenu(MenuTree* menu)
{
    menu->draw = &drawWidthLimit;
    menu->moveCursorLR = &moveWidthLimitCursorLR;
}

#define NUM_HIGHLIGHT_COLORS 5
const char* highlightColorNames[NUM_HIGHLIGHT_COLORS] = {
    [C_WhiteBlack] =  "White ",
    [C_GreenBlack] =  "Green ",
    [C_YellowBlack] = "Yellow",
    [C_RedBlack] =    "Red   ",
    [C_CyanBlack] =   "Cyan  "
};

void drawHighlightColor(MenuTree* menu)
{
    if(menu->selected)
    {
        wattrset(menu->win, A_BOLD);
        mvwaddstr(menu->win, 5, 3, "Highlight color:");
        wattrset(menu->win, 0);
    }
    else
    {
        mvwaddstr(menu->win, 5, 3, "Highlight color:");
    }
    wcolor_set(menu->win, config.highlightColor, 0);
    mvwaddstr(menu->win, 5, 20, highlightColorNames[config.highlightColor]);
    wcolor_set(menu->win, C_WhiteBlack, 0);
}

void moveHighlightColorCursorLR(bool left, MenuTree*)
{
    moveCursor(&config.highlightColor, left, NUM_COLOR_PAIRS - 2); //-2 since we want to exclude BlackCyan
}

void initHighlightColorMenu(MenuTree* menu)
{
    menu->draw = &drawHighlightColor;
    menu->moveCursorLR = &moveHighlightColorCursorLR;
}