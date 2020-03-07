/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "wifi.h"
#include "Flash.h"
#include "taskSNTP.h"
#include "Light.h"
#include "MCP3201.h"
#include "Settings.h"
#include "SoilService.h"

static const char * TAG="Main";

void app_main() {
    printf("Starting!\n");
    uint8_t mac[8];

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    esp_efuse_mac_get_default(mac);
    ESP_LOGI(TAG, "Default mac: "MACSTR, MAC2STR(mac));

    initSettings();
    initFlash();
    initWifi();
    initAdc();
    startSntpTask();
    startLight();
    initSoilServiceTask();
}
