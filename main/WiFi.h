#ifndef WIFI_H
#define WIFI_H

esp_err_t wifi_event_handler(void *ctx, system_event_t *event);

void wifi_init(void);

#endif