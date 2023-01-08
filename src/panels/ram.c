#include "ram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../display.h"

#define KB_TO_GB(X) ((X) / 1048576.0f)

typedef struct {
    uint64_t totalKb;
    uint64_t freeKb;
    float usagePercent;
} RAMStatus;

uint8_t readRAMUsage(Panel* panel)
{
    RAMStatus* ram = (RAMStatus*) panel->data;

    uint64_t memFree, memAvailable, buffers, cached;

    FILE* meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL)
    {
        return 1;
    }
    if(fscanf(meminfo, "MemTotal: %lu kB MemFree: %lu kB MemAvailable: %lu kB Buffers: %lu kB Cached: %lu kB",
                    &ram->totalKb, &memFree, &memAvailable, &buffers, &cached) != 5)
    {
        fclose(meminfo);
        return 2;
    }
    fclose(meminfo);

    ram->freeKb = memFree + buffers + cached;
    ram->usagePercent = (1.0f - ((float) ram->freeKb) / ram->totalKb) * 100;

    return 0;
}

void initRAMPanel(Panel* panel)
{
    panel->type = P_RAM;
    panel->window = newwin(PANEL_HEIGHT, PANEL_WIDTH, 0, 0);
    panel->data = malloc(sizeof(RAMStatus));
    strcpy(panel->title, "");
}

void drawRAMPanelContents(Panel* panel)
{
    char buffer[PANEL_WIDTH];
    RAMStatus* ram = (RAMStatus*) panel->data;

    drawBarWithPercentage(panel->window, 2, 1, ram->usagePercent);
    sprintf(buffer, "Used: %4.1f/%4.1f GiB", KB_TO_GB(ram->totalKb - ram->freeKb), KB_TO_GB(ram->totalKb));
    mvwaddstr(panel->window, 3, 1, buffer);
}