// LED 显示
#ifndef OLED_H
#define OLED_H

#include <Wire.h>
#include "SSD1306Wire.h"

// #define IIC_SDA 21 // 定义IIC数据管脚
// #define IIC_SCL 22 // 定义IIC时钟管脚
// #define IIC_SDA -1 // IIC用wire定义的缺省管脚
// #define IIC_SCL -1 // IIC用wire定义的缺省管脚

#define OLED_ADDRESS  0x3c // loled地址：0x3c

// OLED显示屏的初始化设置
void InitOled(void);

// 设置对齐和字体
void SetFont(uint8_t size);

// OLED显示屏显示字符
void DrawString(uint8_t x, uint8_t y, String buf);

// 刷新显示
void OledDisplay(void);

// 获取第一行缓存地址
char *GetOledLine1Buf(void);

// 获取第二行缓存地址
char *GetOledLine2Buf(void);

// 获取第三行缓存地址
char *GetOledLine3Buf(void);

// OLED显示屏显示字符
void OledLoop(void);
#endif // OLED_H