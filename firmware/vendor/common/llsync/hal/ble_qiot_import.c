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
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "ble_qiot_import.h"
#include "ble_qiot_common.h"
#include "ble_qiot_utils_mesh.h"
#include "ble_qiot_mesh_cfg.h"
#include "ble_qiot_log.h"

// 设备三元组信息
#define PRODUCT_ID    "PRODUCTID"
#define DEVICE_NAME   "DEVICE_NAME"  //注意此处的dev name使用设备的mac地址来命名
#define DEVICE_SECRET "DEVICE_PSK"

/**
 * @brief 获取设备名
 *
 * @return 设备名称地址
 */
char *DevNameGet(void)
{
    return DEVICE_NAME;
}

/**
 * @brief 获取设备信息
 *
 * @param dev_info
 * @return 0 is success
 */
ble_qiot_ret_status_t llsync_mesh_dev_info_get(ble_device_info_t *dev_info)
{
    return LLSYNC_MESH_RS_OK;
}

/**
 * @brief 开启/关闭广播
 *
 * @param adv_status 开启/关闭
 * @param data_buf 广播内容
 * @param data_len 广播数据长度
 * @return 0 is success
 */
ble_qiot_ret_status_t llsync_mesh_adv_handle(e_llsync_mesh_user adv_status, uint8_t *data_buf, uint8_t data_len)
{
    if (LLSYNC_MESH_ADV_START == adv_status) {
        ble_qiot_log_i("adv start");
    } else if (LLSYNC_MESH_ADV_STOP == adv_status) {
        ble_qiot_log_i("adv stop");
    }
    return LLSYNC_MESH_RS_OK;
}

typedef struct ble_esp32_timer_id_ {
    uint8_t       type;
    ble_timer_cb  handle;
    TimerHandle_t timer;
} ble_esp32_timer_id;

/**
 * @brief 创建定时器
 *
 * @param type 定时器类型：单次/循环
 * @param timeout_handle 回调函数
 * @return ble_timer_t 定时器句柄
 */
ble_timer_t llsync_mesh_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    return NULL;
}

/**
 * @brief 启动定时器
 *
 * @param timer_id 定时器句柄
 * @param period 周期
 * @return 0 is success
 */
ble_qiot_ret_status_t llsync_mesh_timer_start(ble_timer_t timer_id, uint32_t period)
{
    return LLSYNC_MESH_RS_OK;
}

/**
 * @brief 停止定时器
 *
 * @param timer_id 定时器句柄
 * @return 0 is success
 */
ble_qiot_ret_status_t llsync_mesh_timer_stop(ble_timer_t timer_id)
{
    ble_esp32_timer_id *p_timer = (ble_esp32_timer_id *)timer_id;
    xTimerStop(p_timer->timer, portMAX_DELAY);

    return LLSYNC_MESH_RS_OK;
}

/**
 * @brief flash 读写函数
 *
 * @param type 读/写
 * @param flash_addr 地址
 * @param buf 读取或写入buffer
 * @param len 读取或写入长度
 * @return 读取或写入成功则返回读取或写入长度
 */
int llsync_mesh_flash_handle(e_llsync_flash_user type, uint32_t flash_addr, char *buf, uint16_t len)
{
    return 0;
}

#ifdef __cplusplus
}
#endif
