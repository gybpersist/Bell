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
 * @brief websocket ��ʼ��
 * 
 */
void App_WS_Init(void);

/**
 * @brief ���� websocket ��Ƶ�ͻ���
 *
 * @param dir �������
 */
void App_WS_Audio_Start(char *dir);

/**
 * @brief �ر� websocket ��Ƶ�ͻ���
 *
 * @param dir �������
 */
void App_WS_Audio_Close(char *dir);

/**
 * @brief wssocket���Ͷ���������
 *
 */
void App_WS_SendBinData(esp_websocket_client_handle_t client, char *data, int len);

/**
 * @brief ��Ƶ�����ʼ��
 * 
 */
void App_WS_Video_Client_Init(void);

/**
 * @brief ��Ƶ��������
 * 
 */
void App_WS_Video_Client_Start(void);

/**
 * @brief ��Ƶ����ر�
 * 
 */
void App_WS_Video_Client_Close(void);

#endif /* __APP_WS_H__ */
