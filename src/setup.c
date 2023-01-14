#include "setup.h"

#include <stdlib.h>
#include <string.h>
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

uint8_t numPanels;
PanelType* panelTypes;

void drawSetup(WINDOW* win)
{
    char buffer[SETUP_WIN_WIDTH - 1];

    drawTitledWindow(win, "Setup", SETUP_WIN_WIDTH);

    mvwaddstr(win, 1, 3, "Refresh interval:");
    drawSlider(win, 1, 21, NUM_REFRESH_INTERVALS, refreshIntervalIndex);
    sprintf(buffer, "%3.1fs", refreshIntervals[refreshIntervalIndex] / 1000.0f);
    mvwaddstr(win, 1, 29, buffer);

    mvwaddstr(win, 3, 3, "Panels:");

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

PanelType* getPanelTypes()
{
    return panelTypes;
}

char* getFullConfigPath()
{
    size_t length = strlen(getenv("HOME")) + strlen("/.config/") + strlen(CONFIG_NAME) + 1;
    char* configPath = malloc(length);
    strcpy(configPath, getenv("HOME"));
    strcat(configPath, "/.config/");
    strcat(configPath, CONFIG_NAME);
    return configPath;
}

uint8_t saveConfig()
{
    char* configPath = getFullConfigPath();
    FILE* config = fopen(configPath, "wb");
    free(configPath);
    if(config == NULL)
    {
        return 1;
    }

    //Refresh interval
    if(fwrite(&refreshIntervalIndex, sizeof(refreshIntervalIndex), 1, config) != 1)
    {
        return 2;
    }

    //Panels
    if(fwrite(&numPanels, sizeof(numPanels), 1, config) != 1)
    {
        return 3;
    }
    if(fwrite(panelTypes, sizeof(PanelType), numPanels, config) != numPanels)
    {
        return 4;
    }

    fclose(config);
    return 0;
}

uint8_t loadConfig()
{
    char* configPath = getFullConfigPath();
    FILE* config = fopen(configPath, "rb");
    free(configPath);
    if(config == NULL)
    {
        return 1;
    }

    //Refresh interval
    if(fread(&refreshIntervalIndex, sizeof(refreshIntervalIndex), 1, config) != 1)
    {
        return 2;
    }
    
    //Panels
    if(fread(&numPanels, sizeof(numPanels), 1, config) != 1)
    {
        return 3;
    }
    panelTypes = malloc(sizeof(PanelType) * numPanels);
    if(fread(panelTypes, sizeof(PanelType), numPanels, config) != numPanels)
    {
        return 4;
    }

    fclose(config);
    return 0;
}

void setInitialConfig()
{
    refreshIntervalIndex = 1;
    numPanels = 4;
    panelTypes = malloc(sizeof(PanelType) * 4);
    panelTypes[0] = P_CPU;
    panelTypes[1] = P_RAM;
    panelTypes[2] = P_GPU;
    panelTypes[3] = P_NETWORK;
}

void quitSetup()
{
    free(panelTypes);
}