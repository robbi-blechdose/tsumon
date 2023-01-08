#ifndef RAM_H
#define RAM_H

#include <stdint.h>
#include "../panel.h"

uint8_t readRAMUsage(Panel* panel);

void initRAMPanel(Panel* panel);
void drawRAMPanelContents(Panel* panel);

#endif