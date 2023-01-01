#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

#define INTERFACE_LENGTH 28

typedef struct {
    char interface[INTERFACE_LENGTH];
    uint64_t down;
    uint64_t up;
} NetStatus;

/**
 * Reads the network usage from the first non-loopback interface (in bytes)
 * Also stores the interface name
 * @param net Struct to store results into
 * @return 0 for success
 **/
uint8_t readNetworkUsage(NetStatus* net);

#endif