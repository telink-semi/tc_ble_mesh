/********************************************************************************************************
 * @file	aes_ecb.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE Group
 * @date	2017
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

#ifndef AES_H
#define AES_H
 #include "../../../ble_lt_mesh/vendor/common/app_provison.h"
void aes(char *p, int plen, char *key); 
void deAes(char *c, int c_len, char *key);
void vc_test_ecb();
int vc_cal_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8]);
int check_hash_right_or_not(u8* phash,u8 *p_random,u8 *p_key,u16 ele_adr);
#endif