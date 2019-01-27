//
// Created by paolo on 25/01/19.
//

#include <FreeRTOS.h>
#include <task.h>
#include <espressif/esp_common.h>

#include "Pins.h"

static bool ledStatus = false;
static uint8_t counter = 5;

bool wifiOn=false;

void wifiTask(void *args) {

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    gpio_enable(WIFI_LED, GPIO_OUTPUT);
    gpio_write(WIFI_LED, 0);

    /* Wait until we have joined AP and are assigned an IP */
    while (1) {
        switch (sdk_wifi_station_get_connect_status()) {
            case STATION_GOT_IP:
                ledStatus = true;
                wifiOn=true;
                break;
            case STATION_CONNECTING:
                counter--;
                if (counter == 0) {
                    counter = 5;
                    ledStatus = !ledStatus;
                }
                wifiOn=false;
                break;
            default:
                ledStatus = !ledStatus;
                wifiOn=false;
                break;
        }
        gpio_write(WIFI_LED, ledStatus);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}