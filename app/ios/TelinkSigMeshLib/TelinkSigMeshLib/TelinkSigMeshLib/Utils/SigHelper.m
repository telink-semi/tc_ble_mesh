/********************************************************************************************************
 * @file     SigHelper.m
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
//  SigHelper.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigHelper.h"

@implementation SigHelper

+ (SigHelper *)share{
    static SigHelper *shareHelper = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareHelper = [[SigHelper alloc] init];
    });
    return shareHelper;
}

- (BOOL)isValidAddress:(UInt16)address{
    return address < 0xFF00 || address > 0xFFFB;
}

- (BOOL)isUnassignedAddress:(UInt16)address{
    return address == MeshAddress_unassignedAddress;
}

- (BOOL)isUnicastAddress:(UInt16)address{
    return (address & 0x8000) == 0x0000 && ![self isUnassignedAddress:address];
}

- (BOOL)isVirtualAddress:(UInt16)address{
    return (address & 0xC000) == 0x8000;
}

- (BOOL)isGroupAddress:(UInt16)address{
    return (address & 0xC000) == 0xC000 && [self isValidAddress:address];
}

- (int)getPeriodFromSteps:(SigStepResolution)steps {
    switch (steps) {
        case SigStepResolution_hundredsOfMilliseconds:
            return (int)steps * 100;
            break;
        case SigStepResolution_seconds:
            return (int)steps * 1000;
            break;
        case SigStepResolution_tensOfSeconds:
            return (int)steps * 10000;
            break;
        case SigStepResolution_tensOfMinutes:
            return (int)steps * 600000;
            break;
        default:
            break;
    }
}

- (NSString *)getNodeAddressString:(UInt16)address {
    return [self getUint16String:address];
}

- (NSString *)getUint16String:(UInt16)address {
    return [NSString stringWithFormat:@"%04X",address];
}

- (NSString *)getUint32String:(UInt32)address {
    return [NSString stringWithFormat:@"%08X",(unsigned int)address];
}

- (NSString *)getUint64String:(UInt64)address {
    return [NSString stringWithFormat:@"%016llX",address];
}

- (float)getRandomfromA:(float)a toB:(float)b {
    int aInt = a * 1000;
    int bInt = b * 1000;
    int cInt = arc4random() % (aInt+bInt+1);
    float c = cInt / 1000.0;
    return c;
}


/// The TTL field is a 7-bit field. The following values are defined:
/// • 0 = has not been relayed and will not be relayed
/// • 1 = may have been relayed, but will not be relayed
/// • 2 to 126 = may have been relayed and can be relayed
/// • 127 = has not been relayed and can be relayed
///
/// @param ttl TTL (Time To Live)
- (BOOL)isValidTTL:(UInt8)ttl {
    if (ttl == 0 || (ttl >= 2 && ttl <= 127)) {
        return YES;
    }
    return NO;
}

/// The TTL field is a 7-bit field. The following values are defined:
/// • 0 = has not been relayed and will not be relayed
/// • 1 = may have been relayed, but will not be relayed
/// • 2 to 126 = may have been relayed and can be relayed
/// • 127 = has not been relayed and can be relayed
///
/// @param ttl TTL (Time To Live)
- (BOOL)isRelayedTTL:(UInt8)ttl {
    if (ttl >= 2 && ttl <= 127) {
        return YES;
    }
    return NO;
}

- (UInt16)getUint16LightnessFromUInt8Lum:(UInt8)lum {
    return [self getUInt16LightnessFromSInt16Level:[self getSInt16LevelFromUInt8Lum:lum]];
}

- (UInt8)getUInt8LumFromUint16Lightness:(UInt16)lightness {
    return [self getUInt8LumFromSInt16Level:[self getSInt16LevelFromUInt16Lightness:lightness]];
}

- (SInt16)getSInt16LevelFromUInt8Lum:(UInt8)lum {
    if(lum > 100){
        lum  = 100;
    }
    return -32768 + [self getDivisionRoundWithValue:65535*lum dividend:100];
}

- (UInt8)getUInt8LumFromSInt16Level:(SInt16)level {
    UInt32 lightness = level + 32768;
    UInt32 fix_1p2 = 0;
    if(lightness){    // fix decimals
        #define LEVEL_UNIT_1P2    (65535/100/2)
        if(lightness < LEVEL_UNIT_1P2 + 2){     // +2 to fix accuracy missing
            lightness = LEVEL_UNIT_1P2 * 2;        // make sure lum is not zero when light on.
        }
        fix_1p2 = LEVEL_UNIT_1P2;
    }
    return (((lightness + fix_1p2)*100)/65535);
}

- (UInt16)getUint16TemperatureFromUInt8Temperature100:(UInt8)temperature100 {
    if(temperature100 > 100){
        temperature100  = 100;
    }
    return (CTL_TEMP_MIN + ((CTL_TEMP_MAX - CTL_TEMP_MIN)*temperature100)/100);
}

/// use for driver pwm, 0--100 is absolute value, not related to temp range
- (UInt8)getUInt8Temperature100HWFromUint16Temperature:(UInt16)temperature {
    if(temperature < CTL_TEMP_MIN){
        temperature = CTL_TEMP_MIN;
    }
    if(temperature > CTL_TEMP_MAX){
        temperature = CTL_TEMP_MAX;
    }
    UInt32 fix_1p2 = (CTL_TEMP_MAX - CTL_TEMP_MIN)/100/2;    // fix decimals
    return (((temperature - CTL_TEMP_MIN + fix_1p2)*100)/(CTL_TEMP_MAX - CTL_TEMP_MIN));   // temp100 can be zero.
}

- (UInt8)getUInt8Temperature100FromUint16Temperature:(UInt16)temperature {
    return [self getUInt8Temperature100HWFromUint16Temperature:temperature];// comfirm later, related with temp range
}

- (UInt32)getDivisionRoundWithValue:(UInt32)value dividend:(UInt32)dividend {
    return (value + dividend/2)/dividend;
}

- (UInt16)getUInt16FromSInt16:(SInt16)s16 {
    return s16 + 32768;
}

- (SInt16)getSInt16FromUInt16:(UInt16)u16 {
    return u16 - 32768;
}

- (UInt8)getOnOffFromeSInt16Level:(SInt16)level {
    return (level != LEVEL_OFF) ? 1 : 0;
}

- (UInt16)getUInt16LightnessFromSInt16Level:(SInt16)level {
    return [self getUInt16FromSInt16:level];
}

- (SInt16)getSInt16LevelFromUInt16Lightness:(UInt16)lightness {
    return [self getSInt16FromUInt16:lightness];
}

//SigOpCode_configAppKeyGet:0x8001->SigOpCode_configAppKeyList:0x8002
- (int)getResponseOpcodeWithSendOpcode:(int)sendOpcode {
    int responseOpcode = 0;
    switch (sendOpcode) {
        case SigOpCode_configAppKeyAdd:
        case SigOpCode_configAppKeyUpdate:
        case SigOpCode_configAppKeyDelete:
            responseOpcode = SigOpCode_configAppKeyStatus;
            break;
        case SigOpCode_configAppKeyGet:
            responseOpcode = SigOpCode_configAppKeyList;
            break;
        case SigOpCode_configBeaconGet:
        case SigOpCode_configBeaconSet:
            responseOpcode = SigOpCode_configBeaconStatus;
            break;
        case SigOpCode_configCompositionDataGet:
            responseOpcode = SigOpCode_configCompositionDataStatus;
            break;
        case SigOpCode_configDefaultTtlGet:
        case SigOpCode_configDefaultTtlSet:
            responseOpcode = SigOpCode_configDefaultTtlStatus;
            break;
        case SigOpCode_configFriendGet:
        case SigOpCode_configFriendSet:
            responseOpcode = SigOpCode_configFriendStatus;
            break;
        case SigOpCode_configGATTProxyGet:
        case SigOpCode_configGATTProxySet:
            responseOpcode = SigOpCode_configGATTProxyStatus;
            break;
        case SigOpCode_configKeyRefreshPhaseGet:
        case SigOpCode_configKeyRefreshPhaseSet:
            responseOpcode = SigOpCode_configKeyRefreshPhaseStatus;
            break;;
        case SigOpCode_configModelPublicationGet:
        case SigOpCode_configModelPublicationSet:
        case SigOpCode_configModelPublicationVirtualAddressSet:
            responseOpcode = SigOpCode_configModelPublicationStatus;
            break;
        case SigOpCode_configModelSubscriptionAdd:
        case SigOpCode_configModelSubscriptionDelete:
        case SigOpCode_configModelSubscriptionDeleteAll:
        case SigOpCode_configModelSubscriptionOverwrite:
        case SigOpCode_configModelSubscriptionVirtualAddressAdd:
        case SigOpCode_configModelSubscriptionVirtualAddressDelete:
        case SigOpCode_configModelSubscriptionVirtualAddressOverwrite:
            responseOpcode = SigOpCode_configModelSubscriptionStatus;
            break;
        case SigOpCode_configNetworkTransmitGet:
        case SigOpCode_configNetworkTransmitSet:
            responseOpcode = SigOpCode_configNetworkTransmitStatus;
            break;
        case SigOpCode_configRelayGet:
        case SigOpCode_configRelaySet:
            responseOpcode = SigOpCode_configRelayStatus;
            break;
        case SigOpCode_configSIGModelSubscriptionGet:
            responseOpcode = SigOpCode_configSIGModelSubscriptionList;
            break;
        case SigOpCode_configVendorModelSubscriptionGet:
            responseOpcode = SigOpCode_configVendorModelSubscriptionList;
            break;
        case SigOpCode_configLowPowerNodePollTimeoutGet:
            responseOpcode = SigOpCode_configLowPowerNodePollTimeoutStatus;
            break;
        case SigOpCode_configHeartbeatPublicationGet:
        case SigOpCode_configHeartbeatPublicationSet:
            responseOpcode = SigOpCode_configHeartbeatPublicationStatus;
            break;
        case SigOpCode_configHeartbeatSubscriptionGet:
        case SigOpCode_configHeartbeatSubscriptionSet:
            responseOpcode = SigOpCode_configHeartbeatSubscriptionStatus;
            break;
        case SigOpCode_configModelAppBind:
        case SigOpCode_configModelAppUnbind:
            responseOpcode = SigOpCode_configModelAppStatus;
            break;
        case SigOpCode_configNetKeyGet:
            responseOpcode = SigOpCode_configNetKeyList;
            break;
        case SigOpCode_configNetKeyAdd:
        case SigOpCode_configNetKeyDelete:
        case SigOpCode_configNetKeyUpdate:
            responseOpcode = SigOpCode_configNetKeyStatus;
            break;
        
        case SigOpCode_configNodeIdentityGet:
        case SigOpCode_configNodeIdentitySet:
            responseOpcode = SigOpCode_configNodeIdentityStatus;
            break;

        case SigOpCode_configNodeReset:
            responseOpcode = SigOpCode_configNodeResetStatus;
            break;
        case SigOpCode_configSIGModelAppGet:
            responseOpcode = SigOpCode_configSIGModelAppList;
            break;
        case SigOpCode_configVendorModelAppGet:
            responseOpcode = SigOpCode_configVendorModelAppList;
            break;
            
            // remote provision
        case SigOpCode_remoteProvisioningScanCapabilitiesGet:
            responseOpcode = SigOpCode_remoteProvisioningScanCapabilitiesStatus;
            break;
        case SigOpCode_remoteProvisioningScanGet:
        case SigOpCode_remoteProvisioningScanStart:
        case SigOpCode_remoteProvisioningScanStop:
            responseOpcode = SigOpCode_remoteProvisioningScanStatus;
            break;
        case SigOpCode_remoteProvisioningLinkGet:
        case SigOpCode_remoteProvisioningLinkOpen:
        case SigOpCode_remoteProvisioningLinkClose:
            responseOpcode = SigOpCode_remoteProvisioningLinkStatus;
            break;

        case SigOpCode_genericOnOffGet:
        case SigOpCode_genericOnOffSet:
            responseOpcode = SigOpCode_genericOnOffStatus;
            break;
        case SigOpCode_genericLevelGet:
        case SigOpCode_genericLevelSet:
        case SigOpCode_genericMoveSet:
        case SigOpCode_genericDeltaSet:
            responseOpcode = SigOpCode_genericLevelStatus;
            break;
        case SigOpCode_genericDefaultTransitionTimeGet:
        case SigOpCode_genericDefaultTransitionTimeSet:
            responseOpcode = SigOpCode_genericDefaultTransitionTimeStatus;
            break;
        case SigOpCode_genericOnPowerUpGet:
        case SigOpCode_genericOnPowerUpSet:
            responseOpcode = SigOpCode_genericOnPowerUpStatus;
            break;
        case SigOpCode_genericPowerLevelGet:
        case SigOpCode_genericPowerLevelSet:
            responseOpcode = SigOpCode_genericPowerLevelStatus;
            break;
        case SigOpCode_genericPowerLastGet:
            responseOpcode = SigOpCode_genericPowerLastStatus;
            break;
        case SigOpCode_genericPowerDefaultGet:
        case SigOpCode_genericPowerDefaultSet:
            responseOpcode = SigOpCode_genericPowerDefaultStatus;
            break;
        case SigOpCode_genericPowerRangeGet:
        case SigOpCode_genericPowerRangeSet:
            responseOpcode = SigOpCode_genericPowerRangeStatus;
            break;
        case SigOpCode_genericBatteryGet:
            responseOpcode = SigOpCode_genericBatteryStatus;
            break;
        case SigOpCode_sensorDescriptorGet:
            responseOpcode = SigOpCode_sensorDescriptorStatus;
            break;
        case SigOpCode_sensorGet:
            responseOpcode = SigOpCode_sensorStatus;
            break;
        case SigOpCode_sensorColumnGet:
            responseOpcode = SigOpCode_sensorColumnStatus;
            break;
        case SigOpCode_sensorSeriesGet:
            responseOpcode = SigOpCode_sensorSeriesStatus;
            break;
        case SigOpCode_sensorCadenceGet:
        case SigOpCode_sensorCadenceSet:
            responseOpcode = SigOpCode_sensorCadenceStatus;
            break;
        case SigOpCode_sensorSettingsGet:
            responseOpcode = SigOpCode_sensorSettingsStatus;
            break;
        case SigOpCode_sensorSettingGet:
        case SigOpCode_sensorSettingSet:
            responseOpcode = SigOpCode_sensorSettingStatus;
            break;
        case SigOpCode_timeGet:
        case SigOpCode_timeSet:
            responseOpcode = SigOpCode_timeStatus;
            break;
        case SigOpCode_timeRoleGet:
        case SigOpCode_timeRoleSet:
            responseOpcode = SigOpCode_timeRoleStatus;
            break;
        case SigOpCode_timeZoneGet:
        case SigOpCode_timeZoneSet:
            responseOpcode = SigOpCode_timeZoneStatus;
            break;
        case SigOpCode_TAI_UTC_DeltaGet:
        case SigOpCode_TAI_UTC_DeltaSet:
            responseOpcode = SigOpCode_TAI_UTC_DeltaStatus;
            break;
        case SigOpCode_sceneGet:
        case SigOpCode_sceneRecall:
            responseOpcode = SigOpCode_sceneStatus;
            break;
        case SigOpCode_sceneStore:
        case SigOpCode_sceneDelete:
        case SigOpCode_sceneRegisterGet:
            responseOpcode = SigOpCode_sceneRegisterStatus;
            break;
        case SigOpCode_schedulerActionGet:
        case SigOpCode_schedulerActionSet:
            responseOpcode = SigOpCode_schedulerActionStatus;
            break;
        case SigOpCode_schedulerGet:
            responseOpcode = SigOpCode_schedulerStatus;
            break;
        case SigOpCode_lightLightnessGet:
        case SigOpCode_lightLightnessSet:
            responseOpcode = SigOpCode_lightLightnessStatus;
            break;
        case SigOpCode_lightLightnessLinearGet:
        case SigOpCode_lightLightnessLinearSet:
            responseOpcode = SigOpCode_lightLightnessLinearStatus;
            break;
        case SigOpCode_lightLightnessLastGet:
            responseOpcode = SigOpCode_lightLightnessLastStatus;
            break;
        case SigOpCode_lightLightnessDefaultGet:
        case SigOpCode_lightLightnessDefaultSet:
            responseOpcode = SigOpCode_lightLightnessDefaultStatus;
            break;
        case SigOpCode_lightLightnessRangeGet:
        case SigOpCode_lightLightnessRangeSet:
            responseOpcode = SigOpCode_lightLightnessRangeStatus;
            break;
        case SigOpCode_lightCTLGet:
        case SigOpCode_lightCTLSet:
            responseOpcode = SigOpCode_lightCTLStatus;
            break;
        case SigOpCode_lightCTLTemperatureGet:
        case SigOpCode_lightCTLTemperatureSet:
            responseOpcode = SigOpCode_lightCTLTemperatureStatus;
            break;
        case SigOpCode_lightCTLTemperatureRangeGet:
        case SigOpCode_lightCTLTemperatureRangeSet:
            responseOpcode = SigOpCode_lightCTLTemperatureRangeStatus;
            break;
        case SigOpCode_lightCTLDefaultGet:
        case SigOpCode_lightCTLDefaultSet:
            responseOpcode = SigOpCode_lightCTLDefaultStatus;
            break;
        case SigOpCode_lightHSLGet:
        case SigOpCode_lightHSLSet:
            responseOpcode = SigOpCode_lightHSLStatus;
            break;
        case SigOpCode_lightHSLHueGet:
        case SigOpCode_lightHSLHueSet:
            responseOpcode = SigOpCode_lightHSLHueStatus;
            break;
        case SigOpCode_lightHSLSaturationGet:
        case SigOpCode_lightHSLSaturationSet:
            responseOpcode = SigOpCode_lightHSLSaturationStatus;
            break;
        case SigOpCode_lightHSLTargetGet:
            responseOpcode = SigOpCode_lightHSLTargetStatus;
            break;
        case SigOpCode_lightHSLDefaultGet:
        case SigOpCode_lightHSLDefaultSet:
            responseOpcode = SigOpCode_lightHSLDefaultStatus;
            break;
        case SigOpCode_lightHSLRangeGet:
        case SigOpCode_lightHSLRangeSet:
            responseOpcode = SigOpCode_lightHSLRangeStatus;
            break;
        case SigOpCode_lightXyLGet:
        case SigOpCode_lightXyLSet:
            responseOpcode = SigOpCode_lightXyLStatus;
            break;
        case SigOpCode_lightXyLTargetGet:
            responseOpcode = SigOpCode_lightXyLTargetStatus;
            break;
        case SigOpCode_lightXyLDefaultGet:
        case SigOpCode_lightXyLDefaultSet:
            responseOpcode = SigOpCode_lightXyLDefaultStatus;
            break;
        case SigOpCode_lightXyLRangeGet:
        case SigOpCode_lightXyLRangeSet:
            responseOpcode = SigOpCode_lightXyLRangeStatus;
            break;
        case SigOpCode_LightLCModeGet:
        case SigOpCode_LightLCModeSet:
            responseOpcode = SigOpCode_LightLCModeStatus;
            break;
        case SigOpCode_LightLCOMGet:
        case SigOpCode_LightLCOMSet:
            responseOpcode = SigOpCode_LightLCOMStatus;
            break;
        case SigOpCode_LightLCLightOnOffGet:
        case SigOpCode_LightLCLightOnOffSet:
            responseOpcode = SigOpCode_LightLCLightOnOffStatus;
            break;
        case SigOpCode_LightLCPropertyGet:
        case SigOpCode_LightLCPropertySet:
            responseOpcode = SigOpCode_LightLCPropertyStatus;
            break;
            
            // Firmware Update Messages
        case SigOpCode_FirmwareUpdateInformationGet:
            responseOpcode = SigOpCode_FirmwareUpdateInformationStatus;
            break;
        case SigOpCode_FirmwareUpdateFirmwareMetadataCheck:
            responseOpcode = SigOpCode_FirmwareUpdateFirmwareMetadataStatus;
            break;
        case SigOpCode_FirmwareUpdateGet:
        case SigOpCode_FirmwareUpdateStart:
        case SigOpCode_FirmwareUpdateCancel:
        case SigOpCode_FirmwareUpdateApply:
            responseOpcode = SigOpCode_FirmwareUpdateStatus;
            break;
        case SigOpCode_FirmwareDistributionGet:
        case SigOpCode_FirmwareDistributionStart:
        case SigOpCode_FirmwareDistributionCancel:
            responseOpcode = SigOpCode_FirmwareDistributionStatus;
            break;
        case SigOpCode_FirmwareDistributionNodesGet:
            responseOpcode = SigOpCode_FirmwareDistributionNodesList;
            break;

            // BLOB Transfer Messages
        case SigOpCode_BLOBTransferGet:
        case SigOpCode_BLOBTransferStart:
        case SigOpCode_BLOBTransferCancel:
            responseOpcode = SigOpCode_BLOBTransferStatus;
            break;
        case SigOpCode_BLOBBlockStart:
            responseOpcode = SigOpCode_ObjectBlockTransferStatus;
            break;
        case SigOpCode_BLOBBlockGet:
            responseOpcode = SigOpCode_BLOBBlockStatus;
            break;
        case SigOpCode_BLOBInformationGet:
            responseOpcode = SigOpCode_BLOBInformationStatus;
            break;
        default:
            TeLogVerbose(@"Warning:undefault or noAck sendOpcode:0x%x",sendOpcode);
            break;
    }
    return responseOpcode;
}

/// Yes means message need response, No means needn't response.
/// @param message message
- (BOOL)isAcknowledgedMessage:(SigMeshMessage *)message {
    if ([message isKindOfClass:[SigConfigAppKeyAdd class]]) {
        return YES;
    } else {
        int responseOpCode = [self getResponseOpcodeWithSendOpcode:message.opCode];
        return responseOpCode != 0;
    }
}

@end
