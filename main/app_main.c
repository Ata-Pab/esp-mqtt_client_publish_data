#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "wifi_manager.h"
#include "mqtt_app.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting mqtt_client_demo");

    // Init Wi-Fi manager
    wifi_manager_init();

    // Start MQTT client
    mqtt_app_start();
}