//
// Created by paolo on 18/12/19.
//

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "SoilService.h"
#include "Settings.h"
#include "MCP3201.h"
#include "Flash.h"
#include "wifi.h"
#include "GroupSignals.h"

static const char * TAG="SoilService";
static void soilServiceTask(void *);

void initSoilServiceTask() {
    xTaskCreate(soilServiceTask, "soil task", 1024, NULL, 10, NULL);
}

static void soilServiceTask(void *args){
    struct DataSample sample;
    ESP_LOGI(TAG, "Sample interval: %d minute", sampleIntervalMinutes);
    xEventGroupWaitBits(wifi_event_group, TIME_VALID,  false, true, portMAX_DELAY);
    while(1){
        vTaskDelay(((ulong )sampleIntervalMinutes * 60000) / portTICK_PERIOD_MS);
        uint16_t soil = adcRead();
        ESP_LOGI(TAG, "soil: %d", soil);
        sample.soil = soil;
        saveSample(&sample);
    }
}
