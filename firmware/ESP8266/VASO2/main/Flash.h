//
// Created by paolo on 13/12/19.
//

#ifndef VASO2_FLASH_H
#define VASO2_FLASH_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif




void initFlash(void);

time_t getChunkContaining(timer_t time);

time_t getFirstDataChunk();

time_t getLastDataChunk();

void setFirstDataChunk(time_t day);

void setLastDataChunk(time_t day);

void saveSample(struct DataSample *sample);

#ifdef __cplusplus
}
#endif

#endif //VASO2_FLASH_H
