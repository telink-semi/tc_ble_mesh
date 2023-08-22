/********************************************************************************************************
 * @file	ip_adapt.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "../tl_common.h"

#if(MODULE_ETH_ENABLE)
#include "ip_adapt_i.h"

void ipa_set_my_mac(u8 *mac){
	u8 *my_mac = ipa_get_my_mac();
	assert((((int)mac) >> 2) << 2 == ((int)mac));			// address must alighn to 4
	assert((((int)my_mac) >> 2) << 2 == ((int)my_mac));	// address must alighn to 4
	*(u32*)my_mac = *(u32*)mac;
	*(u16*)(my_mac+4) = *(u16*)(mac+4);
}

void ipa_set_my_ip(u8 *ip){
	u8 *my_ip = ipa_get_my_ip();
	assert((((int)ip) >> 2) << 2 == ((int)ip));			// address must alighn to 4
	assert((((int)my_ip) >> 2) << 2 == ((int)my_ip));		// address must alighn to 4
	*((u32*)my_ip) = *((u32*)ip);
}

void ipa_set_gateway(u8 *ip){
	u8 *my_ip = ipa_get_gateway();
	assert((((int)ip) >> 2) << 2 == ((int)ip));			// address must alighn to 4
	assert((((int)my_ip) >> 2) << 2 == ((int)my_ip));		// address must alighn to 4
	*((u32*)my_ip) = *((u32*)ip);
}

void ipa_set_netmask(u8 *ip){
	u8 *my_ip = ipa_get_netmask();
	assert((((int)ip) >> 2) << 2 == ((int)ip));			// address must alighn to 4
	assert((((int)my_ip) >> 2) << 2 == ((int)my_ip));		// address must alighn to 4
	*((u32*)my_ip) = *((u32*)ip);
}

#endif

//  stub, use for telnetd application. called when received a input command
void shell_input(char *cmd, int len){
}


