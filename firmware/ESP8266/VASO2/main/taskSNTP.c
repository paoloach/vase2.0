//
// Created by paolo on 14/01/19.
//

#include <esp_log.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <time.h>
#include <lwip/apps/sntp.h>

#include "wifi.h"
#include "taskSNTP.h"
#include "GroupSignals.h"

static const char *TIME_ZONE = "EUROPE/ROME+2";
static const char *TAG = "SNTP";
static const uint32_t UPDATE_SNTP_TASK_MINUTE=10;


static void sntpInit(void);

static void obtainTime(void);

static void sntpTask(void *arg);



void startSntpTask() {
    // SNTP service uses LwIP, please allocate large stack space.
    xTaskCreate(sntpTask, "sntp task", 2048, NULL, 10, NULL);
}

static void obtainTime(void) {
    xEventGroupWaitBits(wifi_event_group, VASO_CONNECTED_BIT,  false, true, portMAX_DELAY);
    sntpInit();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

}


static void sntpTask(void *arg) {
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];

    time(&now);
    localtime_r(&now, &timeinfo);

    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtainTime();
    }

    setenv("TZ", TIME_ZONE, 1);
    tzset();

    xEventGroupSetBits(wifi_event_group, TIME_VALID);
    while (1) {
        // update 'now' variable with current time
        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_year < (2016 - 1900)) {
            ESP_LOGE(TAG, "The current date/time error");
        } else {
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            ESP_LOGI(TAG, "The current date/time in Rome is: %s", strftime_buf);
        }

        ESP_LOGI(TAG, "Free heap size: %d\n", esp_get_free_heap_size());
        vTaskDelay(UPDATE_SNTP_TASK_MINUTE*60*1000 / portTICK_RATE_MS);
    }
}


void sntpInit() {
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}
