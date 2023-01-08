#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include "../panel.h"

uint8_t readNetworkUsage(Panel* panel);

void initNetworkPanel(Panel* panel);
void drawNetworkPanelContents(Panel* panel);

#endif