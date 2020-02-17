//
// Created by paolo on 25/01/19.
//

#ifndef VASO2_TASKSNTP_H
#define VASO2_TASKSNTP_H




void startSntpTask();
void lightTask(void *pvParameters);
void initIO();
void onLight();
void offLight();
void savePeriodLed();

#endif //VASO2_TASKSNTP_H
