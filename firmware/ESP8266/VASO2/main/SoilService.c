//
// Created by paolo on 18/12/19.
//

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "SoilService.h"
#include "Settings.h"
#include "MCP3201.h"

static const char * TAG="SoilService";
static void soilServiceTask(void *);

void initSoilServiceTask() {
    xTaskCreate(soilServiceTask, "soil task", 1024, NULL, 10, NULL);
}

static void soilServiceTask(void *args){
    while(1){
        vTaskDelay(((ulong )soilCheckInterval*1000) / portTICK_PERIOD_MS);
        uint16_t soil = adcRead();
        ESP_LOGI(TAG, "soil: %d", soil);
    }
}
