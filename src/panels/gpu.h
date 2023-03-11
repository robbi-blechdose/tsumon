#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include "../panel.h"

uint8_t initGPU(void);
void quitGPU(void);

uint8_t readGPUUsage(Panel* panel);
uint8_t readGPUMemoryUsage(Panel* panel);
uint8_t readGPUTemperature(Panel* panel);

uint8_t initGPUPanel(Panel* panel);
void drawGPUPanelContents(Panel* panel);

#endif