//
// Created by paolo on 14/01/19.
//

#include <stdio.h>
#include <string.h>

#include <esp_log.h>

#include <lwip/err.h>
#include <lwip/dns.h>
#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>

#include <driver/gpio.h>

/* Add extras/sntp component to makefile for this include to work */
#include "Pins.h"
#include "sntp.h"
#include "taskSNTP.h"
#include <time.h>

#define SNTP_SERVERS                                                           \
  "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org", "3.pool.ntp.org"


static const char * TAG="SNTP";

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

#include <esp8266/pin_mux_register.h>
#include <stdio.h>

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

  ESP_LOGI(TAG,"SNTP:  %20.6Lf    delta: %10.3Lf ms %3.1Lf ppm\n", TV2LD(new),
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

void sntpStart(void *pvParameters) {
  char *servers[] = {SNTP_SERVERS};

  periodLed.start.hour = 6;
  periodLed.start.minute = 30;
  periodLed.end.hour = 21;
  periodLed.end.minute = 0;

  /* Start SNTP */
  ESP_LOGI(TAG,"Starting SNTP... ");
  initIO();
  sntp_set_update_delay(60 * 60 * 1000);
  /* Set GMT+1 zone, daylight savings off */
  const struct timezone tz = {1 * 60, 0};
  /* SNTP initialization */
  sntp_initialize(&tz);
  /* Servers must be configured right after initialization */
  sntp_set_servers(servers, sizeof(servers) / sizeof(char *));
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
  gpioConfig.pull_up_en =GPIO_PULLUP_DISABLE;
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