
#ifndef AES_H
#define AES_H
 #include "../../../ble_lt_mesh/vendor/common/app_provison.h"
void aes(char *p, int plen, char *key); 
void deAes(char *c, int clen, char *key);
void vc_test_ecb();
int vc_cal_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8]);
int check_hash_right_or_not(u8* phash,u8 *p_random,u8 *p_key,u16 ele_adr);
#endif