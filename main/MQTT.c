#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "app_main.h"
#include "MQTT.h"
#include "GPIO.h"
#include "ADC.h"

static void mqtt_connected_event_handler(esp_mqtt_client_handle_t client);
static void mqtt_data_event_handler(esp_mqtt_event_handle_t event);
static void mqtt_valve_execute_command(gpio_num_t output_num, char* data);


void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
        .event_handle = mqtt_event_handler,
        // .user_context = (void *)your_context
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_connected_event_handler(client);
            break;

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
            mqtt_data_event_handler(event);
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


static void mqtt_data_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    //TODO remove these printfs
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
    if (strcmp(event->topic, TOPIC_VALVE_0_COMMAND) == 0)
    {
        gpio_num_t output_num = GPIO_OUTPUT_IO_0;
        mqtt_valve_execute_command(output_num, event->data);
    }
    else if (strcmp(event->topic, TOPIC_VALVE_1_COMMAND) == 0)
    {
        gpio_num_t output_num = GPIO_OUTPUT_IO_1;
        mqtt_valve_execute_command(output_num, event->data);
    }
    else if (strcmp(event->topic, TOPIC_TANKLEVEL_SUB) == 0)
    {
        mqtt_tank_level_request_handler(client);
    }
}


static void mqtt_connected_event_handler(esp_mqtt_client_handle_t client)
{
    int msg_id;
    
    msg_id = esp_mqtt_client_publish(client, "/info", "ESP connected", 0, 1, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, TOPIC_VALVE_0_COMMAND, 1);
    ESP_LOGI(TAG, "subscribed to %s, msg_id=%d", TOPIC_VALVE_0_COMMAND, msg_id);

    msg_id = esp_mqtt_client_subscribe(client, TOPIC_VALVE_1_COMMAND, 1);
    ESP_LOGI(TAG, "subscribed to %s, msg_id=%d", TOPIC_VALVE_1_COMMAND, msg_id);
}


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
        ESP_LOGE(TAG, "UNKNOWN VALVE COMMAND");
    }
}

static void mqtt_tank_level_request_handler(esp_mqtt_client_handle_t client)
{
    int msg_id;
    double level = get_tank_level();
    char buffer[8];
    sprintf(buffer, "%5.2f", level);

    msg_id = esp_mqtt_client_publish(client, TOPIC_TANKLEVEL_PUB, buffer, 0, 1, 1);
    ESP_LOGI(TAG, "tanklevel %s, msg_id=%d", buffer, msg_id);
}