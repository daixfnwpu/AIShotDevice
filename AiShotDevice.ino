// #include "Wire.h"
// #include "motion.h"
// #include "sk60.h"
// #include "stepmotor.h"
// #include "st7789.h"
// #include "camera.h"
// #include "oled.h"
// #include "ble_server.h"
// #include "esp_system.h"


// // 定义 led闪烁控制脚
// #define LED_BUILT_IN 21
// void myCrashHandler(TaskHandle_t xTask, UBaseType_t uxTaskPriority);
// void myCrashHandler(TaskHandle_t xTask, UBaseType_t uxTaskPriority) {
//     // 自定义的错误处理代码
//     Serial.println("ESP32 has encountered an error. No automatic restart.");
//     // 这里可以添加更多的处理逻辑，比如保存错误信息等
// }
// void setup() {
//   Serial.begin(115200);
//   psramInit();
//   // 调试串口设置波特率

//  // esp_set_error_handler(myCrashHandler);
//   if (psramFound())
//   {
//     Serial.println("PSRAM found and enabled");
//   } else {
//     Serial.println("PSRAM not found");
//   }

//   // 检查内存
//   Serial.printf("Total head: %d\n", ESP.getHeapSize());
//   Serial.printf("Free head: %d\n", ESP.getFreeHeap());
//   Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
//   Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());



//   // 激光测距仪初始化
//   SK60Init();


//   // 陀螺仪相关的初始化设置
//   MotionInit();

//   // 马达初始化设置，每步1.8度，即一圈200步
//   // StepperInit(30, 200);

//   // 显示屏的初始化设置
//   InitSt7789();

//   // OLED显示屏的初始化设置
//   InitOled();

//   // 摄像头的初始化设置
//   CameraInit();

//   // BLE 的初始化设置
//   ble_server_setup();
// }

// int frame_num = 0; // 图像帧计数，用于主循环的时间切片，模拟多线程
// int sk60_num = 0; // 测距仪开启计数，0表示关闭状态。

// void loop() {
//   frame_num++;
//   if (frame_num % 50 == 0) // 每50帧执行以下指令
//   {
//     // 读取陀螺仪数据
//     // Serial.println("start motion...");
//     float data[6] = {0.0};
//     ReadMotion(data);
//     // 显示到oled屏上
//     memset(GetOledLine1Buf(), 0, 64);
//     sprintf(GetOledLine1Buf(), "g:%.1f  %.1f  %.1f", data[0], data[1], data[2]); // 角度
//     memset(GetOledLine2Buf(), 0, 64);
//     sprintf(GetOledLine2Buf(), "a:%.1f  %.1f  %.1f", data[3], data[4], data[5]); // 坐标
//     OledDisplay();

//     // 步进电机操作
//     // Serial.println("start setp motor...");
//     // StepperLoop();

//     // 蓝牙提示通知
//     turn_opertae(BLE_TURN_LEFT);

//     // 激光测距仪
//     if ((frame_num % 500 == 0) && (sk60_num == 0)) // 开启测距仪
//     {
//       sk60_num++;
//       Sk60Start();
//     }
//     // 测距仪计时
//     if (sk60_num > 0) // 如果开启
//     {
//       sk60_num++;
//       if (sk60_num > 2)
//       {
//         uint8_t sk60_buf[13] = {0};
//         Sk60Read(sk60_buf);

//         // 在oled中显示测距仪数据
//         memset(GetOledLine3Buf(), 0, 64);
//         sprintf(GetOledLine3Buf(), "%dmm %d", swap_32b_bytes(*(uint32_t *)(sk60_buf+6)), swap_16b_bytes(*(uint16_t *)(sk60_buf+10)));
//         OledDisplay();
//       }
//     }

//     if (sk60_num > 5) // 读取5次后关闭测距仪
//     {
//       sk60_num = 0;
//       Sk60Stop();
//     }

//   }

//   // 显示视频
//   CameraShow();
//   if (frame_num > 10000)
//   {
//     frame_num = 0;
//   }

// }




// put your setup code here, to run once:
#include <stdio.h>
#include <img_converters.h>
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "dl_tool.hpp"
#include "esp_camera.h"
#include "appconfig.h"
#include "camera_pins.h"
#include "conture.h"
#include "test.h"
#include "engine.h"
#include <WiFi.h>
#include "Wire.h"
#include "motion.h"
#include "sk60.h"
#include "stepmotor.h"
#include "st7789.h"
#include "camera.h"
#include "oled.h"
#include "ble_server.h"
#include "NVSConfig.h"

#ifdef DEBUG
#include "esp_heap_trace.h"
#define HEAP_TRACE_ALL
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS];

#endif
// 定义 led闪烁控制脚
#define LED_BUILT_IN 21

int frame_num = 0;  // 图像帧计数，用于主循环的时间切片，模拟多线程
int sk60_num = 0;   // 测距仪开启计数，0表示关闭状态。

