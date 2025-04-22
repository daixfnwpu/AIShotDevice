#include "nvs_flash.h"
#include "nvs.h"
#include "NVSConfig.h"

void init_nvs() {
    // 初始化NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();  // 擦除旧版本
        nvs_flash_init();
    }
}



esp_err_t load_characteristic_value(const char* key, int32_t* value) {
    nvs_handle_t handle;
    esp_err_t err;

    // 打开NVS句柄，NVS的命名空间为"storage"
    err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    // 读取整数类型的值
    err = nvs_get_i32(handle, key, value);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // 如果没有找到，提供一个默认值
        *value = 0;
        err = ESP_OK;
    }

    // 关闭NVS句柄
    nvs_close(handle);
    return err;
}

esp_err_t load_characteristic_value(const char* key, char* value, size_t max_len) {
    nvs_handle_t handle;
    esp_err_t err;

    // 打开NVS句柄，NVS的命名空间为"storage"
    err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    // 获取字符串长度
    size_t required_len = max_len;
    err = nvs_get_str(handle, key, NULL, &required_len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // 如果没有找到，提供一个默认值
        strncpy(value, "", max_len);  // 默认值为空字符串
        err = ESP_OK;
    } else if (err == ESP_OK) {
        // 确保缓冲区足够大，读取字符串
        if (required_len <= max_len) {
            err = nvs_get_str(handle, key, value, &required_len);
        } else {
            err = ESP_ERR_NO_MEM;  // 缓冲区太小
        }
    }

    // 关闭NVS句柄
    nvs_close(handle);
    return err;
}



esp_err_t save_characteristic_values(const CharacteristicValue* characteristics) {
    nvs_handle_t handle;
    esp_err_t err;

    // 打开 NVS 存储区域，"storage" 是命名空间，NVS_READWRITE 表示读写权限
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;  // 打开 NVS 时出错，返回错误代码
    }

    // 批量保存特性值，减少调用 commit 的次数
    err = nvs_set_i32(handle, "radius", characteristics->radius);
    if (err != ESP_OK) {
        nvs_close(handle);  // 如果保存失败，关闭 NVS 句柄并返回错误
        return err;
    }

    err = nvs_set_i32(handle, "velocity", characteristics->velocity);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "velocityAngle", characteristics->velocityAngle);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "density", characteristics->density);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "eyeToBowDistance", characteristics->eyeToBowDistance);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "eyeToAxisDistance", characteristics->eyeToAxisDistance);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "shotDoorWidth", characteristics->shotDoorWidth);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "shotHeadWidth", characteristics->shotHeadWidth);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "shotDistance", characteristics->shotDistance);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "shotDiffDistance", characteristics->shotDiffDistance);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "angleTarget", characteristics->angleTarget);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "shotPointHead", characteristics->shotPointHead);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_set_i32(handle, "command", characteristics->command);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    // 提交所有保存操作
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        nvs_close(handle);  // 提交失败，关闭 NVS 句柄并返回错误
        return err;
    }

    // 关闭 NVS 句柄
    nvs_close(handle);

    return ESP_OK;  // 返回成功
}


esp_err_t load_all_characteristics(CharacteristicValue* characteristics) {
    nvs_handle_t handle;
    esp_err_t err;

    // 打开 NVS 存储区域，"storage" 是命名空间，NVS_READONLY 表示只读权限
    err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;  // 如果打开 NVS 时出错，返回错误代码
    }

    // 批量读取特性值
    err = nvs_get_i32(handle, "radius", &characteristics->radius);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);  // 读取失败，关闭 NVS 句柄并返回错误
        return err;
    }

    err = nvs_get_i32(handle, "velocity", &characteristics->velocity);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "velocityAngle", &characteristics->velocityAngle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "density", &characteristics->density);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "eyeToBowDistance", &characteristics->eyeToBowDistance);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "eyeToAxisDistance", &characteristics->eyeToAxisDistance);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "shotDoorWidth", &characteristics->shotDoorWidth);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "shotHeadWidth", &characteristics->shotHeadWidth);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "shotDistance", &characteristics->shotDistance);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "shotDiffDistance", &characteristics->shotDiffDistance);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "angleTarget", &characteristics->angleTarget);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "shotPointHead", &characteristics->shotPointHead);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "command", &characteristics->command);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return err;
    }

    // 关闭 NVS 句柄
    nvs_close(handle);

    return ESP_OK;  // 返回成功
}



esp_err_t save_string_value(const char* key, const char* value) {
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    // 保存字符串
    err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}


esp_err_t save_characteristic_value(const char* key, char* value) {
    nvs_handle_t handle;
    esp_err_t err;

    // 打开NVS句柄，NVS的命名空间为"storage"
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    // 保存整数类型的值
    err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    // 提交写入操作
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    // 关闭NVS句柄
    nvs_close(handle);
    return ESP_OK;
}

esp_err_t save_characteristic_value(const char* key, int32_t value) {
    nvs_handle_t handle;
    esp_err_t err;

    // 打开NVS句柄，NVS的命名空间为"storage"
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    // 保存整数类型的值
    err = nvs_set_i32(handle, key, value);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    // 提交写入操作
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    // 关闭NVS句柄
    nvs_close(handle);
    return ESP_OK;
}

