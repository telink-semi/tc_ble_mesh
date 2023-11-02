/********************************************************************************************************
 * @file	cbcchk.c
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

#include "inc/cbc.h"
#include "inc/debug.h"
#include "inc/cbcchk.h"


void cbcCheck(cbc_info_t *pCBC)
{
    cbc_ctx     ctx[1];
    uint8_t     errVal = NO_CBC_ERROR;
    ret_type    retVal = RETURN_GOOD;


    dbgp_app("-------------------Enter CBC Check------------------\n\n");

    dbgp_app("Key Length: %d\n", pCBC->keyLen);
    
    if (pCBC->keyLen) {
        outputHexStr("Key:\n\r", pCBC->key, pCBC->keyLen);
    } else {
        printf("\n   no key to use ...\n");
        getchar();
        return;
    }

    if (pCBC->nonceLen) {
        outputHexStr("IV:\n\r", pCBC->nonce,pCBC->nonceLen);
    } else {
        printf("\n   no iv to use ...\n");
        getchar();
        return;
    }

    if (pCBC->nonceLen != 16) {
        printf("\n   the iv length is wrong ...\n");
        getchar();
        return;
    }


    /* First encrypt */
    if (pCBC->fEncryption) {
        dbgp_app("Message Length: %d\n", pCBC->mstrLen);
        if (pCBC->mstrLen) {
            outputHexStr("Plain Text:\n\r", pCBC->mStr, pCBC->mstrLen);
        } else {
            printf("   no data to encrypt (only header authenticated)....\n");
            getchar();
            return;
        }

        retVal = cbc_init_and_enkey(pCBC->key, pCBC->keyLen, ctx);
        if (retVal != RETURN_GOOD) {
            errVal = CBCKEY_ERROR;
            //do_exit(errVal);
        }

        printf("Encrypting...\n\r");
        retVal = cbc_encrypt_message(pCBC->nonce, pCBC->nonceLen, pCBC->mStr, pCBC->mstrLen, pCBC->micE, pCBC->micLen>2?pCBC->micLen:2, ctx);
        if (retVal != RETURN_GOOD) {
            errVal = CBCENCR_ERROR;
            do_exit(errVal);
        }

        if (pCBC->mstrLen) {
            outputHexStr("Cipher Text:\n\r", pCBC->mStr, pCBC->mstrLen);
        }
    }


    /* Then decrypt */
    else {
        printf("Decrypting...\n\r");
        dbgp_app("Message Length: %d\n", pCBC->mstrLen);
        if (pCBC->mstrLen) {
            outputHexStr("Cipher Text:\n\r", pCBC->mStr, pCBC->mstrLen);
        } else {
            printf("   no data to decrypt (only header authenticated)....\n");
            getchar();
            return;
        }
        
        retVal = cbc_init_and_dekey(pCBC->key, pCBC->keyLen, ctx);
        if (retVal != RETURN_GOOD) {
            errVal = CBCKEY_ERROR;
            //do_exit(errVal);
        }
        retVal = cbc_decrypt_message(pCBC->nonce, pCBC->nonceLen, pCBC->mStr, pCBC->mstrLen, pCBC->micE, pCBC->micLen>2?pCBC->micLen:2, pCBC->micD, ctx);
        if (pCBC->mstrLen) {
            outputHexStr("Plain Text:\n\r", pCBC->mStr, pCBC->mstrLen);
        } else {
            printf("   no data to decrypt (only header authenticated)....\n");
        }
        if (RETURN_GOOD == retVal) {
            printf ("MIC match\n\r");
        }
        else {
            outputHexStr("MIC on decrypt side:\n\r", pCBC->micD, pCBC->micLen>2?pCBC->micLen:0);
        }
        
    }


}

