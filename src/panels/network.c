#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B_TO_MB(X) ((X) / 1048576.0f)

typedef struct {
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
    for(uint8_t i = 0; i < 2; i++)
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

uint8_t getInterfaceName(char* name)
{
    FILE* netdev = fopen("/proc/net/dev", "r");
    if(netdev == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    for(uint8_t i = 0; i < 2; i++)
    {
        if(getline(&line, &n, netdev) <= 0)
        {
            fclose(netdev);
            return 2;
        }
    }
    if(line != NULL)
    {
        free(line);
    }

    if(getline(&line, &n, netdev) > 0)
    {
        if(fscanf(netdev, "%s", name) != 1)
        {
            fclose(netdev);
            return 3;
        }
    }

    fclose(netdev);
    return 0;
}

void initNetworkPanel(Panel* panel)
{
    panel->window = newwin(PANEL_HEIGHT, PANEL_WIDTH, 0, 0);
    panel->data = malloc(sizeof(NetStatus));
    if(getInterfaceName(panel->title))
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