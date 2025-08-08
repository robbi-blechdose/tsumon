#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include "../panel.h"

typedef struct {
    uint64_t totalLast;
    uint64_t idleLast;
    float usagePercent;
} CPUUsage;

uint8_t readCPUUsage(char* name, CPUUsage* usage);

void initCPUPanel(Panel* panel);

#endif