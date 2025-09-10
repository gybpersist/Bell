#ifndef __APP_WS_H__
#define __APP_WS_H__

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_crt_bundle.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include <cJSON.h>
#include "Com_Debug.h"
#include "Int_ES8311.h"
#include "freertos/ringbuf.h"
#include "Dri_OV2640.h"
#include "Int_WS2812.h"

#define NO_DATA_TIMEOUT_SEC 5

/**
 * @brief websocket 初始化
 * 
 */
void App_WS_Init(void);

/**
 * @brief 开启 websocket 音频客户端
 *
 * @param dir 传入参数
 */
void App_WS_Audio_Start(char *dir);

/**
 * @brief 关闭 websocket 音频客户端
 *
 * @param dir 传入参数
 */
void App_WS_Audio_Close(char *dir);

/**
 * @brief wssocket发送二进制数据
 *
 */
void App_WS_SendBinData(esp_websocket_client_handle_t client, char *data, int len);

/**
 * @brief 视频传输初始化
 * 
 */
void App_WS_Video_Client_Init(void);

/**
 * @brief 视频传输启动
 * 
 */
void App_WS_Video_Client_Start(void);

/**
 * @brief 视频传输关闭
 * 
 */
void App_WS_Video_Client_Close(void);

#endif /* __APP_WS_H__ */
