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
#include "vendor/common/blt_soft_timer.h"
#include "../ll_app_mesh.h"

#if LLSYNC_ENABLE
// 设备编译默认三元组信息
#if 1
// HEX for BDT write to flash(len = 46): "46 52 47 51 55 41 51 54 51 36 61 34 63 31 33 38 31 31 32 32 33 33 39 6C 2B 4B 76 42 70 46 48 50 77 6E 6D 6E 30 4B 74 69 4E 4D 43 51 3D 3D"
#define PRODUCT_ID    "FRGQUAQTQ6"					// len 10
#define DEVICE_NAME   "a4c138112233"  				// len 12  //注意此处的dev name使用设备的mac地址来命名
#define DEVICE_SECRET "9l+KvBpFHPwnmn0KtiNMCQ=="	// len 24
#else
// HEX for BDT write to flash(len = 46): "46 52 47 51 55 41 51 54 51 36 61 34 63 31 33 38 31 31 32 32 34 34 66 4D 4B 42 48 45 32 33 2F 56 35 79 61 44 61 62 63 63 33 57 37 67 3D 3D"
#define PRODUCT_ID    "FRGQUAQTQ6"					// len 10
#define DEVICE_NAME   "a4c138112244"  				// len 12  //注意此处的dev name使用设备的mac地址来命名
#define DEVICE_SECRET "fMKBHE23/V5yaDabcc3W7g=="	// len 24
#endif

ble_qiot_ret_status_t llsync_mesh_dev_info_get_from_flash(ble_device_info_t *dev_info)
{
	u8 idx =0;
	u32 len_read = sizeof(dev_info->product_id);
	flash_read_page(FLASH_ADR_THREE_PARA_ADR, len_read, (u8 *)(dev_info->product_id));
	foreach_arr(i, dev_info->product_id){
		if(dev_info->product_id[i] != 0xff){
			idx += len_read;
			len_read = min(12, sizeof(dev_info->device_name)); // mac length
			flash_read_page(FLASH_ADR_THREE_PARA_ADR + idx, len_read, (u8 *)dev_info->device_name);

			idx += len_read;
			len_read = sizeof(dev_info->psk); // mac length
			flash_read_page(FLASH_ADR_THREE_PARA_ADR + idx, len_read, (u8 *)dev_info->psk);
			
			return LLSYNC_MESH_RS_OK;
		}
	}	
	
	// use default dev info
    memcpy(dev_info->product_id, PRODUCT_ID, LLSYNC_MESH_PRODUCT_ID_LEN);
    memcpy(dev_info->device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    memcpy(dev_info->psk, DEVICE_SECRET, LLSYNC_MESH_PSK_LEN);

    return LLSYNC_MESH_RS_OK;
}

/**
 * @brief 获取设备名
 *
 * @return 设备名称地址
 */
#if 0 // get from ble_device_info_t->device_name
char *DevNameGet(void)
{
    return DEVICE_NAME;
}
#endif

/**
 * @brief 获取设备信息
 *
 * @param dev_info
 * @return 0 is success
 */
ble_qiot_ret_status_t llsync_mesh_dev_info_get(ble_device_info_t *dev_info)
{
	memcpy(dev_info, &sg_dev_info, sizeof(ble_device_info_t));
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
	// no need to handle data_buf, because it will be handled in adv prepare handle.
	int print_log_flag = (LLSYNC_MESH_SILENCE_ADV_TIMER == sg_mesh_net.timer_type);
	
    if (LLSYNC_MESH_ADV_START == adv_status) {
    	if(print_log_flag){
        	ble_qiot_log_i("adv start");
        }
        mesh_gatt_adv_beacon_enable(1);		// start unprovision beacon and unprovision gatt adv.
    } else if (LLSYNC_MESH_ADV_STOP == adv_status) {
    	if(print_log_flag){
        	ble_qiot_log_i("adv stop");
        }
        mesh_gatt_adv_beacon_enable(0);		// stop unprovision beacon and unprovision gatt adv.
    }
    return LLSYNC_MESH_RS_OK;
}

//typedef void (* TimerHandle_t)();

typedef struct {
    ble_timer_cb  handle;	// if NULL means invalid timer id.
    uint8_t       type;
    u8 stop_flag; // set to 1 when call timer stop.
    u8 rsv[2];
    //TimerHandle_t timer;
} llsync_tlk_timer_id_t;

STATIC_ASSERT(sizeof(llsync_tlk_timer_id_t) % 4 == 0); // for 4 bytes aligned.


static llsync_tlk_timer_id_t llsync_mesh_timer_system; // Specifically for llsync_mesh_timer_cb
static llsync_tlk_timer_id_t llsync_tlk_timer_id_list[MAX_TIMER_NUM];

#define LLSYNC_INVALID_TIMER_ID_INDEX		(9999)

static inline int llsync_is_occupied_timer_id(llsync_tlk_timer_id_t *p)
{
	return (p->handle != NULL);
}

static int llsync_is_valid_timer_id(llsync_tlk_timer_id_t *p)
{
	return llsync_is_occupied_timer_id(p);
}

#if (LLSYNC_LOG_EN)
static int llsync_get_timer_id_index(llsync_tlk_timer_id_t *p)
{
	foreach_arr(i, llsync_tlk_timer_id_list){
		if(p == &llsync_tlk_timer_id_list[i]){
			return i;
		}
	}

	return LLSYNC_INVALID_TIMER_ID_INDEX;
}
#endif

/**
 * @brief 创建定时器
 *
 * @param type 定时器类型：单次/循环
 * @param timeout_handle 回调函数
 * @return ble_timer_t 定时器句柄
 */
ble_timer_t llsync_mesh_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
	llsync_tlk_timer_id_t * p_timer = NULL;
	if((LLSYNC_MESH_TIMER_PERIOD_TYPE == type) && (llsync_mesh_timer_cb == timeout_handle)){
		p_timer = &llsync_mesh_timer_system; // use system_time, because there is no accumulative error for LLSYNC_MESH_TIMER_PERIOD. 
	}else{
		foreach_arr(i, llsync_tlk_timer_id_list){
			llsync_tlk_timer_id_t * p_timer_id_temp = &llsync_tlk_timer_id_list[i];
			if(!llsync_is_occupied_timer_id(p_timer_id_temp)){
				p_timer = p_timer_id_temp;
				break;
			}
		}
	}

	if(p_timer != NULL){
		memset(p_timer, 0, sizeof(llsync_tlk_timer_id_t));
		p_timer->handle = timeout_handle;
		p_timer->type = type;
	}
	
    return p_timer;
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
	ble_qiot_ret_status_t st = LLSYNC_MESH_RS_OK;
	llsync_tlk_timer_id_t * p_timer = (llsync_tlk_timer_id_t *)timer_id;
	
	if(p_timer == &llsync_mesh_timer_system){ // sg_mesh_net.mesh_timer
		// auto start in llsync_mesh_timer_period_proc(). 
		ble_qiot_log_i("mesh_timer start: llsync mesh timer system, type: %d", sg_mesh_net.timer_type);
		if(LLSYNC_MESH_UNNET_ADV_TIMER == sg_mesh_net.timer_type){
			ble_qiot_log_i("enter unnet ADV mode.");
			// nothing 
		}else if(LLSYNC_MESH_SILENCE_ADV_TIMER == sg_mesh_net.timer_type){
			if(is_provision_success()){
                llsync_stop_silence_adv_timeout_check();
			}else{
				ble_qiot_log_i("enter silence ADV mode.");
				// beacon_send.inter = ; // no need to change interval, default 2 second.
			}
		}else{
			// nothing 
		}
	}else{
		if(llsync_is_valid_timer_id(p_timer)){
			int success = blt_soft_timer_update(p_timer->handle, period * 1000);
			st = success ? LLSYNC_MESH_RS_OK : LLSYNC_MESH_RS_ERR;
			if(success){
				ble_qiot_log_i("mesh_timer start id index: %d", llsync_get_timer_id_index(p_timer));
			}
		}else{
			st = LLSYNC_MESH_RS_ERR;
		}
	}
	
    return st;
}

