#include "mqtt_app.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MQTT_APP";
static esp_mqtt_client_handle_t client = NULL;
static int counter = 0;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to broker");
        esp_mqtt_client_subscribe(client, "esp32/test", 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from broker");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "Subscribed to esp32/test");
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Received data:");
        printf("Topic: %.*s\r\n", event->topic_len, event->topic);
        printf("Payload: %.*s\r\n", event->data_len, event->data);
        break;

    default:
        break;
    }
}

/* Dummy Data Publisher Task */
static void mqtt_publisher_task(void *pvParameters)
{
    while (1)
    {
        if (client)
        {
            char payload[64];
            snprintf(payload, sizeof(payload), "{\"device\":\"esp32\",\"counter\":%d}", counter++);
            esp_mqtt_client_publish(client, "esp32/test", payload, 0, 0, 0);
            ESP_LOGI(TAG, "Published: %s", payload);
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 sec
    }
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.hivemq.com",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    xTaskCreate(mqtt_publisher_task, "mqtt_publisher_task", 4096, NULL, 5, NULL);
}
