//
// Created by paolo on 02/02/19.
//

#ifndef VASO2_TASKDHT22_H
#define VASO2_TASKDHT22_H

#define MAX_SAMPLES 336
#define BYTES_MAP 42


struct DataSample {
    time_t timestamp;
    int16_t humidity;
    int16_t temperature;
}  __attribute__ ((packed));

void dht112Task(void *pvParameters);
uint16_t getLastSector();
struct DataSample * getSamples(uint16_t sector);
int16_t getLastSample();

extern int16_t humidity;
extern int16_t temperature;


#endif //VASO2_TASKDHT22_H
