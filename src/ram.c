#include "ram.h"

#include <stdio.h>

uint8_t readRAMUsage(RAMStatus* ram)
{
    uint64_t memFree, memAvailable, buffers, cached;

    FILE* meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL)
    {
        return 1;
    }
    fscanf(meminfo, "MemTotal: %lu kB MemFree: %lu kB MemAvailable: %lu kB Buffers: %lu kB Cached: %lu kB",
                    &ram->totalKb, &memFree, &memAvailable, &buffers, &cached);
    fclose(meminfo);

    ram->freeKb = memFree + buffers + cached;
    ram->usagePercent = (1.0f - ((float) ram->freeKb) / ram->totalKb) * 100;

    return 0;
}