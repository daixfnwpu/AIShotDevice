#ifndef APPCONFIG_H
#define APPCONFIG_H
#include <cstdint>
#include <Arduino.h>
#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>
#include <limits>
#include <functional>
void fmtrgb5652rgb888(const uint16_t *src, uint8_t *dst, int width, int height);
#define CAMERA_MODEL_XIAO_ESP32S3  // Has PSRAM
#define EI_CAMERA_FRAME_BYTE_SIZE 3
#define IMAGE_HEIGHT  240
#define IMAGE_WIDTH  320
#define HEIGHT IMAGE_HEIGHT
#define G  9.81
#define RHO  1.225  // 空气密度 (kg/m^3)
#define CD_AIR  0.47 // 球体的空气阻力系数


struct Point {
    int x, y;
};

struct Line {
    double slope = 0; // slope
    double intercept = 0; // intercept
};

struct RGB {
    int rmin, rmax;
    int gmin, gmax;
    int bmin, bmax;
};

struct Pixel {
    uint8_t r, g, b, a;
};
struct Image {
    int width, height;
    Pixel pixels[IMAGE_HEIGHT][IMAGE_WIDTH];
};


struct RubberDirection {
    int top;
    int down;
};

// struct MPUReturn {
//     float xarg, yarg,zarg;
//     float x_ms2, y_ms2,z_ms2;
// };
#endif

