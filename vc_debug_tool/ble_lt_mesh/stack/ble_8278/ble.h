/********************************************************************************************************
 * @file	ble.h
 *
 * @brief	for TLSR chips
 *
 * @author	public@telink-semi.com;
 * @date	Sep. 18, 2015
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
#ifndef BLE_H_
#define BLE_H_


#include "proj_lib/ble/blt_config.h"
#include "ble_common.h"
#include "stack/ble_8278/ble_format.h"

#include "l2cap/l2cap.h"
//#include "phy/phy.h" 		// comment by weixiong in mesh
//#include "phy/phy_test.h" // comment by weixiong in mesh


#include "proj_lib/ble/att.h"
#include "attr/gatt.h"
#include "proj_lib/ble/uuid.h"



//#include "smp/smp.h" 				// comment by weixiong in mesh
//#include "smp/smp_const.h" 		// comment by weixiong in mesh
//#include "smp/smp_central.h" 		// comment by weixiong in mesh
//#include "smp/smp_peripheral.h" 	// comment by weixiong in mesh
//#include "smp/smp_storage.h" 		// comment by weixiong in mesh

#include "gap/gap.h"
#include "gap/gap_event.h"

//#include "crypt/aes_ccm.h" 		// comment by weixiong in mesh
//#include "crypt/le_crypto.h" 		// comment by weixiong in mesh
//#include "crypt/aes/aes_att.h" 	// comment by weixiong in mesh

//#include "hci/hci.h" 				// comment by weixiong in mesh
//#include "hci/hci_const.h" 		// comment by weixiong in mesh
//#include "hci/hci_event.h" 		// comment by weixiong in mesh
//#include "hci/usb_desc.h" 		// comment by weixiong in mesh

#include "proj_lib/ble/service/ble_ll_ota.h"
//#include "service/device_information.h" 	// comment by weixiong in mesh
//#include "service/hids.h" 				// comment by weixiong in mesh

#include "ll/ll.h"
#include "ll/ll_adv.h"
#include "ll/ll_encrypt.h"
#include "ll/ll_init.h"
#include "ll/ll_pm.h"
#include "ll/ll_scan.h"
#include "ll/ll_whitelist.h"
#include "ll/ll_conn/ll_conn.h"
#include "ll/ll_conn/ll_slave.h"
#include "ll/ll_conn/ll_master.h"
#include "ll/ll_conn/ll_conn_phy.h"
#include "ll/ll_conn/ll_conn_csa.h"

#include "ll/ll_ext.h"
//#include "ll/ll_ext_adv.h" 	// comment by weixiong in mesh
//#include "ll/ll_ext_scan.h" 	// comment by weixiong in mesh


//#include "bqb/bqb_ll.h" 	// comment by weixiong in mesh





volatile void  smemset(register char * dest,register int val,register unsigned int len);
volatile void * smemcpy(register char * out, register char * in, register unsigned int len);



#endif /* BLE_H_ */
