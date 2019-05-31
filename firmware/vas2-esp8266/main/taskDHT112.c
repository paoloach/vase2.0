//
// Created by paolo on 01/02/19.
//

#include <espressif/esp_common.h>

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <dht.h>
#include "Pins.h"
#include "taskDHT112.h"


int16_t humidity;
int16_t temperature;
int16_t soil;

#define FULL_MAP    0x00
#define EMPTY_MAP   0xFF

static struct DataBuffer dataBuffer;

static void saveData();

static uint16_t findSector(time_t ts);

static void initSector(uint16_t sector, time_t ts);

static bool isFullSector();

static void readSector(uint16_t sector);

#define SAMPLE_TIME 10
//#define SAMPLE_TIME 300

void dht112Task(void *pvParameters) {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);

    while (true) {
        bool status = dht_read_data(DHT_TYPE_DHT11, DHT112_LED, &humidity, &temperature);
        if (!status) {
            printf("error reading data from DHT22\n");
        } else {
            saveData();
        }
        vTaskDelay(SAMPLE_TIME*1000 / portTICK_PERIOD_MS);
    }
}


struct DataBuffer {
    uint8_t map[BYTES_MAP];
    struct DataSample samples[MAX_SAMPLES];
}  __attribute__ ((packed));


// every 4096 flash block contains
// 32 bytes for map -> 32*8= 256 samples
// 254 x 16 bytes for data:
//      8 bytes timestamp
//      2 bytes humidity
//      2 bytes temperature
//      2 bytes soil moisture
// use block F0, ... , FF
void saveData() {
    time_t ts = time(NULL);
    uint16_t sector = findSector(ts);
    uint16_t sampleIndex = getLastSample() + 1;
    uint8_t mapIndex = sampleIndex / 8;
    uint8_t bit = sampleIndex % 8;
    uint8_t mapBit = ~(1 << bit);
    uint32_t samplePos = sector * 0x1000 + BYTES_MAP + sampleIndex * sizeof(struct DataSample);
    struct DataSample dataSample;
    dataSample.temperature = temperature;
    dataSample.humidity = humidity;
    dataSample.soil = sdk_system_adc_read();
    dataSample.timestamp = ts;
    sdk_SpiFlashOpResult result = sdk_spi_flash_write(samplePos, (uint32_t *) &dataSample, sizeof(struct DataSample));
    dataBuffer.map[mapIndex] &= mapBit;
    if (result == SPI_FLASH_RESULT_OK ) {
        printf("Write sample index %d, at  %06X, map offset %d, bit %d, new map %02X\n", sampleIndex, samplePos,
               mapIndex, bit, dataBuffer.map[mapIndex]);
        sdk_spi_flash_write((uint32_t) sector * 0x1000, (uint32_t *) &dataBuffer.map, BYTES_MAP);
    } else {
        printf("--------ERROR-------- writing sample index %d at %06X: error code %d\n", sampleIndex, samplePos, result);
    }

}

uint16_t findSector(time_t ts) {
    for (uint16_t sector = 0xF0; sector <= 0xFF; sector++) {
        readSector(sector);
        if (dataBuffer.samples[0].timestamp == 0) {
            initSector(sector, ts);
            return sector;
        }
        if (!isFullSector()) {
            return sector;
        }
    }
    time_t oldest = 0xFFFFFFFF;
    uint16_t oldestSector = 0xF0;
    for (uint16_t sector = 0xF0; sector <= 0xFF; sector++) {
        readSector(sector);
        if (dataBuffer.samples[0].timestamp < oldest) {
            oldest = dataBuffer.samples[0].timestamp;
            oldestSector = sector;
        }
    }

    initSector(oldestSector, ts);
    return oldestSector;
}

static void initSector(uint16_t sector, time_t ts) {
    sdk_spi_flash_erase_sector(sector);
    memset(dataBuffer.map, 0xFF, BYTES_MAP);
    memset(dataBuffer.samples, 0xFF, MAX_SAMPLES * sizeof(struct DataSample));
    sdk_spi_flash_write((uint32_t) sector * 0x1000, (uint32_t *) &dataBuffer, sizeof(struct DataBuffer));
}

uint16_t getLastSector(time_t before) {
    time_t newerTime = 0;
    uint16_t newerSector = 0;
    for (uint16_t sector = 0xF0; sector <= 0xFF; sector++) {
        readSector(sector);
        uint16_t sampleIndex = getLastSample();
        time_t lastTime = dataBuffer.samples[sampleIndex].timestamp;

        if (lastTime > newerTime &&  lastTime <= before) {
            newerTime = lastTime;
            newerSector = sector;
        }
    }
    return newerSector;
}

struct DataSample *getSamples(uint16_t sector) {
    readSector(sector);
    return dataBuffer.samples;
}

static bool isFullSector() {
    for (int mapIndex = 0; mapIndex < BYTES_MAP; mapIndex++) {
        if (dataBuffer.map[mapIndex] != FULL_MAP)
            return false;
    }
    return true;
}

int16_t getLastSample() {
    uint16_t sampleIndex = -1;
    uint8_t * mapIter = dataBuffer.map;
    uint8_t * mapIterEnd = dataBuffer.map+BYTES_MAP;
    for (;mapIter != mapIterEnd; mapIter++){
        if (*mapIter != FULL_MAP) {
            switch(*mapIter){
                case 0xFE:
                    sampleIndex+=1;
                    break;
                case 0xFC:
                    sampleIndex+=2;
                    break;
                case 0xF8:
                    sampleIndex+=3;
                    break;
                case 0xF0:
                    sampleIndex+=4;
                    break;
                case 0xE0:
                    sampleIndex+=5;
                    break;
                case 0xC0:
                    sampleIndex+=6;
                    break;
                case 0x80:
                    sampleIndex+=7;
                    break;
            }
            return sampleIndex;
        }
        sampleIndex+=8;
    }
    return BYTES_MAP * 8 - 1;
}

static void readSector(uint16_t sector) {
    printf("Read sector %02X\n", sector);
    sdk_spi_flash_read((uint32_t) sector * 0x1000, (uint32_t *) &dataBuffer, sizeof(struct DataBuffer));
}
