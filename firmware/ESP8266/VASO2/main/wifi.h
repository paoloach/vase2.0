//
// Created by paolo on 31/05/19.
//

#ifndef VAS2_ESP8266_WIFI_H
#define VAS2_ESP8266_WIFI_H

#include <freertos/event_groups.h>

void initWifi();

extern EventGroupHandle_t wifi_event_group;

#endif //VAS2_ESP8266_WIFI_H
