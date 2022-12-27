#ifndef RAM_H
#define RAM_H

#include <stdint.h>

typedef struct {
    uint64_t totalKb;
    uint64_t freeKb;
    float usagePercent;
} RAMStatus;

uint8_t readRAMUsage(RAMStatus* ram);

#endif