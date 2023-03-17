/********************************************************************************************************
 * @file	mesh_crypto.h
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
#ifndef BLE_MESH_CRYPTO_H
#define BLE_MESH_CRYPTO_H

#include "../../proj_lib/ble/ll/ll.h"
//#include <memory.h>

int test_mesh_sec_func ();
int test_mesh_sec_key ();
int test_mesh_sec_provision ();
int test_mesh_sec_message ();

int mesh_sec_func_s1m (unsigned char *s1, char * m);
int mesh_sec_func_s1 (unsigned char *s1, unsigned char * m, int n);
int mesh_sec_func_k1 (unsigned char *k1, unsigned char * n, int nn, unsigned char *s, unsigned char * p, int np);
int mesh_sec_func_k1_id (unsigned char *k1, unsigned char n[16]);
int mesh_sec_func_k1_beacon (unsigned char *k1, unsigned char n[16]);

int mesh_sec_func_k2 (unsigned char *nid, unsigned char *ek, unsigned char *pk, unsigned char * n, unsigned char * p, int np);
int mesh_sec_func_k3 (unsigned char *k3, unsigned char n[16]);
int mesh_sec_func_k4 (unsigned char *k4, unsigned char n[16]);

int mesh_sec_beacon_auth (unsigned char *key, unsigned char *m, int dec);
int mesh_sec_beacon_dec (unsigned char *m);

int mesh_sec_prov_confirmation_key (unsigned char *key, unsigned char *input, int n, unsigned char ecdh[32]);
int mesh_sec_prov_confirmation (unsigned char *cfm, unsigned char *input, int n, unsigned char ecdh[32],
									unsigned char random[16], unsigned char auth[16]);
int mesh_sec_prov_session_key (unsigned char sk[16], unsigned char *sn, unsigned char *input, int n, unsigned char ecdh[32],
									unsigned char randomProv[16], unsigned char randomDev[16]);
void mesh_sec_prov_salt(unsigned char prov_salt[16],unsigned char *input,unsigned char randomProv[16], unsigned char randomDev[16]);

void mesh_sec_prov_salt(unsigned char prov_salt[16],unsigned char *input,unsigned char randomProv[16], unsigned char randomDev[16]);

int	mesh_sec_msg_enc (unsigned char key[16], unsigned char nonce[13], unsigned char* dat, int n, int mic_length);
int	mesh_sec_msg_dec (unsigned char key[16], unsigned char nonce[13], unsigned char* dat, int n, int mic_length);
int	mesh_sec_msg_enc_ll (unsigned char key[16], unsigned char nonce[13], unsigned char* dat, int n, u8 *aStr, u8 aStrLen, int mic_length);
int	mesh_sec_msg_dec_ll (unsigned char key[16], unsigned char nonce[13], unsigned char* dat, int n, u8 *aStr, u8 aStrLen, int mic_length);

int	mesh_sec_msg_obfuscation (unsigned char key[16], unsigned char iv[4], unsigned char* p_ctl);
int  mesh_sec_msg_enc_nw(u8 *nw, u8 len_lt, u8 swap_type_lt, u8 sec_type, int fri_key_idx, u8 len_nw, u8 adv_type,int nonce_type, u8 nk_array_idx, bool4 retransmit_rx);
int  mesh_sec_msg_enc_nw_rf_buf(u8 *nw, u8 len_lt, u8 sec_type, int fri_key_idx, int nonce_type, u8 nk_array_idx, bool4 retransmit_rx);
int  mesh_sec_msg_enc_apl(u8 *mat, u8 *bear, int mic_length);
int  mesh_sec_msg_dec_nw(u8 *nw, int len_dec_nw_int, u8 nid, u8 nonce_type, int src_type);
int  mesh_sec_msg_dec_apl(u8 *ac, u16 len_ut, const u8 *nw_little, const u8 *ac_backup);
u8 mesh_sec_get_aid(u8 *key);
int mesh_sec_dev_key (unsigned char dev_key[16], unsigned char *salt, unsigned char ecdh[32]);
void mesh_sec_get_nid_ek_pk_master(u8 *nid, u8 *ek, u8 *pk, u8 *nk);
void mesh_sec_get_nid_ek_pk_directed(u8 *nid, u8 *ek, u8 *pk, u8 *nk);
void mesh_sec_get_nid_ek_pk_friend(u8 lpn_idx, u8 *nid, u8 *ek, u8 *pk, u8 *nk);
void mesh_sec_get_network_id(u8 *nw_id, u8 *nk);
void mesh_sec_get_identity_key(u8 *id_key, u8 *nk);
void mesh_sec_get_beacon_key(u8 *bc_key, u8 *nk);
int mesh_sec_func_k1_pri_beaconkey(u8 *bc_key, u8 *nk);
void mesh_sec_get_privacy_bacon_key(u8 *pbc_key,u8 *nk);

int mesh_sec_pri_beacon(u8 key_flag,u8 *p_ivi,u8 *p_random,u8 *pri_beacon_key,u8 *pri_bea_adv);
int mesh_privacy_beacon_dec (unsigned char *m,u8 *key_flag,u8 *ivi_idx);

void tlk_mesh_key_init(u8 *p_name, u8 *p_pw);
u8 aes_att_decryption_packet_private_mesh(u8 *key, u8 *iv, u8 *mic, u8 mic_len, u8 *ps, u8 len);

void endianness_swap_u32(u8 *data);
void endianness_swap_u16(u8 *data);
void endianness_swap_u24(u8 *data);
void endianness_swap_u48(u8 *data);
void endianness_swap_u64(u8 *data);
void endianness_swap_u128(u8 *data);
void endianness_swap(u8 *nw, u8 swap_type);
void endianness_swap_ut(u8 *nw, u8 *par, u32 len);
void endianness_swap_fri_sec_par(u8 *fri_sec_par);
void mesh_swap_nw_lt(u8 *nw, u8 swap_type_lt);
int test_proxy_adv_with_node_identity();

int ecc_shared_compute(void *p_ctx, u8 const *p_sk, u8 const *p_pk, u8 *p_ss);


u8 crc8_rohc(u8 *data,u32 len);
void test_virtual_address();
u16 cala_vir_adr_by_uuid(u8 *p_label_uuid);
void encode_password_ll(u8 *sk_user, u8 *pd, u32 pd_len, int mic_len); // pd_len exclude mic
int decode_password_ll(u8 *sk_user, u8 *pd, u32 pd_len, int mic_len); // pd_len include mic
int mesh_sec_prov_confirmation_key_hmac (unsigned char *key, unsigned char *input, int n, unsigned char ecdh[32],unsigned char auth[32]);
int mesh_sec_prov_confirmation_hmac (unsigned char *cfm, unsigned char *input, int n, unsigned char ecdh[32],
				unsigned char random[32],unsigned char auth[32]);
void mesh_sec_prov_salt_hmac(unsigned char prov_salt[16],unsigned char *input,unsigned char randomProv[32], unsigned char randomDev[32]);
int mesh_sec_prov_session_key_hmac(unsigned char sk[16], unsigned char* sn, unsigned char* input, int n, unsigned char ecdh[32],
	unsigned char randomProv[32], unsigned char randomDev[32]);
#define SECURITY_ENABLE     (1) // use in library, if 0, it must be provisioned before.  //extern u8 security_enable;

//------------------ define----
#define NONCE_TYPE_NETWORK      0x00
#define NONCE_TYPE_APPLICATION  0x01
#define NONCE_TYPE_DEVICE       0x02
#define NONCE_TYPE_PROXY        0x03
#define NONCE_TYPE_SOLICITATION	0x04

enum{
    ERR_NO_NW_DEC_ERR               = 1,    // must 1 for decryption error
    ERR_NO_NW_DEC_ADR_INVALID       = 2,
    ERR_NO_NW_DEC_CACHE_OLD         = 3,
};


#endif 
