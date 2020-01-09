//
// Created by paolo on 13/12/19.
//

#include <esp_log.h>
#include <cstdlib>
#include "Flash.h"
#include "DataChunkFlash.h"
#include "DataChunk.h"

const char * TAG="FLASH";


static void initData(struct DataSample * sample);



static const char *FIRST_SAMPLE_KEY="firstSample";
static const char *LAST_SAMPLE_KEY="lastSample";

DataChunkFlash dataChunkFlash;

void initFlash(void) {
    dataChunkFlash.init();
}


void saveSample(struct DataSample * sample) {

    time_t lastDataChunk = getLastDataChunk();
    if (lastDataChunk == 0){
        initData(sample);
        return;
    }
    time_t currentTime = time(nullptr);
    sample->offset = currentTime-lastDataChunk;

    char blobKeyName[20];
    itoa(lastDataChunk, blobKeyName, 10);

    if (dataChunkFlash.existChunk(blobKeyName)){
        auto chunk = dataChunkFlash.readChunk(blobKeyName);
        if (!chunk->append(sample)){
            ESP_LOGI(TAG, "Chunk full");
            time_t newLastDataChunk = time(nullptr);
            chunk->nextStartTime = newLastDataChunk;
            dataChunkFlash.write(blobKeyName, chunk.get());

            ESP_LOGI(TAG, "new chunk ");
            itoa(newLastDataChunk, blobKeyName, 10);
            sample->offset=0;
            chunk.reset(new DataChunk(sample));
            setLastDataChunk(newLastDataChunk);
            ESP_LOGI(TAG, "new chunk %s", blobKeyName);
        }
        dataChunkFlash.write(blobKeyName, chunk.get());
    } else {
        ESP_LOGE(TAG,"Internal error: last blob  with name %s not found", blobKeyName);
    }
}


time_t getFirstDataChunk() {
    return dataChunkFlash.getU32(FIRST_SAMPLE_KEY);
}

time_t getLastDataChunk() {
    return dataChunkFlash.getU32(LAST_SAMPLE_KEY);
}

void setFirstDataChunk(time_t day){
    dataChunkFlash.setU32(FIRST_SAMPLE_KEY, day);
}
void setLastDataChunk(time_t day){
    dataChunkFlash.setU32(LAST_SAMPLE_KEY, day);
}

time_t getChunkContaining(timer_t time) {
    char blobKeyName[20];

    auto chunkTime = getFirstDataChunk();
    while (chunkTime != 0) {
        itoa(chunkTime, blobKeyName, 10);
        auto chunk = dataChunkFlash.readChunk(blobKeyName);
        if (!chunk) {
            return 0;
        }
        if (chunk->startTime >= time && time < chunk->nextStartTime) {
            return chunkTime;
        }
        chunkTime = chunk->nextStartTime;
    }
    return 0;
}


void initData(struct DataSample *sample) {

    time_t  now = time(nullptr);
    char blobKeyName[20];
    itoa(now, blobKeyName, 10);
    ESP_LOGI(TAG, "Init data. First blob is %s", blobKeyName);

    auto chunk = std::unique_ptr<DataChunk>(new DataChunk(sample));
    chunk->startTime = now;
    dataChunkFlash.write(blobKeyName, chunk.get());
    setFirstDataChunk(now);
    setLastDataChunk(now);
}




