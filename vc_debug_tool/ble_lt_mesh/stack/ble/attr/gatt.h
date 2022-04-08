/********************************************************************************************************
 * @file	gatt.h
 *
 * @brief	for TLSR chips
 *
 * @author	BLE Group
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
#ifndef GATT_H_
#define GATT_H_




/************************************ User Interface  *****************************************/

ble_sts_t	blc_gatt_pushHandleValueNotify  (u16 connHandle, u16 attHandle, u8 *p, int len);

ble_sts_t	blc_gatt_pushHandleValueIndicate(u16 connHandle, u16 attHandle, u8 *p, int len);


ble_sts_t 	blc_gatt_pushWriteComand (u16 connHandle, u16 attHandle, u8 *p, int len);

ble_sts_t 	blc_gatt_pushWriteRequest (u16 connHandle, u16 attHandle, u8 *p, int len);




#if 0  //not available now

ble_sts_t 	blc_gatt_pushFindInformationRequest(u8 *dat, u16 start_attHandle, u16 end_attHandle);

ble_sts_t 	blc_gatt_pushFindByTypeValueRequest (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, u8* attr_value, int len);

ble_sts_t 	blc_gatt_pushReadByTypeRequest (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);

ble_sts_t 	blc_gatt_pushReadRequest (u8 *dat, u16 attHandle);

ble_sts_t 	blc_gatt_pushReadBlobRequest (u8 *dat, u16 attHandle, u16 offset);

ble_sts_t 	blc_gatt_pushReadByGroupTypeRequest (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);


#endif






/****************************** Stack Interface, user can not use!!! ***************************/
u8 blc_gatt_requestServiceAccess(u16 connHandle, int gatt_perm);






#endif /* GATT_H_ */
