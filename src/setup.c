#include "setup.h"

#include <stdlib.h>
#include <string.h>
#include "display.h"

#define CURSOR_REFRESH 0
#define CURSOR_PANELS  1
static int8_t mainCursor = 0;

static bool panelSelected = false;
static int8_t panelCursor = 0;

#define PAC_ADD    0
#define PAC_REMOVE 1
static int8_t panelActionCursor = 0;

static int8_t panelAddCursor = 0;

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
    uint8_t x;
    char buffer[SETUP_WIN_WIDTH - 1];

    drawTitledWindow(win, "Setup", SETUP_WIN_WIDTH);

    //Refresh interval
    mvwaddstr(win, 1, 3, "Refresh interval:");
    drawSlider(win, 1, 21, NUM_REFRESH_INTERVALS, refreshIntervalIndex);
    sprintf(buffer, "%3.1fs", refreshIntervals[refreshIntervalIndex] / 1000.0f);
    mvwaddstr(win, 1, 29, buffer);

    //Panels
    mvwaddstr(win, 3, 3, "Panels:");
    x = 11;
    for(uint8_t i = 0; i < numPanels + 1; i++)
    {
        wattrset(win, A_BOLD);
        mvwaddch(win, 3, x, (mainCursor == CURSOR_PANELS && panelCursor == i) ? '>' : ' ');
        wattrset(win, 0);

        if(i < numPanels)
        {
            mvwaddstr(win, 3, x + 1, panelNames[panels[i].type]);
            x += strlen(panelNames[panels[i].type]) + 2;
        }
    }

    mvwaddstr(win, 5, 5, "Add:");
    x = 11;
    for(uint8_t i = 0; i < NUM_PANEL_TYPES; i++)
    {
        wattrset(win, A_BOLD);
        mvwaddch(win, 5, x, (mainCursor == CURSOR_PANELS && panelSelected && panelActionCursor == PAC_ADD && panelAddCursor == i) ? '>' : ' ');
        wattrset(win, 0);

        mvwaddstr(win, 5, x + 1, panelNames[i]);
        x += strlen(panelNames[i]) + 2;
    }
    mvwaddstr(win, 6, 5, "Remove");

    //Draw main cursor
    wattrset(win, A_BOLD);
    mvwaddch(win, mainCursor ? 3 : 1, 1, '>');
    mvwaddch(win, mainCursor ? 1 : 3, 1, ' ');
    //Draw panel action cursor
    mvwaddch(win, 5, 3, panelSelected && panelActionCursor == PAC_ADD ? '>' : ' ');
    mvwaddch(win, 6, 3, panelSelected && panelActionCursor == PAC_REMOVE ? '>' : ' ');
    wattrset(win, 0);

    wrefresh(win);
}

void moveCursor(int8_t* cursor, bool dec, int8_t max)
{
    if(dec)
    {
        (*cursor)--;
        if(*cursor < 0)
        {
            *cursor = max;
        }
    }
    else
    {
        (*cursor)++;
        if(*cursor > max)
        {
            *cursor = 0;
        }
    }
}

void moveSetupCursorLR(bool left)
{
    switch(mainCursor)
    {
        case CURSOR_REFRESH:
        {
            moveCursor(&refreshIntervalIndex, left, NUM_REFRESH_INTERVALS - 1);
            break;
        }
        case CURSOR_PANELS:
        {
            if(panelSelected)
            {
                moveCursor(&panelAddCursor, left, NUM_PANEL_TYPES - 1);
            }
            else
            {
                moveCursor(&panelCursor, left, numPanels);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void moveSetupCursorUD(bool up)
{
    if(!(mainCursor == CURSOR_PANELS && panelSelected))
    {
        moveCursor(&mainCursor, up, 1);
    }
    else if(panelSelected)
    {
        moveCursor(&panelActionCursor, up, 1);
    }
}

void enterSetupCursor()
{
    if(mainCursor != CURSOR_PANELS)
    {
        return;
    }

    if(!panelSelected)
    {
        panelSelected = true;
    }
    else
    {
        panelSelected = false;

        if(panelActionCursor == PAC_ADD)
        {
            //Insert panel before current one
            Panel* newPanels = malloc(sizeof(Panel) * (numPanels + 1));
            for(uint8_t i = 0; i < panelCursor; i++)
            {
                newPanels[i] = panels[i];
            }
            newPanels[panelCursor].type = panelAddCursor;
            initPanel(&newPanels[panelCursor]);
            for(uint8_t i = panelCursor + 1; i < numPanels + 1; i++)
            {
                newPanels[i] = panels[i - 1];
            }
            numPanels++;
            free(panels);
            panels = newPanels;
        }
        else
        {
            if(panelCursor == numPanels)
            {
                return;
            }
            //Remove current panel, shift rest back
            quitPanel(&panels[panelCursor]);
            Panel* newPanels = malloc(sizeof(Panel) * (numPanels - 1));
            uint8_t ni = 0;
            for(uint8_t i = 0; i < numPanels; i++)
            {
                if(i != panelCursor)
                {
                    newPanels[ni++] = panels[i];
                }
            }
            numPanels--;
            free(panels);
            panels = newPanels;
        }
    }
}

void cancelSetupCursor()
{
    if(mainCursor != CURSOR_PANELS)
    {
        return;
    }
    if(panelSelected)
    {
        panelSelected = false;
    }
}

uint16_t getRefreshInterval()
{
    return refreshIntervals[refreshIntervalIndex];
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
    for(uint8_t i = 0; i < numPanels; i++)
    {
        if(fwrite(&panels[i].type, sizeof(PanelType), 1, config) != 1)
        {
            return 4;
        }
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
    panels = malloc(sizeof(Panel) * numPanels);
    for(uint8_t i = 0; i < numPanels; i++)
    {
        if(fread(&panels[i].type, sizeof(PanelType), 1, config) != 1)
        {
            return 4;
        }
    }

    fclose(config);
    return 0;
}

void setInitialConfig()
{
    refreshIntervalIndex = 1;
    numPanels = 4;
    panels = malloc(sizeof(Panel) * 4);
    panels[0].type = P_CPU;
    panels[1].type = P_RAM;
    panels[2].type = P_GPU;
    panels[3].type = P_NETWORK;
}