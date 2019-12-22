//
// Created by paolo on 13/12/19.
//

#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <stdlib.h>
#include <crc.h>
#include "Flash.h"
#include "Light.h"

#define MAX_SAMPLES 100
#define MAX_SECTORS 100

struct DataBuffer {
    time_t firstTS;
    time_t lastTS;
    struct DataSample samples[MAX_SAMPLES];
} __attribute__((packed));

static void readSector(uint16_t sector) ;
static uint16_t getLastSectorFromFlash(time_t before);

const char * TAG="FLASH";

static struct DataBuffer dataBuffer;


static const char *DATA_PARTITION_NAME="vaso2_data";
static uint16_t lastSector;

void initFlash(void) {
    esp_err_t espError;



//    espError = nvs_flash_init_partition(DATA_PARTITION_NAME);
//    if (espError != ESP_OK){
//        ESP_LOGE(TAG, "error initializing data partition: %d", espError);
//    }
//
//    lastSector = getLastSectorFromFlash(0);

}

struct DataSample *getSamples(uint16_t sector) {
    readSector(sector);
    return dataBuffer.samples;
}

uint16_t getLastSector(time_t before) {
    return 0;
}

int16_t getLastSample() {
    return 0;
}


uint16_t getLastSectorFromFlash(time_t before) {
    time_t newerTime = 0;
    uint16_t newerSector = 0;
    for(int sector=0; sector < MAX_SECTORS; sector++){
        readSector(sector);
        time_t lastTime = dataBuffer.lastTS;
        ESP_LOGI(TAG, "sample index: time %ld\n", dataBuffer.lastTS);
        if (dataBuffer.lastTS > newerTime && lastTime <= before) {
            newerTime = dataBuffer.lastTS;
            newerSector = sector;
        }
    }
    return newerSector;
}

static void readSector(uint16_t sector) {
    ESP_LOGI(TAG, "Read sector %02X\n", sector);
    nvs_handle nvsHandle;
    esp_err_t  error = nvs_open_from_partition(DATA_PARTITION_NAME, "data",  NVS_READWRITE,&nvsHandle );
    if (error != ESP_OK){
        ESP_LOGE(TAG, "error open data partition: %d", error );
    }
    char sectorName[8];
    itoa(sector,sectorName,10 );
    error = nvs_get_blob(nvsHandle, sectorName, (uint32_t *)&dataBuffer, sizeof(struct DataBuffer));
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "error reading sector %d: %d\n", sector, error);
    }
    nvs_close(nvsHandle);
}
