#include "App_WS.h"

static const char *TAG = "websocket";
// 创建一个音频客户端的句柄
esp_websocket_client_handle_t audio_client;
esp_websocket_client_handle_t video_client;

// 两个标志位 表示当前audio客户端正在发送和接收
static uint8_t isSending = 0;
static uint8_t isReceing = 0;

// 发送音频数据的两个任务句柄
TaskHandle_t App_WS_MicToBuffer_Handle;
TaskHandle_t App_WS_BufferToWS_Handle;

// 创建一个视频客户端的任务句柄
TaskHandle_t App_WS_VideoTask_Handle;

// 任务函数声明
void App_WS_MicToBuffer_Task(void *args);
void App_WS_BufferToWS_Task(void *args);
void App_WS_Video_Task(void *args);
void App_WS_video_Task(void *args);

// 环形缓冲区句柄
RingbufHandle_t ringBufferHandle;

// 是否关闭esp2client的标志位
uint8_t isCloseEspToClientFlag = 0;
// 是否关闭视频任务的标志位
uint8_t isCloseVideoFlag = 0;

/**
 * @brief WS回调函数
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
 * @brief WS初始化
 */
void App_WS_Init(void)
{
    // 1. 创建配置对象
    esp_websocket_client_config_t websocket_cfg = {
        .uri = "ws://192.168.243.229:8000/ws/from_esp",
    };

    // 2. 初始化客户端
    audio_client = esp_websocket_client_init(&websocket_cfg);

    // 补充: 创建视频客户端
    websocket_cfg.uri = "ws://192.168.243.229:8000/ws/image";

    video_client = esp_websocket_client_init(&websocket_cfg);

    // 3. 注册回调函数
    esp_websocket_register_events(audio_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)audio_client);
    esp_websocket_register_events(video_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)video_client);

    // 4. 创建环形缓冲器 用来方便将来从mic中读取数据的存储 和 写入给WS
    ringBufferHandle = xRingbufferCreate(16000, RINGBUF_TYPE_NOSPLIT);
}

/**
 * @brief WS的音频客户端的启动
 */
void App_WS_Audio_Start(char *dir)
{
    // 判断audio_client存在  并且 没有启动连接 的时候 才会执行启动audio_client
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
        // 这个位置,是可视化客户端点击开始接收门铃数据的命令执行所在位置
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
 * @brief WS的音频客户端的关闭
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
        // 删除音频发送任务
        isCloseEspToClientFlag = 2;
    }

    // 判断发送和接收都要关闭 才放行
    if (isReceing != 0 || isSending != 0)
    {
        return;
    }

    // 只有audio_client存在 并且没有被关闭  才会执行关闭
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
 * @brief 向服务器发送二进制数据
 */
void App_WS_SendBinData(esp_websocket_client_handle_t client, char *data, int len)
{
    if (client != NULL && esp_websocket_client_is_connected(client) && len > 0)
    {
        esp_websocket_client_send_bin(client, data, len, portMAX_DELAY);
    }
}

/**
 * @brief 读Mic数据传递给缓冲器任务函数
 */
void App_WS_MicToBuffer_Task(void *args)
{
    // 环形缓冲区每次在存入数据的时候 会添加一个8字节的头信息
    uint8_t data[500 - 8] = {0};
    while (1)
    {
        // 从麦克风读数据
        Int_ES8311_ReadFromMic(data, sizeof(data));

        // 把数据写入环形缓冲器
        xRingbufferSend(ringBufferHandle, data, sizeof(data), portMAX_DELAY);

        // 如果需要删除当前任务,则等待任务执行完成后,自杀
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
 * @brief 读缓冲器数据传递给WS任务函数
 */
void App_WS_BufferToWS_Task(void *args)
{
    size_t len = 0;
    while (1)
    {
        // 先从环形缓冲器读取数据
        void *data = xRingbufferReceive(ringBufferHandle, &len, portMAX_DELAY);

        // 判断是否读取到了数据
        if (len > 0)
        {
            App_WS_SendBinData(audio_client, (char *)data, len);
            vRingbufferReturnItem(ringBufferHandle, data);
            len = 0;
        }

        // 如果需要删除当前任务,则等待任务执行完成后,自杀
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
 * @brief 初始化视频客户端
 *
 */
void App_WS_Video_Client_Init(void)
{
    Dri_OV2640_Init();
}

/**
 * @brief 启动视频客户端
 *
 */
void App_WS_Video_Client_Start(void)
{
    // 判断video_client存在  并且 没有启动连接 的时候 才会执行启动video_client
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
 * @brief 关闭视频客户端
 *
 */
void App_WS_Video_Client_Close(void)
{
    // 只有video_client存在 并且没有被关闭  才会执行关闭
    if (video_client != NULL && esp_websocket_client_is_connected(video_client))
    {
        MY_LOGI("close video client......\r\n");
        esp_websocket_client_close(video_client, portMAX_DELAY);

        if (App_WS_VideoTask_Handle != NULL)
        {
            // 删除音频发送任务
            isCloseEspToClientFlag = 1;
        }
    }
}

/**
 * @brief 视频任务函数
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

        // 如果需要删除当前任务,则等待任务执行完成后,自杀
        if (isCloseVideoFlag != 0)
        {
            isCloseVideoFlag--;
            App_WS_VideoTask_Handle = NULL;
            vTaskDelete(NULL);
        }

        vTaskDelay(30);
    }
}
