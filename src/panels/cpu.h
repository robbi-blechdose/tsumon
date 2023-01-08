#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include <ncurses.h>
#include "../panel.h"

#define READ_INTERVAL_MS 1000

uint8_t readCPUUsage(Panel* panel);
uint8_t readCPUTemperature(Panel* panel);

void initCPUPanel(Panel* panel);
void drawCPUPanelContents(Panel* panel);

#endif