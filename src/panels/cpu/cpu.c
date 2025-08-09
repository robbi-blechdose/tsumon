#include "cpu.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "cpuvalues.h"
#include "../../display.h"
#include "../../utils/history.h"

#define CPU_PANEL_HEIGHT 9
#define CPU_PANEL_WIDTH 39

typedef struct {
    char name[CPU_PANEL_WIDTH - 1];
    CPUUsage usage;
    float temperature;
} CPUStatus;

static CPUStatus cpu;
#define HISTORY_SIZE 32
static uint8_t cpuUsageHistory[HISTORY_SIZE];

void updateCPUValues(Panel* panel, uint16_t refreshInterval)
{
    readCPUUsage(CPU_INDEX_PACKAGE, &cpu.usage);
    readCPUTemperature(&cpu.temperature);
    uint8_t newValue = cpu.usage.usagePercent;
    addEntryToHistory(cpuUsageHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
}

void drawCPUPanel(Panel* panel)
{
    drawPanelBorder(panel, "CPU");

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, cpu.name);
    wattrset(panel->window, 0);

    drawTitledBarWithPercentage(panel->window, 2, 1, cpu.usage.usagePercent, "AVG:");
    drawGraphLabels(panel->window, 3, 1, 4, "  0%", "100%");
    drawGraph(panel->window, 3, 6, 4, HISTORY_SIZE, cpuUsageHistory);

    char buffer[PANEL_WIDTH];
    sprintf(buffer, "Temp: %4.1f °C", cpu.temperature);
    mvwaddstr(panel->window, 7, 1, buffer);
}

void initCPUPanel(Panel* panel)
{
    panelInit(panel, CPU_PANEL_HEIGHT, CPU_PANEL_WIDTH);
    panel->update = &updateCPUValues;
    panel->draw = &drawCPUPanel;

    if(readCPUName(cpu.name, CPU_PANEL_WIDTH - 1))
    {
        strcpy(cpu.name, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readCPUUsage(CPU_INDEX_PACKAGE, &cpu.usage);
}