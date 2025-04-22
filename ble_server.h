// BLE 低功耗蓝牙服务，用于向手机发送通知消息
#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEServer.h>
#include "NVSConfig.h"

/*
* 低功耗蓝牙使用的UUID被分为5组：
0x1800 ~ 0x26FF：服务类型；https://www.bluetooth.com/specifications/gatt/services
0x2700 ~ 0x27FF：计量单位；
0x2800 ~ 0x28FF：属性类型；https://www.bluetooth.com/specifications/gatt/declarations
0x2900 ~ 0x29FF：特性描述；https://www.bluetooth.com/specifications/gatt/descriptors
0x2A00 ~ 0x7FFF：特性类型；https://www.bluetooth.com/specifications/gatt/characteristics

*/
// 定义UUID
#define ENV_SERVICE_UUID     0x181A // 定义服务UUID位环境监控服务
#define ENV_ANGLE_UUID    0x2A6E // 角度调整， 这里是用的温度代替
#define ENV_STRENGTH_UUID    0x2A6E // 力度调整， 这里是用的湿度代替
// 定义 UUID（需要与你的特性UUID匹配）
#define ENV_WIFISSID_UUID         0x2A71
#define ENV_WIFIPASS_UUID         0x2A72
#define ENV_RADIUS_UUID         0x2A6E
#define ENV_VELOCITY_UUID       0x2A6F
#define ENV_VELOCITY_ANGLE_UUID 0x2A60
#define ENV_DENSITY_UUID        0x2A61
#define ENV_EYE_TO_BOW_DISTANCE_UUID 0x2A62
#define ENV_EYE_TO_AXIS_DISTANCE_UUID 0x2A63
#define ENV_SHOT_DOOR_WIDTH_UUID 0x2A64
#define ENV_SHOT_HEAD_WIDTH_UUID 0x2A65
#define ENV_SHOT_DISTANCE_UUID  0x2A66
#define ENV_SHOT_DIFF_DISTANCE_UUID 0x2A67
#define ENV_ANGLE_TARGET_UUID   0x2A68
#define ENV_COMMAND_UUID        0x2A70
#define ENV_SHOT_POINT_HEAD_UUID 0x2A69

// NVS key constants
#define NVS_NAMESPACE "my_space"




// 定义通知的值
#define BLE_TURN_LEFT 0x01
#define BLE_TURN_RIGHT 0x02
#define BLE_TURN_UP 0x03
#define BLE_TURN_DOWN 0x04

// 设置初始值
void ble_server_setup(void);
// 向客户端发送通知
void send_notification(BLECharacteristic* pCharacteristic, int value);
void send_notifications(BLEService* pService,CharacteristicValueForClient * cvfc);

// 提示操作
//void turn_opertae(uint8_t operate);
#endif