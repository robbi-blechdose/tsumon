#ifndef FETCH_H
#define FETCH_H

#include <stdint.h>

#include "../panel.h"

uint8_t initFetchPanel(Panel* panel);
void drawFetchPanelContents(Panel* panel);

#endif