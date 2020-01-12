//
// Created by paolo on 16/12/19.
//

#include <driver/gpio.h>
#include <stdbool.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <sys/time.h>
#include <time.h>
#include "Light.h"
#include "Pins.h"

static bool lightOn = false;
static char *TAG = "Light";
enum OverwriteLightStatus overwriteLightStatus;


static bool isOn(void);
static void initIO(void);
static void lightTask(void * arg);


void startLight(void ){
    initIO();
    xTaskCreate(lightTask, "light task", 1024, NULL, 10, NULL);
}

void setOverwriteLightStatus(enum OverwriteLightStatus newOverwriteStatus) {
    overwriteLightStatus = newOverwriteStatus;
}

void lightTask(void *arg) {
    ESP_LOGI(TAG, "Starting LIght task ");
    initIO();

    while (1) {
        if (isOn()) {
            switch (overwriteLightStatus) {
                case NONE:
                    onLight();
                    break;
                case ON_LIGHT:
                    onLight();
                    overwriteLightStatus = NONE;
                    break;
                default:;
            }
        } else {
            switch (overwriteLightStatus) {
                case NONE:
                    offLight();
                    break;
                case OFF_LIGHT:
                    offLight();
                    overwriteLightStatus = NONE;
                    break;
                default:;
            }
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}


static bool isOn(void) {
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);

    struct tm *time = localtime(&timeVal.tv_sec);
    if (time->tm_hour < periodLed.start.hour)
        return false;
    if (time->tm_hour == periodLed.start.hour &&
        time->tm_min < periodLed.start.minute)
        return false;
    if (time->tm_hour > periodLed.end.hour)
        return false;
    if (time->tm_hour == periodLed.end.hour &&
        time->tm_min > periodLed.end.minute)
        return false;
    return true;
}

void initIO(void) {
    ESP_LOGI(TAG, "Init light");
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(LED_PIN);
    gpioConfig.mode = GPIO_MODE_DEF_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);
    offLight();
}


void onLight(void) {
    ESP_LOGI(TAG, "Light on");
    gpio_set_level(LED_PIN, 1);
    lightOn = true;
}

void offLight(void) {
    ESP_LOGI(TAG, "Light off");
    gpio_set_level(LED_PIN, 0);
    lightOn = false;
}

bool isLightOn() {
    return lightOn;
}
