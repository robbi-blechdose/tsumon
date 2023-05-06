#include "gpu.h"

#include <nvml.h>
#include <string.h>
#include <stdlib.h>
#include "../display.h"

static nvmlDevice_t device;

typedef struct {
    char name[PANEL_WIDTH - 1];
    float usagePercent;
    float memPercent;
    float temperature;
} GPUStatus;

static GPUStatus gpu;
#define HISTORY_SIZE 28
static uint8_t gpuUsageHistory[HISTORY_SIZE];

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

uint8_t readGPUUsage()
{
    nvmlUtilization_t util;
    if(nvmlDeviceGetUtilizationRates(device, &util) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUMemoryUsage()
{
    nvmlMemory_t memory;
    if(nvmlDeviceGetMemoryInfo(device, &memory) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.memPercent = (((float) memory.used) / memory.total) * 100;
    return 0;
}

uint8_t readGPUTemperature()
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
    if(nvmlDeviceGetName(device, name, PANEL_WIDTH - 1) != NVML_SUCCESS)
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
}

void drawGPUPanel(Panel* panel)
{
    drawTitledWindow(panel->window, "GPU", PANEL_WIDTH);
    char buffer[PANEL_WIDTH];

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, gpu.name);
    wattrset(panel->window, 0);
    drawTitledBarWithPercentage(panel->window, 2, 1, gpu.usagePercent, "GPU:");

    drawGraphLabels(panel->window, 3, 1, 4, "  0%", "100%");
    drawGraph(panel->window, 3, 6, 4, HISTORY_SIZE, gpuUsageHistory);

    drawTitledBarWithPercentage(panel->window, 7, 1, gpu.memPercent, "MEM:");
    sprintf(buffer, "Temp: %4.1f °C", gpu.temperature);
    mvwaddstr(panel->window, 8, 1, buffer);
}

#define GPU_PANEL_HEIGHT 10

uint8_t initGPUPanel(Panel* panel)
{
    if(initGPU())
    {
        return 1;
    }

    panel->window = newwin(GPU_PANEL_HEIGHT, PANEL_WIDTH, 0, 0);
    panel->height = GPU_PANEL_HEIGHT;

    panel->update = &updateGPUValues;
    panel->draw = &drawGPUPanel;

    if(getGPUName(gpu.name))
    {
        strcpy(gpu.name, "CANNOT DETECT");
    }
    return 0;
}