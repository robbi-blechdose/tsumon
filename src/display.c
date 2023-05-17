#include "display.h"

#include <string.h>
#include <locale.h>

void ncursesSetup(void)
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    //Set up colors
    start_color();
    init_pair(C_WhiteBlack, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_GreenBlack, COLOR_GREEN, COLOR_BLACK);
    init_pair(C_YellowBlack, COLOR_YELLOW, COLOR_BLACK);
    init_pair(C_RedBlack, COLOR_RED, COLOR_BLACK);
    init_pair(C_CyanBlack, COLOR_CYAN, COLOR_BLACK);
    init_pair(C_BlackCyan, COLOR_BLACK, COLOR_CYAN);
}

void setColorViaThreshold(WINDOW* win, float value)
{
    if(value < THRESHOLD_GREEN)
    {
        wcolor_set(win, C_GreenBlack, 0);
    }
    else if(value < THRESHOLD_YELLOW)
    {
        wcolor_set(win, C_YellowBlack, 0);
    }
    else
    {
        wcolor_set(win, C_RedBlack, 0);
    }
}

void drawBar(WINDOW* win, uint8_t y, uint8_t x, float value)
{
    mvwaddch(win, y, x, '[');
    mvwaddch(win, y, x + 21, ']');

    setColorViaThreshold(win, value);

    //Draw bar
    for(uint8_t i = 0; i < 20; i++)
    {
        if(i < value / 5)
        {
            mvwaddch(win, y, x + 1 + i, '|');
        }
        else
        {
            mvwaddch(win, y, x + 1 + i, ' ');
        }
    }

    //Reset color
    wcolor_set(win, C_WhiteBlack, 0);
}

void drawBarWithPercentage(WINDOW* win, uint8_t y, uint8_t x, float value)
{
    drawBar(win, y, x, value);
    #define PERCENT_BUFFER_SIZE 7
    char buffer[PERCENT_BUFFER_SIZE];
    snprintf(buffer, PERCENT_BUFFER_SIZE, "%5.1f%%", value);
    mvwaddstr(win, y, x + 22, buffer);
}

void drawTitledBarWithPercentage(WINDOW* win, uint8_t y, uint8_t x, float value, const char* title)
{
    mvwaddstr(win, y, x, title);
    drawBarWithPercentage(win, y, x + strlen(title) + 1, value);
}

void drawTitledWindow(WINDOW* win, const char* title, uint8_t width)
{
    wcolor_set(win, C_WhiteBlack, 0);
    box(win, 0, 0);
    mvwaddstr(win, 0, width / 2 - strlen(title) / 2, title);
}

void drawSlider(WINDOW* win, uint8_t y, uint8_t x, uint8_t length, uint8_t current)
{
    mvwaddch(win, y, x, '<');
    for(uint8_t i = 0; i < length; i++)
    {
        mvwaddch(win, y, x + 1 + i, '=');
        if(i == current)
        {
            mvwaddch(win, y, x + 1 + i, '#');
        }
    }
    mvwaddch(win, y, x + 1 + length, '>');
}

void drawGraphColor(WINDOW* win, uint8_t y, uint8_t x, uint8_t height, uint8_t width, uint8_t* values, void (*setColor)(WINDOW*, float))
{
    for(uint8_t i = 0; i < width; i++)
    {
        setColor(win, values[i]);

        //Translate percentages into values fitting the graph height
        uint8_t value = values[i] * 0.1f * height;

        //Draw bar
        for(uint8_t j = 0; j < height; j++)
        {
            if(value / 10 > j)
            {
                mvwaddstr(win, y + height - 1 - j, i + x, "\u2588");
            }
            else if(value > j * 10)
            {
                uint8_t diff = value - (value / 10) * 10;

                if(diff < 3)
                {
                    mvwaddstr(win, y + height - 1 - j, i + x, "\u2582");
                }
                else if(diff < 7)
                {
                    mvwaddstr(win, y + height - 1 - j, i + x, "\u2584");
                }
                else
                {
                    mvwaddstr(win, y + height - 1 - j, i + x, "\u2586");
                }
            }
            else
            {
                mvwaddch(win, y + height - 1 - j, i + x, ' ');
            }
        }
    }

    //Reset color
    wcolor_set(win, C_WhiteBlack, 0);
}

void drawGraph(WINDOW* win, uint8_t y, uint8_t x, uint8_t height, uint8_t width, uint8_t* values)
{
    drawGraphColor(win, y, x, height, width, values, &setColorViaThreshold);
}

void drawGraphLabels(WINDOW* win, uint8_t y, uint8_t x, uint8_t height, const char* min, const char* max)
{
    mvwaddstr(win, y, x, max);
    mvwaddstr(win, y + height - 1, x, min);
    wmove(win, y, x + strlen(max));
    wvline(win, ACS_VLINE, height);
}

void drawStringConditionalBold(WINDOW* win, uint8_t y, uint8_t x, const char* str, bool bold)
{
    if(bold)
    {
        wattrset(win, A_BOLD);
        mvwaddstr(win, y, x, str);
        wattrset(win, 0);
    }
    else
    {
        mvwaddstr(win, y, x, str);
    }
}