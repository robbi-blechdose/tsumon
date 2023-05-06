#include "setup.h"

#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "panels/network.h"

#include "setup/menu.h"
#include "setup/general_setup.h"


//TODO: switch to derwin() subwindows
//TODO: add customizable "highlight color" (used for cpu/gpu name, ...)


#define MM_CURSOR_REFRESH     0
#define MM_CURSOR_WIDTHLIMIT  1
#define MM_CURSOR_PANELS      2
#define MM_CURSOR_LAST        MM_CURSOR_PANELS
MenuTree menuTree;

void drawMainMenu(MenuTree* menu, Configuration* config)
{
    drawTitledWindow(menu->win, "Setup", SETUP_WIN_WIDTH);
    
    wattrset(menu->win, A_BOLD);
    mvwaddch(menu->win, 1, 1, menu->selected && menu->cursor == MM_CURSOR_REFRESH ? '>' : ' ');
    mvwaddch(menu->win, 3, 1, menu->selected && menu->cursor == MM_CURSOR_WIDTHLIMIT ? '>' : ' ');
    mvwaddch(menu->win, 5, 1, menu->selected && menu->cursor == MM_CURSOR_PANELS ? '>' : ' ');
    wattrset(menu->win, 0);
}

void moveMainMenuCursor(bool up, MenuTree* menu, Configuration* config)
{
    moveCursor(&menu->cursor, up, MM_CURSOR_LAST);
}

void enterMainCursor(MenuTree* mt, Configuration* config)
{
    mt->children[mt->cursor]->selected = true;
    mt->selected = false;
}

void initSetup(WINDOW* win)
{
    //Main menu (root node, no contents by itself)
    createMenuTree(&menuTree);
    menuTree.win = win;
    menuTree.draw = &drawMainMenu;
    menuTree.moveCursorUD = &moveMainMenuCursor;
    menuTree.enterCursor = &enterMainCursor;
    menuTree.selected = true;

    //General setup
    //Refresh interval
    MenuTree* refreshInterval = malloc(sizeof(MenuTree));
    createMenuTree(refreshInterval);
    initRefreshIntervalMenu(refreshInterval);
    refreshInterval->win = win; //TODO: sub window
    insertMenuTree(&menuTree, refreshInterval);

    //Width limit
    MenuTree* widthLimit = malloc(sizeof(MenuTree));
    createMenuTree(widthLimit);
    initWidthLimitMenu(widthLimit);
    widthLimit->win = win; //TODO: sub window
    insertMenuTree(&menuTree, widthLimit);
}

void drawSetup(Configuration* config)
{
    drawMenuTree(&menuTree, config);
    wrefresh(menuTree.win);
}

void moveCursor(int8_t* cursor, bool dec, int8_t max)
{
    if(dec)
    {
        (*cursor)--;
        if(*cursor < 0)
        {
            *cursor = max;
        }
    }
    else
    {
        (*cursor)++;
        if(*cursor > max)
        {
            *cursor = 0;
        }
    }
}

bool moveSetupCursorLRTree(bool left, Configuration* config, MenuTree* mt)
{
    if(mt->selected)
    {
        if(mt->moveCursorLR != NULL)
        {
            mt->moveCursorLR(left, mt, config);
        }
        return true;
    }
    else
    {
        for(uint8_t i = 0; i < mt->numChildren; i++)
        {
            if(moveSetupCursorLRTree(left, config, mt->children[i]))
            {
                return true;
            }
        }
        return false;
    }
}

void moveSetupCursorLR(bool left, Configuration* config)
{
    moveSetupCursorLRTree(left, config, &menuTree);
}

bool moveSetupCursorUDTree(bool up, Configuration* config, MenuTree* mt)
{
    if(mt->selected)
    {
        if(mt->moveCursorUD != NULL)
        {
            mt->moveCursorUD(up, mt, config);
        }
        return true;
    }
    else
    {
        for(uint8_t i = 0; i < mt->numChildren; i++)
        {
            if(moveSetupCursorUDTree(up, config, mt->children[i]))
            {
                return true;
            }
        }
        return false;
    }
}

void moveSetupCursorUD(bool up, Configuration* config)
{
    moveSetupCursorUDTree(up, config, &menuTree);
}

bool enterSetupCursorTree(Configuration* config, MenuTree* mt)
{
    if(mt->selected)
    {
        if(mt->enterCursor != NULL)
        {
            mt->enterCursor(mt, config);
        }
        return true;
    }
    else
    {
        for(uint8_t i = 0; i < mt->numChildren; i++)
        {
            if(enterSetupCursorTree(config, mt->children[i]))
            {
                return true;
            }
        }
    }
    return false;
}

void enterSetupCursor(Configuration* config)
{
    enterSetupCursorTree(config, &menuTree);
}

bool cancelSetupCursorTree(Configuration* config, MenuTree* mt)
{
    if(mt->selected)
    {
        mt->selected = false;
        mt->parent->selected = true;
        return true;
    }
    
    for(uint8_t i = 0; i < mt->numChildren; i++)
    {
        if(cancelSetupCursorTree(config, mt->children[i]))
        {
            return true;
        }
    }
    return false;
}

void cancelSetupCursor(Configuration* config)
{
    //Can't get out of the root node
    if(menuTree.selected)
    {
        return;
    }

    cancelSetupCursorTree(config, &menuTree);
}

bool canExitSetup()
{
    //TODO
    return true;
}