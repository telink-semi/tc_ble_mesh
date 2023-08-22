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
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_UTILS_MESH_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_UTILS_MESH_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "ble_qiot_common.h"
#include "ble_qiot_log.h"

#define LLSYNC_MESH_CID_VENDOR             (0x013A)  // Tencent Identifiter
#define LLSYNC_MESH_VENDOR_MODEL_SERVER_ID (0x0000)  // server是0x0000, client是0x0001

#define LLSYNC_MESH_DEVICE_UUID_LEN (16)
#define LLSYNC_MESH_RAMDOM_DATA_LEN (16)
#define LLSYNC_MESH_DEV_NAME_LEN    (12)
#define LLSYNC_MESH_ADV_MAX_LEN     (32)

#define LLSYNC_MESH_SDK_VERSION ("1.0.0")  // sdk versions

#define LLSYNC_MESH_PROTOCOL_VERSION (0x01)

#define LLSYNC_MESH_CID_VENDOR_LOW (LLSYNC_MESH_CID_VENDOR & 0xff)

#define LLSYNC_MESH_CID_VENDOR_HIGH ((LLSYNC_MESH_CID_VENDOR >> 8) & 0xff)

/* Set the corresponding bit to 1 */
#define LLSYNC_ADV_STATUS_BIT_SET1(x, y) ((x) |= (0x01 << (y)))

/* Set the corresponding bit to 0 */
#define LLSYNC_ADV_STATUS_BIT_SET0(x, y) ((x) &= ~(0x01 << (y)))

/* Get the corresponding bit */
#define LLSYNC_ADV_STATUS_BIT_GET(x, y) ((x) >> (y)&0x01)

#define LLSYNC_MESH_VENDOR_TID_INDEX (0)

#define LLSYNC_MESH_VENDOR_ATT_TYPE_INDEX (1)

#define LLSYNC_MESH_UNNET_ADV_BIT (0 << 4)

#define LLSYNC_MESH_SILENCE_ADV_BIT (1 << 4)

#define LLSYNC_MESH_ADV_RFU_FLAG2 (0x00)

#define LLSYNC_MESH_ADV_COMMON_HEAD_LEN (11)

// 定时器周期(ms)
#define LLSYNC_MESH_TIMER_PERIOD (10)

// 接收TID复位周期，周期内重复TID不处理
#define LLSYNC_MESH_TID_UPDATE_PERIOD (5 * 1000)

#define LLSYNC_MESH_TID_RESET_VALUE (0xff)

#define LLSYNC_MESH_RESP_DATA_MAX_LEN (380) // 1024 // no need too much to reduce the stack size.

#define LLSYNC_MESH_RECV_MAX_TID (0x7F)

#define LLSYNC_MESH_REPORT_MAX_TID (0xBF)

#define LLSYNC_MESH_REPORT_MIN_TID (0x80)

#define LLSYNC_GET_EXPIRATION_TIME(_cur_time) ((_cur_time) + 60)

#define LLSYNC_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define LLSYNC_Min(a, b)     (((a) < (b)) ? (a) : (b))

#define LLSYNC_MESH_PARAM_VALUE_CHECK(x, err) \
    do {                                      \
        if (x == err) {                       \
            ble_qiot_log_e("Some err.");      \
            return;                           \
        }                                     \
    } while (0)

#define LLSYNC_MESH_POINTER_CHECK(x, err)           \
    do {                                            \
        if (NULL == x) {                            \
            ble_qiot_log_e("The pointer is NULL."); \
            return (err);                           \
        }                                           \
    } while (0)

#define LLSYNC_MESH_ATT_BIND_TYPE         (0xFF02)
#define LLSYNC_MESH_ATT_BIND_RESPOSE_TYPE (0xFF03)
#define LLSYNC_MESH_ATT_BIND_RESP_LOW     (LLSYNC_MESH_ATT_BIND_RESPOSE_TYPE & 0xff)
#define LLSYNC_MESH_ATT_BIND_RESP_HIGH    ((LLSYNC_MESH_ATT_BIND_RESPOSE_TYPE >> 8) & 0xff)
typedef struct ble_bind_data_t_ {
    int nonce;
    int timestamp;
} llsync_mesh_bind_data;

typedef void *ble_timer_t;
typedef struct {
    uint8_t     adv[LLSYNC_MESH_ADV_MAX_LEN];
    uint8_t     data_len;
    uint8_t     adv_status;
    uint8_t     timer_type;
    uint16_t    timer_cnt;
    uint16_t    timer_total_cnt;
    ble_timer_t mesh_timer;
} llsync_mesh_distribution_net_t;

/* timer type */
enum {
    LLSYNC_MESH_TIMER_ONE_SHOT_TYPE = 0,
    LLSYNC_MESH_TIMER_PERIOD_TYPE,
    LLSYNC_MESH_TIMER_BUTT,
};

enum {
    LLSYNC_MESH_NET_START,
    LLSYNC_MESH_NET_SUCCESS,
};

enum {
    LLSYNC_MESH_UNNET_ADV = 0,
    LLSYNC_MESH_SILENCE_ADV,
};

enum {
    LLSYNC_MESH_UNNET_ADV_TIMER,
    LLSYNC_MESH_SILENCE_ADV_TIMER,
    LLSYNC_MESH_RECV_TID_TIMER,
    LLSYNC_MESH_STOP_TIMER,
};

typedef struct ble_core_data_ {
    uint8_t bind_state;
} ble_core_data;

typedef struct {
    uint8_t recv_tid_num;
    uint8_t reoport_tid_num;
} ble_mesh_handle_t;

typedef void (*ble_timer_cb)(void *param);

typedef enum {
    LLSYNC_MESH_ADV_START,
    LLSYNC_MESH_ADV_STOP,
} e_llsync_mesh_user;

typedef enum {
    LLSYNC_MESH_WRITE_FLASH,
    LLSYNC_MESH_READ_FLASH,
} e_llsync_flash_user;

enum {
    LLSYNC_MESH_UNBIND,
    LLSYNC_MESH_BIND_OK,
};

enum {
    LLSYNC_MESH_TID_OK,
    LLSYNC_MESH_TID_REPEAT,
    LLSYNC_MESH_TID_ERR,
};

typedef struct ble_device_info_t_ {
    char product_id[LLSYNC_MESH_PRODUCT_ID_LEN];
    char device_name[LLSYNC_MESH_DEVICE_NAME_LEN + 1];
    char psk[LLSYNC_MESH_PSK_LEN];
} ble_device_info_t;

void llsync_mesh_init(void);

ble_qiot_ret_status_t llsync_mesh_get_dev_info(ble_device_info_t *dev_info);

void llsync_mesh_vendor_data_set(uint8_t *recv_data, uint16_t data_len);

void llsync_mesh_vendor_data_set_unack(uint8_t *recv_data, uint16_t data_len);

void llsync_mesh_vendor_data_get(uint8_t *recv_data, uint16_t data_len);

void llsync_mesh_vendor_op_confirmation(uint8_t *recv_data, uint16_t data_len);

void llsync_mesh_vendor_data_publish(uint32_t opcode, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_UTILS_MESH_H_
