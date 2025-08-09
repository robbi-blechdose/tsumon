#include "cpucores.h"

#include <stdlib.h>
#include <string.h>

#include "cpuvalues.h"
#include "../../display.h"


#define CPU_CORE_PANEL_HEIGHT 6
#define CPU_CORE_PANEL_WIDTH 14

static uint16_t numCpuCores;
static CPUUsage* cpuCoreUsages;

void drawCPUCorePanel(Panel* panel)
{
    drawPanelBorder(panel, "CPU Cores");

    //TODO: don't hardcode with & height
    for(uint8_t i = 0; i < 4; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            uint8_t coreIndex = i + (j * 4);
            char buffer[8];
            sprintf(buffer, "%02d", coreIndex);
            mvwaddstr(panel->window, 1 + j, 1 + i * 3, buffer);
            drawPercentageBlock(panel->window, 1 + j, 1 + i * 3 + 2, cpuCoreUsages[coreIndex].usagePercent);
        }
    }
}

void updateCPUCoreValues(Panel* panel, uint16_t refreshInterval)
{
    for(uint8_t i = 0; i < 4; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            uint8_t coreIndex = i + (j * 4);
            readCPUUsage(coreIndex, &cpuCoreUsages[coreIndex]);
        }
    }
}

void initCPUCoresPanel(Panel* panel)
{
    panelInit(panel, CPU_CORE_PANEL_HEIGHT, CPU_CORE_PANEL_WIDTH);
    panel->update = &updateCPUCoreValues;
    panel->draw = &drawCPUCorePanel;

    readCPUCoreCount(&numCpuCores);
    cpuCoreUsages = malloc(numCpuCores * sizeof(CPUUsage));

    //Do one read to make sure the first actual read has a valid previous value
    //TODO
}