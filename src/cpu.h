#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

#define READ_INTERVAL_MS 1000

float readCPUUsage();
float getCPUTemperature();
uint8_t getCPUName(char* name, uint8_t length);

#endif