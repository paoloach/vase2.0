//
// Created by paolo on 13/12/19.
//

#ifndef VASO2_FLASH_H
#define VASO2_FLASH_H

#include <stdint.h>
#include <time.h>

struct DataSample {
    time_t timestamp;
    int16_t humidity;
    int16_t temperature;
    uint16_t soil;
}  __attribute__ ((aligned (4)));


void initFlash(void);

uint16_t getLastSector(time_t before);
struct DataSample *getSamples(uint16_t sector);
int16_t getLastSample();

#endif //VASO2_FLASH_H
