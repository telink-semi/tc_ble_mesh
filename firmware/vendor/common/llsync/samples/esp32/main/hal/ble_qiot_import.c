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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_bt.h"
#include "esp_spi_flash.h"

#define PRODUCT_ID    "PRODUCTID"
#define DEVICE_NAME   "DEVICE_MAC_ADDR"  //注意此处的dev name使用设备的mac地址来命名
#define DEVICE_SECRET "DEVICE_PSK"

char *DevNameGet(void)
{
    return DEVICE_NAME;
}

ble_qiot_ret_status_t llsync_mesh_dev_info_get(ble_device_info_t *dev_info)
{
    memcpy(dev_info->product_id, PRODUCT_ID, LLSYNC_MESH_PRODUCT_ID_LEN);
    memcpy(dev_info->device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(dev_info->psk, DEVICE_SECRET, LLSYNC_MESH_PSK_LEN);

    return LLSYNC_MESH_RS_OK;
}

ble_qiot_ret_status_t llsync_mesh_adv_handle(e_llsync_mesh_user adv_status, uint8_t *data_buf, uint8_t data_len)
{
    if (LLSYNC_MESH_ADV_START == adv_status) {
        ble_qiot_log_i("adv start");
        // esp_err_t ret = esp_ble_gap_config_adv_data_raw(data_buf, data_len);
        // if (ret) {
        //     ble_qiot_log_i("config adv data failed, error code = %x", ret);
        // }
    } else if (LLSYNC_MESH_ADV_STOP == adv_status) {
        ble_qiot_log_i("adv stop");
        // esp_ble_gap_stop_advertising();
    }

    return LLSYNC_MESH_RS_OK;
}

typedef struct ble_esp32_timer_id_ {
    uint8_t       type;
    ble_timer_cb  handle;
    TimerHandle_t timer;
} ble_esp32_timer_id;

ble_timer_t llsync_mesh_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    ble_esp32_timer_id *p_timer = malloc(sizeof(ble_esp32_timer_id));
    if (NULL == p_timer) {
        return NULL;
    }

    p_timer->type   = type;
    p_timer->handle = timeout_handle;
    p_timer->timer  = NULL;

    return (ble_timer_t)p_timer;
}

ble_qiot_ret_status_t llsync_mesh_timer_start(ble_timer_t timer_id, uint32_t period)
{
    ble_esp32_timer_id *p_timer = (ble_esp32_timer_id *)timer_id;

    if (NULL == p_timer->timer) {
        p_timer->timer =
            (ble_timer_t)xTimerCreate("bro_timer", period / portTICK_PERIOD_MS, pdTRUE, NULL, p_timer->handle);
    }
    xTimerReset(p_timer->timer, portMAX_DELAY);

    return LLSYNC_MESH_RS_OK;
}

ble_qiot_ret_status_t llsync_mesh_timer_stop(ble_timer_t timer_id)
{
    ble_esp32_timer_id *p_timer = (ble_esp32_timer_id *)timer_id;
    xTimerStop(p_timer->timer, portMAX_DELAY);

    return LLSYNC_MESH_RS_OK;
}

int llsync_mesh_flash_handle(e_llsync_flash_user type, uint32_t flash_addr, char *buf, uint16_t len)
{
    int ret = 0;

    if (LLSYNC_MESH_WRITE_FLASH == type) {
        ret = spi_flash_erase_range(flash_addr, LLSYNC_MESH_RECORD_FLASH_PAGESIZE);
        ret = spi_flash_write(flash_addr, buf, len);
    } else if (LLSYNC_MESH_READ_FLASH == type) {
        ret = spi_flash_read(flash_addr, buf, len);
    }

    return ret == ESP_OK ? len : ret;
}

#ifdef __cplusplus
}
#endif
