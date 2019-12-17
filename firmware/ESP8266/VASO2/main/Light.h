//
// Created by paolo on 16/12/19.
//

#ifndef VASO2_LIGHT_H
#define VASO2_LIGHT_H

enum OverwriteLightStatus {
    NONE,
    ON_LIGHT,
    OFF_LIGHT
};

struct TimeLed {
    uint8_t  hour;
    uint8_t  minute;
};

struct PeriodLed {
    struct TimeLed start;
    struct TimeLed end;
};

void startLight(void );
void setOverwriteLightStatus(enum OverwriteLightStatus newOverwriteStatus);
void onLight(void);
void offLight(void);
void savePeriodLed(void);
bool isLightOn();

extern struct PeriodLed periodLed;

#endif //VASO2_LIGHT_H
