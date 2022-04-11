/********************************************************************************************************
 * @file	ctrchk.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>

#include "ctr.h"
#include "debug.h"
#include "ctrchk.h"


void ctrCheck(ctr_info_t  *pCTR)
{
    ctr_ctx     ctx[1];
    uint8_t     errVal = NO_CTR_ERROR;
    ret_type    retVal = RETURN_GOOD;


    dbgp_app("-------------------Enter CTR Check------------------\n\n");

    dbgp_app("Key Length: %d\n", pCTR->keyLen);

    /* Check parameter */
    if (pCTR->keyLen) {
        outputHexStr("Key:\n\r", pCTR->key, pCTR->keyLen);
    } else {
        printf("\n   no key to use ...\n");
    }

    if (pCTR->nonceLen) {
        outputHexStr("Nonce:\n\r", pCTR->nonce,pCTR->nonceLen);
    } else {
        printf("\n   no nonce to use ...\n");
    }

    if (pCTR->counterMod >= 0 && pCTR->counterMod <= 15)
        dbgp_app("\nCounter Mod: %d\n", 128);
    else if (pCTR->counterMod < 128)
        dbgp_app("\nCounter Mod: %d\n", pCTR->counterMod);
    else
    {
        dbgp_app("\nCounter Mod: error\n");
        exit(-1);
    }


    /* First encrypt */
    if (pCTR->fEncryption) {
        dbgp_app("Message Length: %d\n", pCTR->mstrLen);
        if (pCTR->mstrLen) {
            outputHexStr("Plain Text:\n\r", pCTR->mStr, pCTR->mstrLen);
        } else {
            printf("   no data to encrypt (only header authenticated)....\n");
        }

        retVal = ctr_init_and_enkey(pCTR->key, pCTR->keyLen, ctx);
        if (retVal != RETURN_GOOD) {
            errVal = CTRKEY_ERROR;
            //do_exit(errVal);
        }

        printf("\nEncrypting...\n\r");
        retVal = ctr_encrypt_message(pCTR->nonce, pCTR->nonceLen, pCTR->counterMod, pCTR->initCounter, pCTR->mstrLen, pCTR->mStr, ctx);
        if (retVal != RETURN_GOOD) {
            errVal = CTRENCR_ERROR;
            //do_exit(errVal);
        }

        if (pCTR->mstrLen) {
            outputHexStr("Cipher Text:\n\r", pCTR->mStr, pCTR->mstrLen);
        }
    }

    /* Then decrypt */
    else {
        printf("Decrypting...\n\r");
        dbgp_app("Message Length: %d\n", pCTR->mstrLen);
        if (pCTR->mstrLen) {
            outputHexStr("Cipher Text:\n\r", pCTR->mStr, pCTR->mstrLen);
        } else {
            printf("   no data to encrypt (only header authenticated)....\n");
        }
        retVal = ctr_init_and_enkey(pCTR->key, pCTR->keyLen, ctx);
        retVal = ctr_decrypt_message(pCTR->nonce, pCTR->nonceLen, pCTR->counterMod, pCTR->initCounter, pCTR->mstrLen, pCTR->mStr, ctx);
        if (pCTR->mstrLen) {
            outputHexStr("Plain Text:\n\r", pCTR->mStr, pCTR->mstrLen);
        } else {
            printf("   no data to decrypt (only header authenticated)....\n");
        }

    }
    


}

