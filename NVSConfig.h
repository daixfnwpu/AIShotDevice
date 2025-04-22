#ifndef NVSCONFIG_H
#define NVSCONFIG_H

typedef struct {
    int32_t radius;
    int32_t velocity;
    int32_t velocityAngle;
    int32_t density;
    int32_t eyeToBowDistance;
    int32_t eyeToAxisDistance;
    int32_t shotDoorWidth;
    int32_t shotHeadWidth;
    int32_t shotDistance;
    int32_t shotDiffDistance;
    int32_t angleTarget;
    int32_t shotPointHead;
    int32_t command;
} CharacteristicValue;

typedef struct {
// 假设你从 NVS 中读取这些值并发送
    int shotPointHeadValue;  // 例子值
    int angleTargetValue;    // 例子值
    int shotDistanceValue ;   // 例子值
    int eyeToAxisDistanceValue ; // 例子值
    int velocityAngleValue ; // 例子值
} CharacteristicValueForClient;


void init_nvs() ;
esp_err_t save_characteristic_value(const char* key, int32_t value);
esp_err_t save_characteristic_value(const char* key, char* value) ;
esp_err_t load_characteristic_value(const char* key, char* value, size_t max_len);
esp_err_t load_characteristic_value(const char* key, int32_t * value, size_t max_len);
esp_err_t load_all_characteristics(CharacteristicValue* characteristics);
esp_err_t save_characteristic_values(const CharacteristicValue* characteristics);


#endif