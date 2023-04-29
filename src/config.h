#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "panel.h"

//Possible refresh intervals in milliseconds
#define NUM_REFRESH_INTERVALS 4
extern const uint16_t refreshIntervals[NUM_REFRESH_INTERVALS];

//General configuration
#define CONFIG_NAME "tsumon.conf"

typedef struct {
    int8_t refreshIntervalIndex;
    int8_t widthLimit;
} GeneralConfiguration;

uint8_t saveConfig(GeneralConfiguration* config);
uint8_t loadConfig(GeneralConfiguration* config);

void setInitialConfig(GeneralConfiguration* config);

//Panel configuration (panels and respective settings, if any)
#define PANEL_CONFIG_NAME "tsumon_panels.conf"

typedef struct {
    uint8_t numPanels;
    PanelType* panelTypes;
} PanelConfiguration;

uint8_t savePanelConfig(PanelConfiguration* pconfig);
uint8_t loadPanelConfig(PanelConfiguration* pconfig);

void setInitialPanelConfig(PanelConfiguration* pconfig);

void addPanel(PanelConfiguration* pconfig, PanelType toAdd, int8_t position);
void removePanel(PanelConfiguration* pconfig, int8_t index);

typedef struct {
    GeneralConfiguration general;
    PanelConfiguration panels;
} Configuration;

#endif