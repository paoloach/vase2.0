#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <libesphttpd/platform.h>
#include <espressif/esp_common.h>
#include <libesphttpd/cgiwebsocket.h>
#include <esp/uart.h>
#include <FreeRTOS.h>
#include <ssid_config.h>
#include <task.h>

#include "taskSNTP.h"
#include "WifiTask.h"

#define OFF (const void *)0
#define ON (const void *)1
#define START_TIME (const void *)0
#define END_TIME (const void *)1

static struct TimeLed decodeTime(char * data);
static void sendChar(char * buffer,  int8_t c);
static void sendTimeLed(HttpdConnData * connData,  struct TimeLed * timeLed);

/* FreeRTOS event group to signal when we are connected & ready to make a request */

int ICACHE_FLASH_ATTR whoAreYou(HttpdConnData *connData) {
    if (connData->conn == NULL) {
        //Connection aborted. Clean up.
        return HTTPD_CGI_DONE;
    }
    if (connData->requestType != HTTPD_METHOD_GET) {
        httpdStartResponse(connData, 406);  //http error code 'unacceptable'
        httpdEndHeaders(connData);
        return HTTPD_CGI_DONE;
    }
    httpdStartResponse(connData, 200);
    httpdHeader(connData, "Content-Type", "text/plain");
    httpdEndHeaders(connData);

    httpdSend(connData, "I am Vase 2.0\n", -1);

    return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR getStatus(HttpdConnData *connData) {
    if (connData->conn == NULL) {
        return HTTPD_CGI_DONE;
    }
    if (connData->requestType != HTTPD_METHOD_GET) {
        httpdStartResponse(connData, 406);  //http error code 'unacceptable'
        httpdEndHeaders(connData);
        return HTTPD_CGI_DONE;
    }
    httpdStartResponse(connData, 200);
    httpdHeader(connData, "Content-Type", "text/plain");
    httpdEndHeaders(connData);

    httpdSend(connData, "light ", 6);
    if (lightOn)
        httpdSend(connData, "on\n", 3);
    else
        httpdSend(connData, "off\n", 4);
    httpdSend(connData, "start: ", 7);
    sendTimeLed(connData, &periodLed.start);
    httpdSend(connData, "\nend: ", 6);
    sendTimeLed(connData, &periodLed.end);
    time_t ts = time(NULL);
    struct tm * localTime = localtime(&ts);
    struct TimeLed tl;
    tl.hour = localTime->tm_hour;
    tl.minute = localTime->tm_min;
    httpdSend(connData, "\ntime: ",7);
    sendTimeLed(connData, &tl);
    httpdSend(connData, "\n",1);
    return HTTPD_CGI_DONE;
}


int ICACHE_FLASH_ATTR onOff(HttpdConnData *connData) {
    if (connData->conn == NULL) {
        //Connection aborted. Clean up.
        return HTTPD_CGI_DONE;
    }
    if (connData->requestType != HTTPD_METHOD_POST) {
        httpdStartResponse(connData, 406);  //http error code 'unacceptable'
        httpdEndHeaders(connData);
        return HTTPD_CGI_DONE;
    }
    if (connData->cgiArg == OFF) {
        offLight();
        overWriteLight = OFF_LIGHT;
    } else {
        onLight();
        overWriteLight = ON_LIGHT;
    }
    httpdStartResponse(connData, 201);
    httpdEndHeaders(connData);
    return HTTPD_CGI_DONE;
}



int ICACHE_FLASH_ATTR startEndTime(HttpdConnData *connData) {
    if (connData->conn == NULL) {
        //Connection aborted. Clean up.
        return HTTPD_CGI_DONE;
    }
    if (connData->requestType != HTTPD_METHOD_POST) {
        httpdStartResponse(connData, 406);  //http error code 'unacceptable'
        httpdEndHeaders(connData);
        return HTTPD_CGI_DONE;
    }

    if (connData->post != NULL) {
        printf("Post data: %s\n", connData->post->buff);
        struct TimeLed startTime = decodeTime(connData->post->buff);
        if (startTime.minute != -1){
            if (connData->cgiArg == START_TIME)
                periodLed.start = startTime;
            else
                periodLed.end = startTime;
            httpdStartResponse(connData, 201);
        } else {
            httpdStartResponse(connData, 400);
        }
    } else {
        httpdStartResponse(connData, 400);
    }

    httpdEndHeaders(connData);
    return HTTPD_CGI_DONE;
}

static void sendTimeLed(HttpdConnData * connData,  struct TimeLed * timeLed) {
    char buffer[5];
    sendChar(buffer, timeLed->hour);
    buffer[2]=':';
    sendChar(buffer+3, timeLed->minute);
    httpdSend(connData, buffer, 5);
}

static void sendChar(char * buffer,  int8_t c) {
    buffer[0] = c/10 + '0';
    buffer[1] = c%10 + '0';
}


// Expexted format: hh:mm
struct TimeLed decodeTime(char * data){
    struct TimeLed result;
    result.hour = result.minute = -1;
    if (data[2] != ':')
        return result;
    char h1 = data[0];
    if (h1 != '0' && h1 !='1' && h1 != '2')
        return result;
    result.hour = (h1-'0')*10;
    char h2 = data[1];
    if (h2 < '0' || h2 > '9')
        return result;
    result.hour += h2-'0';
    if (result.hour>24)
        return result;
    char m1 = data[3];
    if (m1 < '0' || m1 >= '6'){
        return result;
    }
    result.minute = (m1-'0')*10;
    char m2 = data[4];
    if (m2 <'0' || m2 > '9'){
        result.minute = -1;
        return result;
    }
    result.minute += m2- '0';
    if (result.minute >= 60){
        result.minute = -1;
    }
    return  result;
}

HttpdBuiltInUrl builtInUrls[] = {
    {"*", cgiRedirectApClientToHostname, "esp8266.nonet"},
    {"/who_are_you", whoAreYou, NULL},
    {"/on",          onOff,     ON},
    {"/off",         onOff,     OFF},
    {"/status",      getStatus, NULL},
    {"/onTime",      startEndTime,     START_TIME},
    {"/offTime",     startEndTime,     END_TIME},
};

void user_init(void) {
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    initIO();


    captdnsInit();

    httpdInit(builtInUrls, 80);
    xTaskCreate(sntpTask, "SNTP", 1024, NULL, 1, NULL);
    xTaskCreate(wifiTask, "wifiTask", 512, NULL, 1, NULL);

}