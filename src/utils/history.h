#ifndef HISTORY_H
#define HISTORY_H

#include <stdint.h>
#include <stddef.h>

/**
 * Shifts all entries forwards, then adds a new last entry
 **/
void addEntryToHistory(void* restrict history, uint8_t size, void* restrict newValue, size_t typeSize);

extern const uint64_t byteScales[];
extern const char* byteScaleNames[];
void scaleByteHistory(uint64_t* history, uint8_t size, uint8_t* scaledHistory, uint8_t* scale);

#endif