static const float focalLength = 3.6;   // in mm, OV2640 focal length
static const float sensorWidth = 3.6;   // in mm, OV2640 sensor width
static const float sensorHeight = 2.7;  // in mm, OV2640 sensor height
static float horizontalFOV = 2 * atan(sensorWidth / (2 * focalLength)) * (180 / PI);
static float verticalFOV = 2 * atan(sensorHeight / (2 * focalLength)) * (180 / PI);
float eyetoaxis_pixels = 0;
HumanFaceDetectMSR01 *s1;
HumanFaceDetectMNP01 *s2;

#define TESTONLY
Point eye;
uint8_t *snapshot_buf;
dl::tool::Latency *latency;
extern Line bottomLine;
#define WAKEUP_PIN GPIO_NUM_41


bool firsttimerun = true;
void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  //esp_set_error_handler(myCrashHandler);
  psramInit();
  if (psramFound()) {
    Serial.println("PSRAM found and enabled");
  } else {
    Serial.println("PSRAM not found");
  }

  // 检查内存
  Serial.printf("Total head: %d\n", ESP.getHeapSize());
  Serial.printf("Free head: %d\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());


  // 激光测距仪初始化
  SK60Init();


  // 陀螺仪相关的初始化设置
  MotionInit();

  // 马达初始化设置，每步1.8度，即一圈200步
  // StepperInit(30, 200);

  // 显示屏的初始化设置
  InitSt7789();

  // OLED显示屏的初始化设置
  InitOled();

  // 摄像头的初始化设置
  //CameraInit();

  // BLE 的初始化设置
  ble_server_setup();


#ifdef DEBUG
  // Start heap tracing
  heap_trace_init_standalone(trace_record, NUM_RECORDS);
  heap_trace_start(HEAP_TRACE_ALL);
#endif


  // initialize
  // When the serial monitor is turned on, the program starts to execute

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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;  // 其他photo server上用的：CAMERA_GRAB_LATEST；
  config.jpeg_quality = 12;
  config.fb_count = 2;
  config.frame_size = FRAMESIZE_QVGA;  // (320x240)

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("XIAO ESP32S3 Sense Camera Image Capture");
  Serial.println("Send 'capture' to initiates an image capture\n");

  s1 = new HumanFaceDetectMSR01(0.1F, 0.5F, 10, 0.2F);
  s2 = new HumanFaceDetectMNP01(0.5F, 0.3F, 5);
  latency = new dl::tool::Latency();
  snapshot_buf = (uint8_t *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * EI_CAMERA_FRAME_BYTE_SIZE);
  if (snapshot_buf == NULL) {
    Serial.println("内存分配失败");
  } else {
    Serial.println("内存分配成功");
  }
  // 设置唤醒引脚的触发模式
  // pinMode(WAKEUP_PIN, INPUT_PULLUP);
  // esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 0);
  // Serial.println("设备即将进入深睡眠模式...");


  //esp_deep_sleep_start();

  // for test ;
  sd_init();


  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to get camera frame buffer");
    return;
  }
  esp_camera_fb_return(fb);
  uint8_t *buf = fb->buf;
  int len = fb->len;
  photo_save(NULL, buf, len);


  Serial.println("Connecting to Wi-Fi...");
  char ssid_value[128];
  esp_err_t result = load_characteristic_value("wifissid", ssid_value, sizeof(ssid_value));
  if (result == ESP_OK) {
    printf("Loaded value: %s\n", ssid_value);
  } else {
    printf("Error loading value: %d\n", result);
  }

  char pass_value[128];
  result = load_characteristic_value("wifipass", pass_value, sizeof(pass_value));
  if (result == ESP_OK) {
    printf("Loaded value: %s\n", pass_value);
  } else {
    printf("Error loading value: %d\n", result);
  }
  if (result == ESP_OK) {
    WiFi.begin(pass_value, pass_value);
    int try_nums = 0;
    // 等待连接成功
    while (WiFi.status() != WL_CONNECTED && try_nums < 10) {
      delay(1000);
      Serial.println("Connecting...");
      try_nums++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Connected to Wi-Fi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
  }
}


