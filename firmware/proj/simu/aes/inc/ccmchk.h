/********************************************************************************************************
 * @file	ccmchk.h
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
#ifndef _CCMCHK_H
#define _CCMCHK_H

#pragma pack(1)

/* ====== Typedefs ====== */

typedef enum
{
  NO_CCM_ERROR = 0,

  /* keep the following in alphabetical order */
  CCMDECR_ERROR,
  CCMENCR_ERROR,
  CCMKEY_ERROR,
  MALLOC_ERROR,
} t_errVal;


typedef struct ccm_info {
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
    uint8_t  counterMod;  /* used in CTR mode */
    uint32_t initCounter; /* used in CTR mode */
} ccm_info_t;

void ccmCheck(ccm_info_t* pCCM);


#endif /* _GETMIC_H */

