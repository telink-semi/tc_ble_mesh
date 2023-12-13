/********************************************************************************************************
 * @file	ll_conn_phy.h
 *
 * @brief	for TLSR chips
 *
 * @author	BLE Group
 * @date	July. 4, 2019
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
#ifndef LL_CONN_PHY_H_
#define LL_CONN_PHY_H_


#include "stack/ble/phy/phy.h"



/******************************* Macro & Enumeration variables for Stack, user can not use!!!!  ********************************/
#if (LL_FEATURE_ENABLE_LE_2M_PHY | LL_FEATURE_ENABLE_LE_CODED_PHY)

ll_conn_phy_t*	blt_ll_get_conn_phy_ptr(u16 connHandle);
extern _attribute_aligned_(4) ll_conn_phy_t blt_conn_phy;


typedef int (*ll_conn_phy_update_callback_t)(void);
typedef int (*ll_conn_phy_switch_callback_t)(void);

extern 	ll_conn_phy_update_callback_t	ll_conn_phy_update_cb;
extern 	ll_conn_phy_switch_callback_t	ll_conn_phy_switch_cb;

#endif













/********************************  Macro & Enumeration variables for User *****************************************************/













/********************************  User Interface  ****************************************************************************/













/*******************************  Stack Interface, user can not use!!! *******************************************************/
//2M/Coded PHY
void 		blt_ll_sendPhyReq(void);
int 		blt_ll_updateConnPhy(void);
int 		blt_ll_switchConnPhy(void);




#endif /* LL_CONN_CSA_H_ */
