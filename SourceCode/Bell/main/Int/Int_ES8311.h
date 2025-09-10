#ifndef __INT_ES8311_H__
#define __INT_ES8311_H__
// ��ֹͷ�ļ����ظ�������Ԥ����ָ����δ����__INT_ES8311_H__�����岢�����������ݣ�

// -------------------------- ��������ͷ�ļ� --------------------------
#include "esp_idf_version.h"       // ESP-IDF �汾��Ϣ�����ڰ汾���ݴ���

#include "driver/i2s_std.h"        // I2S ��׼ģʽ��������Ƶ���ݴ���ӿڣ�
#include "driver/i2s_tdm.h"        // I2S TDM ģʽ��������ѡ���˴�δʹ�õ�Ԥ�����ݣ�
#include "soc/soc_caps.h"          // оƬ�������Զ��壨�����ж�Ӳ��֧�ֵĹ��ܣ�

#include "esp_codec_dev.h"         // ESP ��������豸����㣨ͳһ������Ƶ�豸��
#include "esp_codec_dev_defaults.h"// ��������豸Ĭ�����ã��ṩĬ��ʵ�֣�
#include "unity.h"                 // ��Ԫ���Կ�ܣ����������������ԣ�ʵ��Ӧ�ÿ��Ƴ���
#include "es8311_codec.h"          // ES8311 �������ר������������оƬ���в�����

#include "driver/i2c.h"            // I2C �������������� ES8311 �Ĵ�����
#include "driver/gpio.h"           // GPIO ���������ڿ���Ӳ�����ţ��繦��ʹ�ܣ�

// -------------------------- Ӳ�����ź궨�� --------------------------
// I2C ���ƽӿ����Ŷ��壨���� ES8311 �� I2C ���ƶˣ�
#define ES8311_I2C_SDA_PIN      GPIO_NUM_0   // I2C �������ţ�SDA��
#define ES8311_I2C_SCL_PIN      GPIO_NUM_1   // I2C ʱ�����ţ�SCL��

// I2S ��Ƶ���ݽӿ����Ŷ��壨���� ES8311 ����Ƶ���ݶˣ�
#define ES8311_I2S_BCK_PIN      GPIO_NUM_2   // I2S λʱ�����ţ�Bit Clock��
#define ES8311_I2S_MCK_PIN      GPIO_NUM_3   // I2S ��ʱ�����ţ�Master Clock��
#define ES8311_I2S_DATA_IN_PIN  GPIO_NUM_4   // I2S �����������ţ���˷����ݵ� ESP32��
#define ES8311_I2S_DATA_OUT_PIN GPIO_NUM_6   // I2S ����������ţ�ESP32 ���ݵ���������
#define ES8311_I2S_DATA_WS_PIN  GPIO_NUM_5   // I2S ����ѡ�����ţ�Word Select�����������л���

// ���ſ������Ŷ���
#define ES8311_PA_PIN GPIO_NUM_46  // ���������ţ�Power Amplifier��ʹ������

// -------------------------- ��������������ӿڣ� --------------------------
/**
 * @brief ES8311 ��ʼ������
 * @details ��� I2C��I2S �ӿڳ�ʼ�������� ES8311 ���������������豸
 * @note ������ʹ����Ƶ����ǰ���ã��ҽ������һ��
 */
void Int_ES8311_Init(void);

/**
 * @brief ES8311 ȡ����ʼ������
 * @details �ر��豸���ͷ� I2C��I2S ��Դ���ָ�����Ĭ��״̬
 * @note ���豸����ʹ��ʱ���ã�������Դй¶
 */
void Int_ES8311_DeInit(void);

/**
 * @brief ����˷��ȡ��Ƶ����
 * @param data ����������洢��ȡ������Ƶ���ݻ�������16bit ��������С�˸�ʽ��
 * @param len  �����������Ҫ��ȡ�����ݳ��ȣ���λ���ֽڣ���Ϊ 2 �ı�����
 * @note ���ݸ�ʽ�����ʼ��ʱ���õĲ���������16000Hz��16bit��ƥ��
 */
void Int_ES8311_ReadFromMic(uint8_t data[], size_t len);

/**
 * @brief ��������д����Ƶ����
 * @param data ��������������ŵ���Ƶ���ݻ�������16bit ��������С�˸�ʽ��
 * @param len  �����������Ҫд������ݳ��ȣ���λ���ֽڣ���Ϊ 2 �ı�����
 * @note ���ݸ�ʽ�����ʼ��ʱ���õĲ�������һ�£�������ܲ�������
 */
void Int_ES8311_WriteToSpeaker(uint8_t data[], size_t len);

#endif /* __INT_ES8311_H__ */
// ͷ�ļ��������
