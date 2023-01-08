/**
 * Small Performance MONitor
 * 
 * Displays CPU, RAM, GPU and network usage
 * 
 * Copyright (C) 2022-2023 Robbi Blechdose
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/

#include "panels/cpu.h"
#include "display.h"
#include "panels/gpu.h"
#include "panels/ram.h"
#include "panels/network.h"

#include "panel.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define WIN_HEIGHT 5
#define WIN_WIDTH 30

int32_t screenX, screenY;
//Windows
WINDOW* infoWin;

#define NUM_PANELS 4
Panel panels[NUM_PANELS];

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

void drawInfoWin(void)
{
    //Draw help window
    mvwaddstr(infoWin, 0, 1, "F2");
    mvwaddstr(infoWin, 0, 9, "F10");
    wattrset(infoWin, 0);
    wcolor_set(infoWin, C_BC, 0);
    mvwaddstr(infoWin, 0, 3, "Setup");
    mvwaddstr(infoWin, 0, 12, "Quit");
    wcolor_set(infoWin, C_WB, 0);
    wrefresh(infoWin);
}

void repositionWindows(void)
{
    //Clear all windows to prevent garbage appearing
    wclear(stdscr);

    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        wclear(panels[i].window);
    }

    //Get correct layout for screen width
    uint8_t index = (screenX / WIN_WIDTH) - 1;
    if(index > 3)
    {
        index = 3;
    }

    //Reposition windows
    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        mvwin(panels[i].window, screenLayouts[index][i][0], screenLayouts[index][i][1]);
    }

    mvwin(infoWin, screenLayouts[index][4][0], screenLayouts[index][4][1]);

    //Resize windows to prevent breaking when terminal is resized below window size
    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        wresize(panels[i].window, WIN_HEIGHT, WIN_WIDTH);
    }

    wresize(infoWin, 1, WIN_WIDTH);
}

int main(int argc, char* argv[])
{
    ncursesSetup();

    keypad(stdscr, true);
    timeout(0);

    //Set up panels
    panels[0].type = P_CPU;
    panels[1].type = P_RAM;
    panels[2].type = P_GPU;
    panels[3].type = P_NETWORK;
    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        if(initPanel(&panels[i]))
        {
            endwin();
            printf("Failed to initalize panel %d.\n", i);
            return 1;
        }
    }

    //Set up display windows
    //Positions don't matter since they're set by the first repositionWindows() call
    infoWin = newwin(1, WIN_WIDTH, 0, 0);
    getmaxyx(stdscr, screenY, screenX);
    repositionWindows();

    //Do a first read to prevent too-large values
    //readCPUUsage(&cpu);
    //readNetworkUsage(&net);

    while(true)
    {
        //Draw main window
        wattrset(stdscr, A_BOLD);
        mvaddstr(0, screenX / 2 - 2, "spmon");
        refresh();

        for(uint8_t i = 0; i < NUM_PANELS; i++)
        {
            updatePanel(&panels[i]);
        }

        drawInfoWin();

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

    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        quitPanel(&panels[i]);
    }

    endwin();

    return 0;
}