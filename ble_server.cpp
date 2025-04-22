#include "ble_server.h"

// BLE 服务名称
#define bleServerName "CATAPULT_BLESERVER"

// 服务特性
BLECharacteristic *pCharacteristic;
// For Client:  实际运用中，可以通过APP下发来改变；
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"
// 连接状态
bool deviceConnected = false;
bool clientDeviceConnected = false;
//客户端；
BLEClient* pClient = NULL;
BLERemoteCharacteristic* pRemoteCharacteristic = NULL;


// Callback class for client events
class MyClientCallbacks: public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
    Serial.println("Connected to BLE server as client");
    clientDeviceConnected = true;
  }

  void onDisconnect(BLEClient* pClient) {
    Serial.println("Disconnected from BLE server");
    clientDeviceConnected = false;
  }
};




void add_characteristics(BLEService* pService);
// 服务回调函数
class CatapultBLEServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    // 在断开连接后，确保设备继续广播
    BLEDevice::startAdvertising(); // 重新开始广播
  }
};

void ble_server_setup(void)
{
  BLEDevice::init(bleServerName);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new CatapultBLEServerCallbacks());
  
  /*
  * 蓝牙技术联盟SIG定义UUID共用了一个基本的UUID：0x0000xxxx-0000-1000-8000-00805F9B34FB。总共128位，为了进一步简化基本UUID，
  * 每一个蓝牙技术联盟定义的属性有一个唯一的16位UUID，以代替上面的基本UUID的‘x’部分。使用16位的UUID便于记忆和操作，
  * 如SIG定义了“Device Information”的16位UUID为0x180A。
  */
  BLEService *pService = pServer->createService(BLEUUID((uint16_t)ENV_SERVICE_UUID)); // 用环境检查类型的服务
  // pCharacteristic = pService->createCharacteristic(
  //   BLEUUID((uint16_t)ENV_ANGLE_UUID), // 设置位角度的特征值
  //   BLECharacteristic::PROPERTY_NOTIFY // 通知类型
  // );
  // /*
  // * BLE2902描述符特别用于定义客户端特征配置（Client Characteristic Configuration），它允许客户端（如智能手机或其他BLE设备）
  // * 启用或禁用对某个特征的通知（Notifications）或指示（Indications）‌
  // * 在BLE通信中，当服务器（Server）想要向客户端（Client）发送数据（如传感器读数）时，它可以使用通知或指示机制。
  // * 客户端通过修改BLE2902描述符的值来启用或禁用这些机制。如果客户端启用了通知或指示，那么当特征值发生变化时，服务器可以主动向客户端发送数据，
  // * 而无需客户端轮询服务器‌
  // */
  // pCharacteristic->addDescriptor(new BLE2902()); // 这行代码的作用是在一个特征上添加一个BLE2902描述符，以便客户端可以配置该特征的通知或指示行为‌
  add_characteristics(pService);
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);
  pAdvertising->setMinPreferred(0x1F);
  BLEDevice::startAdvertising();


 // Create the client
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallbacks());

  // Set up custom callbacks for characteristic operations
  clientCallbacks = new ClientCharacteristicCallbacksImpl();



   // Scan for devices in setup, to connect to later in loop
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);  // Active scan
  pBLEScan->start(5, false);      // Scan for 5 seconds
  
}


