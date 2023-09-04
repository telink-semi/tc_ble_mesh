/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_LOG_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "ble_qiot_mesh_cfg.h"
//#include "esp_log.h"

typedef enum {
    BLE_QIOT_LOG_LEVEL_NONE = 0,
    BLE_QIOT_LOG_LEVEL_ERR,
    BLE_QIOT_LOG_LEVEL_WARN,
    BLE_QIOT_LOG_LEVEL_INFO,
    BLE_QIOT_LOG_LEVEL_DEBUG,
    BLE_QIOT_LOG_LEVEL_ALL,
} e_ble_qiot_log_level;

// log new line feed type
#define LINE_NONE
#define LINE_LF   "\n"
#define LINE_CR   "\r"
#define LINE_CRLF "\r\n"

// log new line feed type config
#define LOG_LINE_FEED_TYPE LINE_CRLF

extern e_ble_qiot_log_level g_log_level;

#ifndef ble_qiot_log_d
#define ble_qiot_log_d(fmt, args...)                                   \
    do {                                                               \
        if (g_log_level < BLE_QIOT_LOG_LEVEL_DEBUG)                    \
            break;                                                     \
        MESH_LOG_PRINT("qiot debug: " fmt LOG_LINE_FEED_TYPE, ##args); \
    } while (0)
#endif

#ifndef ble_qiot_log_i
#define ble_qiot_log_i(fmt, args...)                                  \
    do {                                                              \
        if (g_log_level < BLE_QIOT_LOG_LEVEL_INFO)                    \
            break;                                                    \
        MESH_LOG_PRINT("qiot info: " fmt LOG_LINE_FEED_TYPE, ##args); \
    } while (0)
#endif

#ifndef ble_qiot_log_w
#define ble_qiot_log_w(fmt, args...)                                                             \
    do {                                                                                         \
        if (g_log_level < BLE_QIOT_LOG_LEVEL_WARN)                                               \
            break;                                                                               \
        MESH_LOG_PRINT("qiot warn(%s|%d): " fmt LOG_LINE_FEED_TYPE, __FILE__, __LINE__, ##args); \
    } while (0)
#endif

#ifndef ble_qiot_log_e
#define ble_qiot_log_e(fmt, args...)                                                            \
    do {                                                                                        \
        if (g_log_level < BLE_QIOT_LOG_LEVEL_ERR)                                               \
            break;                                                                              \
        MESH_LOG_PRINT("qiot err(%s|%d): " fmt LOG_LINE_FEED_TYPE, __FILE__, __LINE__, ##args); \
    } while (0)
#endif

#ifndef ble_qiot_log
#define ble_qiot_log(level, fmt, args...)                                                       \
    do {                                                                                        \
        if (g_log_level < level)                                                                \
            break;                                                                              \
        MESH_LOG_PRINT("qiot log(%s|%d): " fmt LOG_LINE_FEED_TYPE, __FILE__, __LINE__, ##args); \
    } while (0)
#endif

// this function only use for ble_qiot_log_hex
#ifndef ble_qiot_log_raw
#define ble_qiot_log_raw(fmt, args...) \
    do {                               \
        MESH_LOG_PRINT(fmt, ##args);   \
    } while (0)
#endif

void ble_qiot_set_log_level(e_ble_qiot_log_level level);

#if !BLE_QIOT_USER_DEFINE_HEXDUMP
void ble_qiot_log_hex(e_ble_qiot_log_level level, const char *hex_name, const char *data, uint32_t data_len);
#endif  // BLE_QIOT_USER_DEFINE_HEXDUMP

#ifdef __cplusplus
}
#endif
#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_LOG_H_
