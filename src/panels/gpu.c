#include "gpu.h"

#include <dlfcn.h>
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

void* libnvidiaml;
//Functions from libnvidiaml
nvmlReturn_t DECLDIR (*dnvmlInit)(void);
nvmlReturn_t DECLDIR (*dnvmlDeviceGetHandleByIndex_v2)(unsigned int index, nvmlDevice_t *device);
nvmlReturn_t DECLDIR (*dnvmlShutdown)(void);
nvmlReturn_t DECLDIR (*dnvmlDeviceGetUtilizationRates)(nvmlDevice_t device, nvmlUtilization_t *utilization);
nvmlReturn_t DECLDIR (*dnvmlDeviceGetMemoryInfo)(nvmlDevice_t device, nvmlMemory_t *memory);
nvmlReturn_t DECLDIR (*dnvmlDeviceGetTemperature)(nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int *temp);
nvmlReturn_t DECLDIR (*dnvmlDeviceGetName)(nvmlDevice_t device, char *name, unsigned int length);

uint8_t initGPU(void)
{
    //Load NVIDIA library at runtime
    libnvidiaml = dlopen("libnvidia-ml.so", RTLD_NOW);
    if(dlerror())
    {
        return 1;
    }
    //Load all required functions
    dnvmlInit = dlsym(libnvidiaml, "nvmlInit_v2");
    dnvmlDeviceGetHandleByIndex_v2 = dlsym(libnvidiaml, "nvmlDeviceGetHandleByIndex_v2");
    dnvmlShutdown = dlsym(libnvidiaml, "nvmlShutdown");
    dnvmlDeviceGetUtilizationRates = dlsym(libnvidiaml, "nvmlDeviceGetUtilizationRates");
    dnvmlDeviceGetMemoryInfo = dlsym(libnvidiaml, "nvmlDeviceGetMemoryInfo");
    dnvmlDeviceGetTemperature = dlsym(libnvidiaml, "nvmlDeviceGetTemperature");
    dnvmlDeviceGetName = dlsym(libnvidiaml, "nvmlDeviceGetName");
    //This checks for the most recent error, which is enough for us since we only care if an error happened, not what
    if(dlerror())
    {
        return 1;
    }

    if(dnvmlInit() != NVML_SUCCESS)
    {
        return 2;
    }
    if(dnvmlDeviceGetHandleByIndex_v2(0, &device) != NVML_SUCCESS)
    {
        return 3;
    }

    return 0;
}

void quitGPU(void)
{
    dnvmlShutdown();
    dlclose(libnvidiaml);
}

uint8_t readGPUUsage(void)
{
    nvmlUtilization_t util;
    if(dnvmlDeviceGetUtilizationRates(device, &util) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUMemoryUsage(void)
{
    nvmlMemory_t memory;
    if(dnvmlDeviceGetMemoryInfo(device, &memory) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.memPercent = (((float) memory.used) / memory.total) * 100;
    return 0;
}

uint8_t readGPUTemperature(void)
{
    uint32_t temp;
    if(dnvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu.temperature = temp;
    return 0;
}

uint8_t getGPUName(char* name)
{
    if(dnvmlDeviceGetName(device, name, GPU_PANEL_WIDTH - 1) != NVML_SUCCESS)
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

    uint8_t newValue = gpu.usagePercent;
    addEntryToHistory(gpuUsageHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
    newValue = gpu.memPercent;
    addEntryToHistory(gpuMemoryHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
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

/**
 * GPU panel failure display
 * This is shown when the GPU init failed
 **/
void drawGPUPanelFail(Panel* panel)
{
    drawPanelBase(panel, "GPU");

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

    initPanelBase(panel, GPU_PANEL_HEIGHT, GPU_PANEL_WIDTH);

    panel->update = &updateGPUValues;
    panel->draw = &drawGPUPanel;

    if(getGPUName(gpu.name))
    {
        strcpy(gpu.name, "CANNOT DETECT");
    }
    return 0;
}