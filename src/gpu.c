#include "gpu.h"

#include <nvml.h>
#include <string.h>

static nvmlDevice_t device;

uint8_t initGPU()
{
    nvmlReturn_t result = nvmlInit();
    if(result != NVML_SUCCESS)
    {
        return 1;
    }
    result = nvmlDeviceGetHandleByIndex(0, &device);
    if(result != NVML_SUCCESS)
    {
        return 2;
    }
    return 0;
}

void quitGPU()
{
    nvmlShutdown();
}

float readGPUUsage()
{
    nvmlUtilization_t util;
    nvmlReturn_t result = nvmlDeviceGetUtilizationRates(device, &util);
    if(result != NVML_SUCCESS)
    {
        return -1;
    }
    return util.gpu;
}

float getGPUTemperature()
{
    uint32_t temp;
    nvmlReturn_t result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
    if(result != NVML_SUCCESS)
    {
        return -1;
    }
    return temp;
}

uint8_t getGPUName(char* name, uint8_t length)
{
    nvmlReturn_t result = nvmlDeviceGetName(device, name, length);
    if(result != NVML_SUCCESS)
    {
        return 1;
    }
    return 0;
}