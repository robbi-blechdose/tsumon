#include "panel_setup.h"

#include <string.h>
#include "../setup.h"
#include "../display.h"

void movePanelMainCursorLR(bool left, MenuTree* menu, Configuration* config)
{
    moveCursor(&menu->cursor, left, config->panels.numPanels);
}

void drawPanelMain(MenuTree* menu, Configuration* config)
{
    drawStringCondBold(menu->win, 5, 3, "Panels:", menu->selected);
    uint8_t x = 11;

    //Clear panel list, this prevents artifacts when changing the list
    for(uint8_t i = x; i < SETUP_WIN_WIDTH - 1; i++)
    {
        mvwaddch(menu->win, 5, i, ' ');
    }

    for(uint8_t i = 0; i < config->panels.numPanels + 1; i++)
    {
        wattrset(menu->win, A_BOLD);
        mvwaddch(menu->win, 5, x, menu->selected && menu->cursor == i ? '>' : ' ');
        wattrset(menu->win, 0);

        if(i < config->panels.numPanels)
        {
            mvwaddstr(menu->win, 5, x + 1, panelNames[config->panels.panelTypes[i]]);
            x += strlen(panelNames[config->panels.panelTypes[i]]) + 2;
        }
    }
}

void enterPanelMainCursor(MenuTree* mt, Configuration* config)
{
    mt->children[0]->selected = true;
    mt->selected = false;
}

void initPanelMainMenu(MenuTree* menu)
{
    menu->draw = &drawPanelMain;
    menu->moveCursorLR = &movePanelMainCursorLR;
    menu->enterCursor = &enterPanelMainCursor;
}

#define PAC_ADD    0
#define PAC_EDIT   1
#define PAC_REMOVE 2

void drawPanelActionMenu(MenuTree* menu, Configuration* config)
{
    drawStringCondBold(menu->win, 7, 5, "Add:", menu->selected && menu->cursor == PAC_ADD);
    drawStringCondBold(menu->win, 8, 5, "Edit", menu->selected && menu->cursor == PAC_EDIT);
    drawStringCondBold(menu->win, 9, 5, "Remove", menu->selected && menu->cursor == PAC_REMOVE);
    
    //Draw panel action cursor
    wattrset(menu->win, A_BOLD);
    mvwaddch(menu->win, 7, 3, menu->selected && menu->cursor == PAC_ADD ? '>' : ' ');
    mvwaddch(menu->win, 8, 3, menu->selected && menu->cursor == PAC_EDIT ? '>' : ' ');
    mvwaddch(menu->win, 9, 3, menu->selected && menu->cursor == PAC_REMOVE ? '>' : ' ');
    wattrset(menu->win, 0);
}

void movePanelActionCursorUD(bool up, MenuTree* menu, Configuration* config)
{
    moveCursor(&menu->cursor, up, PAC_REMOVE);
}

void enterPanelActionCursor(MenuTree* mt, Configuration* config)
{
    switch(mt->cursor)
    {
        case PAC_ADD:
        {
            mt->children[0]->selected = true;
            mt->selected = false;
            break;
        }
        case PAC_EDIT:
        {
            //TODO
            break;
        }
        case PAC_REMOVE:
        {
            //Nothing to remove if we're in the empty spot
            if(mt->parent->cursor == config->panels.numPanels)
            {
                return;
            }
            removePanel(&config->panels, mt->parent->cursor);
            mt->selected = false;
            mt->parent->selected = true;
            break;
        }
    }
}

void initPanelActionMenu(MenuTree* mt)
{
    mt->draw = &drawPanelActionMenu;
    mt->moveCursorUD = &movePanelActionCursorUD;
    mt->enterCursor = &enterPanelActionCursor;
}

void drawPanelAddMenu(MenuTree* mt, Configuration* config)
{
    uint8_t x = 10;
    for(uint8_t i = 0; i < NUM_PANEL_TYPES; i++)
    {
        wattrset(mt->win, A_BOLD);
        mvwaddch(mt->win, 7, x, (mt->selected && mt->cursor == i) ? '>' : ' ');
        wattrset(mt->win, 0);

        mvwaddstr(mt->win, 7, x + 1, panelNames[i]);
        x += strlen(panelNames[i]) + 2;
    }
}

void movePanelAddCursorLR(bool left, MenuTree* mt, Configuration* config)
{
    moveCursor(&mt->cursor, left, NUM_PANEL_TYPES - 1);
}

void enterPanelAddCursor(MenuTree* mt, Configuration* config)
{
    //Uses the cursor from 2 parents up: 1st parent is panel action menu, 2nd parent is panel main menu
    addPanel(&config->panels, (PanelType) mt->cursor, mt->parent->parent->cursor);
    //Return two parents up
    mt->selected = false;
    mt->parent->parent->selected = true;
}

void initPanelAddMenu(MenuTree* mt)
{
    mt->draw = &drawPanelAddMenu;
    mt->moveCursorLR = &movePanelAddCursorLR;
    mt->enterCursor = &enterPanelAddCursor;
}