/**
 * @brief 停止定时器
 *
 * @param timer_id 定时器句柄
 * @return 0 is success
 */
ble_qiot_ret_status_t llsync_mesh_timer_stop(ble_timer_t timer_id)
{
	ble_qiot_ret_status_t st = LLSYNC_MESH_RS_OK;
	llsync_tlk_timer_id_t * p_timer = (llsync_tlk_timer_id_t *)timer_id;
	
	if(p_timer == &llsync_mesh_timer_system){
		ble_qiot_log_i("mesh_timer stop: llsync mesh timer system"); // sg_mesh_net.timer_type has been set to LLSYNC_MESH_STOP_TIMER before. // auto stop in llsync_mesh_timer_period_proc().
		// nothing. depend on (LLSYNC_MESH_STOP_TIMER == sg_mesh_net.timer_type) to stop
	}else{
		if(llsync_is_valid_timer_id(p_timer)){
			blt_soft_timer_delete(p_timer->handle);
			ble_qiot_log_i("mesh_timer stop id index: %d", llsync_get_timer_id_index(p_timer));
		}
	}

    return st;
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
	uint16_t check_size_max = 256;
	u8 buf_temp[min(len, check_size_max)]; // LLSYNC_MESH_RECORD_FLASH_PAGESIZE
	flash_read_page(flash_addr, sizeof(buf_temp), (u8 *)buf_temp);
	
    if (LLSYNC_MESH_WRITE_FLASH == type) {
    	if((len > LLSYNC_MESH_RECORD_FLASH_PAGESIZE) || ((flash_addr & 0xffff000) != 0)){ // because will be erased without keeping other data.
    		return 0;
    	}else{
    		int save_flag = 1;
    		if(len <= check_size_max){
		        save_flag = (0 != memcmp(buf_temp, buf, len));
	        }
	        
	        if(save_flag){
	        	flash_erase_sector(flash_addr & 0xffff000);
				flash_write_page(flash_addr, len, (u8 *)buf);
	        }
        }
    } else if (LLSYNC_MESH_READ_FLASH == type) {
		foreach(i, sizeof(buf_temp)){
			if(buf_temp[i] != 0xff){ // all "0xff" means invalid data
				memcpy(buf, buf_temp, len);
				break;
			}
		}
    }

    return len;
}
#endif

#ifdef __cplusplus
}
#endif
