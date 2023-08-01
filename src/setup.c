#include "setup.h"

#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "panels/network.h"

#include "setup/menu.h"
#include "setup/general_setup.h"

#define MM_CURSOR_REFRESH     0
#define MM_CURSOR_WIDTHLIMIT  1
#define MM_CURSOR_PANELS      2
#define MM_CURSOR_LAST        MM_CURSOR_PANELS
MenuTree menuTree;

void drawMainMenu(MenuTree* menu)
{
    drawTitledWindow(menu->win, "Setup", SETUP_WIN_WIDTH);
    
    wattrset(menu->win, A_BOLD);
    mvwaddch(menu->win, 1, 1, menu->selected && menu->cursor == MM_CURSOR_REFRESH ? '>' : ' ');
    mvwaddch(menu->win, 3, 1, menu->selected && menu->cursor == MM_CURSOR_WIDTHLIMIT ? '>' : ' ');
    mvwaddch(menu->win, 5, 1, menu->selected && menu->cursor == MM_CURSOR_PANELS ? '>' : ' ');
    wattrset(menu->win, 0);
}

void moveMainMenuCursor(bool up, MenuTree* menu)
{
    moveCursor(&menu->cursor, up, MM_CURSOR_LAST);
}

void enterMainCursor(MenuTree* mt)
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
    refreshInterval->win = win; //TODO: sub window?
    insertMenuTree(&menuTree, refreshInterval);

    //Width limit
    MenuTree* widthLimit = malloc(sizeof(MenuTree));
    createMenuTree(widthLimit);
    initWidthLimitMenu(widthLimit);
    widthLimit->win = win; //TODO: sub window?
    insertMenuTree(&menuTree, widthLimit);

    //Highlight color
    MenuTree* highlightColor = malloc(sizeof(MenuTree));
    createMenuTree(highlightColor);
    initHighlightColorMenu(highlightColor);
    highlightColor->win = win; //TODO: sub window?
    insertMenuTree(&menuTree, highlightColor);
}

void drawSetup(void)
{
    drawMenuTree(&menuTree);
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

bool moveSetupCursorLRTree(bool left, MenuTree* mt)
{
    if(mt->selected)
    {
        if(mt->moveCursorLR != NULL)
        {
            mt->moveCursorLR(left, mt);
        }
        return true;
    }
    else
    {
        for(uint8_t i = 0; i < mt->numChildren; i++)
        {
            if(moveSetupCursorLRTree(left, mt->children[i]))
            {
                return true;
            }
        }
        return false;
    }
}

void moveSetupCursorLR(bool left)
{
    moveSetupCursorLRTree(left, &menuTree);
}

bool moveSetupCursorUDTree(bool up, MenuTree* mt)
{
    if(mt->selected)
    {
        if(mt->moveCursorUD != NULL)
        {
            mt->moveCursorUD(up, mt);
        }
        return true;
    }
    else
    {
        for(uint8_t i = 0; i < mt->numChildren; i++)
        {
            if(moveSetupCursorUDTree(up, mt->children[i]))
            {
                return true;
            }
        }
        return false;
    }
}

void moveSetupCursorUD(bool up)
{
    moveSetupCursorUDTree(up, &menuTree);
}

bool enterSetupCursorTree(MenuTree* mt)
{
    if(mt->selected)
    {
        if(mt->enterCursor != NULL)
        {
            mt->enterCursor(mt);
        }
        return true;
    }
    else
    {
        for(uint8_t i = 0; i < mt->numChildren; i++)
        {
            if(enterSetupCursorTree(mt->children[i]))
            {
                return true;
            }
        }
    }
    return false;
}

void enterSetupCursor(void)
{
    enterSetupCursorTree(&menuTree);
}

bool cancelSetupCursorTree(MenuTree* mt)
{
    if(mt->selected)
    {
        mt->selected = false;
        mt->parent->selected = true;
        return true;
    }
    
    for(uint8_t i = 0; i < mt->numChildren; i++)
    {
        if(cancelSetupCursorTree(mt->children[i]))
        {
            return true;
        }
    }
    return false;
}

void cancelSetupCursor(void)
{
    //Can't get out of the root node
    if(menuTree.selected)
    {
        return;
    }

    cancelSetupCursorTree(&menuTree);
}

bool canExitSetup()
{
    //TODO
    return true;
}