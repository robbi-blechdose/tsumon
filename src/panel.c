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

void addEntryToHistory(void* restrict history, uint8_t size, void* restrict newValue, size_t typeSize)
{
    //Shift previous entries back
    memmove(history, history + typeSize, (size - 1) * typeSize);
    //Add new entry at the end
    memcpy(history + (size - 1) * typeSize, newValue, typeSize);
}

const uint64_t byteScales[] = {
    1024, //1KiB
    64 * 1024, //64KiB
    512 * 1024, //512KiB
    1024 * 1024, //1MiB
    16 * 1024 * 1024, //16MiB
    64 * 1024 * 1024, //64MiB
    512 * 1024 * 1024, //512MiB
    1024 * 1024 * 1024 //1GiB
};

const char* byteScaleNames[] = {
    "  1K",
    " 64K",
    "512K",
    "  1M",
    " 16M",
    "512M",
    "  1G"
};

void scaleByteHistory(uint64_t* history, uint8_t size, uint8_t* scaledHistory, uint8_t* scale)
{
    //Select scale
    *scale = 0;
    for(uint8_t i = 0; i < size; i++)
    {
        while(history[i] > byteScales[*scale])
        {
            (*scale)++;
        }
    }

    //Scale history
    for(uint8_t i = 0; i < size; i++)
    {
        scaledHistory[i] = history[i] * 100.0f / byteScales[*scale];
    }
}