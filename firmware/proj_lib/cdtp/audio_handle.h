/********************************************************************************************************
 * @file	audio_handle.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
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
#ifndef _AUDIO_HANDLE_H_
#define _AUDIO_HANDLE_H_

#if (BLC_AUDIO_PROFILE_EN)



int blt_audio_handleInit(void);

blt_audio_handle_t *blt_audio_getHandle(u8 role);
void blt_audio_dropHandle(blt_audio_handle_t *pHandle);

u16 blc_audio_getConnHandle(u16 cisHandle, u8 role);

blt_audio_handle_t *blt_audio_findHandle(u16 connHandle);
blt_audio_client_t *blt_audio_findClient(u16 connHandle);
blt_audio_server_t *blt_audio_findServer(u16 connHandle);
blt_audio_handle_t *blt_audio_findHandleByRole(u16 connHandle, u8 role);
blt_audio_handle_t *blt_audio_findHandleByCisHandle(u16 cisHandle, u8 role);



#endif //#if (BLC_AUDIO_PROFILE_EN)

#endif //_AUDIO_HANDLE_H_

