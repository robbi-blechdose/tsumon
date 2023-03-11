#include "gpu.h"

#include <nvml.h>
#include <string.h>
#include <stdlib.h>
#include "../display.h"

static nvmlDevice_t device;

typedef struct {
    float usagePercent;
    float memPercent;
    float temperature;
} GPUStatus;

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

uint8_t readGPUUsage(Panel* panel)
{
    nvmlUtilization_t util;
    if(nvmlDeviceGetUtilizationRates(device, &util) != NVML_SUCCESS)
    {
        return 1;
    }
    ((GPUStatus*) panel->data)->usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUMemoryUsage(Panel* panel)
{
    nvmlMemory_t memory;
    if(nvmlDeviceGetMemoryInfo(device, &memory) != NVML_SUCCESS)
    {
        return 1;
    }
    ((GPUStatus*) panel->data)->memPercent = (((float) memory.used) / memory.total) * 100;
    return 0;
}

uint8_t readGPUTemperature(Panel* panel)
{
    uint32_t temp;
    if(nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS)
    {
        return 1;
    }
    ((GPUStatus*) panel->data)->temperature = temp;
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

uint8_t initGPUPanel(Panel* panel)
{
    if(initGPU())
    {
        return 1;
    }

    panel->window = newwin(PANEL_HEIGHT, PANEL_WIDTH, 0, 0);
    panel->data = malloc(sizeof(GPUStatus));
    if(getGPUName(panel->title))
    {
        strcpy(panel->title, "CANNOT DETECT");
    }
    return 0;
}

void drawGPUPanelContents(Panel* panel)
{
    char buffer[PANEL_WIDTH];
    GPUStatus* gpu = (GPUStatus*) panel->data;

    drawTitledBarWithPercentage(panel->window, 2, 1, gpu->usagePercent, "GPU:");
    drawTitledBarWithPercentage(panel->window, 3, 1, gpu->memPercent, "MEM:");
    sprintf(buffer, "Temp: %4.1f °C", gpu->temperature);
    mvwaddstr(panel->window, 4, 1, buffer);
}