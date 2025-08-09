#include "cpu.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "cpuvalues.h"
#include "cpupackage.h"
#include "cpucores.h"

#include "../../display.h"

static char cpuName[64];

void drawCPUPanel(Panel* panel)
{
    drawPanelBorder(panel, "CPU");

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, cpuName);
    wattrset(panel->window, 0);

    panelDrawChildren(panel);
}

#define CPU_PANEL_HEIGHT 12
#define CPU_PANEL_WIDTH 46

void initCPUPanel(Panel* panel)
{
    panelInit(panel, CPU_PANEL_HEIGHT, CPU_PANEL_WIDTH);
    panel->update = &panelUpdateChildren;
    panel->draw = &drawCPUPanel;

    if(readCPUName(cpuName, CPU_PANEL_WIDTH - 1))
    {
        strcpy(cpuName, "CANNOT DETECT");
    }

    //Add package and core windows
    panelAddChild(panel, createCPUPackagePanel(panel));
    panelAddChild(panel, createCPUCoresPanel(panel));
}