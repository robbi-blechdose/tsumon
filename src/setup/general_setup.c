#include "general_setup.h"

#include "../display.h"
#include "../setup.h"

void drawRefreshInterval(MenuTree* menu, Configuration* config)
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
    drawSlider(menu->win, 1, 21, NUM_REFRESH_INTERVALS, config->refreshIntervalIndex);
    sprintf(buffer, "%3.1fs", refreshIntervals[config->refreshIntervalIndex] / 1000.0f);
    mvwaddstr(menu->win, 1, 29, buffer);
}

void moveRefreshIntervalCursorLR(bool left, MenuTree*, Configuration* config)
{
    moveCursor(&config->refreshIntervalIndex, left, NUM_REFRESH_INTERVALS - 1);
}

void initRefreshIntervalMenu(MenuTree* menu)
{
    menu->draw = &drawRefreshInterval;
    menu->moveCursorLR = &moveRefreshIntervalCursorLR;
}

void drawWidthLimit(MenuTree* menu, Configuration* config)
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
    if(config->widthLimit)
    {
        sprintf(buffer, "%d  ", config->widthLimit);
        mvwaddstr(menu->win, 3, 16, buffer);
    }
    else
    {
        mvwaddstr(menu->win, 3, 16, "Off");
    }
}

void moveWidthLimitCursorLR(bool left, MenuTree*, Configuration* config)
{
    moveCursor(&config->widthLimit, left, 8); //TODO: replace limit number with a more sensible constant
}

void initWidthLimitMenu(MenuTree* menu)
{
    menu->draw = &drawWidthLimit;
    menu->moveCursorLR = &moveWidthLimitCursorLR;
}