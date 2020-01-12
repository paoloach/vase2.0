//
// Created by paolo on 08/01/20.
//

#ifndef VASO2_DATACHUNK_H
#define VASO2_DATACHUNK_H

#include <string.h>
#include <time.h>
#include <stdint.h>
#include "DataSample.h"

#define MAX_SAMPLES 100

static const uint32_t MAGIC_V1=0x483fe320;



struct DataChunk {
    DataChunk(){}
    DataChunk(DataSample * firstData) {
        startTime = time(nullptr);
        nextStartTime=0;
        magic=MAGIC_V1;
        memset(samples, 0xFF, MAX_SAMPLES* sizeof(DataSample));
        memcpy(&samples[0], firstData, sizeof(DataSample));
    }

    bool append(DataSample * sample);

    time_t startTime;
    time_t nextStartTime;
    uint32_t magic;
    struct DataSample samples[MAX_SAMPLES];
};

#endif //VASO2_DATACHUNK_H
