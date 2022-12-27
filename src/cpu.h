#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

#define READ_INTERVAL_MS 1000

#define CPU_NAME_LENGTH 29

typedef struct {
    char name[CPU_NAME_LENGTH];
    float usagePercent;
    float temperature;
} CPUStatus;

uint8_t readCPUUsage(CPUStatus* cpu);
uint8_t readCPUTemperature(CPUStatus* cpu);
uint8_t getCPUName(CPUStatus* cpu);

#endif