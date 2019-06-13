//
// Created by paolo on 02/02/19.
//

#ifndef VASO2_TASKDHT22_H
#define VASO2_TASKDHT22_H

#include <termios.h>
#define MAX_SAMPLES 254
#define BYTES_MAP 32



struct DataSample {
    time_t timestamp;
    int16_t humidity;
    int16_t temperature;
    uint16_t soil;
}  __attribute__ ((aligned (4)));


void dht112Task(void *pvParameters);
uint16_t getLastSector(time_t before);
struct DataSample * getSamples(uint16_t sector);
int16_t getLastSample();

extern int16_t humidity;
extern int16_t temperature;


#endif //VASO2_TASKDHT22_H
