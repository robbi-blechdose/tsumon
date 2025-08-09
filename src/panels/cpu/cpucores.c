#include "cpucores.h"

#include <stdlib.h>
#include <string.h>

#include "cpuvalues.h"
#include "../../display.h"

static uint16_t numCpuCores;
static uint8_t numHorizontalCpuCores;
static CPUUsage* cpuCoreUsages;

//2 chars index, 1 char meter
#define CORE_WIDTH 3

void drawCPUCorePanel(Panel* panel)
{
    drawPanelBorder(panel, "Cores");

    //TODO: don't hardcode with & height
    for(uint8_t i = 0; i < numHorizontalCpuCores; i++)
    {
        for(uint8_t j = 0; j < (numCpuCores / numHorizontalCpuCores); j++)
        {
            uint8_t coreIndex = i + (j * numHorizontalCpuCores);
            if(coreIndex >= numCpuCores)
            {
                break;
            }

            char buffer[8];
            sprintf(buffer, "%02d", coreIndex);
            mvwaddstr(panel->window, 1 + j, 1 + i * CORE_WIDTH, buffer);
            drawPercentageBlock(panel->window, 1 + j, 1 + i * CORE_WIDTH + 2, cpuCoreUsages[coreIndex].usagePercent);
        }
    }
}

void updateCPUCoreValues(Panel* panel, uint16_t refreshInterval)
{
    for(uint8_t i = 0; i < numHorizontalCpuCores; i++)
    {
        for(uint8_t j = 0; j < (numCpuCores / numHorizontalCpuCores); j++)
        {
            uint8_t coreIndex = i + (j * numHorizontalCpuCores);
            if(coreIndex >= numCpuCores)
            {
                break;
            }

            readCPUUsage(coreIndex, &cpuCoreUsages[coreIndex]);
        }
    }
}

uint8_t calculatePanelWidthFromCoreCount()
{
    if(numCpuCores <= 16)
    {
        numHorizontalCpuCores = 4;
    }
    else if(numCpuCores <= 64)
    {
        numHorizontalCpuCores = 8;
    }
    else if(numCpuCores <= 144)
    {
        numHorizontalCpuCores = 12;
    }
    else if(numCpuCores <= 256)
    {
        numHorizontalCpuCores = 16;
    }
    else
    {
        numHorizontalCpuCores = 32;
    }

    return 2 + (numHorizontalCpuCores * CORE_WIDTH);
}

uint8_t calculatePanelHeightFromWidth()
{
    return 2 + (numCpuCores / numHorizontalCpuCores);
}

Panel* createCPUCoresPanel(Panel* cpuPanel)
{
    readCPUCoreCount(&numCpuCores);
    cpuCoreUsages = malloc(numCpuCores * sizeof(CPUUsage));

    uint8_t width = calculatePanelWidthFromCoreCount();
    Panel* panel = panelCreate(cpuPanel, 2, 31, calculatePanelHeightFromWidth(), width);
    panel->update = &updateCPUCoreValues;
    panel->draw = &drawCPUCorePanel;

    //Do one read to make sure the first actual read has a valid previous value
    updateCPUCoreValues(panel, 0);

    return panel;
}