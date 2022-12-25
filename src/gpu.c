#include "gpu.h"

#include <nvml.h>
#include <string.h>

static nvmlDevice_t device;

uint8_t initGPU()
{
    nvmlReturn_t result;
    result = nvmlInit();
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
    nvmlDeviceGetUtilizationRates(device, &util);

    return util.gpu;
}

float getGPUTemperature()
{
    uint32_t temp;
    nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
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