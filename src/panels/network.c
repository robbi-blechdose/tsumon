#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../display.h"

#define NETWORK_PANEL_HEIGHT 9
#define NETWORK_PANEL_WIDTH 39

#define B_TO_MB(X) ((X) / 1048576.0f)

static const uint64_t scales[] = {
    1024, //1KiB
    64 * 1024, //64KiB
    512 * 1024, //512KiB
    1024 * 1024, //1MiB
    16 * 1024 * 1024, //16MiB
    64 * 1024 * 1024, //64MiB
    512 * 1024 * 1024, //512MiB
    1024 * 1024 * 1024 //1GiB
};

static const char* scaleNames[] = {
    "  1K",
    " 64K",
    "512K",
    "  1M",
    " 16M",
    "512M",
    "  1G"
};

typedef struct {
    uint8_t numInterfaces;
    uint8_t interfaceIndex;
    char interfaceName[PANEL_WIDTH - 1];
    uint64_t totalDownLast;
    uint64_t totalUpLast;
    uint64_t down;
    uint64_t up;
} NetStatus;

static NetStatus net;
#define HISTORY_SIZE 13
static uint64_t downHistory[HISTORY_SIZE];
static uint64_t upHistory[HISTORY_SIZE];
static uint8_t downHistoryScaled[HISTORY_SIZE];
static uint8_t upHistoryScaled[HISTORY_SIZE];
static uint8_t downHistoryScale;
static uint8_t upHistoryScale;

/**
 * Assumes a 1-second window
 **/
