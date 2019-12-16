//
// Created by paolo on 14/01/19.
//

#include <stdio.h>
#include <string.h>

#include <esp_log.h>

#include <driver/gpio.h>
#include <lwip/err.h>

#include <lwip/dns.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <nvs_flash.h>
#include <stdio.h>

/* Add extras/sntp component to makefile for this include to work */
#include "Pins.h"
#include "taskSNTP.h"
#include <time.h>
#include <esp_sntp.h>

static const char *SNTP_SERVER="tempo.ien.it";
static const char * TIME_ZONE="EU/ROME";

#define SNTP_SERVERS                                                           \
  "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org", "3.pool.ntp.org"

  static void sntpInit();

static const char *TAG = "SNTP";

#define vTaskDelayMs(ms) vTaskDelay((ms) / portTICK_PERIOD_MS)
#define UNUSED_ARG(x) (void)x

struct PeriodLed periodLed;
static struct tm *localTime;

static bool isOn(void);

bool lightOn = false;
#ifndef SNTP_IMPL_STEP_THRESHOLD
#define SNTP_IMPL_STEP_THRESHOLD 125000
#endif

#define TV2LD(TV) ((long double)TV.tv_sec + (long double)TV.tv_usec * 1.e-6)

enum OverWriteLight overWriteLight;
/*
 * Called by lwip/apps/sntp.c through
 * #define SNTP_SET_SYSTEM_TIME_US(S, F) sntp_impl_set_system_time_us(S, F)
 * u32_t matches lwip/apps/sntp.c
 */
void sntp_impl_set_system_time_us(uint32_t secs, uint32_t us) {

  struct timeval new;
  struct timeval old;
  struct timeval dt;

#ifdef TIMEKEEPING_SET_AND_MEASURE_ONLY
  static long double time_has_been_set_at;
#endif

  gettimeofday(&old, NULL);

  new.tv_sec = secs;
  new.tv_usec = us;

  timersub(&new, &old, &dt);

#ifdef TIMEKEEPING_SET_AND_MEASURE_ONLY

  if (time_has_been_set_at == 0) {
    settimeofday(&new, NULL);
    time_has_been_set_at = TV2LD(new);
  }

  ESP_LOGI(TAG, "SNTP:  %20.6Lf    delta: %10.3Lf ms %3.1Lf ppm\n", TV2LD(new),
           TV2LD(dt) * 1e3,
           (TV2LD(dt) / (TV2LD(new) - time_has_been_set_at)) * 1e6);

#else /* Normal operation */

  if (secs || abs(us) > SNTP_IMPL_STEP_THRESHOLD) {
    settimeofday(&new, NULL);
  } else {
    adjtime(&dt, NULL);
  }

  ESP_LOGI(TAG, "SNTP:  %ld    delta: %ld ms\n", new.tv_sec, dt.tv_sec * 1000);

#endif
}

void savePeriodLed() {
    esp_err_t espError;
    espError = nvs_flash_init();
    if (espError == ESP_OK){
        nvs_handle nvsHandle;

        espError = nvs_open("Vase2_0", NVS_READWRITE, &nvsHandle);
        if (espError == ESP_OK){
            nvs_set_u8(nvsHandle, "start_hour",periodLed.start.hour);
            nvs_set_u8(nvsHandle, "start_minute",periodLed.start.minute);
            nvs_set_u8(nvsHandle, "end_hour",periodLed.end.hour);
            nvs_set_u8(nvsHandle, "end_minute",periodLed.end.minute);
            nvs_close(nvsHandle);
        }
    }
}

void lightTask(void *pvParameters) {
  /* Start SNTP */
  ESP_LOGI(TAG, "Starting SNTP... ");
  initIO();
  /* Set GMT+1 zone, daylight savings off */
  const struct timezone tz = {1 * 60, 0};
  /* Servers must be configured right after initialization */
  sntpInit();
  ESP_LOGI(TAG, "DONE!\n");

  /* Print date and time each 5 seconds */
  while (1) {
    if (isOn()) {
      switch (overWriteLight) {
      case NONE:
        onLight();
        break;
      case ON_LIGHT:
        onLight();
        overWriteLight = NONE;
        break;
      default:;
      }
    } else {
      switch (overWriteLight) {
      case NONE:
        offLight();
        break;
      case OFF_LIGHT:
        offLight();
        overWriteLight = NONE;
        break;
      default:;
      }
    }
    vTaskDelayMs(20000);
  }
}

static bool isOn(void) {
  struct timeval timeVal;
  gettimeofday(&timeVal, NULL);

  localTime = localtime(&timeVal.tv_sec);
  if (localTime->tm_hour < periodLed.start.hour)
    return false;
  if (localTime->tm_hour == periodLed.start.hour &&
      localTime->tm_min < periodLed.start.minute)
    return false;
  if (localTime->tm_hour > periodLed.end.hour)
    return false;
  if (localTime->tm_hour == periodLed.end.hour &&
      localTime->tm_min > periodLed.end.minute)
    return false;
  return true;
}

void initIO() {
  gpio_config_t gpioConfig;
  gpioConfig.pin_bit_mask = LED_PIN;
  gpioConfig.mode = GPIO_MODE_DEF_OUTPUT;
  gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpioConfig.intr_type = GPIO_INTR_DISABLE;

  gpio_config(&gpioConfig);
  offLight();
}

void onLight() {
  gpio_set_level(LED_PIN, 1);
  lightOn = true;
}

void offLight() {
  gpio_set_level(LED_PIN, 0);
  lightOn = false;
}



void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void sntpInit() {
    time_t now = 0;
    struct tm timeinfo = { 0 };

    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
    // wait for time to be set
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    char strftime_buf[64];
    setenv("TZ",TIME_ZONE, 1);
    tzset();
    ESP_LOGI(TAG, "Timezone set to %s",TIME_ZONE);
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
    vTaskDelete(0);
    return;
}