void add_characteristics(BLEService* pService) {
  // 添加 'wifissid' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_wifissid = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_WIFISSID_UUID), // 使用你的 UUID
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_wifissid->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

// 添加 'wifipass' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_wifipass = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_WIFIPASS_UUID), // 使用你的 UUID
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_wifipass->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'radius' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_radius = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_RADIUS_UUID), // 使用你的 UUID
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_radius->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'velocity' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_velocity = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_VELOCITY_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_velocity->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'velocityAngle' 特性，具有读取和写入属性
    BLECharacteristic* pCharacteristic_velocityAngle = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_VELOCITY_ANGLE_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE // 允许读取和写入
    );
    pCharacteristic_velocityAngle->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'density' 特性，具有读取和写入属性
    BLECharacteristic* pCharacteristic_density = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_DENSITY_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE // 允许读取和写入
    );
    pCharacteristic_density->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'eyeToBowDistance' 特性，具有读取和写入属性
    BLECharacteristic* pCharacteristic_eyeToBowDistance = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_EYE_TO_BOW_DISTANCE_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE // 允许读取和写入
    );
    pCharacteristic_eyeToBowDistance->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'eyeToAxisDistance' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_eyeToAxisDistance = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_EYE_TO_AXIS_DISTANCE_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_eyeToAxisDistance->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'shotDoorWidth' 特性，具有读取和写入属性
    BLECharacteristic* pCharacteristic_shotDoorWidth = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_SHOT_DOOR_WIDTH_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE // 允许读取和写入
    );
    pCharacteristic_shotDoorWidth->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'shotHeadWidth' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_shotHeadWidth = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_SHOT_HEAD_WIDTH_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_shotHeadWidth->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'shotDistance' 特性，具有读取、写入和通知属性
    BLECharacteristic* pCharacteristic_shotDistance = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_SHOT_DISTANCE_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // 允许读取、写入和通知
    );
    pCharacteristic_shotDistance->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'shotDiffDistance' 特性，具有读取和写入属性
    BLECharacteristic* pCharacteristic_shotDiffDistance = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_SHOT_DIFF_DISTANCE_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE // 允许读取和写入
    );
    pCharacteristic_shotDiffDistance->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'angleTarget' 特性，具有读取和写入属性
    BLECharacteristic* pCharacteristic_angleTarget = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_ANGLE_TARGET_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE // 允许读取和写入
    );
    pCharacteristic_angleTarget->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'command' 特性，具有写入属性
    BLECharacteristic* pCharacteristic_command = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_COMMAND_UUID),
        BLECharacteristic::PROPERTY_WRITE // 允许写入
    );
    pCharacteristic_command->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符

    // 添加 'shotPointHead' 特性，具有写入属性
    BLECharacteristic* pCharacteristic_shotPointHead = pService->createCharacteristic(
        BLEUUID((uint16_t)ENV_SHOT_POINT_HEAD_UUID),
        BLECharacteristic::PROPERTY_WRITE // 允许写入
    );
    pCharacteristic_shotPointHead->addDescriptor(new BLE2902()); // 添加 BLE2902 描述符
}


// BLE 回调处理类
class BleServerChacCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) override {
        std::string value = pCharacteristic->getValue().c_str(); // 获取客户端写入的数据

        // 读取特性UUID
        BLEUUID characteristicUUID = pCharacteristic->getUUID();
        
        // 将客户端写入的数据转为整数值进行保存
        // 将客户端写入的数据转为整数值进行保存
        if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_WIFISSID_UUID))) {
            save_characteristic_value("wifissid", value.c_str());
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_WIFIPASS_UUID))) { 
            save_characteristic_value("wifipass", value.c_str());
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_RADIUS_UUID))) {
            int radiusValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("radius", radiusValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_VELOCITY_UUID))) {
            int velocityValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("velocity", velocityValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_VELOCITY_ANGLE_UUID))) {
            int velocityAngleValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("velocityAngle", velocityAngleValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_DENSITY_UUID))) {
            int densityValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("density", densityValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_EYE_TO_BOW_DISTANCE_UUID))) {
            int eyeToBowDistanceValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("eyeToBowDistance", eyeToBowDistanceValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_EYE_TO_AXIS_DISTANCE_UUID))) {
            int eyeToAxisDistanceValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("eyeToAxisDistance", eyeToAxisDistanceValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_SHOT_DOOR_WIDTH_UUID))) {
            int shotDoorWidthValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("shotDoorWidth", shotDoorWidthValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_SHOT_HEAD_WIDTH_UUID))) {
            int shotHeadWidthValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("shotHeadWidth", shotHeadWidthValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_SHOT_DISTANCE_UUID))) {
            int shotDistanceValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("shotDistance", shotDistanceValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_SHOT_DIFF_DISTANCE_UUID))) {
            int shotDiffDistanceValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("shotDiffDistance", shotDiffDistanceValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_ANGLE_TARGET_UUID))) {
            int angleTargetValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("angleTarget", angleTargetValue);
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_COMMAND_UUID))) {
            // 处理命令，不需要保存，只需要执行
            // 例如：发送数据，执行特定动作等
        } else if (characteristicUUID.equals(BLEUUID((uint16_t)ENV_SHOT_POINT_HEAD_UUID))) {
            int shotPointHeadValue = atoi(value.c_str());  // 转为整数
            save_characteristic_value("shotPointHead", shotPointHeadValue);
        }
    }
};

// 向客户端发送通知
void send_notification(BLECharacteristic* pCharacteristic, int value) {
    // 将值转为字符串并发送通知
    std::string strValue = std::to_string(value);
    pCharacteristic->setValue(String(strValue.c_str()));
    pCharacteristic->notify();
}

