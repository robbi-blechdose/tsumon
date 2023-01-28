#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B_TO_MB(X) ((X) / 1048576.0f)

typedef struct {
    uint8_t numInterfaces;
    uint8_t interfaceIndex;
    uint64_t totalDownLast;
    uint64_t totalUpLast;
    uint64_t down;
    uint64_t up;
} NetStatus;

uint8_t readNetworkUsage(Panel* panel)
{
    NetStatus* net = (NetStatus*) panel->data;

    uint64_t totalDown;
    uint64_t totalUp;

    FILE* netdev = fopen("/proc/net/dev", "r");
    if(netdev == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    for(uint8_t i = 0; i < net->interfaceIndex + 1; i++)
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
    net->down = totalDown - net->totalDownLast;
    net->up = totalUp - net->totalUpLast;

    fclose(netdev);

    //Swap values
    net->totalDownLast = totalDown;
    net->totalUpLast = totalUp;

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

void initNetworkPanel(Panel* panel)
{
    panel->window = newwin(PANEL_HEIGHT, PANEL_WIDTH, 0, 0);
    panel->data = malloc(sizeof(NetStatus));
    NetStatus* net = (NetStatus*) panel->data;

    if(getNumInterfaces(&net->numInterfaces))
    {
        strcpy(panel->title, "NO INTERFACES");
    }
    net->interfaceIndex = 1; //Default to first interface after LO
    if(getInterfaceName(panel->title, net->interfaceIndex))
    {
        strcpy(panel->title, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readNetworkUsage(panel);
}

void drawNetworkPanelContents(Panel* panel)
{
    char buffer[PANEL_WIDTH];
    NetStatus* net = (NetStatus*) panel->data;

    sprintf(buffer, "Down: %6.2f MiB/s", B_TO_MB(net->down));
    mvwaddstr(panel->window, 2, 1, buffer);
    sprintf(buffer, "Up:   %6.2f MiB/s", B_TO_MB(net->up));
    mvwaddstr(panel->window, 3, 1, buffer);
}

void drawNetworkPanelSettings(WINDOW* win, Panel* panel)
{
    NetStatus* net = (NetStatus*) panel->data;

    char buffer[16];
    sprintf(buffer, "Interface: %d", net->interfaceIndex);
    mvwaddstr(win, 1, 1, buffer);
}

void moveNetworkPanelSettingsCursor(Panel* panel, bool up)
{
    NetStatus* net = (NetStatus*) panel->data;

    if(up)
    {
        net->interfaceIndex++;
        if(net->interfaceIndex >= net->numInterfaces)
        {
            net->interfaceIndex = 0;
        }
    }
    else
    {
        if(net->interfaceIndex == 0)
        {
            net->interfaceIndex = net->numInterfaces - 1;
        }
        else
        {
            net->interfaceIndex--;
        }
    }
    
    //Reload name and usage
    if(getInterfaceName(panel->title, net->interfaceIndex))
    {
        strcpy(panel->title, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readNetworkUsage(panel);
}