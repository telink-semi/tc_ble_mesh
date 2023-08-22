/********************************************************************************************************
 * @file	app_pair.h
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
#ifndef APP_PAIR_H_
#define APP_PAIR_H_


int user_tbl_slave_mac_add(u8 adr_type, u8 *adr);
int user_tbl_slave_mac_search(u8 adr_type, u8 * adr);

int user_tbl_slave_mac_delete_by_adr(u8 adr_type, u8 *adr);
void user_tbl_salve_mac_unpair_proc(void);

void user_master_host_pairing_management_init(void);


#endif /* APP_PAIR_H_ */
