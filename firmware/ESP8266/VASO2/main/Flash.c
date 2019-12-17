//
// Created by paolo on 13/12/19.
//

#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_spi_flash.h>
#include <nvs_flash.h>
#include "Flash.h"
#include "taskSNTP.h"
#include "Light.h"

#define FULL_MAP 0x00
#define MAX_SAMPLES 254
#define BYTES_MAP 32

extern struct PeriodLed periodLed;

struct DataBuffer {
    uint8_t map[BYTES_MAP];
    struct DataSample samples[MAX_SAMPLES];
} __attribute__((packed));

static void readSector(uint16_t sector) ;

const char * TAG="FLASH";

static struct DataBuffer dataBuffer;


void initFlash(void) {
    esp_err_t espError;

    periodLed.start.hour = 6;
    periodLed.start.minute = 30;
    periodLed.end.hour = 21;
    periodLed.end.minute = 0;


    espError = nvs_flash_init();
    if (espError == ESP_OK){
        nvs_handle nvsHandle;

        espError = nvs_open("Vase2_0", NVS_READONLY, &nvsHandle);
        if (espError == ESP_OK){
            uint8_t  value;
            if (nvs_get_u8(nvsHandle, "start_hour", &value )==ESP_OK){
                periodLed.start.hour = value;
            }
            if (nvs_get_u8(nvsHandle, "start_minute", &value )==ESP_OK){
                periodLed.start.minute = value;
            }
            if (nvs_get_u8(nvsHandle, "end_hour", &value )==ESP_OK){
                periodLed.end.hour = value;
            }
            if (nvs_get_u8(nvsHandle, "end_minute", &value )==ESP_OK){
                periodLed.end.minute = value;
            }

            nvs_close(nvsHandle);
        }
    }
}

struct DataSample *getSamples(uint16_t sector) {
    readSector(sector);
    return dataBuffer.samples;
}

int16_t getLastSample() {
    uint16_t sampleIndex = -1;
    uint8_t *mapIter = dataBuffer.map;
    uint8_t *mapIterEnd = dataBuffer.map + BYTES_MAP;
    for (; mapIter != mapIterEnd; mapIter++) {
        if (*mapIter != FULL_MAP) {
            switch (*mapIter) {
                case 0xFE:
                    sampleIndex += 1;
                    break;
                case 0xFC:
                    sampleIndex += 2;
                    break;
                case 0xF8:
                    sampleIndex += 3;
                    break;
                case 0xF0:
                    sampleIndex += 4;
                    break;
                case 0xE0:
                    sampleIndex += 5;
                    break;
                case 0xC0:
                    sampleIndex += 6;
                    break;
                case 0x80:
                    sampleIndex += 7;
                    break;
                case 0xFF:
                    break;
                default:
                    ESP_LOGE(TAG, "invalid map value %d at index %d", *mapIter,(uint16_t )(mapIter - dataBuffer.map));
            }
            return sampleIndex;
        }
        sampleIndex += 8;
    }
    return BYTES_MAP * 8 - 1;
}


uint16_t getLastSector(time_t before) {
    time_t newerTime = 0;
    uint16_t newerSector = 0;
    for (uint16_t sector = 0xF0; sector <= 0xFF; sector++) {
        readSector(sector);
        uint16_t sampleIndex = getLastSample();
        if (sampleIndex!=0xFFFF) {
            time_t lastTime = dataBuffer.samples[sampleIndex].timestamp;
            ESP_LOGI(TAG, "sample index: %d, time %ld\n", sampleIndex, lastTime);
            if (lastTime > newerTime && lastTime <= before) {
                newerTime = lastTime;
                newerSector = sector;
            }
        } else {
            ESP_LOGI(TAG,"sector %d is empty\n", sector);
        }
    }
    return newerSector;
}

static void readSector(uint16_t sector) {
    ESP_LOGI(TAG, "Read sector %02X\n", sector);
    esp_err_t error =
            spi_flash_read((uint32_t)sector * 0x1000, (uint32_t *)&dataBuffer, sizeof(struct DataBuffer));
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "error reading sector %d: %d\n", sector, error);
        memset(&dataBuffer.map, 0xFF, BYTES_MAP);
    }
}
