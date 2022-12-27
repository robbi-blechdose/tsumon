#include "ram.h"

#include <stdio.h>

uint64_t memTotal;
uint64_t memUnused;

float readRAMUsage()
{
    uint64_t memFree, memAvailable, buffers, cached;

    FILE* meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL)
    {
        return -1;
    }
    fscanf(meminfo, "MemTotal: %lu kB MemFree: %lu kB MemAvailable: %lu kB Buffers: %lu kB Cached: %lu kB",
                    &memTotal, &memFree, &memAvailable, &buffers, &cached);
    fclose(meminfo);

    memUnused = memFree + buffers + cached;

    float usage = 1.0f - ((float) memUnused) / memTotal;
    return usage * 100;
}

uint64_t getTotalRAM()
{
    return memTotal;
}

uint64_t getFreeRAM()
{
    return memUnused;
}