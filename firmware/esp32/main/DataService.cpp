//
// Created by paolo on 08/01/20.
//

#include <bits/unique_ptr.h>
#include <esp_log.h>
#include <time.h>
#include "DataService.h"
#include "Flash.h"
#include "DataChunkFlash.h"

constexpr const char *STATUS_200 = "200 OK";
constexpr const char *STATUS_400 = "400 BAD REQUEST";
constexpr const char *TAG = "DataService";
constexpr const int BUFFER_SIZE=2000;
constexpr const int MIN_BUFFER_SIZE=80;

static std::unique_ptr<DataChunk> getChunkContaining(timer_t time);
static void sendChunk(httpd_req_t *req, DataChunk * chunk, uint32_t startIndex, char *buffer, time_t untilTime, bool * addComma);

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
    httpd_resp_set_type(req, HTTPD_TYPE_JSON);

    time_t end = time(nullptr);
    time_t start = 0;

    {
        size_t bufferLen = httpd_req_get_url_query_len(req) + 1;
        auto buffer = std::unique_ptr<char>(new char[bufferLen]);
        if (buffer) {
            if (httpd_req_get_url_query_str(req, buffer.get(), bufferLen) == ESP_OK) {
                char param[32];
                if (httpd_query_key_value(buffer.get(), "start", param, sizeof(param)) == ESP_OK) {
                    start = strtol(param, nullptr, 10);
                }
                if (httpd_query_key_value(buffer.get(), "end", param, sizeof(param)) == ESP_OK) {
                    end = strtol(param, nullptr, 10);
                }
            }
        }
    }
    ESP_LOGI(TAG, "start: %ld, end: %ld\n", start, end);
    if (start > end){
        httpd_resp_set_status(req, STATUS_400);
        httpd_resp_send(req, "Invalid parameter: start parameter should be before end parameter\n",-1);
    }

    auto chunk = getChunkContaining(start);
    if (!chunk){
        httpd_resp_send(req, "no data\n",-1);
    }

    ESP_LOGI(TAG,"Start chunk: %ld to %ld", chunk->startTime, chunk->nextStartTime);

    time_t startChunkTime = chunk->startTime;
    uint8_t startIndex=0;
    for(; startIndex < MAX_SAMPLES; startIndex++){
        time_t  sampleTime = startChunkTime + chunk->samples[startIndex].offset;
        if (sampleTime >= start){
            break;
        }
    }

    auto buffer = std::unique_ptr<char>(new char[BUFFER_SIZE]);
    httpd_resp_send_chunk(req, "[", 1);
    bool addComma=false;
    while (true) {
        sendChunk(req, chunk.get(),startIndex, buffer.get(), end, &addComma);
        time_t  nextChunk = chunk.get()->nextStartTime;
        if (nextChunk==0) {
            ESP_LOGI(TAG,"Chunks finished");
            break;
        }

        chunk = dataChunkFlash.getChunk(nextChunk);
        ESP_LOGI(TAG,"new chunk: %ld to %ld", chunk->startTime, chunk->nextStartTime);

        if (chunk->startTime >= end) {
            ESP_LOGI(TAG,"next chunk too old");
            break;
        }
        startIndex=0;
    }
    ESP_LOGI(TAG, "Finished send data chunk");
    httpd_resp_send_chunk(req, "]\n", 2);
    esp_err_t error =httpd_resp_send_chunk(req, nullptr, 0);
    if (error != ESP_OK){
        ESP_LOGE(TAG, "Error sending chunk: %04X", error);
    }

    return ESP_OK;
}

void sendChunk(httpd_req_t *req, DataChunk * chunk, uint32_t startIndex, char *buffer, time_t untilTime, bool * addComma){
    ESP_LOGI(TAG,"Send chunk from %ld to %ld. startInidex: %d", chunk->startTime, untilTime, startIndex);
    DataSample * iter = chunk->samples+startIndex;
    DataSample * end = chunk->samples+MAX_SAMPLES;
    uint32_t  bufferPos=0;
    esp_err_t  error;
    for(;iter != end; iter++){
        time_t timestamp = chunk->startTime+iter->offset;
        if (timestamp >= untilTime){
            if (bufferPos != 0) {
                error = httpd_resp_send_chunk(req, buffer, bufferPos);
                if (error != ESP_OK) {
                    ESP_LOGE(TAG, "Error sending chunk: %04X", error);
                }
            }
            return;
        }
        size_t remaining = BUFFER_SIZE-bufferPos;
        if (remaining < MIN_BUFFER_SIZE){
            error = httpd_resp_send_chunk(req, buffer, bufferPos);
            if (error != ESP_OK){
                ESP_LOGE(TAG, "Error sending chunk: %04X", error);
            }
            bufferPos=0;
            remaining=MAX_SAMPLES;
        }
        uint32_t used = snprintf(buffer+bufferPos, remaining,  "{\"ts\": %ld, \"temperature\":  %d, \"huidity\": %d, \"soil\": %d}",  timestamp, iter->temperature,
                                 iter->humidity, iter->soil);
        bufferPos+=used;
        if (addComma){
            buffer[bufferPos]=',';
            buffer[bufferPos+1] = '\n';
            bufferPos+=2;
        } else {
            *addComma=true;
        }
    }
    if (bufferPos != 0) {
        error = httpd_resp_send_chunk(req, buffer, bufferPos);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Error sending chunk: %04X", error);
        }
    }
}


std::unique_ptr<DataChunk> getChunkContaining(timer_t time) {
    time_t chunkTime = getFirstDataChunk();
    while (chunkTime != 0) {
        auto chunk = dataChunkFlash.getChunk(chunkTime);
        if (!chunk) {
            return std::unique_ptr<DataChunk>();
        }
        if (chunk->startTime >= time && ( time < chunk->nextStartTime || chunk->nextStartTime==0)) {
            return chunk;
        }
        chunkTime = chunk->nextStartTime;
    }
    return std::unique_ptr<DataChunk>();
}