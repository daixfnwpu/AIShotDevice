// 读取陀螺仪数据
// motion.h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include "appconfig.h"

#ifndef MOTION_H
#define MOTION_H

#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))
#define ACC_RANGE 1 // 0: -/+2G; 1: +/-4G
// convert factor g to m/s2 ==> [-32768, +32767] ==> [-2g, +2g]
#define CONVERT_G_TO_MS2 (9.81/(16384.0/(1.+ACC_RANGE)))

// 初始化陀螺仪
void MotionInit(void);

// 读陀螺仪数据
void ReadMotion(float *data);

// 陀螺仪测试
void MotionLoop(void);
#endif // MOTION_H