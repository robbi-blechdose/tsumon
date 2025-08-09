#include "history.h"

#include <string.h>

void addEntryToHistory(void* restrict history, uint8_t size, void* restrict newValue, size_t typeSize)
{
    //Shift previous entries back
    memmove(history, history + typeSize, (size - 1) * typeSize);
    //Add new entry at the end
    memcpy(history + (size - 1) * typeSize, newValue, typeSize);
}

const uint64_t byteScales[] = {
    1024, //1KiB
    64 * 1024, //64KiB
    512 * 1024, //512KiB
    1024 * 1024, //1MiB
    16 * 1024 * 1024, //16MiB
    64 * 1024 * 1024, //64MiB
    512 * 1024 * 1024, //512MiB
    1024 * 1024 * 1024 //1GiB
};

const char* byteScaleNames[] = {
    "  1K",
    " 64K",
    "512K",
    "  1M",
    " 16M",
    "512M",
    "  1G"
};

void scaleByteHistory(uint64_t* history, uint8_t size, uint8_t* scaledHistory, uint8_t* scale)
{
    //Select scale
    *scale = 0;
    for(uint8_t i = 0; i < size; i++)
    {
        while(history[i] > byteScales[*scale])
        {
            (*scale)++;
        }
    }

    //Scale history
    for(uint8_t i = 0; i < size; i++)
    {
        scaledHistory[i] = history[i] * 100.0f / byteScales[*scale];
    }
}
