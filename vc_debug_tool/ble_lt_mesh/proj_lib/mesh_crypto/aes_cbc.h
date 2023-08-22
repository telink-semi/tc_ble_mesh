/********************************************************************************************************
 * @file	aes_cbc.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	Sep. 30, 2010
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
#ifndef _AES_H_
#define _AES_H_

#ifndef CBC
  #define CBC 1
#endif

#ifndef ECB
  #define ECB 0
#endif

#ifndef CTR
  #define CTR 0
#endif

#define AES128 1
#define AES192 0
#define AES256 0

#define AES_BLOCKLEN 16 //Block length in bytes AES is 128b block only

#if defined(AES256) && (AES256 == 1)
    #define AES_KEYLEN 32
    #define AES_keyExpSize 240
#elif defined(AES192) && (AES192 == 1)
    #define AES_KEYLEN 24
    #define AES_keyExpSize 208
#else
    #define AES_KEYLEN 16   // Key length in bytes
    #define AES_keyExpSize 176
	#define AES_128_HW_MODE 1 // use hardware aes128
#endif

#ifndef AES_128_HW_MODE
#define AES_128_HW_MODE		0
#endif

typedef struct
{
  uint8_t RoundKey[AES_keyExpSize];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
  uint8_t Iv[AES_BLOCKLEN];
#endif
}AES_ctx;

extern AES_ctx ctx;

void AES_init_ctx(AES_ctx* ctx, const uint8_t* key);

//note:padding_buf must AES_BLOCKLEN(16) larger than src_buf.
void AES128_pkcs7_padding(u8 *src_buf, u32 len, u8 *padding_buf);
void tn_aes_128(unsigned char *key, unsigned char *plaintext, unsigned char *result);
int aes_decrypt(unsigned char *Key, unsigned char *Data, unsigned char *Result);
void aes_ll_decryption(unsigned char *Key, unsigned char *Data, unsigned char *Result);

#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
void AES_init_ctx_iv(AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv(AES_ctx* ctx, const uint8_t* iv);
#endif

#if defined(ECB) && (ECB == 1)
void AES_ECB_encrypt(const AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(const AES_ctx* ctx, uint8_t* buf);
#endif // #if defined(ECB) && (ECB == !)

#if defined(CBC) && (CBC == 1)
void AES_CBC_encrypt_buffer(AES_ctx* ctx, uint8_t* buf, uint32_t length);
void AES_CBC_decrypt_buffer(AES_ctx* ctx, uint8_t* buf, uint32_t length);
void aes_cbc_encrypt(uint8_t *buf, uint32_t length, AES_ctx* ctx, uint8_t *key, const u8 *iv);
void aes_cbc_decrypt(uint8_t *buf, uint32_t length, AES_ctx* ctx, uint8_t *key, const u8 *iv);
#endif

#if defined(CTR) && (CTR == 1)
void AES_CTR_xcrypt_buffer(AES_ctx* ctx, uint8_t* buf, uint32_t length);
#endif


#endif
