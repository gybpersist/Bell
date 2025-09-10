#include "App_WS.h"

static const char *TAG = "websocket";
// ����һ����Ƶ�ͻ��˵ľ��
esp_websocket_client_handle_t audio_client;
esp_websocket_client_handle_t video_client;

// ������־λ ��ʾ��ǰaudio�ͻ������ڷ��ͺͽ���
static uint8_t isSending = 0;
static uint8_t isReceing = 0;

// ������Ƶ���ݵ�����������
TaskHandle_t App_WS_MicToBuffer_Handle;
TaskHandle_t App_WS_BufferToWS_Handle;

// ����һ����Ƶ�ͻ��˵�������
TaskHandle_t App_WS_VideoTask_Handle;

// ����������
void App_WS_MicToBuffer_Task(void *args);
void App_WS_BufferToWS_Task(void *args);
void App_WS_Video_Task(void *args);
void App_WS_video_Task(void *args);

// ���λ��������
RingbufHandle_t ringBufferHandle;

// �Ƿ�ر�esp2client�ı�־λ
uint8_t isCloseEspToClientFlag = 0;
// �Ƿ�ر���Ƶ����ı�־λ
uint8_t isCloseVideoFlag = 0;

/**
 * @brief WS�ص�����
 */
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_BEGIN:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_BEGIN");
        break;
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        // ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        // ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        if (data->op_code == 0x2)
        { // Opcode 0x2 indicates binary data
            // ESP_LOG_BUFFER_HEX("Received binary data", data->data_ptr, data->data_len);
            MY_LOGI("Received binary data");
            Int_ES8311_WriteToSpeaker((uint8_t *)data->data_ptr, (uint16_t)data->data_len);
        }
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    case WEBSOCKET_EVENT_FINISH:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_FINISH");
        break;
    }
}

/**
 * @brief WS��ʼ��
 */
void App_WS_Init(void)
{
    // 1. �������ö���
    esp_websocket_client_config_t websocket_cfg = {
        .uri = "ws://192.168.243.229:8000/ws/from_esp",
    };

    // 2. ��ʼ���ͻ���
    audio_client = esp_websocket_client_init(&websocket_cfg);

    // ����: ������Ƶ�ͻ���
    websocket_cfg.uri = "ws://192.168.243.229:8000/ws/image";

    video_client = esp_websocket_client_init(&websocket_cfg);

    // 3. ע��ص�����
    esp_websocket_register_events(audio_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)audio_client);
    esp_websocket_register_events(video_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)video_client);

    // 4. �������λ����� �������㽫����mic�ж�ȡ���ݵĴ洢 �� д���WS
    ringBufferHandle = xRingbufferCreate(16000, RINGBUF_TYPE_NOSPLIT);
}

/**
 * @brief WS����Ƶ�ͻ��˵�����
 */
void App_WS_Audio_Start(char *dir)
{
    // �ж�audio_client����  ���� û���������� ��ʱ�� �Ż�ִ������audio_client
    if (audio_client != NULL && !esp_websocket_client_is_connected(audio_client))
    {
        MY_LOGI("open audio client......\r\n");
        Int_ES8311_Init();
        esp_websocket_client_start(audio_client);
    }

    if (strcmp(dir, "client2esp") == 0)
    {
        isReceing = 1;
    }
    else if (strcmp(dir, "esp2client") == 0)
    {
        isSending = 1;
        // ���λ��,�ǿ��ӻ��ͻ��˵����ʼ�����������ݵ�����ִ������λ��
        if (App_WS_MicToBuffer_Handle == NULL)
        {
            xTaskCreate(App_WS_MicToBuffer_Task, "App_WS_MicToBuffer_Task", 2048, (void *)NULL, 5, &App_WS_MicToBuffer_Handle);
        }

        if (App_WS_BufferToWS_Handle == NULL)
        {
            xTaskCreate(App_WS_BufferToWS_Task, "App_WS_BufferToWS_Task", 2048, (void *)NULL, 5, &App_WS_BufferToWS_Handle);
        }
    }
}

/**
 * @brief WS����Ƶ�ͻ��˵Ĺر�
 */
void App_WS_Audio_Close(char *dir)
{
    if (strcmp(dir, "client2esp") == 0)
    {
        isReceing = 0;
    }
    else if (strcmp(dir, "esp2client") == 0)
    {
        isSending = 0;
        // ɾ����Ƶ��������
        isCloseEspToClientFlag = 2;
    }

    // �жϷ��ͺͽ��ն�Ҫ�ر� �ŷ���
    if (isReceing != 0 || isSending != 0)
    {
        return;
    }

    // ֻ��audio_client���� ����û�б��ر�  �Ż�ִ�йر�
    if (audio_client != NULL && esp_websocket_client_is_connected(audio_client))
    {
        MY_LOGI("close audio client......\r\n");
        Int_ES8311_DeInit();
        esp_websocket_client_close(audio_client, portMAX_DELAY);
    }

    // if (App_WS_MicToBuffer_Handle != NULL)
    // {
    //     vTaskDelete(App_WS_MicToBuffer_Handle);
    //     App_WS_MicToBuffer_Handle = NULL;
    // }
    // if (App_WS_BufferToWS_Handle != NULL)
    // {
    //     vTaskDelete(App_WS_BufferToWS_Handle);
    //     App_WS_BufferToWS_Handle = NULL;
    // }
}

