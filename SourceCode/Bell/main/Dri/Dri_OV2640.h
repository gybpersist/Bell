#ifndef __DRI_OV2640_H__
#define __DRI_OV2640_H__

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"

#include "Com/Com_Debug.h"

#define CAM_PIN_PWDN 40
#define CAM_PIN_RESET 42 // software reset will be performed
#define CAM_PIN_VSYNC 41
#define CAM_PIN_HREF 39
#define CAM_PIN_PCLK 34
#define CAM_PIN_XCLK 37
#define CAM_PIN_SIOD 0
#define CAM_PIN_SIOC 1
#define CAM_PIN_D0 9
#define CAM_PIN_D1 11
#define CAM_PIN_D2 12
#define CAM_PIN_D3 10
#define CAM_PIN_D4 33
#define CAM_PIN_D5 35
#define CAM_PIN_D6 36
#define CAM_PIN_D7 38

/**
 * @brief OV2640初始化
 * 
 */
void Dri_OV2640_Init(void);

/**
 * @brief 拿到照片数据
 * 
 * @param data 拿到的数据
 * @param len 数据长度
 */
void Dri_OV2640_TakePic(uint8_t **data,uint32_t *len);

/**
 * @brief 把照片数据上传到服务端
 * 
 */
void Dri_OV2640_ReturnPic(void);

#endif /* __DRI_OV2640_H__ */
