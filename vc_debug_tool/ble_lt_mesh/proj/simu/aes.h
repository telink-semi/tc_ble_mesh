/********************************************************************************************************
 * @file	aes.h
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
#ifndef __AES_H__
#define __AES_H__


 /**
 *  @brief zigBee API AES mode definition
 */
typedef enum aes_mode {
    AES_MODE_ECB = 0,        /*!< AES mode: ECB */
    AES_MODE_CBC,            /*!< AES mode: CBC */
    AES_MODE_CTR,            /*!< AES mode: CTR */
    AES_MODE_CCM = 5,        /*!< AES mode: CCM */
    AES_MODE_MMO,            /*!< AES mode: MMO */
    AES_MODE_BYPASS,         /*!< AES mode: Bypass */
    AES_END,
} aes_mode_t;

/**  
 *  @brief AES enc/dec select type definition 
 */                                                                                     
typedef enum aes_operation{                                                                         
    AES_STS_ENCRYPTION = 0,   /*!< AES ENC/DEC select: ENCRYPTION */                                                      
    AES_STS_DECRYPTION,       /*!< AES ENC/DEC select: DECRYPTION */                                                                                                                            
} aes_operation_t;                                                                                   


/**  
 *  @brief ECB mode configure type definition 
 */
typedef struct ecb_cfg {
    aes_operation_t op;       /*!< AES operation: Encryption or Decryption */
    u8  keyLen;          /*!< AES key size */
    u8  *key;            /*!< AES key content */
    u32 srcLen;          /*!< AES text length, the content of text is in the mzsp_aes parameter */
} ecb_cfg_t;

/**  
 *  @brief CBC mode configure type definition 
 */
typedef struct cbc_cfg {
    aes_operation_t op;        /*!< AES operation: Encryption or Decryption */
    u8 keyLen;            /*!< AES key size */
    u8 *key;              /*!< AES key content */
    u8 ivLen;             /*!< AES initialize vector length */
    u8 *iv;               /*!< AES initialize vector content */
    u8 micLen;            /*!< AES mic length */
    u8 *micE;             /*!< AES mic content, used in Encryption */
    u8 *micD;             /*!< AES mic content, used in Decryption */
    u32 srcLen;           /*!< AES text length, the content of text is in the mzsp_aes parameter */
} cbc_cfg_t;

/**  
 *  @brief CTR mode configure type definition 
 */
typedef struct ctr_cfg {
    aes_operation_t op;        /*!< AES operation: Encryption or Decryption */
    u8   keyLen;          /*!< AES key size */
    u8   *key;            /*!< AES key content */
    u8   ivLen;           /*!< AES initialize vector length */
    u8   *iv;             /*!< AES initialize vector content */
    u32  counterMod;      /*!< AES counter mod */
    u32  initCount;       /*!< AES initialize count */
    u32  srcLen;          /*!< AES text length, the content of text is in the mzsp_aes parameter */
} ctr_cfg_t;

/**  
 *  @brief MMO mode configure type definition 
 */
typedef struct mmo_cfg {
    aes_operation_t op;       /*!< AES operation: Encryption or Decryption */
    u8 keyLen;           /*!< AES key size */
    u8 *key;             /*!< AES key content */
    u32 srcLen;          /*!< AES text length, the content of text is in the mzsp_aes parameter */
} mmo_cfg_t;

/**  
 *  @brief CCM mode configure type definition 
 */
typedef struct ccm_cfg {
    aes_operation_t op;      /*!< AES operation: Encryption or Decryption */
    u8  keyLen;         /*!< AES key size */
    u8  *key;           /*!< AES key content */
    u8  ivLen;          /*!< AES initialize vector length */
    u8  *iv;            /*!< AES initialize vector content */
    u8  micLen;         /*!< AES mic length */
    u8  *micE;          /*!< AES mic content, used in Encryption */
    u8  *micD;          /*!< AES mic content, used in Decryption */
    u32 aStrLen;        /*!< AES aheader length */
    u8  *aStr;          /*!< AES aheader content */
    u32 srcLen;         /*!< AES text length, the content of text is in the mzsp_aes parameter */
} ccm_cfg_t;

u32 mzsp_aes(u8 aesMode, u8 * src, u8 * dst, void * parameter);




#endif /* __AES_H__ */
