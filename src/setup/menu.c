#include "menu.h"

#include <stdlib.h>

void createMenuTree(MenuTree* mt)
{
    mt->selected = false;
    mt->cursor = 0;

    //Set all functions to NULL so we don't have to do this manually later
    mt->moveCursorLR = NULL;
    mt->moveCursorUD = NULL;
    mt->enterCursor = NULL;

    mt->numChildren = 0;
    mt->children = NULL;
    mt->parent = NULL;
}

void insertMenuTree(MenuTree* mt, MenuTree* child)
{
    child->parent = mt;
    
    if(mt->numChildren == 0)
    {
        mt->numChildren = 1;
        mt->children = malloc(sizeof(MenuTree*));
        mt->children[0] = child;
    }
    else
    {
        mt->numChildren++;
        MenuTree** newChildren = malloc(sizeof(MenuTree*) * mt->numChildren);
        for(uint8_t i = 0; i < mt->numChildren - 1; i++)
        {
            newChildren[i] = mt->children[i];
        }
        newChildren[mt->numChildren - 1] = child;
        free(mt->children);
        mt->children = newChildren;
    }
}

void drawMenuTree(MenuTree* mt)
{
    mt->draw(mt);
    for(uint8_t i = 0; i < mt->numChildren; i++)
    {
        drawMenuTree(mt->children[i]);
    }
}

void destroyMenuTree(MenuTree* mt)
{
    for(uint8_t i = 0; i < mt->numChildren; i++)
    {
        destroyMenuTree(mt->children[i]);
    }
    free(mt->children);
}