#ifndef GPU_VALUES_H
#define GPU_VALUES_H

#include <stdint.h>

uint8_t initGPU(void);
void quitGPU(void);

uint8_t readGPUUsage(float* usagePercent);
uint8_t readGPUTemperature(float* temperature);
uint8_t readGPUMemoryUsage(float* memPercent);

uint8_t getGPUName(char* buffer, uint8_t bufferLength);

#endif