#include "gpu.h"

#include <nvml.h>
#include <string.h>

static nvmlDevice_t device;

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

uint8_t readGPUUsage(GPUStatus* gpu)
{
    nvmlUtilization_t util;
    if(nvmlDeviceGetUtilizationRates(device, &util) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu->usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUTemperature(GPUStatus* gpu)
{
    uint32_t temp;
    if(nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS)
    {
        return 1;
    }
    gpu->temperature = temp;
    return 0;
}

uint8_t getGPUName(GPUStatus* gpu)
{
    if(nvmlDeviceGetName(device, gpu->name, GPU_NAME_LENGTH) != NVML_SUCCESS)
    {
        return 1;
    }
    return 0;
}