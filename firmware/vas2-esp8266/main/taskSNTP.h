//
// Created by paolo on 25/01/19.
//

#ifndef VASO2_TASKSNTP_H
#define VASO2_TASKSNTP_H



enum OverWriteLight {
    NONE,
    ON_LIGHT,
    OFF_LIGHT
};

struct TimeLed {
    int8_t  hour;
    int8_t  minute;
};

struct PeriodLed {
    struct TimeLed start;
    struct TimeLed end;
};

extern struct PeriodLed periodLed;
extern bool lightOn;
extern enum OverWriteLight overWriteLight;

void sntpStart(void *pvParameters);
void initIO();
void onLight();
void offLight();
void savePeriodLed();

#endif //VASO2_TASKSNTP_H
