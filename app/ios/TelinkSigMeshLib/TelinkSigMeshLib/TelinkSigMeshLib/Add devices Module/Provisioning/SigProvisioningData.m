/********************************************************************************************************
 * @file     SigProvisioningData.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  SigProvisioningData.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/22.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigProvisioningData.h"
#import "SigModel.h"
#import "OpenSSLHelper.h"
#import "ec.h"
#import "SigECCEncryptHelper.h"

NSString *const sessionKeyOfCalculateKeys = @"sessionKeyOfCalculateKeys";
NSString *const sessionNonceOfCalculateKeys = @"sessionNonceOfCalculateKeys";
NSString *const deviceKeyOfCalculateKeys = @"deviceKeyOfCalculateKeys";

@interface SigProvisioningData ()

@property (nonatomic, strong) SigDataSource *network;

@property (nonatomic, strong) NSData *authValue;
@property (nonatomic, strong) NSData *deviceConfirmation;
@property (nonatomic, strong) NSData *deviceRandom;

/// The Confirmation Inputs is built over the provisioning process. It is composed for: Provisioning Invite PDU, Provisioning Capabilities PDU, Provisioning Start PDU, Provisioner's Public Key and device's Public Key.
@property (nonatomic, strong) NSData *confirmationInputs;//1 + 11 + 5 + 64 + 64

@end

@implementation SigProvisioningData{
    EC_KEY *_seckey;
    NSData *_publicKeyDataLessThanIOS10;
}

- (instancetype)initWithAlgorithm:(Algorithm)algorithm {
    if (self = [super init]) {
        [self generateProvisionerRandomAndProvisionerPublicKey];
    }
    return self;
}

- (void)prepareWithNetwork:(SigDataSource *)network networkKey:(SigNetkeyModel *)networkKey unicastAddress:(UInt16)unicastAddress {
//    self.sharedSecret = [LibTools nsstringToHex:@"d9d6d598bfd6d858fd0bb63e953699ccdf29476d5fb10a6f041a7557101a6884"];
//    self.confirmationInputs = [LibTools nsstringToHex:@"0002000100000000000000000000000000C41398C821363D66101F7CF61D8D893D6BD74000ABE3319ABB54474BD401493FE706F293F211F3770484F893A25B6B5B1A2221F63B484A9796577D8CC0202AD0D04AAB0E4B51223486823D16DEC56FE57746306CF0A108F15D5F960C468DF3C3823C6E7D18F4A7CB6AB6DE5B44704748D904ED6A72E1C81BE3F79BD0FB991A3C"];
//    NSData *deviceRandom = [LibTools nsstringToHex:@"6f012d40a4d305858c859be38629acde"];
//    NSData *auth = [LibTools nsstringToHex:@"00000000000000000000000000000000"];
//    NSData *result = [self calculateConfirmationWithRandom:deviceRandom authValue:auth];
//    TeLogDebug(@"result==========%@",[LibTools convertDataToHexStr:result]);

    _network = network;
    _ivIndex = networkKey.ivIndex;
    _networkKey = networkKey;
    _unicastAddress = unicastAddress;
}

/// This method adds the given PDU to the Provisioning Inputs. Provisioning Inputs are used for authenticating the Provisioner and the Unprovisioned Device.
///
/// This method must be called (in order) for:
/// * Provisioning Invite
/// * Provisioning Capabilities
/// * Provisioning Start
/// * Provisioner Public Key
/// * Device Public Key
- (void)accumulatePduData:(NSData *)data {
    NSMutableData *tem = [[NSMutableData alloc] initWithData:self.confirmationInputs];
    [tem appendData:data];
    self.confirmationInputs = tem;
}

/// Call this method when the device Public Key has been obtained. This must be called after generating keys.
/// @param data The device Public Key.
- (void)provisionerDidObtainWithDevicePublicKey:(NSData *)data {
    if (data == nil || data.length == 0) {
        TeLogError(@"current piblickey isn't specified.");
        return;
    }
    self.sharedSecret = [SigECCEncryptHelper.share getSharedSecretWithDevicePublicKey:data];
}

/// Call this method when the Auth Value has been obtained.
- (void)provisionerDidObtainAuthValue:(NSData *)data {
    self.authValue = data;
}

/// Call this method when the device Provisioning Confirmation has been obtained.
- (void)provisionerDidObtainWithDeviceConfirmation:(NSData *)data {
    self.deviceConfirmation = data;
}

/// Call this method when the device Provisioning Random has been obtained.
- (void)provisionerDidObtainWithDeviceRandom:(NSData *)data {
    self.deviceRandom = data;
}

/// This method validates the received Provisioning Confirmation and matches it with one calculated locally based on the Provisioning Random received from the device and Auth Value.
- (BOOL)validateConfirmation {
    if (!self.deviceRandom || self.deviceRandom.length == 0 || !self.authValue || self.authValue.length == 0 || !self.sharedSecret || self.sharedSecret.length == 0) {
        TeLogDebug(@"provision info is lack.");
        return NO;
    }
    NSData *confirmation = [self calculateConfirmationWithRandom:self.deviceRandom authValue:self.authValue];
    if (![self.deviceConfirmation isEqualToData:confirmation]) {
        TeLogDebug(@"calculate Confirmation fail.");
        return NO;
    }
    return YES;
}

/// Returns the Provisioner Confirmation value. The Auth Value must be set prior to calling this method.
- (NSData *)provisionerConfirmation {
    return [self calculateConfirmationWithRandom:self.provisionerRandom authValue:self.authValue];
}

/// Returns the encrypted Provisioning Data together with MIC. Data will be encrypted using Session Key and Session Nonce. For that, all properties should be set when this method is called.
/// Returned value is 25 + 8 bytes long, where the MIC is the last 8 bytes.
- (NSData *)encryptedProvisioningDataWithMic {
    NSDictionary *dict = [self calculateKeys];
    self.deviceKey = dict[deviceKeyOfCalculateKeys];
    NSData *sessionNonce = dict[sessionNonceOfCalculateKeys];
    NSData *sessionKey = dict[sessionKeyOfCalculateKeys];
    
    struct Flags flags = {};
    flags.value = 0;
    if (self.networkKey.phase == finalizing) {
        flags.value |= (1 << 0);
    }
    if (self.ivIndex.updateActive) {
        flags.value |= (1 << 1);
    }
    
    NSMutableData *mData = [NSMutableData dataWithData:self.network.curNetKey];
    UInt16 ind = CFSwapInt16BigToHost(self.networkKey.index);;
    NSData *nIndexData = [NSData dataWithBytes:&ind length:2];
    UInt8 f = flags.value;
    NSData *fData = [NSData dataWithBytes:&f length:1];
    UInt32 iv = CFSwapInt32HostToBig(self.ivIndex.index);
    NSData *ivData = [NSData dataWithBytes:&iv length:4];
    UInt16 address = CFSwapInt16BigToHost(self.unicastAddress);
    NSData *addressData = [NSData dataWithBytes:&address length:2];
    [mData appendData:nIndexData];
    [mData appendData:fData];
    [mData appendData:ivData];
    [mData appendData:addressData];
    NSData *resultData = [[OpenSSLHelper share] calculateCCM:mData withKey:sessionKey nonce:sessionNonce andMICSize:8 withAdditionalData:nil];
    return resultData;
}

#pragma mark - Helper methods

- (void)generateProvisionerRandomAndProvisionerPublicKey {
    _provisionerRandom = [LibTools createRandomDataWithLength:16];
    _provisionerPublicKey = [SigECCEncryptHelper.share getPublicKeyData];
}

/// This method calculates the Provisioning Confirmation based on the Confirmation Inputs, 16-byte Random and 16-byte AuthValue.
///
/// - parameter random:    An array of 16 random bytes.
/// - parameter authValue: The Auth Value calculated based on the Authentication Method.
/// - returns: The Provisioning Confirmation value.
- (NSData *)calculateConfirmationWithRandom:(NSData *)data authValue:(NSData *)authValue {
    // Calculate the Confirmation Salt = s1(confirmationInputs).
//    TeLogDebug(@"confirmationInputs=%@",[LibTools convertDataToHexStr:self.confirmationInputs]);
    NSData *confirmationSalt = [[OpenSSLHelper share] calculateSalt:self.confirmationInputs];
    
    // Calculate the Confirmation Key = k1(ECDH Secret, confirmationSalt, 'prck')
    NSData *confirmationKey = [[OpenSSLHelper share] calculateK1WithN:self.sharedSecret salt:confirmationSalt andP:[@"prck" dataUsingEncoding:NSASCIIStringEncoding]];

    // Calculate the Confirmation Provisioner using CMAC(random + authValue)
    NSMutableData *confirmationData = [NSMutableData dataWithData:data];
    [confirmationData appendData:authValue];
    NSData *resultData = [[OpenSSLHelper share] calculateCMAC:confirmationData andKey:confirmationKey];

    return resultData;
}

/// This method calculates the Session Key, Session Nonce and the Device Key based on the Confirmation Inputs, 16-byte Provisioner Random and 16-byte device Random.
///
/// - returns: The Session Key, Session Nonce and the Device Key.
- (NSDictionary *)calculateKeys {
    // Calculate the Confirmation Salt = s1(confirmationInputs).
    NSData *confirmationSalt = [[OpenSSLHelper share] calculateSalt:self.confirmationInputs];
    
    // Calculate the Provisioning Salt = s1(confirmationSalt + provisionerRandom + deviceRandom)
    NSMutableData *mData = [NSMutableData dataWithData:confirmationSalt];
    [mData appendData:self.provisionerRandom];
    [mData appendData:self.deviceRandom];
    NSData *provisioningSalt = [[OpenSSLHelper share] calculateSalt:mData];

    // The Session Key is derived as k1(ECDH Shared Secret, provisioningSalt, "prsk")
    NSData *sessionKey = [[OpenSSLHelper share] calculateK1WithN:self.sharedSecret salt:provisioningSalt andP:[@"prsk" dataUsingEncoding:NSASCIIStringEncoding]];

    // The Session Nonce is derived as k1(ECDH Shared Secret, provisioningSalt, "prsn")
    // Only 13 least significant bits of the calculated value are used.
    NSData *prsnData = [@"prsn" dataUsingEncoding:NSASCIIStringEncoding];
    NSData *sessionNoncek1 = [[OpenSSLHelper share] calculateK1WithN:self.sharedSecret salt:provisioningSalt andP:prsnData];
    NSData *sessionNonce = [sessionNoncek1 subdataWithRange:NSMakeRange(3, sessionNoncek1.length - 3)];

    // The Device Key is derived as k1(ECDH Shared Secret, provisioningSalt, "prdk")
    NSData *deviceKey = [[OpenSSLHelper share] calculateK1WithN:self.sharedSecret salt:provisioningSalt andP:[@"prdk" dataUsingEncoding:NSASCIIStringEncoding]];

    NSDictionary *resultDict = @{sessionKeyOfCalculateKeys:sessionKey,sessionNonceOfCalculateKeys:sessionNonce,deviceKeyOfCalculateKeys:deviceKey};
    
    return resultDict;
}

@end
