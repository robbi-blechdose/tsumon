/**
 * tsumon - Terminal System Usage MONitor
 * 
 * Displays near-realtime system usage statistics in a terminal UI
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
#include "config.h"

int32_t screenX, screenY;
//Windows
WINDOW* infoWin;
WINDOW* setupWin;
WINDOW* setupEditWin;

Configuration config;

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
    getmaxyx(stdscr, screenY, screenX);
    
    //Check for width limit
    if(config.general.widthLimit)
    {
        if(screenX > config.general.widthLimit * PANEL_WIDTH)
        {
            screenX = config.general.widthLimit * PANEL_WIDTH;
        }
    }
    
    //Clear all windows to prevent garbage appearing
    wclear(stdscr);
    for(uint8_t i = 0; i < numPanels; i++)
    {
        wclear(panels[i].window);
    }
    wclear(setupWin);
    wclear(setupEditWin);

    //Layout windows
    if(setupOpen)
    {
        mvwin(infoWin, SETUP_WIN_HEIGHT + 1, 0);
    }
    else
    {
        //We can only layout for a minimum width
        if(screenX < PANEL_WIDTH)
        {
            screenX = PANEL_WIDTH;
        }
        uint8_t y = 1;
        uint8_t x = 0;
        for(uint8_t i = 0; i < numPanels; i++)
        {
            mvwin(panels[i].window, y, x);
            x += PANEL_WIDTH;
            if(x + PANEL_WIDTH > screenX || i == numPanels - 1)
            {
                x = 0;
                y += PANEL_HEIGHT;
            }
        }

        mvwin(infoWin, y, 0);
    }

    //Resize windows to prevent breaking when terminal is resized below window size
    for(uint8_t i = 0; i < numPanels; i++)
    {
        wresize(panels[i].window, PANEL_HEIGHT, PANEL_WIDTH);
    }

    wresize(infoWin, 1, PANEL_WIDTH);
    wresize(setupWin, SETUP_WIN_HEIGHT, SETUP_WIN_WIDTH);
    wresize(setupEditWin, SETUP_EDIT_WIN_HEIGHT, SETUP_EDIT_WIN_WIDTH);
}

uint8_t initPanels()
{
    numPanels = config.panels.numPanels;
    panels = malloc(sizeof(Panel) * numPanels);
    for(uint8_t i = 0; i < numPanels; i++)
    {
        panels[i].type = config.panels.panelTypes[i];
        if(initPanel(&panels[i]))
        {
            endwin();
            printf("Failed to initalize panel %d.\n", i);
            return 1;
        }
    }
    return 0;
}

void quitPanels()
{
    for(uint8_t i = 0; i < numPanels; i++)
    {
        quitPanel(&panels[i]);
    }
    free(panels);
}

int main(int argc, char* argv[])
{
    ncursesSetup();

    keypad(stdscr, true);
    timeout(0);
    ESCDELAY = 100;

    //Load configs, possibly set initial configs when loading was unsuccessful
    if(loadConfig(&config.general))
    {
        setInitialConfig(&config.general);
    }
    if(loadPanelConfig(&config.panels))
    {
        setInitialPanelConfig(&config.panels);
    }

    //Set up panels
    if(initPanels())
    {
        return 1;
    }

    //Set up display windows
    //Positions don't matter since they're set by the first repositionWindows() call
    infoWin = newwin(1, PANEL_WIDTH, 0, 0);
    setupWin = newwin(SETUP_WIN_HEIGHT, SETUP_WIN_WIDTH, 1, 0);
    setupEditWin = newwin(SETUP_EDIT_WIN_HEIGHT, SETUP_EDIT_WIN_WIDTH,
                            SETUP_WIN_HEIGHT / 2 - SETUP_EDIT_WIN_HEIGHT / 2, SETUP_WIN_WIDTH / 2 - SETUP_EDIT_WIN_WIDTH / 2);
    repositionWindows();

    initSetup(setupWin);

    while(true)
    {
        //Draw main window
        wattrset(stdscr, A_BOLD);
        uint8_t titleX = (screenX / PANEL_WIDTH) * PANEL_WIDTH / 2 - 2;
        if(titleX > numPanels * PANEL_WIDTH / 2 - 2)
        {
            titleX = numPanels * PANEL_WIDTH / 2 - 2;
        }
        mvaddstr(0, titleX, "tsumon");
        refresh();

        if(setupOpen)
        {
            drawSetup(&config);
        }
        else
        {
            for(uint8_t i = 0; i < numPanels; i++)
            {
                updatePanel(&panels[i], refreshIntervals[config.general.refreshIntervalIndex]);
            }
        }

        drawInfoWin();

        if(setupOpen)
        {
            usleep(16 * 1000); //16ms ~ 60fps, this is done so we don't burn CPU cycles needlessly
        }
        else
        {
            usleep(refreshIntervals[config.general.refreshIntervalIndex] * 1000);
        }

        int32_t c = getch();
        if(c == KEY_F(10))
        {
            break;
        }
        else if(c == KEY_F(2))
        {
            if(setupOpen && canExitSetup())
            {
                setupOpen = false;
                //We closed the setup screen
                saveConfig(&config.general);
                savePanelConfig(&config.panels);
                //Reinit panels in case we changed anything
                quitPanels();
                initPanels();
                repositionWindows();
            }
            else if(!setupOpen)
            {
                setupOpen = true;
                repositionWindows();
            }
        }
        else if((c == KEY_LEFT || c == KEY_RIGHT) && setupOpen)
        {
            moveSetupCursorLR(c == KEY_LEFT, &config);
        }
        else if((c == KEY_UP || c == KEY_DOWN) && setupOpen)
        {
            moveSetupCursorUD(c == KEY_UP, &config);
        }
        else if((c == KEY_ENTER || c == '\n') && setupOpen)
        {
            enterSetupCursor(&config);
        }
        else if(c == 27 && setupOpen) //Escape key
        {
            cancelSetupCursor();
        }
        else if(c == KEY_RESIZE)
        {
            repositionWindows();
        }
    }

    quitPanels();
    endwin();

    return 0;
}