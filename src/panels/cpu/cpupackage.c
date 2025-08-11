#include "cpupackage.h"

#include "cpuvalues.h"

#include "../../display.h"
#include "../../utils/history.h"

#define CPU_PANEL_HEIGHT 9
#define CPU_PANEL_WIDTH 30

typedef struct {
    CPUUsage usage;
    float temperature;
    float frequency;
    uint16_t numCores;
} CPUStatus;

static CPUStatus cpu;
#define HISTORY_SIZE 22
static uint8_t cpuUsageHistory[HISTORY_SIZE];

void updateCPUValues(Panel* panel, uint16_t refreshInterval)
{
    readCPUUsage(CPU_INDEX_PACKAGE, &cpu.usage);
    readCPUTemperature(&cpu.temperature);
    readCPUFrequency(&cpu.frequency, cpu.numCores, CPUFM_AVERAGE);
    uint8_t newValue = cpu.usage.usagePercent;
    addEntryToHistory(cpuUsageHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
}

void drawCPUPackagePanel(Panel* panel)
{
    drawPanelBorder(panel, "Package");

    char buffer[PANEL_WIDTH];
    sprintf(buffer, "Freq: %4.1f GHz", cpu.frequency); //TODO: read frequency
    mvwaddstr(panel->window, 1, 1, buffer);
    
    sprintf(buffer, "Temp: %4.1f °C", cpu.temperature);
    mvwaddstr(panel->window, 2, 1, buffer);

    drawBarWithPercentage(panel->window, 3, 1, cpu.usage.usagePercent);
    drawGraphWithLabels(panel->window, 4, 1, 4, HISTORY_SIZE, cpuUsageHistory, "  0%", "100%");
}

Panel* createCPUPackagePanel(Panel* cpuPanel)
{
    Panel* panel = panelCreate(cpuPanel, 2, 1, CPU_PANEL_HEIGHT, CPU_PANEL_WIDTH);
    panel->update = &updateCPUValues;
    panel->draw = &drawCPUPackagePanel;

    readCPUCoreCount(&cpu.numCores);
    //Do one read to make sure the first actual read has a valid previous value
    readCPUUsage(CPU_INDEX_PACKAGE, &cpu.usage);

    return panel;
}