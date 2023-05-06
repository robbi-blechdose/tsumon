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

uint8_t saveConfig(Configuration* config)
{
    char* configPath = getFullConfigPath(CONFIG_NAME);
    FILE* configFile = fopen(configPath, "wb");
    free(configPath);
    if(configFile == NULL)
    {
        return 1;
    }

    if(fwrite(config, sizeof(Configuration), 1, configFile) != 1)
    {
        return 2;
    }

    fclose(configFile);
    return 0;
}

uint8_t loadConfig(Configuration* config)
{
    char* configPath = getFullConfigPath(CONFIG_NAME);
    FILE* configFile = fopen(configPath, "rb");
    free(configPath);
    if(configFile == NULL)
    {
        return 1;
    }

    if(fread(config, sizeof(Configuration), 1, configFile) != 1)
    {
        return 2;
    }
    
    fclose(configFile);
    return 0;
}

void setInitialConfig(Configuration* config)
{
    config->refreshIntervalIndex = 1;
    config->widthLimit = 0;
}