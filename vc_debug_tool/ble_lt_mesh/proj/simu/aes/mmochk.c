/********************************************************************************************************
 * @file	mmochk.c
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
/* ************************************************************
 * This simple test application encrypts a message and then
 * decrypts it right back, and prints the calculated MIC.
 * The Key, Nonce, Header etc. used are whatever is assigned
 * to the variables "Key", "Nnc", "Hdr" etc. via conditional
 * compilation.
 *
 * Usage: getmic
 *
 * ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>

#include "../../common/types.h"
#include "inc/ecb.h"
#include "inc/mmo.h"
#include "inc/debug.h"
#include "inc/mmochk.h"


void mmoCheck(mmo_info_t  *pMMO)
{
    mmo_ctx     ctx[1];
    uint8_t     errVal = NO_MMO_ERROR;
    ret_type    retVal = RETURN_GOOD;


    dbgp_app("-------------------Enter MMO Check------------------\n\n");

    dbgp_app("Key Length: %d\n", pMMO->keyLen);

    if (pMMO->keyLen) {
        outputHexStr("Key:\n\r", pMMO->key, pMMO->keyLen);
    } else {
        printf("\n   no key to use ...\n");
    }

    /* First encrypt */

    dbgp_app("Message Length: %d\n", pMMO->mstrLen);
    if (pMMO->mstrLen) {
        outputHexStr("Plain Text:\n\r", pMMO->mStr, pMMO->mstrLen);
    } else {
        printf("   no data to encrypt (only header authenticated)....\n");
    }

    retVal = mmo_init_and_key(pMMO->key, pMMO->keyLen, ctx);
    if (retVal != RETURN_GOOD) {
        errVal = MMOKEY_ERROR;
        return;
    }

    printf("\nEncrypting...\n\r");
    retVal = mmo_encrypt_message(pMMO->mstrLen, pMMO->mStr, ctx, pMMO->result);
    if (retVal != RETURN_GOOD) {
        errVal = MMOENCR_ERROR;
        //do_exit(errVal);
    }


    printf("Hit any key for the next message...\n\r");
    //getchar();


}
