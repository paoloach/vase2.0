//
// Created by paolo on 30/05/19.
//
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "httpServer.h"
//#include "taskDHT112.h"
#include "taskSNTP.h"
#include "Flash.h"


#include <esp_http_server.h>
#include "MCP3201.h"

#define WIFI_SSID "TWSP-2_4"
#define WIFI_PASS "caciottinatuttook"

static const char *STATUS_200 = "200 OK";
static const char *STATUS_201 = "201 CREATE";
static const char *STATUS_400 = "400 BAD REQUEST";

static const char *TAG = "HttpServer";
httpd_handle_t server = NULL;

static esp_err_t methodNotSupported(httpd_req_t *connData);

static void sendTimeLed(char *buffer, struct TimeLed *timeLed);

static void sendChar(char *buffer, int8_t c);

static struct TimeLed decodeTime(char *data);

static int commonGet(httpd_req_t *connData, const char *field, int16_t value);

esp_err_t whoAreYouHandler(httpd_req_t *req) {
    if (req->method != HTTP_GET) {
        return methodNotSupported(req);
    }
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, "I am Vase 2.0\n", -1);

    return ESP_OK;
}

esp_err_t onHandler(httpd_req_t *req) {
    onLight();
    overWriteLight = ON_LIGHT;
    httpd_resp_set_status(req, STATUS_201);
    httpd_resp_send(req, "\n", -1);
    return ESP_OK;
}

esp_err_t offHandler(httpd_req_t *req) {
    offLight();
    overWriteLight = OFF_LIGHT;
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

    if (lightOn)
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

esp_err_t startEndTimeHandler(httpd_req_t *req) {
    char buf[5];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret == 5) {
        struct TimeLed startTime = decodeTime(buf);
        if (startTime.minute != -1) {
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

esp_err_t httpSoilMoistureHandler(httpd_req_t *connData) {
    uint16_t soilMoisture = adcRead();
    return commonGet(connData, "soilMoisture", soilMoisture);
}

esp_err_t commonGet(httpd_req_t *req, const char *field, int16_t value) {
    esp_err_t buffer[40];
    httpd_resp_set_status(req, STATUS_200);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    strcpy(buffer, field);
    sprintf(buffer + strlen(buffer), ": %d\n", value);
    httpd_resp_send(req, buffer, -1);
    return ESP_OK;
}

/**
 * query params availabes;
 * start='integer'  default = 0
 * size='integer' default 50
 * Generate response:
 * list of
 *    epochTime temperature humidity soilMoisture
 * @param connData
 * @param temperature
 * @return
 */
esp_err_t httpData(httpd_req_t *req) {
    httpd_resp_set_status(req, STATUS_200);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

    int32_t size = 50;
    int32_t start = 0;

    size_t bufferLen = httpd_req_get_url_query_len(req) + 1;
    if (bufferLen > 1) {
        char *buffer = malloc(bufferLen);
        if (buffer != NULL) {
            if (httpd_req_get_url_query_str(req, buffer, bufferLen) == ESP_OK) {
                char param[32];
                if (httpd_query_key_value(buffer, "start", param, sizeof(param)) ==
                    ESP_OK) {
                    start = atoi(param);
                }
                if (httpd_query_key_value(buffer, "size", param, sizeof(param)) ==
                    ESP_OK) {
                    size = atoi(param);
                }
            }
            free(buffer);
        }
    }
    ESP_LOGI(TAG, "start: %d, size: %d\n", start, size);

    int32_t toSkip = start;
    time_t before = time(NULL);
    char *buffer = malloc(100);
    while (true) {
        uint16_t newerSector = getLastSector(before);
        ESP_LOGI(TAG, "newerSector %02X for time %u", newerSector,
                 (uint32_t) before);
        if (newerSector == 0)
            break;
        struct DataSample *dataSamples = getSamples(newerSector);
        int16_t lastSample = getLastSample();
        if (lastSample <= toSkip) {
            ESP_LOGI(TAG, "Skip all the sector\n");
            toSkip -= lastSample;
            before = dataSamples->timestamp - 1;
            continue;
        }
        ESP_LOGI(TAG, "Remain to skip %d\n", toSkip);
        int16_t startSample = lastSample - toSkip;
        toSkip = 0;
        struct DataSample *iter = dataSamples + startSample;

        for (; iter != dataSamples && size >= 0; iter--, size--) {
            sprintf(buffer, "%ld %d %d %d\n", iter->timestamp, iter->temperature,
                    iter->humidity, iter->soil);
            ESP_LOGI(TAG, "%s", buffer);
            httpd_resp_send_chunk(req, buffer, strlen(buffer));
        }

        ESP_LOGI(TAG, "Remain %d data to send\n", size);
        if (size <= 0)
            break;
        before = dataSamples->timestamp - 1;
    }
    free(buffer);
    ESP_LOGI(TAG, "End chunk\n");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

httpd_uri_t getWhoAreYou = {.uri = "/who_are_you",
        .method = HTTP_GET,
        .handler = whoAreYouHandler,
        .user_ctx = NULL};
httpd_uri_t postOn = {
        .uri = "/on", .method = HTTP_POST, .handler = onHandler, .user_ctx = NULL};
httpd_uri_t postOff = {.uri = "/off",
        .method = HTTP_POST,
        .handler = offHandler,
        .user_ctx = NULL};
httpd_uri_t getStatus = {.uri = "/status",
        .method = HTTP_GET,
        .handler = getStatusHandler,
        .user_ctx = NULL};
httpd_uri_t postOnTime = {.uri = "/onTime",
        .method = HTTP_POST,
        .handler = startEndTimeHandler,
        .user_ctx = 0};
httpd_uri_t postOffTime = {.uri = "/offTime",
        .method = HTTP_POST,
        .handler = startEndTimeHandler,
        .user_ctx = 1};
httpd_uri_t getSoil = {.uri = "/soil",
        .method = HTTP_GET,
        .handler = httpSoilMoistureHandler,
        .user_ctx = NULL};
httpd_uri_t getData = {
        .uri = "/data", .method = HTTP_GET, .handler = httpData, .user_ctx = 0};

httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &getWhoAreYou);
        httpd_register_uri_handler(server, &getStatus);
        httpd_register_uri_handler(server, &getSoil);
        httpd_register_uri_handler(server, &getData);
        httpd_register_uri_handler(server, &postOn);
        httpd_register_uri_handler(server, &postOff);
        httpd_register_uri_handler(server, &postOnTime);
        httpd_register_uri_handler(server, &postOffTime);

        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(void) { httpd_stop(server); }

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

static void sendChar(char *buffer, int8_t c) {
    buffer[0] = c / 10 + '0';
    buffer[1] = c % 10 + '0';
}

// Expexted format: hh:mm
static struct TimeLed decodeTime(char *data) {
    struct TimeLed result;
    result.hour = result.minute = -1;
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
