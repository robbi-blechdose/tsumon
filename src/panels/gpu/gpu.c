#include "gpu.h"

#include <string.h>
#include <stdlib.h>

#include "gpuvalues.h"

#include "../../display.h"
#include "../../utils/history.h"

#define GPU_PANEL_HEIGHT 9
#define GPU_PANEL_WIDTH 70

typedef struct {
    char name[GPU_PANEL_WIDTH - 1];
    float usagePercent;
    float memPercent;
    float temperature;
} GPUStatus;

static GPUStatus gpu;
#define HISTORY_SIZE 27
static uint8_t gpuUsageHistory[HISTORY_SIZE];
static uint8_t gpuMemoryHistory[HISTORY_SIZE];

void updateGPUValues(Panel* panel, uint16_t refreshInterval)
{
    readGPUUsage(&gpu.usagePercent);
    readGPUMemoryUsage(&gpu.memPercent);
    readGPUTemperature(&gpu.temperature);

    uint8_t newValue = gpu.usagePercent;
    addEntryToHistory(gpuUsageHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
    newValue = gpu.memPercent;
    addEntryToHistory(gpuMemoryHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
}

void drawGPUPanel(Panel* panel)
{
    drawPanelBorder(panel, "GPU");
    char buffer[GPU_PANEL_WIDTH];

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, gpu.name);
    wattrset(panel->window, 0);

    drawTitledBarWithPercentage(panel->window, 2, 1, gpu.usagePercent, "GPU:");
    drawGraphWithLabels(panel->window, 3, 1, 4, HISTORY_SIZE, gpuUsageHistory, "  0%", "100%");

    drawTitledBarWithPercentage(panel->window, 2, 36, gpu.memPercent, "MEM:");
    drawGraphWithLabels(panel->window, 3, 36, 4, HISTORY_SIZE, gpuMemoryHistory, "  0%", "100%");

    sprintf(buffer, "Temp: %4.1f °C", gpu.temperature);
    mvwaddstr(panel->window, 7, 1, buffer);
}

/**
 * GPU panel failure display
 * This is shown when the GPU init failed
 **/
void drawGPUPanelFail(Panel* panel)
{
    drawPanelBorder(panel, "GPU");

    wcolor_set(panel->window, C_RedBlack, 0);
    mvwaddstr(panel->window, 4, 1, "GPU init failed - is libnvidiaml installed?");
    wcolor_set(panel->window, C_WhiteBlack, 0);
}

uint8_t initGPUPanel(Panel* panel)
{
    if(initGPU())
    {
        panel->draw = &drawGPUPanelFail;
        return 1;
    }

    panelInit(panel, GPU_PANEL_HEIGHT, GPU_PANEL_WIDTH);

    panel->update = &updateGPUValues;
    panel->draw = &drawGPUPanel;

    if(getGPUName(gpu.name, GPU_PANEL_WIDTH - 1))
    {
        strcpy(gpu.name, "CANNOT DETECT");
    }
    return 0;
}