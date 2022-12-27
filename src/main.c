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
#include "network.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define B_TO_KB(X) ((X) / 1024.0f)
#define B_TO_MB(X) ((X) / 1048576.0f)
#define KB_TO_GB(X) ((X) / 1048576.0f)

#define WIN_WIDTH 30

int32_t screenX, screenY;
//Windows
WINDOW* cpuWin;
WINDOW* ramWin;
WINDOW* gpuWin;
WINDOW* netWin;
WINDOW* helpWin;
//Values
CPUStatus cpu;
RAMStatus ram;
GPUStatus gpu;
NetStatus net;

const uint8_t screenLayouts[4][5][2] = {
    //1-column
    {{2, 0},
     {7, 0},
     {12, 0},
     {17, 0},
     {22, 0}},
    //2-column
    {{2, 0}, {2, 30},
     {7, 0}, {7, 30},
     {12, 0}},
    //3-column
    {{2, 0}, {2, 30}, {2, 60},
     {7, 0},
     {12, 0}},
    //4-column
    {{2, 0}, {2, 30}, {2, 60}, {2, 90},
     {7, 0}}
};

void drawWindows(void)
{
    char buffer[WIN_WIDTH - 1];

    //Draw main window
    wattrset(stdscr, A_BOLD);
    mvaddstr(0, screenX / 2 - 2, "spmon");
    refresh();

    //Draw CPU window
    drawTitledWindow(cpuWin, "CPU", WIN_WIDTH);
    wattrset(cpuWin, A_BOLD);
    mvwaddstr(cpuWin, 1, 1, cpu.name);
    wattrset(cpuWin, 0);
    drawBarWithPercentage(cpuWin, 2, 1, cpu.usagePercent);
    sprintf(buffer, "Temp: %4.1f °C", cpu.temperature);
    mvwaddstr(cpuWin, 3, 1, buffer);
    wrefresh(cpuWin);

    //Draw RAM window
    drawTitledWindow(ramWin, "RAM", WIN_WIDTH);
    sprintf(buffer, "%4.1f GiB", KB_TO_GB(ram.totalKb));
    wattrset(ramWin, A_BOLD);
    mvwaddstr(ramWin, 1, 1, buffer);
    wattrset(ramWin, 0);
    drawBarWithPercentage(ramWin, 2, 1, ram.usagePercent);
    sprintf(buffer, "Used: %4.1f/%4.1f GiB", KB_TO_GB(ram.totalKb - ram.freeKb), KB_TO_GB(ram.totalKb));
    mvwaddstr(ramWin, 3, 1, buffer);
    wrefresh(ramWin);

    //Draw GPU window
    drawTitledWindow(gpuWin, "GPU", WIN_WIDTH);
    wattrset(gpuWin, A_BOLD);
    mvwaddstr(gpuWin, 1, 1, gpu.name);
    wattrset(gpuWin, 0);
    drawBarWithPercentage(gpuWin, 2, 1, gpu.usagePercent);
    sprintf(buffer, "Temp: %4.1f °C", gpu.temperature);
    mvwaddstr(gpuWin, 3, 1, buffer);
    wrefresh(gpuWin);

    //Draw Network window
    drawTitledWindow(netWin, "Network", WIN_WIDTH);
    wattrset(netWin, A_BOLD);
    mvwaddstr(netWin, 1, 1, net.interface);
    wattrset(netWin, 0);
    sprintf(buffer, "Down: %6.2f MiB/s", B_TO_MB(net.down));
    mvwaddstr(netWin, 2, 1, buffer);
    sprintf(buffer, "Up:   %6.2f MiB/s", B_TO_MB(net.up));
    mvwaddstr(netWin, 3, 1, buffer);
    wrefresh(netWin);

    //Draw help window
    mvwaddstr(helpWin, 0, 1, "F10");
    wattrset(helpWin, 0);
    wcolor_set(helpWin, C_BC, 0);
    mvwaddstr(helpWin, 0, 4, "Quit");
    wcolor_set(helpWin, C_WB, 0);
    wrefresh(helpWin);
}

void repositionWindows(void)
{
    //Clear all windows to prevent garbage appearing
    wclear(stdscr);
    wclear(cpuWin);
    wclear(ramWin);
    wclear(gpuWin);
    wclear(netWin);

    //Get correct layout for screen width
    uint8_t index = (screenX / 30) - 1;
    if(index > 3)
    {
        index = 3;
    }
    //Reposition windows
    mvwin(cpuWin, screenLayouts[index][0][0], screenLayouts[index][0][1]);
    mvwin(ramWin, screenLayouts[index][1][0], screenLayouts[index][1][1]);
    mvwin(gpuWin, screenLayouts[index][2][0], screenLayouts[index][2][1]);
    mvwin(netWin, screenLayouts[index][3][0], screenLayouts[index][3][1]);
    mvwin(helpWin, screenLayouts[index][4][0], screenLayouts[index][4][1]);
}

int main(int argc, char* argv[])
{
    ncursesSetup();

    keypad(stdscr, true);
    timeout(0);

    //Set up display windows
    //Positions don't matter since they're set by the first repositionWindows() call
    cpuWin = newwin(5, WIN_WIDTH, 0, 0);
    ramWin = newwin(5, WIN_WIDTH, 0, 0);
    gpuWin = newwin(5, WIN_WIDTH, 0, 0);
    netWin = newwin(5, WIN_WIDTH, 0, 0);
    helpWin = newwin(1, WIN_WIDTH, 0, 0);
    getmaxyx(stdscr, screenY, screenX);
    repositionWindows();

    //Init subsystems
    if(initGPU())
    {
        //Failure to init GPU
        endwin();
        printf("Could not initialize NVML.\nExiting.\n");
        exit(1);
    }

    //Get CPU name
    if(getCPUName(&cpu))
    {
        //We keep going as it's not critical
        strcpy(cpu.name, "CANNOT DETECT");
    }

    //Get GPU name
    if(getGPUName(&gpu))
    {
        //We keep going as it's not critical
        strcpy(gpu.name, "CANNOT DETECT");
    }

    while(true)
    {
        //Discard errors for now
        readCPUUsage(&cpu);
        readCPUTemperature(&cpu);

        readRAMUsage(&ram);

        readGPUUsage(&gpu);
        readGPUTemperature(&gpu);

        readNetworkUsage(&net);

        drawWindows();

        sleep(READ_INTERVAL_MS / 1000);

        int32_t c = getch();
        if(c == KEY_F(10))
        {
            break;
        }
        else if(c == KEY_RESIZE)
        {
            getmaxyx(stdscr, screenY, screenX);
            repositionWindows();
        }
    }

    //Quit
    quitGPU();
    endwin();

    return 0;
}