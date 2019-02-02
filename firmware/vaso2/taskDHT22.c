//
// Created by paolo on 01/02/19.
//

#include <espressif/esp_common.h>
#include <esp/uart.h>

#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>

#include <ssid_config.h>
#include <dht.h>
#include "Pins.h"


int16_t humidity;
int16_t temperature;
void dht22Task(void *pvParameters){
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);

    while(true){
        bool status = dht_read_data(DHT_TYPE_DHT11,DHT22_LED, &humidity, &temperature );
        if (status){
            printf("humidity: %d, temperature: %d\n", humidity, temperature);
        } else {
            printf("error reading data from DHT22\n");
        }
        vTaskDelay(10000/portTICK_PERIOD_MS);
    }
}