void loop() {

  //starting from xuebin;
  frame_num++;
  if (frame_num % 50 == 0)  // 每50帧执行以下指令
  {
    // 读取陀螺仪数据
    Serial.println("start motion...");
    float data[6] = { 0.0 };
    ReadMotion(data);
    // 显示到oled屏上
    memset(GetOledLine1Buf(), 0, 64);
    sprintf(GetOledLine1Buf(), "g:%.1f  %.1f  %.1f", data[0], data[1], data[2]);  // 角度
    memset(GetOledLine2Buf(), 0, 64);
    sprintf(GetOledLine2Buf(), "a:%.1f  %.1f  %.1f", data[3], data[4], data[5]);  // 坐标
    OledDisplay();

    // 步进电机操作
    // Serial.println("start setp motor...");
    // StepperLoop();

    // 蓝牙提示通知
    //turn_opertae(BLE_TURN_LEFT);

    // 激光测距仪
    if ((frame_num % 500 == 0) && (sk60_num == 0))  // 开启测距仪
    {
      sk60_num++;
      Sk60Start();
    }
    // 测距仪计时
    if (sk60_num > 0)  // 如果开启
    {
      sk60_num++;
      if (sk60_num > 2) {
        uint8_t sk60_buf[13] = { 0 };
        Sk60Read(sk60_buf);

        // 在oled中显示测距仪数据
        memset(GetOledLine3Buf(), 0, 64);
        sprintf(GetOledLine3Buf(), "%dmm %d", swap_32b_bytes(*(uint32_t *)(sk60_buf + 6)), swap_16b_bytes(*(uint16_t *)(sk60_buf + 10)));
        OledDisplay();
      }
    }

    if (sk60_num > 5)  // 读取5次后关闭测距仪
    {
      sk60_num = 0;
      Sk60Stop();
    }
  }

  // 显示视频
  //CameraShow();
  if (frame_num > 10000) {
    frame_num = 0;
  }

  //end from xuebin

  delay(1000);
  latency->start();

  int src_len = IMAGE_HEIGHT * IMAGE_WIDTH * EI_CAMERA_FRAME_BYTE_SIZE;
#ifdef TESTONLY
  uint8_t *buf = loadJpeg();

  size_t out_len;
  //fmt2rgb888 里面会对snapshot_buf进行重新分批；
  bool converted = fmt2rgb888(buf, src_len, PIXFORMAT_JPEG, snapshot_buf);
  Serial.println("out_len is: ");
  Serial.println(out_len);
  if (!converted)
    Serial.println("converted rgb 888 failture");
#else

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to get camera frame buffer");
    return;
  }
  if (fb->format == PIXFORMAT_JPEG) {
    size_t out_len;
    bool converted = fmt2rgb888(fb->buf, src_len, PIXFORMAT_JPEG, snapshot_buf);
    if (!converted)
      Serial.println("converted rgb 888 failture");
    esp_camera_fb_return(fb);
  }
#endif

  Image *img = loadImage(snapshot_buf);
  const auto &[findContour, topLine] = findContours(img);
  // bool findContour = std::get<0>(contourFinded);
  if (!findContour)
    return;
  // Line topLine = contourFinded.second;
  double eyetoaxis = 0;
  free(img);
  bool findeye = false;
  std::list<dl::detect::result_t> &candidates = s1->infer((uint8_t *)snapshot_buf, { IMAGE_HEIGHT, IMAGE_WIDTH, EI_CAMERA_FRAME_BYTE_SIZE });
  std::list<dl::detect::result_t> &results = s2->infer((uint8_t *)snapshot_buf, { IMAGE_HEIGHT, IMAGE_WIDTH, EI_CAMERA_FRAME_BYTE_SIZE }, candidates);
  for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++) {
    Serial.println("right eye:");
    Serial.println(prediction->keypoint[6]);
    Serial.println(prediction->keypoint[7]);
    eye = { prediction->keypoint[6], prediction->keypoint[7] };
    float distance = perpendicularDistance(topLine, eye);

    // 从图片上没有办法计算眼睛到轴的距离，因为其计算出来的只是像素的差异，而非实际的值。

    Serial.print("For test ,the distance between the eye and topLine is : ");
    eyetoaxis_pixels = perpendicularDistanceByAxis(eye, topLine, bottomLine);
    Serial.print("For test ,the distance between the eye and axis is : ");
    Serial.println(distance);
    findeye = true;
  }
  if (findeye) {
    double theta0 = 45;
    double distancehandtoeye = 0.7;  // 该值该怎么获得呢？可以从图片中推算出来吗？如果不行的话，需要通过校正来获得；
    double shotdistance = 20;        // 从激光测距仪中得到;
    double r = 0.005;
    double v0 = 60;
    double r0 = 7.6;
    float objectAngle = (float)eyetoaxis_pixels / IMAGE_WIDTH * horizontalFOV;
    eyetoaxis = 2 * distancehandtoeye * tan(objectAngle / 2 * (PI / 180));
    ;  // only for test;
    std::pair<double, double> tpos = initdistanceandtrajectory(r, v0, theta0, r0, shotdistance);
    double positionOnShotHead = run(tpos, theta0, distancehandtoeye, eyetoaxis, shotdistance);
    Serial.print("the positionOnShotHead is: ");
    Serial.println(positionOnShotHead);  // 去点亮那个对应的led；
  }
#ifdef DEBUG
  heap_trace_stop();
  // Print trace results
  heap_trace_dump();
#endif
  // if(findeye == true)
  //   delay(4000);
  // else
  //   delay(1000);
}
