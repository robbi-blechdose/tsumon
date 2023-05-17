#include "fetch.h"

#include <stdlib.h>
#include <string.h>
#include "../display.h"

#define FETCH_PANEL_WIDTH 30

typedef struct {
    char os[PANEL_WIDTH - 5];
    char kernel[PANEL_WIDTH - 9];
    char hostname[PANEL_WIDTH - 11];
    double uptime;
} FetchData;

static FetchData fetchData;

uint8_t getOS(char* os)
{
    FILE* issue = fopen("/etc/issue", "r");
    if(issue == NULL)
    {
        return 1;
    }

    if(fgets(os, PANEL_WIDTH - 5, issue) == NULL)
    {
        fclose(issue);
        return 2;
    }

    //Terminate string before "\n \l"
    os[strcspn(os, "\\")] = 0;

    fclose(issue);
    return 0;
}

uint8_t getKernel(char* kernel)
{
    FILE* version = fopen("/proc/version", "r");
    if(version == NULL)
    {
        return 1;
    }

    if(fscanf(version, "%*s %*s %s", kernel) != 1)
    {
        fclose(version);
        return 2;
    }

    fclose(version);
    return 0;
}

uint8_t getHostname(char* hostname)
{
    FILE* hnfile = fopen("/proc/sys/kernel/hostname", "r");
    if(hnfile == NULL)
    {
        return 1;
    }

    if(fscanf(hnfile, "%s", hostname) != 1)
    {
        fclose(hnfile);
        return 2;
    }

    fclose(hnfile);
    return 0;
}

uint8_t readUptime(void)
{
    FILE* uptime = fopen("/proc/uptime", "r");
    if(uptime == NULL)
    {
        return 1;
    }

    if(fscanf(uptime, "%lf", &fetchData.uptime) != 1)
    {
        fclose(uptime);
        return 2;
    }

    fclose(uptime);
    return 0;
}

void updateFetchValues(Panel* panel, uint16_t refreshInterval)
{
    readUptime();
}

void drawFetchPanel(Panel* panel)
{
    drawPanelBase(panel, "System");

    //Draw titles
    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, "OS:");
    mvwaddstr(panel->window, 2, 1, "Kernel:");
    mvwaddstr(panel->window, 3, 1, "Hostname:");
    mvwaddstr(panel->window, 4, 1, "Uptime:");
    //Draw contents
    wattrset(panel->window, 0);
    mvwaddstr(panel->window, 1, 5, fetchData.os);
    mvwaddstr(panel->window, 2, 9, fetchData.kernel);
    mvwaddstr(panel->window, 3, 11, fetchData.hostname);

    char buffer[PANEL_WIDTH - 9];
    uint32_t hours = fetchData.uptime / 3600;
    float mins = fetchData.uptime / 60 - (hours * 60);
    sprintf(buffer, "%2.0f:%02.0f", (float) hours, mins);
    mvwaddstr(panel->window, 4, 9, buffer);
}

uint8_t initFetchPanel(Panel* panel)
{
    initPanelBase(panel, PANEL_HEIGHT, FETCH_PANEL_WIDTH);
    panel->update = &updateFetchValues;
    panel->draw = &drawFetchPanel;

    //Read data - this doesn't change so doing it on startup is enough
    uint8_t err = getOS(fetchData.os);
    if(err)
    {
        return err;
    }
    err = getHostname(fetchData.hostname);
    if(err)
    {
        return err;
    }
    return getKernel(fetchData.kernel);
}