//
// Created by paolo on 08/01/20.
//

#include "DataChunk.h"

bool DataChunk::append(DataSample *newSample) {
    for(auto & sample: samples){
        if (sample.offset==0xFFFF){
            sample.offset = newSample->offset;
            sample.humidity = newSample->humidity;
            sample.temperature=newSample->temperature;
            sample.soil = newSample->soil;
            return true;
        }
    }
    return false;
}

