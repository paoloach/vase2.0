//
// Created by paolo on 25/01/19.
//

#ifndef VASO2_TASKSNTP_H
#define VASO2_TASKSNTP_H


#include "esp8266.h"

void sntpTask(void *pvParameters);
struct TimeLed {
    int8_t  hour;
    int8_t  minute;
};

struct PeriodLed {
    struct TimeLed start;
    struct TimeLed end;
};

extern struct PeriodLed periodLed;
extern bool wifiOn;

void initIO();
void onLight();
void offLight();

#endif //VASO2_TASKSNTP_H
