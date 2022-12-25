#include "cpu.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

float readCPUUsage()
{
    static uint64_t totalLast;
    static uint64_t idleLast;
    
    //Read new values from /proc/stat
    FILE* stat = fopen("/proc/stat", "r");
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    fscanf(stat, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
    fclose(stat);

    //Calculate total CPU time
    uint64_t cpuTotal = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    uint64_t cpuIdle = idle + iowait;
    //Calculate delta times
    float totalDelta = cpuTotal - totalLast;
    float idleDelta = cpuIdle - idleLast;
    //Calculate CPU usage
    float usage = (1.0f - (idleDelta / totalDelta)) * (READ_INTERVAL_MS / 10 / sysconf(_SC_CLK_TCK));
    usage *= 100;

    //Swap values
    totalLast = cpuTotal;
    idleLast = cpuIdle;

    return usage;
}

float getCPUTemperature()
{
    FILE* hwmon1 = fopen("/sys/class/hwmon/hwmon1/temp1_input", "r");
    if(hwmon1 == NULL)
    {
        return -1;
    }

    uint32_t temp;
    fscanf(hwmon1, "%d", &temp);
    fclose(hwmon1);

    return ((float) temp) / 1000;
}

uint8_t getCPUName(char* name, uint8_t length)
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
            strncpy(name, start + 2, length);
            fclose(cpuinfo);
            return 0;
        }
    }
    fclose(cpuinfo);
    return 2;
}