#include "panel.h"

#include <stdlib.h>
#include <string.h>
#include "display.h"

Panel* panelCreate(Panel* parent, uint8_t y, uint8_t x, uint8_t height, uint8_t width)
{
    Panel* panel = malloc(sizeof(Panel));
    if(parent == NULL)
    {
        panel->window = newwin(height, width, y, x);
    }
    else
    {
        panel->window = derwin(parent->window, height, width, y, x);
    }
    panel->height = height;
    panel->width = width;
    panel->children = NULL;
    panel->numChildren = 0;
    return panel;
}

void panelDestroy(Panel* panel)
{
    free(panel);
}

void panelInit(Panel* panel, uint8_t height, uint8_t width)
{
    panel->height = height;
    panel->width = width;
    panel->window = newwin(height, width, 0, 0);
    panel->children = NULL;
    panel->numChildren = 0;
}

void panelQuit(Panel* panel)
{
    if(panel->numChildren > 0)
    {
        free(panel->children);
    }
}

void panelAddChild(Panel* panel, Panel* child)
{
    if(panel->numChildren == 0)
    {
        panel->children = malloc(sizeof(Panel*));
        panel->numChildren++;
        panel->children[0] = child;
    }
    else
    {
        panel->children = realloc(panel->children, sizeof(Panel*) * (panel->numChildren + 1));
        panel->children[panel->numChildren++] = child;
    }
}

void panelDrawChildren(Panel* panel)
{
    for(uint8_t i = 0; i < panel->numChildren; i++)
    {
        panel->children[i]->draw(panel->children[i]);
    }
}

void panelUpdateChildren(Panel* panel, uint16_t refreshInterval)
{
    for(uint8_t i = 0; i < panel->numChildren; i++)
    {
        panel->children[i]->update(panel->children[i], refreshInterval);
    }
}

void drawPanelBorder(Panel* panel, const char* title)
{
    wcolor_set(panel->window, C_WhiteBlack, 0);
    box(panel->window, 0, 0);

    mvwaddstr(panel->window, 0, panel->width / 2 - strlen(title) / 2 - 2, "\u257C ");
    if(strlen(title) % 2 == 0)
    {
        mvwaddstr(panel->window, 0, panel->width / 2 + strlen(title) / 2, " \u257E");
    }
    else
    {
        mvwaddstr(panel->window, 0, panel->width / 2 + strlen(title) / 2 + 1, " \u257E");
    }
    wcolor_set(panel->window, config.highlightColor, 0);
    mvwaddstr(panel->window, 0, panel->width / 2 - strlen(title) / 2, title);

    wcolor_set(panel->window, C_WhiteBlack, 0);
}
