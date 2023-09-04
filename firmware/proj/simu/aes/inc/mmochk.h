/********************************************************************************************************
 * @file	mmochk.h
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
#ifndef _MMOCHK_H
#define _MMOCHK_H

#pragma pack(1)

/* ====== Typedefs ====== */


enum
{
  NO_MMO_ERROR = 0,

  /* keep the following in alphabetical order */
  MMODECR_ERROR,
  MMOENCR_ERROR,
  MMOKEY_ERROR,
  MMOMALLOC_ERROR,
};


typedef struct mmo_info {
    uint8_t  fEncryption;
    uint8_t  micLen;
    uint8_t  keyLen;
    uint8_t  nonceLen;
    uint32_t ahdrLen;
    uint32_t mstrLen;
    uint8_t  *key;
    uint8_t  *nonce;
    uint8_t  *aHdr;
    uint8_t  *mStr;
    uint8_t  *micE;
    uint8_t  *micD;
    uint8_t  result[16];
} mmo_info_t;

void mmoCheck(mmo_info_t* pMMO);


#endif /* _MMOCHK_H */
