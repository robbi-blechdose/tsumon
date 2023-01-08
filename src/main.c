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

#define B_TO_KB(X) ((X) / 1024.0f)

#define WIN_HEIGHT 5
#define WIN_WIDTH 30

int32_t screenX, screenY;
//Windows
WINDOW* helpWin;

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

void drawWindows(void)
{
    //Draw help window
    mvwaddstr(helpWin, 0, 1, "F2");
    mvwaddstr(helpWin, 0, 9, "F10");
    wattrset(helpWin, 0);
    wcolor_set(helpWin, C_BC, 0);
    mvwaddstr(helpWin, 0, 3, "Setup");
    mvwaddstr(helpWin, 0, 12, "Quit");
    wcolor_set(helpWin, C_WB, 0);
    wrefresh(helpWin);
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
    mvwin(panels[0].window, screenLayouts[index][0][0], screenLayouts[index][0][1]);
    mvwin(panels[1].window, screenLayouts[index][1][0], screenLayouts[index][1][1]);
    mvwin(panels[2].window, screenLayouts[index][2][0], screenLayouts[index][2][1]);
    mvwin(panels[3].window, screenLayouts[index][3][0], screenLayouts[index][3][1]);

    mvwin(helpWin, screenLayouts[index][4][0], screenLayouts[index][4][1]);

    //Resize windows to prevent breaking when terminal is resized below window size
    wresize(panels[0].window, WIN_HEIGHT, WIN_WIDTH);
    wresize(panels[1].window, WIN_HEIGHT, WIN_WIDTH);
    wresize(panels[2].window, WIN_HEIGHT, WIN_WIDTH);
    wresize(panels[3].window, WIN_HEIGHT, WIN_WIDTH);

    wresize(helpWin, 1, WIN_WIDTH);
}

int main(int argc, char* argv[])
{
    ncursesSetup();

    keypad(stdscr, true);
    timeout(0);

    //Set up panels
    initCPUPanel(&panels[0]);
    initRAMPanel(&panels[1]);
    if(initGPUPanel(&panels[2]))
    {
        endwin();
        printf("Could not initialize NVML.\nExiting.\n");
        return 1;
    }
    initNetworkPanel(&panels[3]);

    //Set up display windows
    //Positions don't matter since they're set by the first repositionWindows() call
    helpWin = newwin(1, WIN_WIDTH, 0, 0);
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

    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        quitPanel(&panels[i]);
    }

    endwin();

    return 0;
}