//
// Created by paolo on 08/01/20.
//

#include <esp_log.h>
#include "DataChunk.h"

bool DataChunk::append(DataSample *newSample) {
    uint32_t  index=0;
    for(auto & sample: samples){
        if (sample.offset==0xFFFF){
            ESP_LOGI("DataChunk", "Add at index %d", index);
            sample.offset = newSample->offset;
            sample.humidity = newSample->humidity;
            sample.temperature=newSample->temperature;
            sample.soil = newSample->soil;
            return true;
        }
        index++;
    }
    return false;
}

