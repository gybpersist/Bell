#ifndef __INT_ES8311_H__
#define __INT_ES8311_H__
// 防止头文件被重复包含（预处理指令：如果未定义__INT_ES8311_H__，则定义并包含以下内容）

// -------------------------- 引入依赖头文件 --------------------------
#include "esp_idf_version.h"       // ESP-IDF 版本信息（用于版本兼容处理）

#include "driver/i2s_std.h"        // I2S 标准模式驱动（音频数据传输接口）
#include "driver/i2s_tdm.h"        // I2S TDM 模式驱动（可选，此处未使用但预留兼容）
#include "soc/soc_caps.h"          // 芯片功能特性定义（用于判断硬件支持的功能）

#include "esp_codec_dev.h"         // ESP 编解码器设备抽象层（统一管理音频设备）
#include "esp_codec_dev_defaults.h"// 编解码器设备默认配置（提供默认实现）
#include "unity.h"                 // 单元测试框架（可能用于驱动测试，实际应用可移除）
#include "es8311_codec.h"          // ES8311 编解码器专用驱动（包含芯片特有操作）

#include "driver/i2c.h"            // I2C 驱动（用于配置 ES8311 寄存器）
#include "driver/gpio.h"           // GPIO 驱动（用于控制硬件引脚，如功放使能）

// -------------------------- 硬件引脚宏定义 --------------------------
// I2C 控制接口引脚定义（连接 ES8311 的 I2C 控制端）
#define ES8311_I2C_SDA_PIN      GPIO_NUM_0   // I2C 数据引脚（SDA）
#define ES8311_I2C_SCL_PIN      GPIO_NUM_1   // I2C 时钟引脚（SCL）

// I2S 音频数据接口引脚定义（连接 ES8311 的音频数据端）
#define ES8311_I2S_BCK_PIN      GPIO_NUM_2   // I2S 位时钟引脚（Bit Clock）
#define ES8311_I2S_MCK_PIN      GPIO_NUM_3   // I2S 主时钟引脚（Master Clock）
#define ES8311_I2S_DATA_IN_PIN  GPIO_NUM_4   // I2S 数据输入引脚（麦克风数据到 ESP32）
#define ES8311_I2S_DATA_OUT_PIN GPIO_NUM_6   // I2S 数据输出引脚（ESP32 数据到扬声器）
#define ES8311_I2S_DATA_WS_PIN  GPIO_NUM_5   // I2S 声道选择引脚（Word Select，左右声道切换）

// 功放控制引脚定义
#define ES8311_PA_PIN GPIO_NUM_46  // 扬声器功放（Power Amplifier）使能引脚

// -------------------------- 函数声明（对外接口） --------------------------
/**
 * @brief ES8311 初始化函数
 * @details 完成 I2C、I2S 接口初始化，配置 ES8311 工作参数并启动设备
 * @note 必须在使用音频功能前调用，且仅需调用一次
 */
void Int_ES8311_Init(void);

/**
 * @brief ES8311 取消初始化函数
 * @details 关闭设备，释放 I2C、I2S 资源，恢复引脚默认状态
 * @note 在设备不再使用时调用，避免资源泄露
 */
void Int_ES8311_DeInit(void);

/**
 * @brief 从麦克风读取音频数据
 * @param data 输出参数：存储读取到的音频数据缓冲区（16bit 单声道，小端格式）
 * @param len  输入参数：需要读取的数据长度（单位：字节，需为 2 的倍数）
 * @note 数据格式需与初始化时配置的采样参数（16000Hz、16bit）匹配
 */
void Int_ES8311_ReadFromMic(uint8_t data[], size_t len);

/**
 * @brief 向扬声器写入音频数据
 * @param data 输入参数：待播放的音频数据缓冲区（16bit 单声道，小端格式）
 * @param len  输入参数：需要写入的数据长度（单位：字节，需为 2 的倍数）
 * @note 数据格式需与初始化时配置的采样参数一致，否则可能产生噪音
 */
void Int_ES8311_WriteToSpeaker(uint8_t data[], size_t len);

#endif /* __INT_ES8311_H__ */
// 头文件结束标记
