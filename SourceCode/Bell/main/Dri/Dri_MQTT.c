#include "Dri_MQTT.h"
static const char *TAG = "mqtt";

// app_main������
extern TaskHandle_t main_task_handle;

/**
 * @brief ���ݽ��ճɹ��Ĵ�����
 */
void Dri_MQTT_Data_Handle(char *data)
{
    // 1. ʹ��CJson ��json�ַ���תΪJson����
    cJSON *root = cJSON_Parse(data);
    cJSON *cmd_obj = cJSON_GetObjectItem(root, "cmd");
    cJSON *type_obj = cJSON_GetObjectItem(root, "type");
    cJSON *dir_obj = cJSON_GetObjectItem(root, "dir");

    // 2. �õ�json�����ÿһ���ַ���
    char *cmd = cJSON_GetStringValue(cmd_obj);
    char *type = cJSON_GetStringValue(type_obj);
    char *dir = cJSON_GetStringValue(dir_obj);

    MY_LOGI("cmd = %s,type = %s,dir = %s", cmd, type, dir);

    // ��ʼ��һ��ֵ֪ͨ
    uint8_t action = MQTT_ACTION_AUDIO_VIDEO_NO;

    // 3. �����ݽ��з���
    if (strcmp(type, "audio") == 0)
    {
        if (strcmp(cmd, "on") == 0)
        {
            if (strcmp(dir, "client2esp") == 0)
            {
                action = MQTT_ACTION_AUDIO_CLIENT2ESP_ON;
            }
            else if (strcmp(dir, "esp2client") == 0)
            {
                action = MQTT_ACTION_AUDIO_ESP2CLIENT_ON;
            }
        }
        else if (strcmp(cmd, "off") == 0)
        {
            if (strcmp(dir, "client2esp") == 0)
            {
                action = MQTT_ACTION_AUDIO_CLIENT2ESP_OFF;
            }
            else if (strcmp(dir, "esp2client") == 0)
            {
                action = MQTT_ACTION_AUDIO_ESP2CLIENT_OFF;
            }
        }
    }
    else if (strcmp(type, "video") == 0)
    {
        if (strcmp(cmd, "on") == 0)
        {
            action = MQTT_ACTION_VIDEO_ESP2CLIENT_ON;
        }
        else if (strcmp(cmd, "off") == 0)
        {
            action = MQTT_ACTION_VIDEO_ESP2CLIENT_OFF;
        }
    }

    // ����֪ͨ��main����
    xTaskNotify(app_main_handle, action, eSetValueWithOverwrite);
}

// MQTT�ص�����
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "guo/cmd", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        // printf("DATA=%.*s\r\n", event->data_len, event->data);
        Dri_MQTT_Data_Handle(event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/**
 * @brief MQTT��ʼ��
 */
void Dri_MQTT_Init(void)
{
    // MQTT���������
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "ws://broker.emqx.io:8083/mqtt",
    };

    // ��ʼ��MQTT����
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // ע��MQTT�ص�����
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // ����MQTT
    esp_mqtt_client_start(client);
}
