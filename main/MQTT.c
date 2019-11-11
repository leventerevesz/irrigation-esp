#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "app_main.h"
#include "MQTT.h"
#include "GPIO.h"

static void mqtt_valve_execute_command(gpio_num_t output_num, char* data)
{
    if (strcmp(data, "o") == 0)
    {
        gpio_set_level(output_num, 1);
    }
    else if (strcmp(data, "c") == 0)
    {
        gpio_set_level(output_num, 0);
    }
    else
    {
        ESP_LOGE(TAG, "MQTT UNKNOWN COMMAND");
    }
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/info", "ESP connected", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/commands/valve/1", 1);
            ESP_LOGI(TAG, "subscribed to /commands/valve/1, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/commands/valve/2", 1);
            ESP_LOGI(TAG, "subscribed to /commands/valve/1, msg_id=%d", msg_id);

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            if (strcmp(event->topic, "commands/v/0") == 0)
            {
                gpio_num_t output_num = GPIO_OUTPUT_IO_0;
                mqtt_valve_execute_command(output_num, event->data);
            }
            else if (strcmp(event->topic, "commands/v/1") == 0)
            {
                gpio_num_t output_num = GPIO_OUTPUT_IO_1;
                mqtt_valve_execute_command(output_num, event->data);
            }
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}