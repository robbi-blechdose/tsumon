#include "cpuvalues.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

//TODO: maybe switch to finding line with "cpu%d" with the highest number?
uint8_t readCPUCoreCount(uint16_t* numCpuCores)
{
    *numCpuCores = 0;

    FILE* stat = fopen("/proc/stat", "r");
    if(stat == NULL)
    {
        return 1;
    }

    char* line = NULL;
    size_t n;
    while(getline(&line, &n, stat) > 0)
    {
        if(strncmp(line, "cpu", 3) == 0)
        {
            (*numCpuCores)++;
        }
    }
    if(line != NULL)
    {
        free(line);
    }
    fclose(stat);

    *numCpuCores -= 1; //Remove "overall" line
    return 0;
}

uint8_t readCPUName(char* buffer, uint8_t bufferLength)
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
            strncpy(buffer, start + 2, bufferLength - 1);
            buffer[strcspn(buffer, "\n")] = '\0'; //Replace newline by null terminator
            buffer[bufferLength - 1] = '\0'; //Add null terminator
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

uint8_t readCPUUsage(int16_t index, CPUUsage* usage)
{
    //Read new values from /proc/stat
    FILE* stat = fopen("/proc/stat", "r");
    if(stat == NULL)
    {
        return 1;
    }

    char pattern[64];
    if(index == -1)
    {
        strcpy(pattern, "cpu");
    }
    else
    {
        sprintf(pattern, "cpu%d", index);
    }
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

uint8_t readCPUTemperature(float* temperature)
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

    *temperature = ((float) temp) / 1000;
    return 0;
}



