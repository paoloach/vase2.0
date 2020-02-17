//
// Created by paolo on 08/01/20.
//

#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>
#include <stdlib.h>
#include "DataChunkFlash.h"
#include "Flash.h"

constexpr const char * PARTITION_NAME="vaso2_data";
constexpr const char * TAG="DataChunkFlash";

DataChunkFlash dataChunkFlash;

void DataChunkFlash::init() {
    nvs_flash_init_partition(PARTITION_NAME);
    esp_err_t  error = nvs_open_from_partition(PARTITION_NAME, "data",  NVS_READWRITE,&handle );
    if (error != ESP_OK){
        ESP_LOGE(TAG, "error open data partition: %04X", error );
        handle=0;
    }
}

DataChunkFlash::~DataChunkFlash() {
    if (handle!=0){
        nvs_close(handle);
    }
}

bool DataChunkFlash::existChunk(char *chunkName) {
    if (handle==0)
        return false;
    size_t  size;
    return  nvs_get_blob(handle, chunkName, NULL,&size ) == ESP_OK;
}

std::unique_ptr<DataChunk> DataChunkFlash::getChunk(const char *chunkName) {
    auto chunk = std::unique_ptr<DataChunk>(new DataChunk());
    size_t size = sizeof(DataChunk);
    auto error = nvs_get_blob(handle, chunkName, chunk.get(), &size);
    if (error == ESP_OK){
        return chunk;
    } else {
        ESP_LOGE(TAG,"Error reading chunk %04X", error);
        return std::unique_ptr<DataChunk>();
    }
}

void DataChunkFlash::setChunk(const char *chunkName, const DataChunk *dataChunk) {
    esp_err_t  error;
    ESP_LOGI(TAG, "update chunk %s, next is %ld", chunkName, dataChunk->nextStartTime);
    do {
        error = nvs_set_blob(handle, chunkName, dataChunk, sizeof(DataChunk));
        if (error == ESP_ERR_NVS_NOT_ENOUGH_SPACE){
            freeSpace();
        }
    } while (error == ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    if (error != ESP_OK){
        ESP_LOGE(TAG,"Error writing chunk %04X", error);
    }
    nvs_commit(handle);
}

uint32_t DataChunkFlash::getU32(const char *key) {
    uint32_t value;
    auto error = nvs_get_u32(handle, key, &value);
    if (error != ESP_OK){
        if (error != ESP_ERR_NVS_NOT_FOUND){
            ESP_LOGE(TAG, "error reading uint32 key %s: %04X", key,  error);
        }
        value=0;
    }
    return value;
}

void DataChunkFlash::setU32(const char *key, uint32_t value) {
    esp_err_t  error;
    ESP_LOGI(TAG, "Update key %s", key);
    do {
        error = nvs_set_u32(handle, key, value );
        if (error == ESP_ERR_NVS_NOT_ENOUGH_SPACE){
            freeSpace();
        }
    } while (error == ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    if (error != ESP_OK){
        if (error != ESP_ERR_NVS_NOT_FOUND){
            ESP_LOGE(TAG, "error writing uint32 key %s: %04X", key,  error);
        }
    }
    nvs_commit(handle);
}

void DataChunkFlash::freeSpace() {
    ESP_LOGI(TAG, "free first chunk");
    char blobKeyName[20];
    time_t firstChunk = getFirstDataChunk();
    itoa(firstChunk, blobKeyName, 10);
    auto chunk = getChunk(blobKeyName);
    if (chunk) {
        setFirstDataChunk(chunk->nextStartTime);
    }
    if (nvs_erase_key(handle, blobKeyName) != ESP_OK) {
        ESP_LOGE(TAG, "error removing first chunk: %s", blobKeyName);
    }
}

void DataChunkFlash::eraseData() {
    ESP_LOGW(TAG, " ERASING ALL DATA");
    char blobKeyName[20];


    time_t firstChunk = getFirstDataChunk();
    setFirstDataChunk(0);
    setLastDataChunk(0);
    while(firstChunk != 0) {
        auto chunk = getChunk(firstChunk);
        itoa(firstChunk, blobKeyName, 10);
        if (nvs_erase_key(handle, blobKeyName) != ESP_OK) {
            ESP_LOGE(TAG, "error removing chunk: %s", blobKeyName);
        }
        firstChunk = chunk->nextStartTime;
    }
    ESP_LOGW(TAG, " ALL DATA ERASED");
}
