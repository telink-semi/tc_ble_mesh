/********************************************************************************************************
 * @file     OpenSSLHelper+EPA.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/11/24
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#import "OpenSSLHelper+EPA.h"
#import "openssl/cmac.h"
#import "openssl/evp.h"
#import "openssl/rand.h"
#import "openssl/pem.h"

@implementation OpenSSLHelper (EPA)

- (NSData *)calculateSalt2:(NSData *)someData {
    //For S2, the key is constant
    unsigned char key[32] = {0x00};

    NSData *keyData = [[NSData alloc] initWithBytes: key length: 32];
    return [self calculateHMAC_SHA256: someData andKey: keyData];
}

- (NSData *)calculateK5WithN:(NSData *)N salt:(NSData *)salt andP:(NSData *)P {
    /*
     The key (T) is computed as follows:
     T = HMAC-SHA-256SALT (N)
     The output of the derivation function k5 is as follows:
     k5(N, SALT, P) = HMAC-SHA-256T (P)
     */
    // Calculate K5 (outputs the confirmationKey).
    // T is calculated first using HMAC-SHA-256 N with SALT.
    NSData *t = [self calculateHMAC_SHA256: N andKey: salt];
    // Then calculating HMAC-SHA-256 P with salt T.
    NSData *output = [self calculateHMAC_SHA256: P andKey: t];
    return output;
}

- (NSData *)calculateHMAC_SHA256:(NSData *)someData andKey:(NSData *)key {
    /*
     HMAC-SHA-256k(m) = SHA-256((k0 ⊕ opad) || SHA-256((k0 ⊕ ipad) || m))
     Where:
     k0 is the k with the 0x00 octet appended 32 times to the end
     ipad is the 0x36 octet repeated 64 times
     opad is the 0x5C octet repeated 64 times
     SHA-256 is the secure hash algorithm defined in [30]
     */

    //k0
    unsigned char tem[64] = {0x00};
    NSMutableData *k0 = [NSMutableData dataWithData:key];
    [k0 appendBytes:tem length:32];

    //ipad
    memset(tem, 0x36, 64);
    NSData *ipad = [NSData dataWithBytes:tem length:64];

    //opad
    memset(tem, 0x5C, 64);
    NSData *opad = [NSData dataWithBytes:tem length:64];

    NSMutableData *temData = [NSMutableData dataWithData:[self calculateXORWithFirstData:k0 secondData:ipad]];
    [temData appendData:someData];
    temData = [NSMutableData dataWithData:[self calculateSHA256:temData]];
    NSMutableData *input = [NSMutableData dataWithData:[self calculateXORWithFirstData:k0 secondData:opad]];
    [input appendData:temData];
    NSData *output = [self calculateSHA256:input];

    return output;
}

- (NSData *)calculateSHA256:(NSData *)someData {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, someData.bytes, someData.length);
    SHA256_Final(hash, &sha256);
    NSData *output = [NSData dataWithBytes:hash length:SHA256_DIGEST_LENGTH];
    return output;
}

@end
