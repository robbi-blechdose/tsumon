#include "config.h"

#include <stdlib.h>
#include <string.h>

const uint16_t refreshIntervals[NUM_REFRESH_INTERVALS] = {
     250,
     500,
    1000,
    2000
};

char* getFullConfigPath(const char* name)
{
    size_t length = strlen(getenv("HOME")) + strlen("/.config/") + strlen(name) + 1;
    char* configPath = malloc(length);
    strcpy(configPath, getenv("HOME"));
    strcat(configPath, "/.config/");
    strcat(configPath, name);
    return configPath;
}

uint8_t saveConfig(GeneralConfiguration* config)
{
    char* configPath = getFullConfigPath(CONFIG_NAME);
    FILE* configFile = fopen(configPath, "wb");
    free(configPath);
    if(configFile == NULL)
    {
        return 1;
    }

    if(fwrite(config, sizeof(GeneralConfiguration), 1, configFile) != 1)
    {
        return 2;
    }

    fclose(configFile);
    return 0;
}

uint8_t loadConfig(GeneralConfiguration* config)
{
    char* configPath = getFullConfigPath(CONFIG_NAME);
    FILE* configFile = fopen(configPath, "rb");
    free(configPath);
    if(configFile == NULL)
    {
        return 1;
    }

    if(fread(config, sizeof(GeneralConfiguration), 1, configFile) != 1)
    {
        return 2;
    }
    
    fclose(configFile);
    return 0;
}

void setInitialConfig(GeneralConfiguration* config)
{
    config->refreshIntervalIndex = 1;
    config->widthLimit = 0;
}

uint8_t savePanelConfig(PanelConfiguration* pconfig)
{
    char* configPath = getFullConfigPath(PANEL_CONFIG_NAME);
    FILE* configFile = fopen(configPath, "wb");
    free(configPath);
    if(configFile == NULL)
    {
        return 1;
    }

    if(fwrite(&pconfig->numPanels, sizeof(uint8_t), 1, configFile) != 1)
    {
        fclose(configFile);
        return 2;
    }

    if(fwrite(pconfig->panelTypes, sizeof(PanelType), pconfig->numPanels, configFile) != pconfig->numPanels)
    {
        fclose(configFile);
        return 3;
    }

    fclose(configFile);
    return 0;
}

uint8_t loadPanelConfig(PanelConfiguration* pconfig)
{
    char* configPath = getFullConfigPath(PANEL_CONFIG_NAME);
    FILE* configFile = fopen(configPath, "rb");
    free(configPath);
    if(configFile == NULL)
    {
        return 1;
    }
    
    if(fread(&pconfig->numPanels, sizeof(uint8_t), 1, configFile) != 1)
    {
        fclose(configFile);
        return 2;
    }

    pconfig->panelTypes = malloc(pconfig->numPanels);
    if(fread(pconfig->panelTypes, sizeof(PanelType), pconfig->numPanels, configFile) != pconfig->numPanels)
    {
        fclose(configFile);
        return 3;
    }
    
    fclose(configFile);
    return 0;
}

void setInitialPanelConfig(PanelConfiguration* pconfig)
{
    pconfig->numPanels = 5;
    pconfig->panelTypes = malloc(sizeof(PanelType) * pconfig->numPanels);
    pconfig->panelTypes[0] = P_CPU;
    pconfig->panelTypes[1] = P_RAM;
    pconfig->panelTypes[2] = P_GPU;
    pconfig->panelTypes[3] = P_NETWORK;
    pconfig->panelTypes[4] = P_FETCH;
}

void addPanel(PanelConfiguration* pconfig, PanelType toAdd, int8_t position)
{
    PanelType* newPanelTypes = malloc(sizeof(PanelType) * (pconfig->numPanels + 1));
    uint8_t oi = 0;
    for(uint8_t i = 0; i < pconfig->numPanels + 1; i++)
    {
        if(i == position)
        {
            newPanelTypes[i] = toAdd;
        }
        else
        {
            newPanelTypes[i] = pconfig->panelTypes[oi++];
        }
    }

    pconfig->numPanels++;
    free(pconfig->panelTypes);
    pconfig->panelTypes = newPanelTypes;
}

void removePanel(PanelConfiguration* pconfig, int8_t toRemove)
{
    if(toRemove >= pconfig->numPanels)
    {
        return;
    }

    //Remove panel, shift rest back (if necessary)
    PanelType* newPanelTypes = malloc(sizeof(PanelType) * (pconfig->numPanels - 1));
    uint8_t ni = 0;
    for(uint8_t i = 0; i < pconfig->numPanels; i++)
    {
        if(i != toRemove)
        {
            newPanelTypes[ni++] = pconfig->panelTypes[i];
        }
    }
    pconfig->numPanels--;
    free(pconfig->panelTypes);
    pconfig->panelTypes = newPanelTypes;
}