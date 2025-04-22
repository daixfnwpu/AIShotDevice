#include "HardwareSerial.h"
#include "st7789.h"

TFT_eSPI mytft = TFT_eSPI();
// TFT_eSprite eSprite = TFT_eSprite(&mytft);

// 显示初始化设置
void InitSt7789(void)
{
  Serial.println("init st 7789");
  mytft.init();
  mytft.setRotation(0);
  mytft.fillScreen(TFT_BLUE);
  mytft.initDMA();
  // eSprite.setTextFont(1);
  // eSprite.setTextColor(TFT_WHITE);
  // eSprite.createSprite(240, 240); 
  Serial.println("inited st 7789");
  LoopSt7789();
}

// 获取tft
TFT_eSPI *GetTft(void)
{
  return &mytft;
}

// 显示测试
void LoopSt7789(void)
{

  mytft.fillScreen(TFT_BLACK);  
  mytft.setTextColor(TFT_WHITE);
  mytft.drawString("Hello World!", 50,40,1);

  /*
  eSprite.setTextFont(1);
  eSprite.setCursor(20, 20);
  eSprite.println("Hello World\n");
  eSprite.setCursor(40, 40);
  eSprite.println("Hello World\n");
  eSprite.pushSprite(0, 0);
  */
  delay(1000);
}