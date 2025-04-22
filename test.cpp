#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <TJPG_Decoder.h>


#define TESTFILEPATH "/imagetest.jpg"

#define IMAGE_MAX_WIDTH  320         // 假设最大图像宽度
#define IMAGE_MAX_HEIGHT 240         // 假设最大图像高度

// 定义全局图像缓冲区
uint8_t imageBuffer[IMAGE_MAX_WIDTH * IMAGE_MAX_HEIGHT * 2];  // 假设 RGB565 格式，2 字节每像素
uint8_t *pBuffer = imageBuffer;  // 指向缓冲区的指针
// 定义全局变量存储图像宽度和高度
int imgWidth = 0;
int imgHeight = 0;

void writeFile(const char *path, uint8_t *data, size_t len);
bool sd_init() {
  bool sd_sign;
  // Initialize SD card
  if (!SD.begin(21)) {
    Serial.println("Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();

  // Determine if the type of SD card is available
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  sd_sign = true;  // sd initialization check passes
  return sd_sign;
}



// Save pictures to SD card
void photo_save(const char *fileName,uint8_t * buf,int len) {

  if (fileName == NULL) {
    char filename[32];
    sprintf(filename, TESTFILEPATH, 0);
    fileName = filename;
  }
  // Save photo to file
  writeFile(fileName, buf, len);
  // Release image buffer
  Serial.println("Photo saved to file :");
  Serial.println(TESTFILEPATH);
}

// SD card write file
void writeFile(const char *path, uint8_t *data, size_t len) {
  Serial.printf("Writing file: %s\n", path);
  fs::FS &fs = SD;
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.write(data, len) == len) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// 自定义回调函数，用于将解码后的像素存储到缓冲区
bool imageOutputCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // 在第一次回调时，记录图像的宽度和高度
  if (y == 0 && x == 0) {
    imgWidth = w;
    imgHeight = h;
  }

  // 确保 x, y 不超出图像范围
  if (x >= IMAGE_MAX_WIDTH || y >= IMAGE_MAX_HEIGHT) return 0;

  // 逐行存储解码后的像素
  for (int i = 0; i < h; i++) {
    memcpy(&pBuffer[(y + i) * IMAGE_MAX_WIDTH * 2 + x * 2], &bitmap[i * w], w * 2);
  }

  return 1;  // 返回 1 表示成功
}

// 函数：从 SD 卡加载 JPEG 并返回图像缓冲区指针
uint8_t* loadJpeg() {
  // 打开 JPEG 文件
  File jpgFile = SD.open(TESTFILEPATH);
  if (!jpgFile) {
    Serial.println("Failed to open file for reading");
    return NULL;
  }

  // 注册自定义回调函数，将解码后的像素存储到缓冲区
  TJpgDec.setCallback(imageOutputCallback);

  // 解码 JPEG 文件（此时像素数据将存储在 imageBuffer 中）
  TJpgDec.drawFsJpg(0, 0, TESTFILEPATH);

  // 打印图像尺寸
  Serial.print("Width: ");
  Serial.println(imgWidth);
  Serial.print("Height: ");
  Serial.println(imgHeight);

  // 返回指向图像缓冲区的指针
  return imageBuffer;
}
