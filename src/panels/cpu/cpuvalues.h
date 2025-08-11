#ifndef CPU_VALUES_H
#define CPU_VALUES_H

#include <stdint.h>

uint8_t readCPUCoreCount(uint16_t* numCpuCores);
uint8_t readCPUName(char* buffer, uint8_t bufferLength);

typedef struct {
    uint64_t totalLast;
    uint64_t idleLast;
    float usagePercent;
} CPUUsage;

#define CPU_INDEX_PACKAGE -1
/**
 * Read the usage from the specified CPU into the struct
 * @param index CPU (or core) index to read. Specify CPU_INDEX_PACKAGE for package
 * @param usage Usage struct to write into
 * @return 0 for success
 */
uint8_t readCPUUsage(int16_t index, CPUUsage* usage);

uint8_t readCPUTemperature(float* temperature);

typedef enum {
    CPUFM_HIGHEST,
    CPUFM_AVERAGE
} CPUFrequencyMode;

uint8_t readCPUFrequency(float* frequency, uint16_t numCpuCores, CPUFrequencyMode mode);

#endif