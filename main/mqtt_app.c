#include "mqtt_app.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_manager.h"

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

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Received data:");
        printf("Topic: %.*s\r\n", event->topic_len, event->topic);
        printf("Payload: %.*s\r\n", event->data_len, event->data);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS)
        {
            ESP_LOGE(TAG, "TLS/SSL error (esp_tls): 0x%08x", event->error_handle->esp_tls_last_esp_err);
        }
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
            int len = snprintf(payload, sizeof(payload), "{\"device\":\"esp32\",\"counter\":%d}", counter++);
            if (len > 0)
            {
                int msg_id = esp_mqtt_client_publish(client, "esp32/test", payload, 0, 1, 0);
                ESP_LOGI(TAG, "Published msg_id=%d payload=%s", msg_id, payload);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 sec
    }
}

void mqtt_app_start(void)
{
    /* Ensure Wi-Fi connected and time synced before starting TLS connection */
    ESP_LOGI(TAG, "Waiting for Wi-Fi and time sync before MQTT...");
    if (wifi_manager_wait_connected(pdMS_TO_TICKS(20000)) != pdTRUE)
    {
        ESP_LOGW(TAG, "Wi-Fi not ready after timeout — MQTT will still attempt but may fail.");
    }

    const esp_mqtt_client_config_t mqtt_cfg = {
        /* Use mqtts:// to force TLS and port 8883 */
        .broker.address.uri = "mqtts://broker.hivemq.com",
        .credentials.client_id = "ESP32_MQTT_TLS_CLIENT"};

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    xTaskCreate(mqtt_publisher_task, "mqtt_publisher_task", 4096, NULL, 5, NULL);
}
