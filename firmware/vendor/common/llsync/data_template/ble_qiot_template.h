/*
 * Copyright (C) 2022 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_DATA_TEMPLATE_BLE_QIOT_TEMPLATE_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_DATA_TEMPLATE_BLE_QIOT_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// ---------------------------------------------------------------------
// data struct
// ---------------------------------------------------------------------

/**
 * @brief attribute type
 *
 */
typedef enum {
    // General Property
    LLSYNC_MESH_VENDOR_ONOFF_TYPE         = 0xF000,
    LLSYNC_MESH_VENDOR_UNIX_TIME_TYPE     = 0xF001,
    LLSYNC_MESH_VENDOR_POWER_PERCENT_TYPE = 0xF002,

    // Electrician Lighting
    LLSYNC_MESH_VENDOR_HSL_TYPE              = 0xF100,
    LLSYNC_MESH_VENDOR_LIGHTNESS_LEVEL_TYPE  = 0xF101,
    LLYNSC_MESH_VENDOR_COLOR_TEMPRETURE_TYPE = 0xF102,
} mesh_vendor_type_e;

/**
 * @brief switch
 *
 */
typedef struct {
    uint8_t onoff;
} __attribute__((packed)) llsync_mesh_onoff;

/**
 * @brief timestamp
 *
 */
typedef struct {
    uint8_t unix_time[6];
} __attribute__((packed)) llsync_mesh_unix_time;

/**
 * @brief Electricity
 *
 */
typedef struct {
    uint8_t power_percent;
} __attribute__((packed)) llsync_mesh_power_percent;

/**
 * @brief color
 *
 */
typedef struct {
    uint16_t HSL[3];
} __attribute__((packed)) llsync_mesh_HSL;

/**
 * @brief lightness
 *
 */
typedef struct {
    uint16_t lightness_level;
} __attribute__((packed)) llsync_mesh_lightness_level;

/**
 * @brief color temperature
 *
 */
typedef struct {
    uint16_t color_temperature;
} __attribute__((packed)) llsync_mesh_color_temperature;

// --------------------------------------------------------------------------
// property data struct
// --------------------------------------------------------------------------

/**
 * @brief define property set handle return 0 if success, other is error
 *        sdk call the function that inform the server data to the device
 */
typedef int (*property_set_cb)(const char *data, uint16_t len);

/**
 * @brief define property get handle. return the data length obtained, -1 is error, 0 is no data
 *        sdk call the function fetch user data and send to the server,
 *        the data should wrapped by user adn skd just transmit
 */
typedef int (*property_get_cb)(char *buf, uint16_t buf_len);

/**
 * @brief each property have a struct ble_property_t
 *
 */
typedef struct {
    property_set_cb set_cb;    // set callback
    property_get_cb get_cb;    // get callback
    uint16_t        type;      // attr type
    uint16_t        data_len;  //
} ble_property_t;

// --------------------------------------------------------------
// property api
// --------------------------------------------------------------
uint16_t        llsync_mesh_vendor_size_get(void);
ble_property_t *llsync_mesh_property_array_get(void);

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_DATA_TEMPLATE_BLE_QIOT_TEMPLATE_H_