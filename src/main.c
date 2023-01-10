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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "display.h"
#include "panel.h"
#include "setup.h"

#define WIN_HEIGHT 5
#define WIN_WIDTH 30

int32_t screenX, screenY;
//Windows
WINDOW* infoWin;
WINDOW* setupWin;

#define NUM_PANELS 4
Panel panels[NUM_PANELS];

bool setupOpen = false;

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

    //Layout windows
    //We can only layout for a minimum width
    if(screenX < PANEL_WIDTH)
    {
        screenX = PANEL_WIDTH;
    }
    uint8_t y = 1;
    uint8_t x = 0;
    for(uint8_t i = 0; i < NUM_PANELS; i++)
    {
        mvwin(panels[i].window, y, x);
        x += PANEL_WIDTH;
        if(x + PANEL_WIDTH > screenX || i == NUM_PANELS - 1)
        {
            x = 0;
            y += PANEL_HEIGHT;
        }
    }

    mvwin(infoWin, y, 0);

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
    setupWin = newwin(SETUP_WIN_HEIGHT, SETUP_WIN_WIDTH, 2, 0);
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

        if(setupOpen)
        {
            drawSetup(setupWin);
        }
        else
        {
            for(uint8_t i = 0; i < NUM_PANELS; i++)
            {
                updatePanel(&panels[i]);
            }
        }

        drawInfoWin();

        if(!setupOpen)
        {
            usleep(getRefreshInterval() * 1000);
        }

        int32_t c = getch();
        if(c == KEY_F(10))
        {
            break;
        }
        else if(c == KEY_F(2))
        {
            setupOpen = !setupOpen;
            wclear(stdscr);
        }
        else if((c == KEY_LEFT || c == KEY_RIGHT) && setupOpen)
        {
            moveSetupCursorLR(c == KEY_LEFT);
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