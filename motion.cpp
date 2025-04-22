// 读取陀螺仪数据相关操作
#include "motion.h"

// 定义陀螺仪部分全局变量
MPU6050 imu;

static unsigned long last_interval_ms = 0; // 最后一次读取时间
float gyro_angle_x, gyro_angle_y, gyro_angle_z;
float angle_x = 0, angle_y = 0, angle_z = 0;
int16_t ax, ay, az, gx, gy, gz;
float accel_angle_x, accel_angle_y, accel_angle_z;
float dt = 0.01;
static bool imu_success = false;
// 初始化陀螺仪
void MotionInit(void)
{
  Serial.println("Initializing MPU6500 I2C devices...");
  Wire.begin();
  imu.initialize();

  delay(50);
  // verify connection
  if (imu.testConnection()) {
    Serial.println("IMU connected");
    imu_success= true;
    delay(500);
      //Set MCU 6050 OffSet Calibration
      imu.setXAccelOffset(-4732);
      imu.setYAccelOffset(4703);
      imu.setZAccelOffset(8867);
      imu.setXGyroOffset(61);
      imu.setYGyroOffset(-73);
      imu.setZGyroOffset(35);

      /* Set full-scale accelerometer range.
      * 0 = +/- 2g
      * 1 = +/- 4g
      * 2 = +/- 8g
      * 3 = +/- 16g
      */
      imu.setFullScaleAccelRange(ACC_RANGE);

  }
  else {
    Serial.println("IMU Error");
  }
}

// 读陀螺仪数据
void ReadMotion(float *data)
{
  if(imu_success== false) {
    Serial.println("error:IMU can not be read!");
    return;
  }
  if (millis() > last_interval_ms + INTERVAL_MS) // 两次采样间隔
  {
    last_interval_ms = millis();
    // imu.getAcceleration(&ax, &ay, &az);
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float ax_m_s2 = ax * CONVERT_G_TO_MS2;
    float ay_m_s2 = ay * CONVERT_G_TO_MS2;
    float az_m_s2 = az * CONVERT_G_TO_MS2;
    Serial.printf("ax_m_s2: %.2f ay_m_s2: %.2f az_m_s2: %.2f\n", ax_m_s2, ay_m_s2, az_m_s2);
 
    // 计算基于加速度计的角度
    accel_angle_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * 180 / PI;
    accel_angle_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * 180 / PI;
    accel_angle_z = atan(sqrt(pow(ax, 2) + pow(ay, 2)) / az) * 180 / PI;

    // 将陀螺仪的角速度转为角度
    gyro_angle_x += gx / 131.0 * dt;  // 陀螺仪角速度单位为度每秒, 采样时间为dt秒
    gyro_angle_y += gy / 131.0 * dt;
    gyro_angle_z += gz / 131.0 * dt;

    // 互补滤波器结合加速度计和陀螺仪的数据
    angle_x = 0.98 * (angle_x + gyro_angle_x * dt) + 0.02 * accel_angle_x;
    angle_y = 0.98 * (angle_y + gyro_angle_y * dt) + 0.02 * accel_angle_y;
    angle_z = 0.98 * (angle_z + gyro_angle_z * dt) + 0.02 * accel_angle_z;

    Serial.printf("angle_x: %.2f angle_y: %.2f angle_z: %.2f\n", angle_x, angle_y, angle_z); 
     
    *(data) = angle_x;
    *(data+1) = angle_y;
    *(data+2) = angle_z;
    *(data+3) = ax_m_s2;
    *(data+4) = ay_m_s2;
    *(data+5) = az_m_s2;

  }
}