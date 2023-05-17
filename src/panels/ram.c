#include "ram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../display.h"

#define RAM_PANEL_HEIGHT 8
#define RAM_PANEL_WIDTH 30

#define KB_TO_GB(X) ((X) / 1048576.0f)

typedef struct {
    uint64_t totalKb;
    uint64_t freeKb;
    float usagePercent;
} RAMStatus;

static RAMStatus ram;
#define HISTORY_SIZE 23
static uint8_t ramUsageHistory[HISTORY_SIZE];

uint8_t readRAMUsage(void)
{
    uint64_t memFree, memAvailable, buffers, cached;

    FILE* meminfo = fopen("/proc/meminfo", "r");
    if(meminfo == NULL)
    {
        return 1;
    }
    if(fscanf(meminfo, "MemTotal: %lu kB MemFree: %lu kB MemAvailable: %lu kB Buffers: %lu kB Cached: %lu kB",
                    &ram.totalKb, &memFree, &memAvailable, &buffers, &cached) != 5)
    {
        fclose(meminfo);
        return 2;
    }
    fclose(meminfo);

    ram.freeKb = memFree + buffers + cached;
    ram.usagePercent = (1.0f - ((float) ram.freeKb) / ram.totalKb) * 100;

    return 0;
}

void updateRAMValues(Panel* panel, uint16_t refreshInterval)
{
    readRAMUsage();
    addEntryToHistory(ramUsageHistory, HISTORY_SIZE, ram.usagePercent);
}

void drawRAMPanel(Panel* panel)
{
    drawPanelBase(panel, "RAM");
    char buffer[RAM_PANEL_WIDTH];

    drawBarWithPercentage(panel->window, 1, 1, ram.usagePercent);
    sprintf(buffer, "Used: %4.1f/%4.1f GiB", KB_TO_GB(ram.totalKb - ram.freeKb), KB_TO_GB(ram.totalKb));
    mvwaddstr(panel->window, 2, 1, buffer);

    drawGraphLabels(panel->window, 3, 1, 4, "  0%", "100%");
    drawGraph(panel->window, 3, 6, 4, HISTORY_SIZE, ramUsageHistory);
}

void initRAMPanel(Panel* panel)
{
    initPanelBase(panel, RAM_PANEL_HEIGHT, RAM_PANEL_WIDTH);
    panel->update = &updateRAMValues;
    panel->draw = &drawRAMPanel;
}