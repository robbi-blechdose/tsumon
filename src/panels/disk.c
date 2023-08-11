#include "disk.h"

#include <stdio.h>
#include "../display.h"

#define DISK_PANEL_HEIGHT 9

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
#define HISTORY_SIZE 11
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
    drawPanelBase(panel, "Disk");

    wattrset(panel->window, A_BOLD);
    mvwaddstr(panel->window, 1, 1, disk.name);
    wattrset(panel->window, 0);

    mvwaddstr(panel->window, 2, 1, "Read:");
    mvwaddstr(panel->window, 2, 18, "Write:");
    char buffer[PANEL_WIDTH];
    sprintf(buffer, "%8d KiB/s   %8d KiB/s", disk.readsPerSec / 1024, disk.writesPerSec / 1024);
    mvwaddstr(panel->window, 3, 3, buffer);

    drawGraphLabels(panel->window, 4, 1, 4, "   0", byteScaleNames[readHistoryScale]);
    drawGraphColor(panel->window, 4, 6, 4, HISTORY_SIZE, readHistoryScaled, &colorDiskGraph);
    drawGraphLabels(panel->window, 4, 18, 4, "   0", byteScaleNames[writeHistoryScale]);
    drawGraphColor(panel->window, 4, 23, 4, HISTORY_SIZE, writeHistoryScaled, &colorDiskGraph);
}

uint8_t initDiskPanel(Panel* panel)
{
    initPanelBase(panel, DISK_PANEL_HEIGHT, PANEL_WIDTH);
    readDiskName();
    panel->update = &updateDiskValues;
    panel->draw = &drawDiskPanel;

    //Do one read to make sure the first actual read has a valid previous value
    updateDiskValues(panel, 0);
    return 0;
}