/**
 * @brief ����������Ͷ���������
 */
void App_WS_SendBinData(esp_websocket_client_handle_t client, char *data, int len)
{
    if (client != NULL && esp_websocket_client_is_connected(client) && len > 0)
    {
        esp_websocket_client_send_bin(client, data, len, portMAX_DELAY);
    }
}

/**
 * @brief ��Mic���ݴ��ݸ�������������
 */
void App_WS_MicToBuffer_Task(void *args)
{
    // ���λ�����ÿ���ڴ������ݵ�ʱ�� �����һ��8�ֽڵ�ͷ��Ϣ
    uint8_t data[500 - 8] = {0};
    while (1)
    {
        // ����˷������
        Int_ES8311_ReadFromMic(data, sizeof(data));

        // ������д�뻷�λ�����
        xRingbufferSend(ringBufferHandle, data, sizeof(data), portMAX_DELAY);

        // �����Ҫɾ����ǰ����,��ȴ�����ִ����ɺ�,��ɱ
        if (isCloseEspToClientFlag != 0)
        {
            /* code */
            isCloseEspToClientFlag--;
            App_WS_MicToBuffer_Handle = NULL;
            vTaskDelete(NULL);
        }

        vTaskDelay(10);
    }
}

/**
 * @brief �����������ݴ��ݸ�WS������
 */
void App_WS_BufferToWS_Task(void *args)
{
    size_t len = 0;
    while (1)
    {
        // �ȴӻ��λ�������ȡ����
        void *data = xRingbufferReceive(ringBufferHandle, &len, portMAX_DELAY);

        // �ж��Ƿ��ȡ��������
        if (len > 0)
        {
            App_WS_SendBinData(audio_client, (char *)data, len);
            vRingbufferReturnItem(ringBufferHandle, data);
            len = 0;
        }

        // �����Ҫɾ����ǰ����,��ȴ�����ִ����ɺ�,��ɱ
        if (isCloseEspToClientFlag != 0)
        {
            /* code */
            isCloseEspToClientFlag--;
            App_WS_BufferToWS_Handle = NULL;
            vTaskDelete(NULL);
        }

        vTaskDelay(10);
    }
}

/**
 * @brief ��ʼ����Ƶ�ͻ���
 *
 */
void App_WS_Video_Client_Init(void)
{
    Dri_OV2640_Init();
}

/**
 * @brief ������Ƶ�ͻ���
 *
 */
void App_WS_Video_Client_Start(void)
{
    // �ж�video_client����  ���� û���������� ��ʱ�� �Ż�ִ������video_client
    if (video_client != NULL && !esp_websocket_client_is_connected(video_client))
    {
        MY_LOGI("open video client......\r\n");
        esp_websocket_client_start(video_client);
    }

    if (App_WS_VideoTask_Handle == NULL)
    {
        xTaskCreate(App_WS_video_Task, "App_WS_video_Task", 4096, (void *)NULL, 10, &App_WS_VideoTask_Handle);
    }
}

/**
 * @brief �ر���Ƶ�ͻ���
 *
 */
void App_WS_Video_Client_Close(void)
{
    // ֻ��video_client���� ����û�б��ر�  �Ż�ִ�йر�
    if (video_client != NULL && esp_websocket_client_is_connected(video_client))
    {
        MY_LOGI("close video client......\r\n");
        esp_websocket_client_close(video_client, portMAX_DELAY);

        if (App_WS_VideoTask_Handle != NULL)
        {
            // ɾ����Ƶ��������
            isCloseEspToClientFlag = 1;
        }
    }
}

/**
 * @brief ��Ƶ������
 *
 * @param args
 */
void App_WS_video_Task(void *args)
{
    uint8_t *data;
    uint32_t len = 0;
    while (1)
    {
        Dri_OV2640_TakePic(&data, &len);
        App_WS_SendBinData(video_client, (char *)data, (int)len);
        Dri_OV2640_ReturnPic();

        // �����Ҫɾ����ǰ����,��ȴ�����ִ����ɺ�,��ɱ
        if (isCloseVideoFlag != 0)
        {
            isCloseVideoFlag--;
            App_WS_VideoTask_Handle = NULL;
            vTaskDelete(NULL);
        }

        vTaskDelay(30);
    }
}
