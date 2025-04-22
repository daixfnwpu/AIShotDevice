#include "camera.h"
#include "st7789.h"
#include <TJpg_Decoder.h>

// 设置显示模式, 0=压缩过的图片jpeg显示; 1=没压缩的像素
bool display_mode = 0; 

TFT_eSPI *tft = GetTft();

// 显示图像回调函数
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // 超出屏幕底部的数据丢弃
  if ( y >= tft->height() ) return 0;

  // 画image到屏幕
  tft->pushImage(x, y, w, h, bitmap);

  return 1;
} 

// 摄像头初始化设置
void CameraInit(void)
{
  display_mode = 0; // 设置显示模式

  if(!display_mode)
  {
    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output); // 设置回调函数
  }
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    if (display_mode)
    {
      config.pixel_format = PIXFORMAT_RGB565;
      config.frame_size = FRAMESIZE_240X240;
    }else{
      config.frame_size = FRAMESIZE_240X240; // 320x240
    }
    
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  Serial.println("XIAO ESP32S3 Sense Camera Image Capture");

}

// 显示图像
void CameraShow(void)
{
  camera_fb_t *fb = esp_camera_fb_get();
  // Serial.printf("buf_len: %d\n", fb->len); // fb->len = 240*240*2(每个像素点占用2个字节)

  if (display_mode)
  {
    // 没压缩的像素显示
    unsigned long i=0;
    for (uint32_t y = 0; y < SCREEN_HEIGHT; y++) // SCREEN_HEIGHT = 198
    {
      for (uint32_t x = 0; x < SCREEN_WIDTH; x++) // SCREEN_WIDTH = 240
      {
        tft->drawPixel(x, y, 0x0000ffff&((fb->buf[2*i]<<8)|(fb->buf[2*i+1])));
        i++;
      }
    }
  }
  else 
  {
    // 显示压缩的jpeg格式图像显示
    if(!fb || fb->format != PIXFORMAT_JPEG){
      Serial.println("Camera capture failed");
      esp_camera_fb_return(fb);
      return;
    }else{
      TJpgDec.drawJpg(0,0,(const uint8_t*)fb->buf, fb->len);
      esp_camera_fb_return(fb);
    }
  }
  
}


