#include "panel.h"

#include <stdlib.h>
#include "display.h"

void initPanelBase(Panel* panel, uint8_t height, uint8_t width)
{
    panel->height = height;
    panel->width = width;
    panel->window = newwin(height, width, 0, 0);
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