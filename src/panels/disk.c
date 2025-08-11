#include "disk.h"

#include <stdio.h>

#include "../display.h"
#include "../utils/history.h"

#define DISK_RW_PANEL_WIDTH 22
#define DISK_PANEL_WIDTH  (DISK_RW_PANEL_WIDTH * 2 + 2)
#define DISK_PANEL_HEIGHT 12
#define DISK_RW_PANEL_HEIGHT (DISK_PANEL_HEIGHT - 3)

#define DISK_NAME_LENGTH 32

//Sector size is 512 bytes according to https://lkml.org/lkml/2015/8/17/269
#define SECTOR_SIZE 512
#define SECTORS_TO_B(X) ((X) * SECTOR_SIZE)
#define SECTORS_TO_KB(X) (((X) * SECTOR_SIZE) / 1024)

typedef struct {
    char name[DISK_NAME_LENGTH];
    uint64_t sectorsRead;
    uint64_t sectorsWritten;

    uint64_t readsPerSec;
    uint64_t writesPerSec;
} DiskStatus;

DiskStatus disk;
#define HISTORY_SIZE 14
static uint64_t readHistory[HISTORY_SIZE];
static uint64_t writeHistory[HISTORY_SIZE];
static uint8_t readHistoryScaled[HISTORY_SIZE];
static uint8_t writeHistoryScaled[HISTORY_SIZE];
static uint8_t readHistoryScale;
static uint8_t writeHistoryScale;

uint8_t readDiskName()
{
    FILE* diskstats = fopen("/proc/diskstats", "r");
    if(diskstats == NULL)
    {
        return 1;
    }
    if(fscanf(diskstats, "%*u %*u %32s", disk.name) != 1)
    {
        fclose(diskstats);
        return 2;
    }
    fclose(diskstats);
    return 0;
}

uint8_t readDiskValues(uint16_t refreshInterval)
{
    FILE* diskstats = fopen("/proc/diskstats", "r");
    if(diskstats == NULL)
    {
        return 1;
    }
    
    uint64_t sectorsRead, sectorsWritten;
    if(fscanf(diskstats, "%*u %*u %*s %*u %*u %lu %*u %*u %*u %lu", &sectorsRead, &sectorsWritten) != 2)
    {
        fclose(diskstats);
        return 2;
    }
    fclose(diskstats);
    
    //Calculate delta times
    float readDelta = sectorsRead - disk.sectorsRead;
    float writeDelta = sectorsWritten - disk.sectorsWritten;
    //Calculate disk usage
    disk.readsPerSec = SECTORS_TO_B(readDelta) * (refreshInterval / 1000.0f);
    disk.writesPerSec = SECTORS_TO_B(writeDelta) * (refreshInterval / 1000.0f);
    //Swap values
    disk.sectorsRead = sectorsRead;
    disk.sectorsWritten = sectorsWritten;

    return 0;
}

void updateDiskValues(Panel* panel, uint16_t refreshInterval)
{
    readDiskValues(refreshInterval);

    addEntryToHistory(readHistory, HISTORY_SIZE, &disk.readsPerSec, sizeof(uint64_t));
    addEntryToHistory(writeHistory, HISTORY_SIZE, &disk.writesPerSec, sizeof(uint64_t));
    scaleByteHistory(readHistory, HISTORY_SIZE, readHistoryScaled, &readHistoryScale);
    scaleByteHistory(writeHistory, HISTORY_SIZE, writeHistoryScaled, &writeHistoryScale);
}

void colorDiskGraph(WINDOW* win, float value)
{
    wcolor_set(win, C_CyanBlack, 0);
}

void drawDiskPanel(Panel* panel)
{
    drawPanelBorder(panel, "Disk");

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, disk.name);
    wattrset(panel->window, 0);

    panelDrawChildren(panel);
}

void drawDiskReadPanel(Panel* panel)
{
    drawPanelBorder(panel, "Read");

    //TODO: scale units just like the graphs do
    char buffer[PANEL_WIDTH];
    sprintf(buffer, "%8d KiB/s", disk.readsPerSec / 1024);
    mvwaddstr(panel->window, 1, 1, buffer);
    drawGraphWithLabelsColor(panel->window, 2, 1, 6, HISTORY_SIZE, readHistoryScaled, &colorDiskGraph, "   0", byteScaleNames[readHistoryScale]);
}

Panel* createDiskReadPanel(Panel* diskPanel)
{
    Panel* panel = panelCreate(diskPanel, 2, 1, DISK_RW_PANEL_HEIGHT, DISK_RW_PANEL_WIDTH);
    panel->update = NULL; //Taken care of by the parent panel
    panel->draw = &drawDiskReadPanel;

    return panel;
}

void drawDiskWritePanel(Panel* panel)
{
    drawPanelBorder(panel, "Write");

    //TODO: scale units just like the graphs do
    char buffer[PANEL_WIDTH];
    sprintf(buffer, "%8d KiB/s", disk.writesPerSec / 1024);
    mvwaddstr(panel->window, 1, 1, buffer);
    drawGraphWithLabelsColor(panel->window, 2, 1, 6, HISTORY_SIZE, writeHistoryScaled, &colorDiskGraph, "   0", byteScaleNames[writeHistoryScale]);
}

Panel* createDiskWritePanel(Panel* diskPanel)
{
    Panel* panel = panelCreate(diskPanel, 2, DISK_RW_PANEL_WIDTH + 1, DISK_RW_PANEL_HEIGHT, DISK_RW_PANEL_WIDTH);
    panel->update = NULL; //Taken care of by the parent panel
    panel->draw = &drawDiskWritePanel;

    return panel;
}

//TODO: allow multiple instances of the disk panel, and for that allow setting the disk to be monitored
uint8_t initDiskPanel(Panel* panel)
{
    panelInit(panel, DISK_PANEL_HEIGHT, DISK_PANEL_WIDTH);
    readDiskName();
    panel->update = &updateDiskValues;
    panel->draw = &drawDiskPanel;

    panelAddChild(panel, createDiskReadPanel(panel));
    panelAddChild(panel, createDiskWritePanel(panel));

    //Do one read to make sure the first actual read has a valid previous value
    updateDiskValues(panel, 0);
    return 0;
}