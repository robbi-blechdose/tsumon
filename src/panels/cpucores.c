#include "cpucores.h"

#include <stdlib.h>
#include <string.h>
#include "../display.h"

#include "cpu.h"

#define CPU_CORE_PANEL_HEIGHT 6
#define CPU_CORE_PANEL_WIDTH 14

uint8_t numCpuCores;
CPUUsage* cpuCoreUsages;

void drawCPUCorePanel(Panel* panel)
{
    drawPanelBase(panel, "CPU Cores");

    //TODO: don't hardcode with & height
    for(uint8_t i = 0; i < 4; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            char buffer[PANEL_WIDTH];
            sprintf(buffer, "%02d", i + (j * 4));
            mvwaddstr(panel->window, 1 + j, 1 + i * 3, buffer);
            drawPercentageBlock(panel->window, 1 + j, 1 + i * 3 + 2, cpuCoreUsages[i + (j * 4)].usagePercent);
        }
    }
}

void updateCPUCoreValues(Panel* panel, uint16_t refreshInterval)
{
    for(uint8_t i = 0; i < 4; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            char buffer[16];
            sprintf(buffer, "cpu%d", i + (j * 4));
            readCPUUsage(buffer, &cpuCoreUsages[i + (j * 4)]);
        }
    }
}

//TODO: maybe switch to finding line with "cpu%d" with the highest number?
uint8_t readCPUCoreCount(void)
{
    numCpuCores = 0;

    FILE* stat = fopen("/proc/stat", "r");
    if(stat == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    while(getline(&line, &n, stat) > 0)
    {
        if(strncmp(line, "cpu", 3) == 0)
        {
            numCpuCores++;
        }
    }
    if(line != NULL)
    {
        free(line);
    }
    fclose(stat);

    numCpuCores -= 1; //Remove "overall" line

    return 0;
}

void initCPUCoresPanel(Panel* panel)
{
    initPanelBase(panel, CPU_CORE_PANEL_HEIGHT, CPU_CORE_PANEL_WIDTH);
    panel->update = &updateCPUCoreValues;
    panel->draw = &drawCPUCorePanel;

    readCPUCoreCount();
    cpuCoreUsages = malloc(numCpuCores * sizeof(CPUUsage));

    //Do one read to make sure the first actual read has a valid previous value
    //TODO
}