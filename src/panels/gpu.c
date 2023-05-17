#include "gpu.h"

#include <nvml.h>
#include <string.h>
#include <stdlib.h>
#include "../display.h"

#define GPU_PANEL_HEIGHT 10
#define GPU_PANEL_WIDTH 69

static nvmlDevice_t device;

typedef struct {
    char name[GPU_PANEL_WIDTH - 1];
    float usagePercent;
    float memPercent;
    float temperature;
} GPUStatus;

static GPUStatus gpu;
#define HISTORY_SIZE 28
static uint8_t gpuUsageHistory[HISTORY_SIZE];
static uint8_t gpuMemoryHistory[HISTORY_SIZE];

uint8_t initGPU(void)
{
    if(nvmlInit() != NVML_SUCCESS)
    {
        return 1;
    }
    if(nvmlDeviceGetHandleByIndex(0, &device) != NVML_SUCCESS)
    {
        return 2;
    }
    return 0;
}

void quitGPU(void)
{
    nvmlShutdown();
}

uint8_t readGPUUsage(void)
{
    nvmlUtilization_t util;
    if(nvmlDeviceGetUtilizationRates(device, &util) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUMemoryUsage(void)
{
    nvmlMemory_t memory;
    if(nvmlDeviceGetMemoryInfo(device, &memory) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.memPercent = (((float) memory.used) / memory.total) * 100;
    return 0;
}

uint8_t readGPUTemperature(void)
{
    uint32_t temp;
    if(nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.temperature = temp;
    return 0;
}

uint8_t getGPUName(char* name)
{
    if(nvmlDeviceGetName(device, name, GPU_PANEL_WIDTH - 1) != NVML_SUCCESS)
    {
        return 1;
    }
    return 0;
}

void updateGPUValues(Panel* panel, uint16_t refreshInterval)
{
    readGPUUsage();
    readGPUMemoryUsage();
    readGPUTemperature();

    addEntryToHistory(gpuUsageHistory, HISTORY_SIZE, gpu.usagePercent);
    addEntryToHistory(gpuMemoryHistory, HISTORY_SIZE, gpu.memPercent);
}

void drawGPUPanel(Panel* panel)
{
    drawPanelBase(panel, "GPU");
    char buffer[GPU_PANEL_WIDTH];

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, gpu.name);
    wattrset(panel->window, 0);

    drawTitledBarWithPercentage(panel->window, 2, 1, gpu.usagePercent, "GPU:");
    drawGraphLabels(panel->window, 3, 1, 4, "  0%", "100%");
    drawGraph(panel->window, 3, 6, 4, HISTORY_SIZE, gpuUsageHistory);

    drawTitledBarWithPercentage(panel->window, 2, 35, gpu.memPercent, "MEM:");
    drawGraphLabels(panel->window, 3, 35, 4, "  0%", "100%");
    drawGraph(panel->window, 3, 40, 4, HISTORY_SIZE, gpuMemoryHistory);

    sprintf(buffer, "Temp: %4.1f °C", gpu.temperature);
    mvwaddstr(panel->window, 8, 1, buffer);
}

uint8_t initGPUPanel(Panel* panel)
{
    if(initGPU())
    {
        return 1;
    }

    initPanelBase(panel, GPU_PANEL_HEIGHT, GPU_PANEL_WIDTH);

    panel->update = &updateGPUValues;
    panel->draw = &drawGPUPanel;

    if(getGPUName(gpu.name))
    {
        strcpy(gpu.name, "CANNOT DETECT");
    }
    return 0;
}