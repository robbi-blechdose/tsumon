#ifndef _GPU_H
#define _GPU_H

#include <stdint.h>

uint8_t initGPU();
void quitGPU();

float readGPUUsage();
float getGPUTemperature();
uint8_t getGPUName(char* name, uint8_t length);

#endif