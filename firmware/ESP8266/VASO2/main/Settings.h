//
// Created by paolo on 18/12/19.
//

#ifndef VASO2_SETTINGS_H
#define VASO2_SETTINGS_H



#include <stdint.h>

extern uint16_t soilCheckInterval;
extern char  Wifi_SSID[32];
extern char  Wifi_Passwd[64];

void initSettings();
void setSolCheckInterval(uint16_t newCheckInterval);

#endif //VASO2_SETTINGS_H
