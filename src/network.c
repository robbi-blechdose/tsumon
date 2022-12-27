#include "network.h"

#include <stdio.h>

uint8_t readNetworkUsage(NetStatus* net)
{
    static uint64_t downLast;
    uint64_t down;
    static uint64_t upLast;
    uint64_t up;

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
        if(fscanf(netdev, "%s %lu %*u %*u %*u %*u %*u %*u %*u %lu", net->interface, &down, &up) != 3)
        {
            fclose(netdev);
            return 3;
        }
    }

    //Calculate difference
    net->down = down - downLast;
    net->up = up - upLast;

    fclose(netdev);

    //Swap values
    downLast = down;
    upLast = up;

    return 0;
}