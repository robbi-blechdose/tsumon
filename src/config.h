#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

//Possible refresh intervals in milliseconds
#define NUM_REFRESH_INTERVALS 4
extern const uint16_t refreshIntervals[NUM_REFRESH_INTERVALS];

//General configuration
#define CONFIG_NAME "tsumon.conf"

typedef struct {
    int8_t refreshIntervalIndex;
    int8_t widthLimit;
    int8_t highlightColor;
} Configuration;

extern Configuration config;

uint8_t saveConfig(void);
uint8_t loadConfig(void);

void setInitialConfig(void);

#endif