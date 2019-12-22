//
// Created by paolo on 18/12/19.
//

#include <nvs_flash.h>
#include <string.h>
#include "Settings.h"
#include "Light.h"
#include "wifipasswd.h"


static char *const KEY_SOL_CHECK_INTERVAL = "soilCheckInterval";
char  Wifi_SSID[32];
char  Wifi_Passwd[64];
static char *const KET_WIFI_SSID = "Wifi-ssid";
static char *const KET_WIFI_PASSWD = "Wifi-pass";
struct PeriodLed periodLed;
uint16_t soilCheckInterval;

void initSettings() {
    periodLed.start.hour = 6;
    periodLed.start.minute = 30;
    periodLed.end.hour = 21;
    periodLed.end.minute = 0;
    soilCheckInterval=60;

    esp_err_t espError;
    espError = nvs_flash_init();
    if (espError == ESP_OK){
        nvs_handle nvsHandle;

        espError = nvs_open("Vase2_0", NVS_READONLY, &nvsHandle);
        if (espError == ESP_OK){
            uint8_t  value;
            if (nvs_get_u8(nvsHandle, "start_hour", &value )==ESP_OK){
                periodLed.start.hour = value;
            }
            if (nvs_get_u8(nvsHandle, "start_minute", &value )==ESP_OK){
                periodLed.start.minute = value;
            }
            if (nvs_get_u8(nvsHandle, "end_hour", &value )==ESP_OK){
                periodLed.end.hour = value;
            }
            if (nvs_get_u8(nvsHandle, "end_minute", &value )==ESP_OK){
                periodLed.end.minute = value;
            }

            nvs_get_u16(nvsHandle, KEY_SOL_CHECK_INTERVAL, &soilCheckInterval );

            size_t  size=sizeof(Wifi_SSID);
            if(nvs_get_str(nvsHandle, KET_WIFI_SSID, Wifi_SSID, &size) != ESP_OK){
                strncpy(Wifi_SSID, WIFI_SSID, sizeof(Wifi_SSID));
            }

            size=sizeof(Wifi_Passwd);
            if(nvs_get_str(nvsHandle, KET_WIFI_PASSWD, Wifi_Passwd, &size) != ESP_OK){
                strncpy(Wifi_Passwd, WIFI_PASS, sizeof(Wifi_Passwd));
            }

            nvs_close(nvsHandle);
        }
    }

}

void setSolCheckInterval(uint16_t newCheckInterval) {
    soilCheckInterval = newCheckInterval;

    esp_err_t espError;
    nvs_handle nvsHandle;
    espError = nvs_open("Vase2_0", NVS_READONLY, &nvsHandle);
    if (espError == ESP_OK){
        nvs_set_u16(nvsHandle, KEY_SOL_CHECK_INTERVAL, soilCheckInterval);
    }
    nvs_close(nvsHandle);
}