uint8_t readNetworkUsage()
{
    uint64_t totalDown;
    uint64_t totalUp;

    FILE* netdev = fopen("/proc/net/dev", "r");
    if(netdev == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    for(uint8_t i = 0; i < net.interfaceIndex + 1; i++)
    {
        if(getline(&line, &n, netdev) <= 0)
        {
            fclose(netdev);
            return 2;
        }
    }

    if(getline(&line, &n, netdev) > 0)
    {
        if(fscanf(netdev, "%*s %lu %*u %*u %*u %*u %*u %*u %*u %lu", &totalDown, &totalUp) != 2)
        {
            fclose(netdev);
            return 3;
        }
    }

    if(line != NULL)
    {
        free(line);
    }

    //Calculate difference
    net.down = totalDown - net.totalDownLast;
    net.up = totalUp - net.totalUpLast;

    fclose(netdev);

    //Swap values
    net.totalDownLast = totalDown;
    net.totalUpLast = totalUp;

    return 0;
}

uint8_t getInterfaceName(char* name, uint8_t interfaceIndex)
{
    FILE* netdev = fopen("/proc/net/dev", "r");
    if(netdev == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    for(uint8_t i = 0; i < interfaceIndex + 1; i++)
    {
        if(getline(&line, &n, netdev) <= 0)
        {
            fclose(netdev);
            return 2;
        }
    }

    if(getline(&line, &n, netdev) > 0)
    {
        if(fscanf(netdev, "%s", name) != 1)
        {
            fclose(netdev);
            return 3;
        }
    }
    
    if(line != NULL)
    {
        free(line);
    }

    fclose(netdev);
    return 0;
}

uint8_t getNumInterfaces(uint8_t* numInterfaces)
{
    FILE* netdev = fopen("/proc/net/dev", "r");
    if(netdev == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    while(getline(&line, &n, netdev) > 0)
    {
        (*numInterfaces)++;
    }
    (*numInterfaces)--; //First line isn't an interface

    if(line != NULL)
    {
        free(line);
    }

    fclose(netdev);
    return 0;
}

void scaleHistory(uint64_t* history, uint8_t* scaledHistory, uint8_t* scale)
{
    //Select scale
    *scale = 0;
    for(uint8_t i = 0; i < HISTORY_SIZE; i++)
    {
        while(history[i] > scales[*scale])
        {
            (*scale)++;
        }
    }

    //Scale history
    for(uint8_t i = 0; i < HISTORY_SIZE; i++)
    {
        scaledHistory[i] = history[i] * 100.0f / scales[*scale];
    }
}

void updateNetworkValues(Panel* panel, uint16_t refreshInterval)
{
    readNetworkUsage();
    net.down *= 1.0f / (refreshInterval / 1000.0f);
    net.up *= 1.0f / (refreshInterval / 1000.0f);

    //Shift previous entries back
    for(uint8_t i = 0; i < HISTORY_SIZE - 1; i++)
    {
        downHistory[i] = downHistory[i + 1];
    }
    //Read new entry
    downHistory[HISTORY_SIZE - 1] = net.down;
    //Shift previous entries back
    for(uint8_t i = 0; i < HISTORY_SIZE - 1; i++)
    {
        upHistory[i] = upHistory[i + 1];
    }
    //Read new entry
    upHistory[HISTORY_SIZE - 1] = net.up;

    //addEntryToHistory(&downHistory, HISTORY_SIZE, net.down);
    //addEntryToHistory(&upHistory, HISTORY_SIZE, net.up);

    //Calculate scaled histories
    scaleHistory(downHistory, downHistoryScaled, &downHistoryScale);
    scaleHistory(upHistory, upHistoryScaled, &upHistoryScale);
}

void drawNetworkPanel(Panel* panel)
{
    drawTitledWindow(panel->window, "Network", NETWORK_PANEL_WIDTH);
    char buffer[NETWORK_PANEL_WIDTH];

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, net.interfaceName);
    wattrset(panel->window, 0);
    sprintf(buffer, "Down: %6.2f MiB/s Up:   %6.2f MiB/s", B_TO_MB(net.down), B_TO_MB(net.up));
    mvwaddstr(panel->window, 2, 1, buffer);

    drawGraphLabels(panel->window, 4, 1, 4, "   0", scaleNames[downHistoryScale]);
    drawGraph(panel->window, 4, 6, 4, HISTORY_SIZE, downHistoryScaled);

    drawGraphLabels(panel->window, 4, 20, 4, "   0", scaleNames[upHistoryScale]);
    drawGraph(panel->window, 4, 25, 4, HISTORY_SIZE, upHistoryScaled);
}

void initNetworkPanel(Panel* panel)
{
    panel->height = NETWORK_PANEL_HEIGHT;
    panel->width = NETWORK_PANEL_WIDTH;
    panel->window = newwin(NETWORK_PANEL_HEIGHT, NETWORK_PANEL_WIDTH, 0, 0);

    panel->update = &updateNetworkValues;
    panel->draw = &drawNetworkPanel;

    if(getNumInterfaces(&net.numInterfaces))
    {
        strcpy(net.interfaceName, "NO INTERFACES");
    }
    net.interfaceIndex = 1; //Default to first interface after LO
    if(getInterfaceName(net.interfaceName, net.interfaceIndex))
    {
        strcpy(net.interfaceName, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readNetworkUsage();
}

/**
void drawNetworkPanelSettings(WINDOW* win, Panel* panel)
{
    char buffer[16];
    sprintf(buffer, "Interface: %d", net.interfaceIndex);
    mvwaddstr(win, 1, 1, buffer);
}

void moveNetworkPanelSettingsCursor(Panel* panel, bool up)
{
    if(up)
    {
        net.interfaceIndex++;
        if(net.interfaceIndex >= net.numInterfaces)
        {
            net.interfaceIndex = 0;
        }
    }
    else
    {
        if(net.interfaceIndex == 0)
        {
            net.interfaceIndex = net.numInterfaces - 1;
        }
        else
        {
            net.interfaceIndex--;
        }
    }
    
    //Reload name and usage
    if(getInterfaceName(net.interfaceName, net.interfaceIndex))
    {
        strcpy(net.interfaceName, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readNetworkUsage(net);
}
**/