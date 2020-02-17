//
// Created by paolo on 18/12/19.
//

#ifndef VASO2_SETTINGS_H
#define VASO2_SETTINGS_H



#include <stdint.h>

extern uint16_t sampleIntervalMinutes;
extern char  Wifi_SSID[32];
extern char  Wifi_Passwd[64];

void initSettings();
void setSampleIntervalMinutes(uint16_t newCheckInterval);
void savePeriodLed(void);

#endif //VASO2_SETTINGS_H
