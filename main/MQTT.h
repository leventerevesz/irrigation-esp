#ifndef MQTT_H
#define MQTT_H

#define TOPIC_VALVE_0_COMMAND "/commands/v/0"
#define TOPIC_VALVE_1_COMMAND "/commands/v/1"

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

#endif