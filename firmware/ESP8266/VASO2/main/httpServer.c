//
// Created by paolo on 30/05/19.
//
#include <esp_log.h>
#include <nvs_flash.h>
//#include "taskDHT112.h"
#include "Flash.h"


#include <esp_http_server.h>
#include <freertos/event_groups.h>
#include "MCP3201.h"
#include "Light.h"
#include "Settings.h"
#include "wifi.h"


static const char *STATUS_200 = "200 OK";
static const char *STATUS_201 = "201 CREATE";
static const char *STATUS_400 = "400 BAD REQUEST";

static const char *TAG = "HttpServer";
httpd_handle_t server = NULL;

static esp_err_t methodNotSupported(httpd_req_t *connData);

static void sendTimeLed(char *buffer, struct TimeLed *timeLed);

static void sendChar(char *buffer, uint8_t c);

static struct TimeLed decodeTime(char *data);

static int commonGet(httpd_req_t *connData, const char *field, int16_t value);

esp_err_t aboutHandler(httpd_req_t *req) {
    if (req->method != HTTP_GET) {
        return methodNotSupported(req);
    }
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, "Vase 2.0\n", -1);

    return ESP_OK;
}

esp_err_t postOnHandler(httpd_req_t *req) {
    onLight();
    setOverwriteLightStatus(ON_LIGHT);
    httpd_resp_set_status(req, STATUS_201);
    httpd_resp_send(req, "\n", -1);
    return ESP_OK;
}

esp_err_t postOffHandler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Off handler");
    offLight();
    setOverwriteLightStatus(OFF_LIGHT);
    httpd_resp_set_status(req, STATUS_201);
    httpd_resp_send(req, "\n", -1);
    return ESP_OK;
}

esp_err_t getStatusHandler(httpd_req_t *req) {
    char *buffer = malloc(100);
    buffer[0] = 0;
    httpd_resp_set_status(req, STATUS_200);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    strcat(buffer, "light");

    if (isLightOn())
        strcat(buffer, "on\n");
    else
        strcat(buffer, "off\n");

    strcat(buffer, "start: ");
    sendTimeLed(buffer + strlen(buffer), &periodLed.start);
    strcat(buffer, "\nend: ");
    sendTimeLed(buffer + strlen(buffer), &periodLed.end);
    time_t ts = time(NULL);
    struct tm *localTime = localtime(&ts);
    struct TimeLed tl;
    tl.hour = localTime->tm_hour;
    tl.minute = localTime->tm_min;
    strcat(buffer, "\ntime: ");
    sendTimeLed(buffer + strlen(buffer), &tl);
    strcat(buffer, "\n");
    httpd_resp_send(req, buffer, -1);
    free(buffer);
    return ESP_OK;
}

esp_err_t postStartEndTimeHandler(httpd_req_t *req) {
    char buf[5];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret == 5) {
        struct TimeLed startTime = decodeTime(buf);
        if (startTime.minute != UINT8_MAX) {
            if (req->user_ctx == 0)
                periodLed.start = startTime;
            else
                periodLed.end = startTime;
            httpd_resp_set_status(req, STATUS_201);
        } else {
            httpd_resp_set_status(req, STATUS_400);
        }
    } else {
        httpd_resp_set_status(req, STATUS_400);
    }

    httpd_resp_send(req, "\n", 1);

    savePeriodLed();

    return ESP_OK;
}

//esp_err_t httpTemperatureHandler(httpd_req_t *connData) {
//  return commonGet(connData, "temperature", temperature);
//}
//
//esp_err_t httpHumidityHandler(httpd_req_t *connData) {
//  return commonGet(connData, "humidity", humidity);
//}

esp_err_t getSoilMoistureHandler(httpd_req_t *connData) {
    uint16_t soilMoisture = adcRead();
    return commonGet(connData, "soilMoisture", soilMoisture);
}

esp_err_t commonGet(httpd_req_t *req, const char *field, int16_t value) {
    char buffer[40];
    httpd_resp_set_status(req, STATUS_200);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    strcpy(buffer, field);
    sprintf(buffer + strlen(buffer), ": %d\n", value);
    httpd_resp_send(req, buffer, -1);
    return ESP_OK;
}


