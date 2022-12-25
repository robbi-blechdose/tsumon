/**
 * Small Performance MONitor
 * 
 * Displays CPU, RAM, GPU and disk usage
 * as percentages
 * 
 * @author Robbi Blechdose
 **/

#include "display.h"
#include "cpu.h"
#include "gpu.h"
#include "ram.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define KB_TO_GB(X) ((X) / 1048576.0f)

#define WIN_WIDTH 30

int main(int argc, char* argv[])
{
    ncursesSetup();

    //Set up display windows
    WINDOW* cpuWin = newwin(5, WIN_WIDTH, 1, 1);
    WINDOW* ramWin = newwin(5, WIN_WIDTH, 1, 31);
    WINDOW* gpuWin = newwin(5, WIN_WIDTH, 1, 61);

    //Init subsystems
    if(initGPU())
    {
        //Failure to init GPU
        endwin();
        printf("Could not initialize NVML.\nExiting.\n");
        exit(1);
    }

    //Get CPU name
    #define CPU_NAME_LENGTH 28
    char cpuName[CPU_NAME_LENGTH];
    if(getCPUName(cpuName, CPU_NAME_LENGTH))
    {
        //We keep going as it's not critical
        strcpy(cpuName, "CANNOT DETECT");
    }

    //Get GPU name
    #define GPU_NAME_LENGTH 28
    char gpuName[GPU_NAME_LENGTH];
    if(getGPUName(gpuName, GPU_NAME_LENGTH))
    {
        //We keep going as it's not critical
        strcpy(gpuName, "CANNOT DETECT");
    }

    float cpu;
    float ram;
    float gpu;

    while(true)
    {
        cpu = readCPUUsage();
        ram = readRAMUsage();
        gpu = readGPUUsage();

        //Draw main window
        box(stdscr, 0, 0);
        refresh();

        char buffer[WIN_WIDTH - 1];

        //Draw CPU window
        drawTitledWindow(cpuWin, "CPU", WIN_WIDTH);
        wattrset(cpuWin, A_BOLD);
        mvwaddstr(cpuWin, 1, 1, cpuName);
        wattrset(cpuWin, 0);
        drawBarWithPercentage(cpuWin, 2, 1, cpu);
        sprintf(buffer, "Temp: %4.1f °C", getCPUTemperature());
        mvwaddstr(cpuWin, 3, 1, buffer);
        wrefresh(cpuWin);
        //Draw RAM window
        drawTitledWindow(ramWin, "RAM", WIN_WIDTH);
        sprintf(buffer, "%4.1f GiB", KB_TO_GB(getTotalRAM()));
        wattrset(ramWin, A_BOLD);
        mvwaddstr(ramWin, 1, 1, buffer);
        wattrset(ramWin, 0);
        drawBarWithPercentage(ramWin, 2, 1, ram);
        sprintf(buffer, "Used: %4.1f/%4.1f GiB", KB_TO_GB(getTotalRAM() - getFreeRAM()), KB_TO_GB(getTotalRAM()));
        mvwaddstr(ramWin, 3, 1, buffer);
        wrefresh(ramWin);
        //Draw GPU window
        drawTitledWindow(gpuWin, "GPU", WIN_WIDTH);
        wattrset(gpuWin, A_BOLD);
        mvwaddstr(gpuWin, 1, 1, gpuName);
        wattrset(gpuWin, 0);
        drawBarWithPercentage(gpuWin, 2, 1, gpu);
        sprintf(buffer, "Temp: %4.1f °C", getGPUTemperature());
        mvwaddstr(gpuWin, 3, 1, buffer);
        wrefresh(gpuWin);

        sleep(READ_INTERVAL_MS / 1000);
    }

    //Quit
    quitGPU();
    endwin();

    return 0;
}