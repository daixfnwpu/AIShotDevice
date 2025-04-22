// 摄像头驱动

#include "esp_camera.h"

#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

#define LED_GPIO_NUM      21


#define SCREEN_HEIGHT  240
#define SCREEN_WIDTH  240
// #define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
// #define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
// #define EI_CAMERA_FRAME_BYTE_SIZE                 3

// 摄像头初始化设置
void CameraInit(void);

// 显示图像
void CameraShow(void);

#endif // CAMERA_H