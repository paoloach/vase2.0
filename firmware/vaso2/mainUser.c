#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <libesphttpd/platform.h>
#include <espressif/esp_common.h>
#include <libesphttpd/cgiwebsocket.h>
#include <esp/uart.h>
#include <FreeRTOS.h>
#include <ssid_config.h>
#include <task.h>

#include "taskSNTP.h"

#define OFF (const void *)0
#define ON (const void *)1
#define START_TIME (const void *)0
#define END_TIME (const void *)1

void wifiTask(void *args);

static struct TimeLed decodeTime(char * data);

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
    httpdHeader(connData, "Content-Type", "text/html");
    httpdEndHeaders(connData);

    //We're going to send the HTML as two pieces: a head and a body. We could've also done
    //it in one go, but this demonstrates multiple ways of calling httpdSend.
    //Send the HTML head. Using -1 as the length will make httpdSend take the length
    //of the zero-terminated string it's passed as the amount of data to send.
    httpdSend(connData, "I am Vaso 2.0\n", -1);

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
    if (connData->cgiArg == OFF)
        offLight();
    else
        onLight();
    httpdStartResponse(connData, 201);
    httpdEndHeaders(connData);
    return HTTPD_CGI_DONE;
}

int ICACHE_FLASH_ATTR setStartEndTime(HttpdConnData *connData) {
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
    {"/", cgiRedirect, "/index.tpl"},
    {"/who_are_you", whoAreYou, NULL},
    {"/on",          onOff,     ON},
    {"/off",         onOff,     OFF},
    {"/onTime",      setStartEndTime,     START_TIME},
    {"/offTime",     setStartEndTime,     END_TIME},
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