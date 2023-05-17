#include "panel.h"

#include <stdlib.h>
#include <string.h>
#include "display.h"

void initPanelBase(Panel* panel, uint8_t height, uint8_t width)
{
    panel->height = height;
    panel->width = width;
    panel->window = newwin(height, width, 0, 0);
}

void drawPanelBase(Panel* panel, const char* title)
{
    wcolor_set(panel->window, C_WhiteBlack, 0);
    box(panel->window, 0, 0);

    wcolor_set(panel->window, config.highlightColor, 0);
    mvwaddstr(panel->window, 0, panel->width / 2 - strlen(title) / 2, title);

    wcolor_set(panel->window, C_WhiteBlack, 0);
}

/**
void drawPanelSettings(WINDOW* win, Panel* panel)
{
    switch(panel->type)
    {
        case P_NETWORK:
        {
            drawNetworkPanelSettings(win, panel);
            wrefresh(win);
            break;
        }
        default:
        {
            break;
        }
    }
}**/

void quitPanel(Panel* panel)
{
    /**
    free(panel->data);

    switch(panel->type)
    {
        case P_GPU:
        {
            quitGPU();
            break;
        }
        default:
        {
            break;
        }
    }**/
}

void addEntryToHistory(uint8_t* history, uint8_t size, uint8_t newValue)
{
    //Shift previous entries back
    for(uint8_t i = 0; i < size - 1; i++)
    {
        history[i] = history[i + 1];
    }
    //Read new entry
    history[size - 1] = newValue;
}