esp_err_t setSampleIntervalHandler(httpd_req_t *req) {
    char buf[10];
    char *end;
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret > 0) {
        uint16_t newInterval = strtol(buf, &end, 10);
        if (end > buf) {
            if (newInterval > 0) {
                setSampleIntervalMinutes(newInterval);
            } else {
                ESP_LOGE(TAG, "Invalid negative value %s", buf);
                httpd_resp_send(req, "Invalid negative value\n\r", -1);
                httpd_resp_set_status(req, STATUS_400);
            }
        } else {
            ESP_LOGE(TAG, "Invalid value: %s", buf);
            httpd_resp_send(req, "Invalid value\n\r", -1);
            httpd_resp_set_status(req, STATUS_400);
        }
    }
    httpd_resp_send(req, "\n\r", -1);
    return ESP_OK;
}


httpd_handle_t start_webserver(void) {
    httpd_handle_t httpServer = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 15;

    httpd_uri_t getWhoAreYou = {.uri = "/who_are_you", .method = HTTP_GET, .handler = aboutHandler, .user_ctx = NULL};
    httpd_uri_t getAbout = {.uri = "/about", .method = HTTP_GET, .handler = aboutHandler, .user_ctx = NULL};
    httpd_uri_t postOn = {.uri = "/on", .method = HTTP_POST, .handler = postOnHandler, .user_ctx = NULL};
    httpd_uri_t postOff = {.uri = "/off", .method = HTTP_POST, .handler = postOffHandler, .user_ctx = NULL};
    httpd_uri_t getStatus = {.uri = "/status", .method = HTTP_GET, .handler = getStatusHandler, .user_ctx = NULL};
    httpd_uri_t postOnTime = {.uri = "/onTime", .method = HTTP_POST, .handler = postStartEndTimeHandler, .user_ctx = 0};
    httpd_uri_t postOffTime = {.uri = "/offTime", .method = HTTP_POST, .handler = postStartEndTimeHandler, .user_ctx = (void *) 1};
    httpd_uri_t getSoil = {.uri = "/soil", .method = HTTP_GET, .handler = getSoilMoistureHandler, .user_ctx = NULL};
    httpd_uri_t setSampleInterval = {.uri = "/sampleInterval", .method = HTTP_POST, .handler = setSampleIntervalHandler, .user_ctx = NULL};
    httpd_uri_t getData = {.uri = "/data", .method = HTTP_GET, .handler = httpData, .user_ctx = 0};


    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&httpServer, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(httpServer, &getWhoAreYou);
        httpd_register_uri_handler(httpServer, &getAbout);
        httpd_register_uri_handler(httpServer, &getStatus);
        httpd_register_uri_handler(httpServer, &getSoil);
        httpd_register_uri_handler(httpServer, &getData);
        httpd_register_uri_handler(httpServer, &postOn);
        httpd_register_uri_handler(httpServer, &postOff);
        httpd_register_uri_handler(httpServer, &postOnTime);
        httpd_register_uri_handler(httpServer, &postOffTime);
        httpd_register_uri_handler(httpServer, &setSampleInterval);
        ESP_LOGI(TAG, "HTTP SERVER STARTED");
        return httpServer;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(void) {
    ESP_LOGI(TAG, "Web server stop");
    httpd_stop(server);
}

esp_err_t methodNotSupported(httpd_req_t *connData) {
    const char *msg = "Request method is not supported by server\n";
    const char *status = "501 Method Not Implemented\n";
    httpd_resp_set_status(connData, status);
    httpd_resp_set_type(connData, HTTPD_TYPE_TEXT);
    return httpd_resp_send(connData, msg, strlen(msg));
}

static void sendTimeLed(char *buffer, struct TimeLed *timeLed) {
    sendChar(buffer, timeLed->hour);
    buffer[2] = ':';
    sendChar(buffer + 3, timeLed->minute);
    buffer[5] = 0;
}

static void sendChar(char *buffer, uint8_t c) {
    buffer[0] = c / 10 + '0';
    buffer[1] = c % 10 + '0';
}

// Expexted format: hh:mm
static struct TimeLed decodeTime(char *data) {
    struct TimeLed result;
    result.hour = result.minute = UINT8_MAX;
    if (data[2] != ':')
        return result;
    char h1 = data[0];
    if (h1 != '0' && h1 != '1' && h1 != '2')
        return result;
    result.hour = (h1 - '0') * 10;
    char h2 = data[1];
    if (h2 < '0' || h2 > '9')
        return result;
    result.hour += h2 - '0';
    if (result.hour > 24)
        return result;
    char m1 = data[3];
    if (m1 < '0' || m1 >= '6') {
        return result;
    }
    result.minute = (m1 - '0') * 10;
    char m2 = data[4];
    if (m2 < '0' || m2 > '9') {
        result.minute = -1;
        return result;
    }
    result.minute += m2 - '0';
    if (result.minute >= 60) {
        result.minute = -1;
    }
    return result;
}
