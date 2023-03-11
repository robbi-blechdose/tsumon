#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include <ncurses.h>
#include "../panel.h"

void initCPUPanel(Panel* panel);
void updateCPUPanel(Panel* panel);

#endif