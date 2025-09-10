#include <stdio.h>
#include "Com_Debug.h"
#include "Int_ES8311.h"
#include "Dri_WIFI.h"
#include "Dri_MQTT.h"
#include "App_WS.h"
#include "Int_WS2812.h"

// 编解码器缓冲区
// uint8_t data[1024] = {0};

// app_main 的任务句柄
TaskHandle_t app_main_handle;

// WIFI创建成功的回调函数
void wifi_succ_cb(void)
{
    printf("\r\n");
    MY_LOGE("wifi is Connected....\r\n");

    // 初始化 MQTT
    Dri_MQTT_Init();

    // 初始化 websocket 客户端
    App_WS_Init();
}

void app_main(void)
{
    // 获取 app_main 的任务句柄
    app_main_handle = xTaskGetCurrentTaskHandle();
    // Int_ES8311_Init();
    Dri_WIFI_Init(wifi_succ_cb);
    App_WS_Video_Client_Init();
    Int_WS2812_Init();

    while (1)
    {
        // Int_ES3811_ReadFromMic(data,1024);
        // Int_ES8311_WriteToSpeaker(data,1024);

        // 保存接收到的通知值
        uint32_t ulValue = MQTT_ACTION_AUDIO_VIDEO_NO;

        // 接收任务通知
        xTaskNotifyWait(UINT32_MAX, UINT32_MAX, &ulValue, portMAX_DELAY);

        switch (ulValue)
        {
        case MQTT_ACTION_AUDIO_CLIENT2ESP_ON: // 开启发送音频
            MY_LOGI("Open the audio client2esp...\r\n");
            App_WS_Audio_Start("client2esp"); // 开启 websocket音频服务端
            break;
        case MQTT_ACTION_AUDIO_CLIENT2ESP_OFF: // 关闭发送音频
            MY_LOGI("Close the audio client2esp...\r\n");
            App_WS_Audio_Close("client2esp"); // 关闭 websocket音频服务端
            break;
        case MQTT_ACTION_AUDIO_ESP2CLIENT_ON: // 开启接收音频
            MY_LOGI("Open the audio esp2client...\r\n");
            App_WS_Audio_Start("esp2client"); // 开启 websocket音频服务端
            break;
        case MQTT_ACTION_AUDIO_ESP2CLIENT_OFF: // 关闭接收音频
            MY_LOGI("Close the audio esp2client...\r\n");
            App_WS_Audio_Close("esp2client"); // 关闭 websocket音频服务端
            break;
        case MQTT_ACTION_VIDEO_ESP2CLIENT_ON: // 开启视频
            MY_LOGI("video is open...\r\n");
            App_WS_Video_Client_Start();
            Int_WS2812_LEDOpen(0, 200, 200, 255);
            Int_WS2812_LEDOpen(1, 200, 200, 255);
            break;
        case MQTT_ACTION_VIDEO_ESP2CLIENT_OFF: // 关闭视频
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
