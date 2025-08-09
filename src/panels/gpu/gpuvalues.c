#include "gpuvalues.h"

#include <dlfcn.h>
#include <nvml.h>
#include <string.h>
#include <stdlib.h>

static nvmlDevice_t device;

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

uint8_t readGPUUsage(float* usagePercent)
{
    nvmlUtilization_t util;
    if(dnvmlDeviceGetUtilizationRates(device, &util) != NVML_SUCCESS)
    {
        return 1;
    }
    *usagePercent = util.gpu;
    return 0;
}

uint8_t readGPUTemperature(float* temperature)
{
    uint32_t temp;
    if(dnvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp) != NVML_SUCCESS)
    {
        return 1;
    }
    *temperature = temp;
    return 0;
}

uint8_t readGPUMemoryUsage(float* memPercent)
{
    nvmlMemory_t memory;
    if(dnvmlDeviceGetMemoryInfo(device, &memory) != NVML_SUCCESS)
    {
        return 1;
    }
    *memPercent = (((float) memory.used) / memory.total) * 100;
    return 0;
}

uint8_t getGPUName(char* buffer, uint8_t bufferLength)
{
    if(dnvmlDeviceGetName(device, buffer, bufferLength) != NVML_SUCCESS)
    {
        return 1;
    }
    return 0;
}
