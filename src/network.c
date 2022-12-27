#include "network.h"

#include <stdio.h>

uint64_t readNetworkUsageDown()
{
    FILE* netdev = fopen("/proc/net/dev", "r");
    if(netdev == NULL)
    {
        //TODO: error message?
        return 0;
    }

    //TODO

    fclose(netdev);

    return 0;
}

uint64_t readNetworkUsageUp()
{
    //TODO

    return 0;
}