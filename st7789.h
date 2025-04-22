// 液晶显示，用于显示摄像头的视频

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>

#ifndef ST7789_H
#define ST7789_H

// 显示初始化设置
void InitSt7789(void);

// 获取tft
TFT_eSPI *GetTft(void);

// 显示测试
void LoopSt7789(void);
#endif // ST7789_H