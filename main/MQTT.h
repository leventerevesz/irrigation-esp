#ifndef _MQTT_H
#define _MQTT_H

#define TOPIC_VALVE_0_COMMAND "/commands/v/0"
#define TOPIC_VALVE_1_COMMAND "/commands/v/1"

void mqtt_app_start(void);

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

#endif