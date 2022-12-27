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

uint8_t readGPUUsage(GPUStatus* gpu)
{
    nvmlUtilization_t util;
    nvmlReturn_t result = nvmlDeviceGetUtilizationRates(device, &util);
    if(result != NVML_SUCCESS)
    {
        return 1;
    }
    gpu->usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUTemperature(GPUStatus* gpu)
{
    uint32_t temp;
    nvmlReturn_t result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
    if(result != NVML_SUCCESS)
    {
        return 1;
    }
    gpu->temperature = temp;
    return 0;
}

uint8_t getGPUName(GPUStatus* gpu)
{
    nvmlReturn_t result = nvmlDeviceGetName(device, gpu->name, GPU_NAME_LENGTH);
    if(result != NVML_SUCCESS)
    {
        return 1;
    }
    return 0;
}