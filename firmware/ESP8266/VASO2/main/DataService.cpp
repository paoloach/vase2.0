//
// Created by paolo on 08/01/20.
//

#include "DataService.h"

static const char *STATUS_200 = "200 OK";
constexpr const char * TAG="DataService";

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
                if (httpd_query_key_value(buffer, "start", param, sizeof(param)) == ESP_OK) {
                    start = strtol(param, NULL, 10);
                }
                if (httpd_query_key_value(buffer, "size", param, sizeof(param)) == ESP_OK) {
                    size = strtol(param, NULL, 10);
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
//        uint16_t newerSector = getLastSector(before);
//        ESP_LOGI(TAG, "newerSector %02X for time %u", newerSector,
//                 (uint32_t) before);
//        if (newerSector == 0)
//            break;
//        struct DataSample *dataSamples = getSamples(newerSector);
//        int16_t lastSample = getLastSample();
//        if (lastSample <= toSkip) {
//            ESP_LOGI(TAG, "Skip all the sector\n");
//            toSkip -= lastSample;
//            before = dataSamples->timestamp - 1;
//            continue;
//        }
//        ESP_LOGI(TAG, "Remain to skip %d\n", toSkip);
//        int16_t startSample = lastSample - toSkip;
//        toSkip = 0;
//        struct DataSample *iter = dataSamples + startSample;
//
//        for (; iter != dataSamples && size >= 0; iter--, size--) {
//            sprintf(buffer, "%ld %d %d %d\n", iter->timestamp, iter->temperature,
//                    iter->humidity, iter->soil);
//            ESP_LOGI(TAG, "%s", buffer);
//            httpd_resp_send_chunk(req, buffer, strlen(buffer));
//        }
//
//        ESP_LOGI(TAG, "Remain %d data to send\n", size);
//        if (size <= 0)
//            break;
//        before = dataSamples->timestamp - 1;
    }
    free(buffer);
    ESP_LOGI(TAG, "End chunk\n");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

