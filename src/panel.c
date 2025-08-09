#include "panel.h"

#include <stdlib.h>
#include <string.h>
#include "display.h"

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

void drawPanelBorder(Panel* panel, const char* title)
{
    wcolor_set(panel->window, C_WhiteBlack, 0);
    box(panel->window, 0, 0);

    wcolor_set(panel->window, config.highlightColor, 0);
    mvwaddstr(panel->window, 0, panel->width / 2 - strlen(title) / 2, title);

    wcolor_set(panel->window, C_WhiteBlack, 0);
}
