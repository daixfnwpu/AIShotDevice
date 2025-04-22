#include "oled.h"

// SSD1306 display(OLED_ADDRESS, IIC_SDA, IIC_SCL); // 指定IIC的管脚
SSD1306Wire display(0x3c, SDA, SCL); // IIC用wire定义的缺省管脚

// 第一行显示buf
char line1_buf[64] = {0};
// 第二行显示buf
char line2_buf[64] = {0};
// 第三行显示buf
char line3_buf[64] = {0};

// OLED显示屏的初始化设置
void InitOled(void)
{
  display.init();
  delay (500); //Wait 0.5 sec
  display.flipScreenVertically();
  SetFont(24);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  // OledLoop();
}

// 设置对齐和字体
void SetFont(uint8_t size)
{
  
  if (size == 10)
  {
    display.setFont(ArialMT_Plain_10);
  }
  else if(size == 16)
  {
    display.setFont(ArialMT_Plain_16);
  }
  else if(size == 24)
  {
    display.setFont(ArialMT_Plain_24);
  }
}

// OLED显示屏显示字符
void DrawString(uint8_t x, uint8_t y, String buf)
{
  display.drawString(x, y, buf);
  display.display();
}

// 刷新显示
void OledDisplay(void)
{
  // clear the display
  display.clear();
  SetFont(16);
  DrawString(0, 0, line1_buf);
  DrawString(0, 20, line2_buf);
  SetFont(24);
  DrawString(0, 38, line3_buf);
  display.display();
}

// 获取第一行缓存地址
char *GetOledLine1Buf(void)
{
  return line1_buf;
}

// 获取第二行缓存地址
char *GetOledLine2Buf(void)
{
  return line2_buf;
}

// 获取第三行缓存地址
char *GetOledLine3Buf(void)
{
  return line3_buf;
}

// OLED显示屏显示字符
void OledLoop(void)
{
  // clear the display
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");

  display.display();
}