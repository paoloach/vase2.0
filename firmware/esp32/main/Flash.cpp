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


constexpr const char *FIRST_SAMPLE_KEY="firstSample";
constexpr const char *LAST_SAMPLE_KEY="lastSample";

void initFlash(void) {
    dataChunkFlash.init();
}


void saveSample(struct DataSample * sample) {

    time_t lastDataChunk = getLastDataChunk();

    ESP_LOGI(TAG, "last data chunk: %ld", lastDataChunk);

    if (lastDataChunk == 0){
        initData(sample);
        return;
    }
    time_t currentTime = time(nullptr);

    ESP_LOGI(TAG,"current time: %ld", currentTime);
    sample->offset = currentTime-lastDataChunk;

    char blobKeyName[20];
    itoa(lastDataChunk, blobKeyName, 10);

    if (dataChunkFlash.existChunk(blobKeyName)){
        auto chunk = dataChunkFlash.getChunk(blobKeyName);
        if (!chunk->append(sample)){
            ESP_LOGI(TAG, "Chunk full");
            time_t newLastDataChunk = time(nullptr);
            chunk->nextStartTime = newLastDataChunk;
            ESP_LOGI(TAG, "update %s chunk with next chuck to %ld", blobKeyName, newLastDataChunk);
            dataChunkFlash.setChunk(blobKeyName, chunk.get());

            ESP_LOGI(TAG, "new chunk ");
            itoa(newLastDataChunk, blobKeyName, 10);
            sample->offset=0;
            chunk.reset(new DataChunk(sample));
            setLastDataChunk(newLastDataChunk);
            ESP_LOGI(TAG, "new chunk %s", blobKeyName);
        } else {
            ESP_LOGI(TAG, "appended to chunk %s", blobKeyName);
        }
        dataChunkFlash.setChunk(blobKeyName, chunk.get());
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

void flashDataErase() {
    dataChunkFlash.eraseData();

}

void initData(struct DataSample *sample) {

    time_t  now = time(nullptr);
    sample->offset=0;
    char blobKeyName[20];
    itoa(now, blobKeyName, 10);
    ESP_LOGI(TAG, "Init data. First blob is %s", blobKeyName);

    auto chunk = std::unique_ptr<DataChunk>(new DataChunk(sample));
    chunk->startTime = now;
    dataChunkFlash.setChunk(blobKeyName, chunk.get());
    setFirstDataChunk(now);
    setLastDataChunk(now);
}




