#include "cpu.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../display.h"

#define CPU_PANEL_HEIGHT 9
#define CPU_PANEL_WIDTH 39

typedef struct {
    char name[CPU_PANEL_WIDTH - 1];
    CPUUsage usage;
    float temperature;
} CPUStatus;

static CPUStatus cpu;
#define HISTORY_SIZE 32
static uint8_t cpuUsageHistory[HISTORY_SIZE];

uint8_t readCPUUsage(char* name, CPUUsage* usage)
{
    //Read new values from /proc/stat
    FILE* stat = fopen("/proc/stat", "r");
    if(stat == NULL)
    {
        return 1;
    }

    char pattern[64];
    strcpy(pattern, name);
    strcat(pattern, " %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu");

    uint64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    char* line = NULL;
    size_t n;
    bool found = false;
    while(getline(&line, &n, stat) > 0)
    {
        if(sscanf(line, pattern, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice) == 10)
        {
            found = true;
            break;
        }
    }
    if(line != NULL)
    {
        free(line);
    }
    fclose(stat);
    if(!found)
    {
        return 2;
    }

    //Calculate total CPU time
    uint64_t cpuTotal = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    uint64_t cpuIdle = idle + iowait;
    //Calculate delta times
    float totalDelta = cpuTotal - usage->totalLast;
    float idleDelta = cpuIdle - usage->idleLast;
    //Calculate CPU usage
    const uint64_t perSecond = (1000 / 10 / sysconf(_SC_CLK_TCK));
    usage->usagePercent = (1.0f - (idleDelta / totalDelta)) * 100 * perSecond;

    //Swap values
    usage->totalLast = cpuTotal;
    usage->idleLast = cpuIdle;

    return 0;
}

uint8_t readCPUTemperature(void)
{
    FILE* hwmon1 = fopen("/sys/class/hwmon/hwmon1/temp1_input", "r");
    if(hwmon1 == NULL)
    {
        return 1;
    }

    uint32_t temp;
    if(fscanf(hwmon1, "%d", &temp) != 1)
    {
        fclose(hwmon1);
        return 2;
    }
    fclose(hwmon1);

    cpu.temperature = ((float) temp) / 1000;
    return 0;
}

uint8_t getCPUName(void)
{
    FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
    if(cpuinfo == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    while(getline(&line, &n, cpuinfo) > 0)
    {
        if(strstr(line, "model name"))
        {
            char* start = strstr(line, ":");
            strncpy(cpu.name, start + 2, CPU_PANEL_WIDTH - 3);
            cpu.name[strcspn(cpu.name, "\n")] = '\0'; //Replace newline by null terminator
            cpu.name[CPU_PANEL_WIDTH - 2] = '\0'; //Add null terminator
            free(line);
            fclose(cpuinfo);
            return 0;
        }
    }
    if(line != NULL)
    {
        free(line);
    }
    fclose(cpuinfo);
    return 2;
}

void updateCPUValues(Panel* panel, uint16_t refreshInterval)
{
    readCPUUsage("cpu", &cpu.usage);
    readCPUTemperature();
    uint8_t newValue = cpu.usage.usagePercent;
    addEntryToHistory(cpuUsageHistory, HISTORY_SIZE, &newValue, sizeof(uint8_t));
}

void drawCPUPanel(Panel* panel)
{
    drawPanelBase(panel, "CPU");

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, cpu.name);
    wattrset(panel->window, 0);

    drawTitledBarWithPercentage(panel->window, 2, 1, cpu.usage.usagePercent, "AVG:");
    drawGraphLabels(panel->window, 3, 1, 4, "  0%", "100%");
    drawGraph(panel->window, 3, 6, 4, HISTORY_SIZE, cpuUsageHistory);

    char buffer[PANEL_WIDTH];
    sprintf(buffer, "Temp: %4.1f °C", cpu.temperature);
    mvwaddstr(panel->window, 7, 1, buffer);
}

void initCPUPanel(Panel* panel)
{
    initPanelBase(panel, CPU_PANEL_HEIGHT, CPU_PANEL_WIDTH);
    panel->update = &updateCPUValues;
    panel->draw = &drawCPUPanel;

    if(getCPUName())
    {
        strcpy(cpu.name, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readCPUUsage("cpu", &cpu.usage);
}