//
// Created by paolo on 14/01/19.
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


/* Add extras/sntp component to makefile for this include to work */
#include <sntp.h>
#include <time.h>
#include "Pins.h"
#include "taskSNTP.h"

#define SNTP_SERVERS    "0.pool.ntp.org", "1.pool.ntp.org", \
                                                "2.pool.ntp.org", "3.pool.ntp.org"

#define vTaskDelayMs(ms)        vTaskDelay((ms)/portTICK_PERIOD_MS)
#define UNUSED_ARG(x)   (void)x

bool wifiOn;
struct PeriodLed periodLed;
static struct tm *localTime;

static bool isOn(void);

void sntpTask(void *pvParameters)
{
    char *servers[] = {SNTP_SERVERS};
    UNUSED_ARG(pvParameters);



    /* Wait until we have joined AP and are assigned an IP */
    while (wifiOn == false){
        vTaskDelay(100/portTICK_PERIOD_MS);
    }


    /* Start SNTP */
    printf("Starting SNTP... ");
    sntp_set_update_delay(24*60*60*1000);
    /* Set GMT+1 zone, daylight savings off */
    const struct timezone tz = {1*60, 0};
    /* SNTP initialization */
    sntp_initialize(&tz);
    /* Servers must be configured right after initialization */
    sntp_set_servers(servers, sizeof(servers) / sizeof(char*));
    printf("DONE!\n");

    /* Print date and time each 5 seconds */
    while(1) {
        vTaskDelayMs(5000);
        if (isOn()){
            onLight();
        } else {
            offLight();
        }
    }
}

static bool isOn(void) {
    time_t ts = time(NULL);
    localTime = localtime(ts);
    if (localTime->tm_hour < periodLed.start.hour)
        return false;
    if (localTime->tm_hour == periodLed.start.hour && localTime->tm_min < periodLed.start.minute)
        return false;
    if (localTime->tm_hour > periodLed.end.hour)
        return false;
    if (localTime->tm_hour == periodLed.end.hour && localTime->tm_min > periodLed.end.minute)
        return false;
    return true;
}

void initIO() {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    gpio_enable(LED_PIN, GPIO_OUTPUT);
    gpio_enable(14, GPIO_OUTPUT);
    offLight();
}

void onLight(){
    printf("LED On\n");
    gpio_write(LED_PIN, true);
    gpio_write(14, true);
}
void offLight() {
    printf("LED Off\n");
    gpio_write(LED_PIN, false);
    gpio_write(14, false);
}