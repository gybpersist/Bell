#ifndef __DRI_MQTT_H__
#define __DRI_MQTT_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "cJSON.h"
#include "Com_Debug.h"

#define MQTT_ACTION_AUDIO_VIDEO_NO 0
#define MQTT_ACTION_AUDIO_CLIENT2ESP_ON 1
#define MQTT_ACTION_AUDIO_CLIENT2ESP_OFF 2
#define MQTT_ACTION_AUDIO_ESP2CLIENT_ON 3
#define MQTT_ACTION_AUDIO_ESP2CLIENT_OFF 4
#define MQTT_ACTION_VIDEO_ESP2CLIENT_ON 5
#define MQTT_ACTION_VIDEO_ESP2CLIENT_OFF 6

// app_main 的任务句柄
extern TaskHandle_t app_main_handle;

/**
 * @brief MQTT 初始化
 * 
 */
void Dri_MQTT_Init(void);


#endif /* __DRI_MQTT_H__ */
