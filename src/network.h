#ifndef _NETWORK_H
#define _NETWORK_H

#include <stdint.h>

#define INTERFACE_LENGTH 28

typedef struct {
    char interface[INTERFACE_LENGTH];
    uint64_t down;
    uint64_t up;
} NetStatus;

uint8_t readNetworkUsage(NetStatus* net);

#endif