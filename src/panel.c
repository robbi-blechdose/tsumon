#include "panel.h"

#include <stdlib.h>
#include "display.h"

/**
void updatePanel(Panel* panel, uint16_t refreshInterval)
{
    drawTitledWindow(panel->window, panelNames[panel->type], PANEL_WIDTH);

    switch(panel->type)
    {
        case P_CPU:
        {
            updateCPUPanel(panel);
            break;
        }
        case P_RAM:
        {
            readRAMUsage(panel);
            drawRAMPanelContents(panel);
            break;
        }
        case P_GPU:
        {
            updateGPUPanel(panel);
            break;
        }
        case P_NETWORK:
        {
            updateNetworkPanel(panel, refreshInterval);
            break;
        }
        case P_FETCH:
        {
            readUptime(panel);
            drawFetchPanelContents(panel);
            break;
        }
    }
    wrefresh(panel->window);
}

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