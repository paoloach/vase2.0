//
// Created by paolo on 13/12/19.
//

#include <driver/gpio.h>
#include <stdbool.h>
#include <esp_log.h>
#include <unistd.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "MCP3201.h"
#include "Pins.h"

static const char * TAG="ADC";

void adcTask(void *arg);

uint16_t adcRead() {
    uint16_t result=0;
    gpio_set_level(ADC_SEL, 0);
    ets_delay_us(1);
    gpio_set_level(ADC_CLK, 1);
    ets_delay_us(1);
    gpio_set_level(ADC_CLK, 0);
    ets_delay_us(1);
    gpio_set_level(ADC_CLK, 1);
    ets_delay_us(1);
    gpio_set_level(ADC_CLK, 0);
    ets_delay_us(1);
    gpio_set_level(ADC_CLK, 1);
    ets_delay_us(1);
    result = gpio_get_level(ADC_DATA);
    for(int i=0; i< 12; i++){
        gpio_set_level(ADC_CLK, 0);
        ets_delay_us(1);
        gpio_set_level(ADC_CLK, 1);
        ets_delay_us(1);
        result = (result << 1) | gpio_get_level(ADC_DATA);
    }
    gpio_set_level(ADC_SEL, 1);
    ets_delay_us(1);
    if (result & 0x1000){
        ESP_LOGE(TAG, "error reading ADC: nullbit is 1");
    }
    return result & 0xFFF;
}

void initAdc() {
    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = BIT(ADC_CLK) | BIT(ADC_SEL)  ;
    gpioConfig.mode = GPIO_MODE_DEF_OUTPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);

    gpioConfig.pin_bit_mask = BIT(ADC_DATA) ;
    gpioConfig.mode = GPIO_MODE_DEF_INPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpioConfig);



    gpio_set_level(ADC_SEL, 1);
    gpio_set_level(ADC_CLK, 0);
}

