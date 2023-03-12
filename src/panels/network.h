#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include "../panel.h"

void initNetworkPanel(Panel* panel);
void drawNetworkPanelContents(Panel* panel);

void drawNetworkPanelSettings(WINDOW* win, Panel* panel);
void updateNetworkPanel(Panel* panel, uint16_t refreshInterval);
void moveNetworkPanelSettingsCursor(Panel* panel, bool up);

#endif