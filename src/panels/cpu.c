#include "cpu.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../display.h"

typedef struct {
    char name[PANEL_WIDTH - 1];
    uint64_t totalLast;
    uint64_t idleLast;
    float usagePercent;
    float temperature;
} CPUStatus;

uint8_t readCPUUsage(CPUStatus* cpu)
{
    //Read new values from /proc/stat
    FILE* stat = fopen("/proc/stat", "r");
    if(stat == NULL)
    {
        return 1;
    }
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    if(fscanf(stat, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice) != 10)
    {
        fclose(stat);
        return 2;
    }
    fclose(stat);

    //Calculate total CPU time
    uint64_t cpuTotal = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    uint64_t cpuIdle = idle + iowait;
    //Calculate delta times
    float totalDelta = cpuTotal - cpu->totalLast;
    float idleDelta = cpuIdle - cpu->idleLast;
    //Calculate CPU usage
    const uint64_t perSecond = (1000 / 10 / sysconf(_SC_CLK_TCK));
    cpu->usagePercent = (1.0f - (idleDelta / totalDelta)) * 100 * perSecond;

    //Swap values
    cpu->totalLast = cpuTotal;
    cpu->idleLast = cpuIdle;

    return 0;
}

uint8_t readCPUTemperature(CPUStatus* cpu)
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

    cpu->temperature = ((float) temp) / 1000;
    return 0;
}

uint8_t getCPUName(char* name)
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
            strncpy(name, start + 2, PANEL_WIDTH - 1);
            name[PANEL_WIDTH - 2] = '\0'; //Add null terminator
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

void initCPUPanel(Panel* panel)
{
    panel->window = newwin(PANEL_HEIGHT, PANEL_WIDTH, 0, 0);
    panel->data = malloc(sizeof(CPUStatus));
    CPUStatus* cpu = (CPUStatus*) panel->data;
    if(getCPUName(cpu->name))
    {
        strcpy(cpu->name, "CANNOT DETECT");
    }
    //Do one read to make sure the first actual read has a valid previous value
    readCPUUsage((CPUStatus*) panel->data);
}

void drawCPUPanelContents(Panel* panel)
{
    char buffer[PANEL_WIDTH];
    CPUStatus* cpu = (CPUStatus*) panel->data;

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, cpu->name);
    wattrset(panel->window, 0);
    drawBarWithPercentage(panel->window, 2, 1, cpu->usagePercent);
    sprintf(buffer, "Temp: %4.1f °C", cpu->temperature);
    mvwaddstr(panel->window, 3, 1, buffer);
}

void updateCPUPanel(Panel* panel)
{
    CPUStatus* cpu = (CPUStatus*) panel->data;
    readCPUUsage(cpu);
    readCPUTemperature(cpu);
    drawCPUPanelContents(panel);
}