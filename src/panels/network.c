#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../display.h"

#define NETWORK_PANEL_HEIGHT 9

#define B_TO_MB(X) ((X) / 1048576.0f)

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
#define HISTORY_SIZE 11
static uint64_t downHistory[HISTORY_SIZE];
static uint64_t upHistory[HISTORY_SIZE];
static uint8_t downHistoryScaled[HISTORY_SIZE];
static uint8_t upHistoryScaled[HISTORY_SIZE];
static uint8_t downHistoryScale;
static uint8_t upHistoryScale;

/**
 * Assumes a 1-second window
 **/
uint8_t readNetworkUsage(void)
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

void updateNetworkValues(Panel* panel, uint16_t refreshInterval)
{
    readNetworkUsage();
    net.down *= 1.0f / (refreshInterval / 1000.0f);
    net.up *= 1.0f / (refreshInterval / 1000.0f);

    addEntryToHistory(downHistory, HISTORY_SIZE, &net.down, sizeof(uint64_t));
    addEntryToHistory(upHistory, HISTORY_SIZE, &net.up, sizeof(uint64_t));
    scaleByteHistory(downHistory, HISTORY_SIZE, downHistoryScaled, &downHistoryScale);
    scaleByteHistory(upHistory, HISTORY_SIZE, upHistoryScaled, &upHistoryScale);
}

void colorNetworkGraph(WINDOW* win, float value)
{
    wcolor_set(win, C_CyanBlack, 0);
}

void drawNetworkPanel(Panel* panel)
{
    drawPanelBase(panel, "Network");
    char buffer[PANEL_WIDTH];

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, net.interfaceName);
    wattrset(panel->window, 0);
    mvwaddstr(panel->window, 2, 1, "Down:");
    mvwaddstr(panel->window, 2, 18, "Up:");
    sprintf(buffer, "%6.2f MiB/s     %6.2f MiB/s", B_TO_MB(net.down), B_TO_MB(net.up));
    mvwaddstr(panel->window, 3, 5, buffer);

    drawGraphLabels(panel->window, 4, 1, 4, "   0", byteScaleNames[downHistoryScale]);
    drawGraphColor(panel->window, 4, 6, 4, HISTORY_SIZE, downHistoryScaled, &colorNetworkGraph);
    drawGraphLabels(panel->window, 4, 18, 4, "   0", byteScaleNames[upHistoryScale]);
    drawGraphColor(panel->window, 4, 23, 4, HISTORY_SIZE, upHistoryScaled, &colorNetworkGraph);
}

void initNetworkPanel(Panel* panel)
{
    initPanelBase(panel, NETWORK_PANEL_HEIGHT, PANEL_WIDTH);

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