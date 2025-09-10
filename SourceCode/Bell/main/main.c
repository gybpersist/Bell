#include <stdio.h>
#include "Com_Debug.h"
#include "Int_ES8311.h"
#include "Dri_WIFI.h"
#include "Dri_MQTT.h"
#include "App_WS.h"
#include "Int_WS2812.h"

// �������������
// uint8_t data[1024] = {0};

// app_main ��������
TaskHandle_t app_main_handle;

// WIFI�����ɹ��Ļص�����
void wifi_succ_cb(void)
{
    printf("\r\n");
    MY_LOGE("wifi is Connected....\r\n");

    // ��ʼ�� MQTT
    Dri_MQTT_Init();

    // ��ʼ�� websocket �ͻ���
    App_WS_Init();
}

void app_main(void)
{
    // ��ȡ app_main ��������
    app_main_handle = xTaskGetCurrentTaskHandle();
    // Int_ES8311_Init();
    Dri_WIFI_Init(wifi_succ_cb);
    App_WS_Video_Client_Init();
    Int_WS2812_Init();

    while (1)
    {
        // Int_ES3811_ReadFromMic(data,1024);
        // Int_ES8311_WriteToSpeaker(data,1024);

        // ������յ���ֵ֪ͨ
        uint32_t ulValue = MQTT_ACTION_AUDIO_VIDEO_NO;

        // ��������֪ͨ
        xTaskNotifyWait(UINT32_MAX, UINT32_MAX, &ulValue, portMAX_DELAY);

        switch (ulValue)
        {
        case MQTT_ACTION_AUDIO_CLIENT2ESP_ON: // ����������Ƶ
            MY_LOGI("Open the audio client2esp...\r\n");
            App_WS_Audio_Start("client2esp"); // ���� websocket��Ƶ�����
            break;
        case MQTT_ACTION_AUDIO_CLIENT2ESP_OFF: // �رշ�����Ƶ
            MY_LOGI("Close the audio client2esp...\r\n");
            App_WS_Audio_Close("client2esp"); // �ر� websocket��Ƶ�����
            break;
        case MQTT_ACTION_AUDIO_ESP2CLIENT_ON: // ����������Ƶ
            MY_LOGI("Open the audio esp2client...\r\n");
            App_WS_Audio_Start("esp2client"); // ���� websocket��Ƶ�����
            break;
        case MQTT_ACTION_AUDIO_ESP2CLIENT_OFF: // �رս�����Ƶ
            MY_LOGI("Close the audio esp2client...\r\n");
            App_WS_Audio_Close("esp2client"); // �ر� websocket��Ƶ�����
            break;
        case MQTT_ACTION_VIDEO_ESP2CLIENT_ON: // ������Ƶ
            MY_LOGI("video is open...\r\n");
            App_WS_Video_Client_Start();
            Int_WS2812_LEDOpen(0, 200, 200, 255);
            Int_WS2812_LEDOpen(1, 200, 200, 255);
            break;
        case MQTT_ACTION_VIDEO_ESP2CLIENT_OFF: // �ر���Ƶ
            MY_LOGI("video is off...\r\n");
            App_WS_Video_Client_Close();
            Int_WS2812_LEDClose();
            break;
        default:
            break;
        }

        vTaskDelay(10);
    }
}
