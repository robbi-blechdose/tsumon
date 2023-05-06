#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include "../panel.h"

uint8_t initGPU(void);
void quitGPU(void);

uint8_t initGPUPanel(Panel* panel);

#endif