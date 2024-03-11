/********************************************************************************************************
 * @file	dual_mode_adapt.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#pragma once

#include "tl_common.h"

#define FW_RAMCODE_SIZE_MAX         (0x4000)    // no limit for bootloader.

#if (DUAL_MESH_ZB_BL_EN || DUAL_MESH_SIG_PVT_EN)
#define DUAL_MODE_ZB_FW_SIZE_MAX_K  (256)
#define FLASH_ADR_UPDATE_NEW_FW     (0x40000)
#define DUAL_MODE_FW_ADDR_SIGMESH   (0x80000)   // if modify, must modify __FW_OFFSET to the same,
#define DUAL_MODE_FW_ADDR_ZIGBEE    (0xC0000)
#else
    #if __PROJECT_BOOTLOADER__
// because bootloader need to have the same product image address, such as FLASH_ADR_UPDATE_NEW_FW
#error please set MESH_USER_DEFINE_MODE to a mode that needed bootloader, such as MESH_ZB_BL_DUAL_ENABLE which is same with product image.
    #endif

#define DUAL_MODE_FW_ADDR_SIGMESH   (0x00000)
#define DUAL_MODE_FW_ADDR_ZIGBEE    (0x40000)
#endif

enum{
	RF_MODE_BLE			= 0,
	RF_MODE_ZIGBEE,
	RF_MODE_PVT_MESH,
};

#if (0 == FW_START_BY_BOOTLOADER_EN)
enum{
    DUAL_MODE_SAVE_ENABLE       = 0x5A,    // dual mode state should be define both 73000 and 76080
    DUAL_MODE_SAVE_DISABLE      = 0x00,
    // all other is disable exclude 0xff
};
#endif

enum{
    DUAL_MODE_NOT_SUPPORT       = 0x00,
    DUAL_MODE_SUPPORT_ENABLE    = 0x01, // must 0xff
    DUAL_MODE_SUPPORT_DISABLE   = 0x02,
};

enum{
    DUAL_MODE_CALI_VAL_FLAG         = 0x5A,
};

// begin telink private mesh
enum{
	DUAL_MODE_NETWORK_DEFAULT 	= 0x00,
	DUAL_MODE_NETWORK_SIG_MESH,
	DUAL_MODE_NETWORK_PVT_MESH,
};

enum{
    MESH_PAIR_NAME1 = 0,
    MESH_PAIR_NAME2,
    MESH_PAIR_PWD1,
    MESH_PAIR_PWD2,
    MESH_PAIR_LTK1,
    MESH_PAIR_LTK2,
    MESH_PAIR_EFFECT_DELAY,
    MESH_PAIR_EFFECT,
    MESH_PAIR_DEFAULT_MESH,
};

#define LGT_CMD_MESH_PAIR       0xC9

typedef struct{
	u32 dma_len;
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 handle;
	u8 handle1;
	u8 value[30];//sno[3],src[2],dst[2],op[1~3],params[0~10],mac-app[5],ttl[1],mac-net[4]
}tlk_mesh_rf_att_cmd_t;

typedef struct{
    u8 sno[3];
    u8 src[2];
    u8 dst[2];
    u8 val[23];// op[1~3],params[0~10],mac-app[5],ttl[1],mac-net[4]
    // get status req: params[0]=tick  mac-app[2-3]=src-mac1...
    // get status rsp: mac-app[0]=ttc  mac-app[1]=hop-count
}tlk_mesh_rf_att_value_t;
// end telink private mesh

extern u8 rf_mode;
extern u8 dual_mode_state;
extern u8 dual_mode_mesh_found;
extern u32 dual_mode_tlk_ac[];

extern u8 pair_ltk[17];


int is_ble_found();
int is_zigbee_found();
u8 dual_mode_proc();
void dual_mode_en_init();
void dual_mode_select();
void dual_mode_disable();
void irq_zigbee_sdk_handler(void);
int pair_dec_packet_mesh (u8 *ps);

// telink mesh
void dual_mode_restore_TLK_4K();
int UI_restore_TLK_4K_with_check();
void tlk_mesh_access_code_backup(u32 ac);