void send_notifications(BLEService* pService,CharacteristicValueForClient * cvfc) {
    // 获取特性
    BLECharacteristic* pCharacteristic_shotPointHead = pService->getCharacteristic(BLEUUID((uint16_t)ENV_SHOT_POINT_HEAD_UUID));
    BLECharacteristic* pCharacteristic_angleTarget = pService->getCharacteristic(BLEUUID((uint16_t)ENV_ANGLE_TARGET_UUID));
    BLECharacteristic* pCharacteristic_shotDistance = pService->getCharacteristic(BLEUUID((uint16_t)ENV_SHOT_DISTANCE_UUID));
    BLECharacteristic* pCharacteristic_eyeToAxisDistance = pService->getCharacteristic(BLEUUID((uint16_t)ENV_EYE_TO_AXIS_DISTANCE_UUID));
    BLECharacteristic* pCharacteristic_velocityAngle = pService->getCharacteristic(BLEUUID((uint16_t)ENV_VELOCITY_ANGLE_UUID));
    
    // 向客户端发送通知
    send_notification(pCharacteristic_shotPointHead, cvfc->shotPointHeadValue);
    send_notification(pCharacteristic_angleTarget, cvfc->angleTargetValue);
    send_notification(pCharacteristic_shotDistance, cvfc->shotDistanceValue);
    send_notification(pCharacteristic_eyeToAxisDistance, cvfc->eyeToAxisDistanceValue);
    send_notification(pCharacteristic_velocityAngle, cvfc->velocityAngleValue);
}


// Client characteristic callback class
class MyClientCharacteristicCallbacks {
  public:
    virtual void onRead(BLERemoteCharacteristic* pRemoteCharacteristic, std::string value) = 0;
    virtual void onWrite(BLERemoteCharacteristic* pRemoteCharacteristic) = 0;
};

// A simple implementation of the callbacks class
class ClientCharacteristicCallbacksImpl : public MyClientCharacteristicCallbacks {
  void onRead(BLERemoteCharacteristic* pRemoteCharacteristic, std::string value) override {
    Serial.print("Read value from characteristic: ");
    Serial.println(value.c_str());
  }

  void onWrite(BLERemoteCharacteristic* pRemoteCharacteristic) override {
    Serial.println("Data successfully written to characteristic");
  }
};


void client_loop() {
  if (!clientDeviceConnected) {
    // Scan and connect to BLE server
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults scanResults = pBLEScan->start(5, false);  // Scan for 5 seconds

    for (int i = 0; i < scanResults.getCount(); ++i) {
      BLEAdvertisedDevice advertisedDevice = scanResults.getDevice(i);

      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(SERVICE_UUID)) {
        Serial.println("Found a device advertising our service, trying to connect...");

        pClient->connect(&advertisedDevice);
        BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
        
        if (pRemoteService != nullptr) {
          pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
          if (pRemoteCharacteristic != nullptr) {
            Serial.println("Successfully connected to remote characteristic");
          }
        }
        break;
      }
    }
  }

  // If connected, periodically read data and trigger callback
  if (clientDeviceConnected && pRemoteCharacteristic != nullptr) {
    if (pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      clientCallbacks->onRead(pRemoteCharacteristic, value);  // Trigger read callback
    }

    // Writing to the characteristic (example)
    if (pRemoteCharacteristic->canWrite()) {
      std::string writeData = "Hello from client";
      pRemoteCharacteristic->writeValue(writeData);
      clientCallbacks->onWrite(pRemoteCharacteristic);  // Trigger write callback
    }

    delay(2000);  // Delay for 2 seconds before next read/write
  }
}

// // 提示操作
// void turn_opertae(uint8_t operate)
// {
//   if ((deviceConnected) && (pCharacteristic)) {
//     switch (operate)
//     {
//       case BLE_TURN_LEFT:
//       {
//         pCharacteristic->setValue("turn_left");
//         break;
//       }
//       case BLE_TURN_RIGHT:
//       {
//         pCharacteristic->setValue("turn_right");
//         break;
//       }
//       case BLE_TURN_UP:
//       {
//         pCharacteristic->setValue("turn_up");
//         break;
//       }
//       case BLE_TURN_DOWN:
//       {
//         pCharacteristic->setValue("turn_down");
//         break;
//       }
//       default:
//         return;
//     }
//     pCharacteristic->notify();  
//   }
// }