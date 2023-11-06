/********************************************************************************************************
 * @file     SigHelper.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/15
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

#import "SigHelper.h"

@implementation SigHelper

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigHelper *shareHelper = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareHelper = [[SigHelper alloc] init];
    });
    return shareHelper;
}

/**
 * @brief   Determine whether the address is a valid address.(inVaild address range is 0xFF00~0xFFF8)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is valid address, NO means address is invalid.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isValidAddress:(UInt16)address {
    return address < 0xFF00 || address > 0xFFF8;
}

/**
 * @brief   Determine whether the address is a unassigned address.(unassigned address is 0)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is unassigned address, NO means address is not unassigned address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isUnassignedAddress:(UInt16)address {
    return address == MeshAddress_unassignedAddress;
}

/**
 * @brief   Determine whether the address is a unicast address.(The range of unicast address is 0x0001~0x7FFF)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is unicast address, NO means address is not unicast address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isUnicastAddress:(UInt16)address {
    return (address & 0x8000) == 0x0000 && ![self isUnassignedAddress:address];
}

/**
 * @brief   Determine whether the address is a virtual address.(The range of virtual address is 0x8000~0xBFFF)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is virtual address, NO means address is not virtual address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isVirtualAddress:(UInt16)address {
    return (address & 0xC000) == 0x8000;
}

/**
 * @brief   Determine whether the address is a group address.(The range of group address is 0xC000~0xEFFF)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is group address, NO means address is not group address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isGroupAddress:(UInt16)address {
    return (address & 0xC000) == 0xC000 && [self isValidAddress:address];
}

/// Get Milliseconds Of Transition Interval.
/// @param periodSteps The Default Transition Number of Steps field is a 6-bit value representing the number of transition steps. The field values represent the states defined in the following table.
/// Value               Description
/// 0x00                The Generic Default Transition Time is immediate.
/// 0x01–0x3E      The number of steps.
/// 0x3E               The value is unknown. The state cannot be set to this value, but an element 0x3F may report an unknown value if a transition is higher than or not determined.
/// @param periodResolution The Default Transition Step Resolution field is a 2-bit bit field that determines the resolution of the Generic Default Transition Time state.
- (int)getMillisecondsOfTransitionIntervalWithPeriodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution {
    switch (periodResolution) {
        case SigStepResolution_hundredsOfMilliseconds:
            return (int)periodSteps * 100;
            break;
        case SigStepResolution_seconds:
            return (int)periodSteps * 1000;
            break;
        case SigStepResolution_tensOfSeconds:
            return (int)periodSteps * 10000;
            break;
        case SigStepResolution_tensOfMinutes:
            return (int)periodSteps * 600000;
            break;
        default:
            break;
    }
}

/**
 * @brief   Change UInt16 node address to hexadecimal string.
 * @param   address    The unicastAddress of node.
 * @return  The hexadecimal string of node address.
 */
- (NSString *)getNodeAddressString:(UInt16)address {
    return [self getUint16String:address];
}

/**
 * @brief   Change UInt16 number to hexadecimal string.
 * @param   address    The UInt16 number address.
 * @return  The hexadecimal string of UInt16 number address.
 */
- (NSString *)getUint16String:(UInt16)address {
    return [NSString stringWithFormat:@"%04X",address];
}

/**
 * @brief   Change UInt32 number to hexadecimal string.
 * @param   address    The UInt32 number address.
 * @return  The hexadecimal string of UInt32 number address.
 */
- (NSString *)getUint32String:(UInt32)address {
    return [NSString stringWithFormat:@"%08X",(unsigned int)address];
}

/**
 * @brief   Change UInt64 number to hexadecimal string.
 * @param   address    The UInt64 number address.
 * @return  The hexadecimal string of UInt64 number address.
 */
- (NSString *)getUint64String:(UInt64)address {
    return [NSString stringWithFormat:@"%016llX",address];
}

/// Randomly generate floating point numbers directly from float number A and float number B, retaining 3 valid decimal places.
/// @param a float number A
/// @param b float number B
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

/// Get response opcode with send opcode, eg://SigOpCode_configAppKeyGet:0x8001->SigOpCode_configAppKeyList:0x8002
/// @param sendOpcode opcode of send command.
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
            
            // private beacon message
        case SigOpCode_PrivateBeaconGet:
        case SigOpCode_PrivateBeaconSet:
            responseOpcode = SigOpCode_PrivateBeaconStatus;
            break;
        case SigOpCode_PrivateGattProxyGet:
        case SigOpCode_PrivateGattProxySet:
            responseOpcode = SigOpCode_PrivateGattProxyStatus;
            break;
        case SigOpCode_PrivateNodeIdentityGet:
        case SigOpCode_PrivateNodeIdentitySet:
            responseOpcode = SigOpCode_PrivateNodeIdentityStatus;
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
        case SigOpCode_FirmwareDistributionApply:
        case SigOpCode_FirmwareDistributionCancel:
            responseOpcode = SigOpCode_FirmwareDistributionStatus;
            break;
        case SigOpCode_FirmwareDistributionReceiversGet:
            responseOpcode = SigOpCode_FirmwareDistributionReceiversList;
            break;
        case SigOpCode_FirmwareDistributionCapabilitiesGet:
            responseOpcode = SigOpCode_FirmwareDistributionCapabilitiesStatus;
            break;
        case SigOpCode_FirmwareDistributionReceiversAdd:
        case SigOpCode_FirmwareDistributionReceiversDeleteAll:
            responseOpcode = SigOpCode_FirmwareDistributionReceiversStatus;
            break;
        case SigOpCode_FirmwareDistributionUploadGet:
        case SigOpCode_FirmwareDistributionUploadStart:
        case SigOpCode_FirmwareDistributionUploadOOBStart:
        case SigOpCode_FirmwareDistributionUploadCancel:
            responseOpcode = SigOpCode_FirmwareDistributionUploadStatus;
            break;
        case SigOpCode_FirmwareDistributionFirmwareGet:
        case SigOpCode_FirmwareDistributionFirmwareGetByIndex:
        case SigOpCode_FirmwareDistributionFirmwareDelete:
        case SigOpCode_FirmwareDistributionFirmwareDeleteAll:
            responseOpcode = SigOpCode_FirmwareDistributionFirmwareStatus;
            break;

            // BLOB Transfer Messages
        case SigOpCode_BLOBTransferGet:
        case SigOpCode_BLOBTransferStart:
        case SigOpCode_BLOBTransferCancel:
            responseOpcode = SigOpCode_BLOBTransferStatus;
            break;
        case SigOpCode_BLOBBlockStart:
        case SigOpCode_BLOBBlockGet:
            responseOpcode = SigOpCode_BLOBBlockStatus;
            break;
        case SigOpCode_BLOBInformationGet:
            responseOpcode = SigOpCode_BLOBInformationStatus;
            break;
            
            // On-demand proxy
        case SigOpCode_OnDemandPrivateProxyGet:
        case SigOpCode_OnDemandPrivateProxySet:
            responseOpcode = SigOpCode_OnDemandPrivateProxyStatus;
            break;
            
            // SAR configuration
        case SigOpCode_SARTransmitterGet:
        case SigOpCode_SARTransmitterSet:
            responseOpcode = SigOpCode_SARTransmitterStatus;
            break;
        case SigOpCode_SARReceiverGet:
        case SigOpCode_SARReceiverSet:
            responseOpcode = SigOpCode_SARReceiverStatus;
            break;
            
            // Opcodes aggregator
        case SigOpCode_OpcodesAggregatorSequence:
            responseOpcode = SigOpCode_OpcodesAggregatorStatus;
            break;

            // Large Composition Data
        case SigOpCode_LargeCompositionDataGet:
            responseOpcode = SigOpCode_LargeCompositionDataStatus;
            break;
        case SigOpCode_ModelsMetadataGet:
            responseOpcode = SigOpCode_ModelsMetadataStatus;
            break;
            
            // 4.3.7 Solicitation PDU RPL Configuration messages
            // 以下3个opcode未定，未找到文档。
        case SigOpCode_SolicitationPduRplItemsClear:
            responseOpcode = SigOpCode_SolicitationPduRplItemsStatus;
            break;
            
            // subnet bridge Messages
        case SigOpCode_BridgingTableSizeGet:
            responseOpcode = SigOpCode_BridgingTableSizeStatus;
            break;
        case SigOpCode_BridgeTableGet:
            responseOpcode = SigOpCode_BridgeTableList;
            break;
        case SigOpCode_BridgeTableAdd:
        case SigOpCode_BridgeTableRemove:
            responseOpcode = SigOpCode_BridgeTableStatus;
            break;
        case SigOpCode_BridgeSubnetsGet:
            responseOpcode = SigOpCode_BridgeSubnetsList;
            break;
        case SigOpCode_SubnetBridgeGet:
        case SigOpCode_SubnetBridgeSet:
            responseOpcode = SigOpCode_SubnetBridgeStatus;
            break;
            
            // Direct Forwarding
        case SigOpCode_DirectControlGet:
        case SigOpCode_DirectControlSet:
            responseOpcode = SigOpCode_DirectControlStatus;
            break;
        case SigOpCode_ForwardingTableAdd:
        case SigOpCode_ForwardingTableDelete:
            responseOpcode = SigOpCode_ForwardingTableStatus;
            break;
            
        default:
            TeLogVerbose(@"Warning:undefault or noAck sendOpcode:0x%x",sendOpcode);
            break;
    }
    return responseOpcode;
}

/// Get mesh message class with opcode
/// @param opCode opcode of send command.
- (_Nullable Class)getMeshMessageWithOpCode:(SigOpCode)opCode {
    Class messageType = nil;
//    TeLogVerbose(@"解析opCode=0x%08x",(unsigned int)opCode);
    if ((opCode & 0xC00000) == 0xC00000) {
        // Vendor Messages
//        MessageType = networkManager.meshNetworkManager.vendorTypes[opCode] ?? UnknownMessage.self
    } else {
        switch (opCode) {
                // Composition Data
            case SigOpCode_configCompositionDataGet:
                messageType = [SigConfigCompositionDataGet class];
                break;
            case SigOpCode_configCompositionDataStatus:
                messageType = [SigConfigCompositionDataStatus class];
                break;
                // Secure Network Beacon configuration
            case SigOpCode_configBeaconGet:
                messageType = [SigConfigBeaconGet class];
                break;
            case SigOpCode_configBeaconSet:
                messageType = [SigConfigBeaconSet class];
                break;
            case SigOpCode_configBeaconStatus:
                messageType = [SigConfigBeaconStatus class];
                break;
                // Relay configuration
            case SigOpCode_configRelayGet:
                messageType = [SigConfigRelayGet class];
                break;
            case SigOpCode_configRelaySet:
                messageType = [SigConfigRelaySet class];
                break;
            case SigOpCode_configRelayStatus:
                messageType = [SigConfigRelayStatus class];
                break;
                // GATT Proxy configuration
            case SigOpCode_configGATTProxyGet:
                messageType = [SigConfigGATTProxyGet class];
                break;
            case SigOpCode_configGATTProxySet:
                messageType = [SigConfigGATTProxySet class];
                break;
            case SigOpCode_configGATTProxyStatus:
                messageType = [SigConfigGATTProxyStatus class];
                break;
                // Key Refresh Phase
            case SigOpCode_configKeyRefreshPhaseGet:
                messageType = [SigConfigKeyRefreshPhaseGet class];
                break;
            case SigOpCode_configKeyRefreshPhaseSet:
                messageType = [SigConfigKeyRefreshPhaseSet class];
                break;
            case SigOpCode_configKeyRefreshPhaseStatus:
                messageType = [SigConfigKeyRefreshPhaseStatus class];
                break;
                // Friend configuration
            case SigOpCode_configFriendGet:
                messageType = [SigConfigFriendGet class];
                break;
            case SigOpCode_configFriendSet:
                messageType = [SigConfigFriendSet class];
                break;
            case SigOpCode_configFriendStatus:
                messageType = [SigConfigFriendStatus class];
                break;
                // Network Transmit configuration
            case SigOpCode_configNetworkTransmitGet:
                messageType = [SigConfigNetworkTransmitGet class];
                break;
            case SigOpCode_configNetworkTransmitSet:
                messageType = [SigConfigNetworkTransmitSet class];
                break;
            case SigOpCode_configNetworkTransmitStatus:
                messageType = [SigConfigNetworkTransmitStatus class];
                break;
                // Network Keys Management
            case SigOpCode_configNetKeyAdd:
                messageType = [SigConfigNetKeyAdd class];
                break;
            case SigOpCode_configNetKeyDelete:
                messageType = [SigConfigNetKeyDelete class];
                break;
            case SigOpCode_configNetKeyUpdate:
                messageType = [SigConfigNetKeyUpdate class];
                break;
            case SigOpCode_configNetKeyStatus:
                messageType = [SigConfigNetKeyStatus class];
                break;
            case SigOpCode_configNetKeyGet:
                messageType = [SigConfigNetKeyGet class];
                break;
            case SigOpCode_configNetKeyList:
                messageType = [SigConfigNetKeyList class];
                break;
                // App Keys Management
            case SigOpCode_configAppKeyAdd:
                messageType = [SigConfigAppKeyAdd class];
                break;
            case SigOpCode_configAppKeyDelete:
                messageType = [SigConfigAppKeyDelete class];
                break;
            case SigOpCode_configAppKeyUpdate:
                messageType = [SigConfigAppKeyUpdate class];
                break;
            case SigOpCode_configAppKeyStatus:
                messageType = [SigConfigAppKeyStatus class];
                break;
            case SigOpCode_configAppKeyGet:
                messageType = [SigConfigAppKeyGet class];
                break;
            case SigOpCode_configAppKeyList:
                messageType = [SigConfigAppKeyList class];
                break;
                // Model Bindings
            case SigOpCode_configModelAppBind:
                messageType = [SigConfigModelAppBind class];
                break;
            case SigOpCode_configModelAppUnbind:
                messageType = [SigConfigModelAppUnbind class];
                break;
            case SigOpCode_configModelAppStatus:
                messageType = [SigConfigModelAppStatus class];
                break;
            case SigOpCode_configSIGModelAppGet:
                messageType = [SigConfigSIGModelAppGet class];
                break;
            case SigOpCode_configSIGModelAppList:
                messageType = [SigConfigAppKeyGet class];
                break;
            case SigOpCode_configVendorModelAppGet:
                messageType = [SigConfigVendorModelAppGet class];
                break;
            case SigOpCode_configVendorModelAppList:
                messageType = [SigConfigVendorModelAppList class];
                break;
                // Publications
            case SigOpCode_configModelPublicationGet:
                messageType = [SigConfigModelPublicationGet class];
                break;
            case SigOpCode_configModelPublicationSet:
                messageType = [SigConfigModelPublicationSet class];
                break;
            case SigOpCode_configModelPublicationVirtualAddressSet:
                messageType = [SigConfigModelPublicationVirtualAddressSet class];
                break;
            case SigOpCode_configModelPublicationStatus:
                messageType = [SigConfigModelPublicationStatus class];
                break;
                // Subscriptions
            case SigOpCode_configModelSubscriptionAdd:
                messageType = [SigConfigModelSubscriptionAdd class];
                break;
            case SigOpCode_configModelSubscriptionDelete:
                messageType = [SigConfigModelSubscriptionDelete class];
                break;
            case SigOpCode_configModelSubscriptionDeleteAll:
                messageType = [SigConfigModelSubscriptionDeleteAll class];
                break;
            case SigOpCode_configModelSubscriptionOverwrite:
                messageType = [SigConfigModelSubscriptionOverwrite class];
                break;
            case SigOpCode_configModelSubscriptionStatus:
                messageType = [SigConfigModelSubscriptionStatus class];
                break;
            case SigOpCode_configModelSubscriptionVirtualAddressAdd:
                messageType = [SigConfigModelSubscriptionVirtualAddressAdd class];
                break;
            case SigOpCode_configModelSubscriptionVirtualAddressDelete:
                messageType = [SigConfigModelSubscriptionVirtualAddressDelete class];
                break;
            case SigOpCode_configModelSubscriptionVirtualAddressOverwrite:
                messageType = [SigConfigModelSubscriptionVirtualAddressOverwrite class];
                break;
            case SigOpCode_configSIGModelSubscriptionGet:
                messageType = [SigConfigSIGModelSubscriptionGet class];
                break;
            case SigOpCode_configSIGModelSubscriptionList:
                messageType = [SigConfigSIGModelSubscriptionList class];
                break;
            case SigOpCode_configVendorModelSubscriptionGet:
                messageType = [SigConfigVendorModelSubscriptionGet class];
                break;
            case SigOpCode_configVendorModelSubscriptionList:
                messageType = [SigConfigVendorModelSubscriptionList class];
                break;
                
                // Low Power Node Poll Timeout
            case SigOpCode_configLowPowerNodePollTimeoutGet:
                messageType = [SigConfigLowPowerNodePollTimeoutGet class];
                break;
            case SigOpCode_configLowPowerNodePollTimeoutStatus:
                messageType = [SigConfigLowPowerNodePollTimeoutStatus class];
                break;

                // Heartbeat Publication
            case SigOpCode_configHeartbeatPublicationGet:
                messageType = [SigConfigHeartbeatPublicationGet class];
                break;
            case SigOpCode_configHeartbeatPublicationSet:
                messageType = [SigConfigHeartbeatPublicationSet class];
                break;
            case SigOpCode_configHeartbeatPublicationStatus:
                messageType = [SigConfigHeartbeatPublicationStatus class];
                break;
                // node identity
            case SigOpCode_configHeartbeatSubscriptionGet:
                messageType = [SigConfigHeartbeatSubscriptionGet class];
                break;
            case SigOpCode_configHeartbeatSubscriptionSet:
                messageType = [SigConfigHeartbeatSubscriptionSet class];
                break;
            case SigOpCode_configHeartbeatSubscriptionStatus:
                messageType = [SigConfigHeartbeatSubscriptionStatus class];
                break;

                // node identity
            case SigOpCode_configNodeIdentityGet:
                messageType = [SigConfigNodeIdentityGet class];
                break;
            case SigOpCode_configNodeIdentitySet:
                messageType = [SigConfigNodeIdentitySet class];
                break;
            case SigOpCode_configNodeIdentityStatus:
                messageType = [SigConfigNodeIdentityStatus class];
                break;

                // Resetting Node
            case SigOpCode_configNodeReset:
                messageType = [SigConfigNodeReset class];
                break;
            case SigOpCode_configNodeResetStatus:
                messageType = [SigConfigNodeResetStatus class];
                break;
                // Default TTL
            case SigOpCode_configDefaultTtlGet:
                messageType = [SigConfigDefaultTtlGet class];
                break;
            case SigOpCode_configDefaultTtlSet:
                messageType = [SigConfigDefaultTtlSet class];
                break;
            case SigOpCode_configDefaultTtlStatus:
                messageType = [SigConfigDefaultTtlStatus class];
                break;
                // remote provision
            case SigOpCode_remoteProvisioningScanCapabilitiesGet:
                messageType = [SigRemoteProvisioningScanCapabilitiesGet class];
                break;
            case SigOpCode_remoteProvisioningScanCapabilitiesStatus:
                messageType = [SigRemoteProvisioningScanCapabilitiesStatus class];
                break;
            case SigOpCode_remoteProvisioningScanGet:
                messageType = [SigRemoteProvisioningScanGet class];
                break;
            case SigOpCode_remoteProvisioningScanStart:
                messageType = [SigRemoteProvisioningScanStart class];
                break;
            case SigOpCode_remoteProvisioningScanStop:
                messageType = [SigRemoteProvisioningScanStop class];
                break;
            case SigOpCode_remoteProvisioningScanStatus:
                messageType = [SigRemoteProvisioningScanStatus class];
                break;
            case SigOpCode_remoteProvisioningScanReport:
                messageType = [SigRemoteProvisioningScanReport class];
                break;
            case SigOpCode_remoteProvisioningExtendedScanStart:
                messageType = [SigRemoteProvisioningExtendedScanStart class];
                break;
            case SigOpCode_remoteProvisioningExtendedScanReport:
                messageType = [SigRemoteProvisioningExtendedScanReport class];
                break;
            case SigOpCode_remoteProvisioningLinkGet:
                messageType = [SigRemoteProvisioningLinkGet class];
                break;
            case SigOpCode_remoteProvisioningLinkOpen:
                messageType = [SigRemoteProvisioningLinkOpen class];
                break;
            case SigOpCode_remoteProvisioningLinkClose:
                messageType = [SigRemoteProvisioningLinkClose class];
                break;
            case SigOpCode_remoteProvisioningLinkStatus:
                messageType = [SigRemoteProvisioningLinkStatus class];
                break;
            case SigOpCode_remoteProvisioningLinkReport:
                messageType = [SigRemoteProvisioningLinkReport class];
                break;
            case SigOpCode_remoteProvisioningPDUSend:
                messageType = [SigRemoteProvisioningPDUSend class];
                break;
            case SigOpCode_remoteProvisioningPDUOutboundReport:
                messageType = [SigRemoteProvisioningPDUOutboundReport class];
                break;
            case SigOpCode_remoteProvisioningPDUReport:
                messageType = [SigRemoteProvisioningPDUReport class];
                break;
                                
                // private beacon message
            case SigOpCode_PrivateBeaconGet:
                messageType = [SigPrivateBeaconGet class];
                break;
            case SigOpCode_PrivateBeaconSet:
                messageType = [SigPrivateBeaconSet class];
                break;
            case SigOpCode_PrivateBeaconStatus:
                messageType = [SigPrivateBeaconStatus class];
                break;
            case SigOpCode_PrivateGattProxyGet:
                messageType = [SigPrivateGattProxyGet class];
                break;
            case SigOpCode_PrivateGattProxySet:
                messageType = [SigPrivateGattProxySet class];
                break;
            case SigOpCode_PrivateGattProxyStatus:
                messageType = [SigPrivateGattProxyStatus class];
                break;
            case SigOpCode_PrivateNodeIdentityGet:
                messageType = [SigPrivateNodeIdentityGet class];
                break;
            case SigOpCode_PrivateNodeIdentitySet:
                messageType = [SigPrivateNodeIdentitySet class];
                break;
            case SigOpCode_PrivateNodeIdentityStatus:
                messageType = [SigPrivateNodeIdentityStatus class];
                break;

                // Generics
            case SigOpCode_genericOnOffGet:
                messageType = [SigGenericOnOffGet class];
                break;
            case SigOpCode_genericOnOffSet:
                messageType = [SigGenericOnOffSet class];
                break;
            case SigOpCode_genericOnOffSetUnacknowledged:
                messageType = [SigGenericOnOffSetUnacknowledged class];
                break;
            case SigOpCode_genericOnOffStatus:
                messageType = [SigGenericOnOffStatus class];
                break;
            case SigOpCode_genericLevelGet:
                messageType = [SigGenericLevelGet class];
                break;
            case SigOpCode_genericLevelSet:
                messageType = [SigGenericLevelSet class];
                break;
            case SigOpCode_genericLevelSetUnacknowledged:
                messageType = [SigGenericLevelSetUnacknowledged class];
                break;
            case SigOpCode_genericLevelStatus:
                messageType = [SigGenericLevelStatus class];
                break;
            case SigOpCode_genericDeltaSet:
                messageType = [SigGenericDeltaSet class];
                break;
            case SigOpCode_genericDeltaSetUnacknowledged:
                messageType = [SigGenericDeltaSetUnacknowledged class];
                break;
            case SigOpCode_genericMoveSet:
                messageType = [SigGenericMoveSet class];
                break;
            case SigOpCode_genericMoveSetUnacknowledged:
                messageType = [SigGenericMoveSetUnacknowledged class];
                break;
            case SigOpCode_genericDefaultTransitionTimeGet:
                messageType = [SigGenericDefaultTransitionTimeGet class];
                break;
            case SigOpCode_genericDefaultTransitionTimeSet:
                messageType = [SigGenericDefaultTransitionTimeSet class];
                break;
            case SigOpCode_genericDefaultTransitionTimeSetUnacknowledged:
                messageType = [SigGenericDefaultTransitionTimeSetUnacknowledged class];
                break;
            case SigOpCode_genericDefaultTransitionTimeStatus:
                messageType = [SigGenericDefaultTransitionTimeStatus class];
                break;
            case SigOpCode_genericOnPowerUpGet:
                messageType = [SigGenericOnPowerUpGet class];
                break;
            case SigOpCode_genericOnPowerUpSet:
                messageType = [SigGenericOnPowerUpSet class];
                break;
            case SigOpCode_genericOnPowerUpSetUnacknowledged:
                messageType = [SigGenericOnPowerUpSetUnacknowledged class];
                break;
            case SigOpCode_genericOnPowerUpStatus:
                messageType = [SigGenericOnPowerUpStatus class];
                break;
            case SigOpCode_genericPowerLevelGet:
                messageType = [SigGenericPowerLevelGet class];
                break;
            case SigOpCode_genericPowerLevelSet:
                messageType = [SigGenericPowerLevelSet class];
                break;
            case SigOpCode_genericPowerLevelSetUnacknowledged:
                messageType = [SigGenericPowerLevelSetUnacknowledged class];
                break;
            case SigOpCode_genericPowerLevelStatus:
                messageType = [SigGenericPowerLevelStatus class];
                break;
            case SigOpCode_genericPowerLastGet:
                messageType = [SigGenericPowerLastGet class];
                break;
            case SigOpCode_genericPowerLastStatus:
                messageType = [SigGenericPowerLastStatus class];
                break;
            case SigOpCode_genericPowerDefaultGet:
                messageType = [SigGenericPowerDefaultGet class];
                break;
            case SigOpCode_genericPowerDefaultStatus:
                messageType = [SigGenericPowerDefaultStatus class];
                break;
            case SigOpCode_genericPowerRangeGet:
                messageType = [SigGenericPowerRangeGet class];
                break;
            case SigOpCode_genericPowerRangeStatus:
                messageType = [SigGenericPowerRangeStatus class];
                break;
            case SigOpCode_genericPowerDefaultSet:
                messageType = [SigGenericPowerDefaultSet class];
                break;
            case SigOpCode_genericPowerDefaultSetUnacknowledged:
                messageType = [SigGenericPowerDefaultSetUnacknowledged class];
                break;
            case SigOpCode_genericPowerRangeSet:
                messageType = [SigGenericPowerRangeSet class];
                break;
            case SigOpCode_genericPowerRangeSetUnacknowledged:
                messageType = [SigGenericPowerRangeSetUnacknowledged class];
                break;
            case SigOpCode_genericBatteryGet:
                messageType = [SigGenericBatteryGet class];
                break;
            case SigOpCode_genericBatteryStatus:
                messageType = [SigGenericBatteryStatus class];
                break;
                //Sensor
            case SigOpCode_sensorDescriptorGet:
                messageType = [SigSensorDescriptorGet class];
                break;
            case SigOpCode_sensorDescriptorStatus:
                messageType = [SigSensorDescriptorStatus class];
                break;
            case SigOpCode_sensorGet:
                messageType = [SigSensorGet class];
                break;
            case SigOpCode_sensorStatus:
                messageType = [SigSensorStatus class];
                break;
            case SigOpCode_sensorColumnGet:
                messageType = [SigSensorColumnGet class];
                break;
            case SigOpCode_sensorColumnStatus:
                messageType = [SigSensorColumnStatus class];
                break;
            case SigOpCode_sensorSeriesGet:
                messageType = [SigSensorSeriesGet class];
                break;
            case SigOpCode_sensorSeriesStatus:
                messageType = [SigSensorSeriesStatus class];
                break;
                
                //Sensor Setup
            case SigOpCode_sensorCadenceGet:
                messageType = [SigSensorCadenceGet class];
                break;
            case SigOpCode_sensorCadenceSet:
                messageType = [SigSensorCadenceSet class];
                break;
            case SigOpCode_sensorCadenceSetUnacknowledged:
                messageType = [SigSensorCadenceSetUnacknowledged class];
                break;
            case SigOpCode_sensorCadenceStatus:
                messageType = [SigSensorCadenceStatus class];
                break;
            case SigOpCode_sensorSettingsGet:
                messageType = [SigSensorSettingsGet class];
                break;
            case SigOpCode_sensorSettingsStatus:
                messageType = [SigSensorSettingsStatus class];
                break;
            case SigOpCode_sensorSettingGet:
                messageType = [SigSensorSettingGet class];
                break;
            case SigOpCode_sensorSettingSet:
                messageType = [SigSensorSettingSet class];
                break;
            case SigOpCode_sensorSettingSetUnacknowledged:
                messageType = [SigSensorSettingSetUnacknowledged class];
                break;
            case SigOpCode_sensorSettingStatus:
                messageType = [SigSensorSettingStatus class];
                break;
            //Time
            case SigOpCode_timeGet:
                messageType = [SigTimeGet class];
                break;
            case SigOpCode_timeSet:
                messageType = [SigTimeSet class];
                break;
            case SigOpCode_timeStatus:
                messageType = [SigTimeStatus class];
                break;
            case SigOpCode_timeRoleGet:
                messageType = [SigTimeRoleGet class];
                break;
            case SigOpCode_timeRoleSet:
                messageType = [SigTimeRoleSet class];
                break;
            case SigOpCode_timeRoleStatus:
                messageType = [SigTimeRoleStatus class];
                break;
            case SigOpCode_timeZoneGet:
                messageType = [SigTimeZoneGet class];
                break;
            case SigOpCode_timeZoneSet:
                messageType = [SigTimeZoneSet class];
                break;
            case SigOpCode_timeZoneStatus:
                messageType = [SigTimeZoneStatus class];
                break;
            case SigOpCode_TAI_UTC_DeltaGet:
                messageType = [SigTAI_UTC_DeltaGet class];
                break;
            case SigOpCode_TAI_UTC_DeltaSet:
                messageType = [SigTAI_UTC_DeltaSet class];
                break;
            case SigOpCode_TAI_UTC_DeltaStatus:
                messageType = [SigTAI_UTC_DeltaStatus class];
                break;
            //Scene
            case SigOpCode_sceneGet:
                messageType = [SigSceneGet class];
                break;
            case SigOpCode_sceneRecall:
                messageType = [SigSceneRecall class];
                break;
            case SigOpCode_sceneRecallUnacknowledged:
                messageType = [SigSceneRecallUnacknowledged class];
                break;
            case SigOpCode_sceneStatus:
                messageType = [SigSceneStatus class];
                break;
            case SigOpCode_sceneRegisterGet:
                messageType = [SigSceneRegisterGet class];
                break;
            case SigOpCode_sceneRegisterStatus:
                messageType = [SigSceneRegisterStatus class];
                break;
            //Scene Setup
            case SigOpCode_sceneStore:
                messageType = [SigSceneStore class];
                break;
            case SigOpCode_sceneStoreUnacknowledged:
                messageType = [SigSceneStore class];
                break;
            case SigOpCode_sceneDelete:
                messageType = [SigSceneDelete class];
                break;
            case SigOpCode_sceneDeleteUnacknowledged:
                messageType = [SigSceneDeleteUnacknowledged class];
                break;
            //Scheduler
            case SigOpCode_schedulerActionGet:
                messageType = [SigSchedulerActionGet class];
                break;
            case SigOpCode_schedulerActionStatus:
                messageType = [SigSchedulerActionStatus class];
                break;
            case SigOpCode_schedulerGet:
                messageType = [SigSchedulerGet class];
                break;
            case SigOpCode_schedulerStatus:
                messageType = [SigSchedulerStatus class];
                break;
            //Scheduler Setup
            case SigOpCode_schedulerActionSet:
                messageType = [SigSchedulerActionSet class];
                break;
            case SigOpCode_schedulerActionSetUnacknowledged:
                messageType = [SigSchedulerActionSetUnacknowledged class];
                break;
            //Light Lightness
            case SigOpCode_lightLightnessGet:
                messageType = [SigLightLightnessGet class];
                break;
            case SigOpCode_lightLightnessSet:
                messageType = [SigLightLightnessSet class];
                break;
            case SigOpCode_lightLightnessSetUnacknowledged:
                messageType = [SigLightLightnessSetUnacknowledged class];
                break;
            case SigOpCode_lightLightnessStatus:
                messageType = [SigLightLightnessStatus class];
                break;
            case SigOpCode_lightLightnessLinearGet:
                messageType = [SigLightLightnessLinearGet class];
                break;
            case SigOpCode_lightLightnessLinearSet:
                messageType = [SigLightLightnessLinearSet class];
                break;
            case SigOpCode_lightLightnessLinearSetUnacknowledged:
                messageType = [SigLightLightnessLinearSetUnacknowledged class];
                break;
            case SigOpCode_lightLightnessLinearStatus:
                messageType = [SigLightLightnessLinearStatus class];
                break;
            case SigOpCode_lightLightnessLastGet:
                messageType = [SigLightLightnessLastGet class];
                break;
            case SigOpCode_lightLightnessLastStatus:
                messageType = [SigLightLightnessLastStatus class];
                break;
            case SigOpCode_lightLightnessDefaultGet:
                messageType = [SigLightLightnessDefaultGet class];
                break;
            case SigOpCode_lightLightnessDefaultStatus:
                messageType = [SigLightLightnessDefaultStatus class];
                break;
            case SigOpCode_lightLightnessRangeGet:
                messageType = [SigLightLightnessRangeGet class];
                break;
            case SigOpCode_lightLightnessRangeStatus:
                messageType = [SigLightLightnessRangeStatus class];
                break;
            //Light Lightness Setup
            case SigOpCode_lightLightnessDefaultSet:
                messageType = [SigLightLightnessDefaultSet class];
                break;
            case SigOpCode_lightLightnessDefaultSetUnacknowledged:
                messageType = [SigLightLightnessDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightLightnessRangeSet:
                messageType = [SigLightLightnessRangeSet class];
                break;
            case SigOpCode_lightLightnessRangeSetUnacknowledged:
                messageType = [SigLightLightnessRangeSetUnacknowledged class];
                break;
            //Light CTL
            case SigOpCode_lightCTLGet:
                messageType = [SigLightCTLGet class];
                break;
            case SigOpCode_lightCTLSet:
                messageType = [SigLightCTLSet class];
                break;
            case SigOpCode_lightCTLSetUnacknowledged:
                messageType = [SigLightCTLSetUnacknowledged class];
                break;
            case SigOpCode_lightCTLStatus:
                messageType = [SigLightCTLStatus class];
                break;
            case SigOpCode_lightCTLTemperatureGet:
                messageType = [SigLightCTLTemperatureGet class];
                break;
            case SigOpCode_lightCTLTemperatureRangeGet:
                messageType = [SigLightCTLTemperatureRangeGet class];
                break;
            case SigOpCode_lightCTLTemperatureRangeStatus:
                messageType = [SigLightCTLTemperatureRangeStatus class];
                break;
            case SigOpCode_lightCTLTemperatureSet:
                messageType = [SigLightCTLTemperatureSet class];
                break;
            case SigOpCode_lightCTLTemperatureSetUnacknowledged:
                messageType = [SigLightCTLTemperatureSetUnacknowledged class];
                break;
            case SigOpCode_lightCTLTemperatureStatus:
                messageType = [SigLightCTLTemperatureStatus class];
                break;
            case SigOpCode_lightCTLDefaultGet:
                messageType = [SigLightCTLDefaultGet class];
                break;
            case SigOpCode_lightCTLDefaultStatus:
                messageType = [SigLightCTLDefaultStatus class];
                break;
            //Light CTL Setup
            case SigOpCode_lightCTLDefaultSet:
                messageType = [SigLightCTLDefaultSet class];
                break;
            case SigOpCode_lightCTLDefaultSetUnacknowledged:
                messageType = [SigLightCTLDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightCTLTemperatureRangeSet:
                messageType = [SigLightCTLTemperatureRangeSet class];
                break;
            case SigOpCode_lightCTLTemperatureRangeSetUnacknowledged:
                messageType = [SigLightCTLTemperatureRangeSetUnacknowledged class];
                break;
            //Light HSL
            case SigOpCode_lightHSLGet:
                messageType = [SigLightHSLGet class];
                break;
            case SigOpCode_lightHSLHueGet:
                messageType = [SigLightHSLHueGet class];
                break;
            case SigOpCode_lightHSLHueSet:
                messageType = [SigLightHSLHueSet class];
                break;
            case SigOpCode_lightHSLHueSetUnacknowledged:
                messageType = [SigLightHSLHueSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLHueStatus:
                messageType = [SigLightHSLHueStatus class];
                break;
            case SigOpCode_lightHSLSaturationGet:
                messageType = [SigLightHSLSaturationGet class];
                break;
            case SigOpCode_lightHSLSaturationSet:
                messageType = [SigLightHSLSaturationSet class];
                break;
            case SigOpCode_lightHSLSaturationSetUnacknowledged:
                messageType = [SigLightHSLSaturationSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLSaturationStatus:
                messageType = [SigLightHSLSaturationStatus class];
                break;
            case SigOpCode_lightHSLSet:
                messageType = [SigLightHSLSet class];
                break;
            case SigOpCode_lightHSLSetUnacknowledged:
                messageType = [SigLightHSLSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLStatus:
                messageType = [SigLightHSLStatus class];
                break;
            case SigOpCode_lightHSLTargetGet:
                messageType = [SigLightHSLTargetGet class];
                break;
            case SigOpCode_lightHSLTargetStatus:
                messageType = [SigLightHSLTargetStatus class];
                break;
            case SigOpCode_lightHSLDefaultGet:
                messageType = [SigLightHSLDefaultGet class];
                break;
            case SigOpCode_lightHSLDefaultStatus:
                messageType = [SigLightHSLDefaultStatus class];
                break;
            case SigOpCode_lightHSLRangeGet:
                messageType = [SigLightHSLRangeGet class];
                break;
            case SigOpCode_lightHSLRangeStatus:
                messageType = [SigLightHSLRangeStatus class];
                break;
            //Light HSL Setup
            case SigOpCode_lightHSLDefaultSet:
                messageType = [SigLightHSLDefaultSet class];
                break;
            case SigOpCode_lightHSLDefaultSetUnacknowledged:
                messageType = [SigLightHSLDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLRangeSet:
                messageType = [SigLightHSLRangeSet class];
                break;
            case SigOpCode_lightHSLRangeSetUnacknowledged:
                messageType = [SigLightHSLRangeSetUnacknowledged class];
                break;
            //Light xyL
            case SigOpCode_lightXyLGet:
                messageType = [SigLightXyLGet class];
                break;
            case SigOpCode_lightXyLSet:
                messageType = [SigLightXyLSet class];
                break;
            case SigOpCode_lightXyLSetUnacknowledged:
                messageType = [SigLightXyLSetUnacknowledged class];
                break;
            case SigOpCode_lightXyLStatus:
                messageType = [SigLightXyLStatus class];
                break;
            case SigOpCode_lightXyLTargetGet:
                messageType = [SigLightXyLTargetGet class];
                break;
            case SigOpCode_lightXyLTargetStatus:
                messageType = [SigLightXyLTargetStatus class];
                break;
            case SigOpCode_lightXyLDefaultGet:
                messageType = [SigLightXyLDefaultGet class];
                break;
            case SigOpCode_lightXyLDefaultStatus:
                messageType = [SigLightXyLDefaultStatus class];
                break;
            case SigOpCode_lightXyLRangeGet:
                messageType = [SigLightXyLRangeGet class];
                break;
            case SigOpCode_lightXyLRangeStatus:
                messageType = [SigLightXyLRangeStatus class];
                break;
                //Light xyL Setup
            case SigOpCode_lightXyLDefaultSet:
                messageType = [SigLightXyLDefaultSet class];
                break;
            case SigOpCode_lightXyLDefaultSetUnacknowledged:
                messageType = [SigLightXyLDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightXyLRangeSet:
                messageType = [SigLightXyLRangeSet class];
                break;
            case SigOpCode_lightXyLRangeSetUnacknowledged:
                messageType = [SigLightXyLRangeSetUnacknowledged class];
                break;
                //Light Control
            case SigOpCode_LightLCModeGet:
                messageType = [SigLightLCModeGet class];
                break;
            case SigOpCode_LightLCModeSet:
                messageType = [SigLightLCModeSet class];
                break;
            case SigOpCode_LightLCModeSetUnacknowledged:
                messageType = [SigLightLCModeSetUnacknowledged class];
                break;
            case SigOpCode_LightLCModeStatus:
                messageType = [SigLightLCModeStatus class];
                break;
            case SigOpCode_LightLCOMGet:
                messageType = [SigLightLCOMGet class];
                break;
            case SigOpCode_LightLCOMSet:
                messageType = [SigLightLCOMSet class];
                break;
            case SigOpCode_LightLCOMSetUnacknowledged:
                messageType = [SigLightLCOMSetUnacknowledged class];
                break;
            case SigOpCode_LightLCOMStatus:
                messageType = [SigLightLCOMStatus class];
                break;
            case SigOpCode_LightLCLightOnOffGet:
                messageType = [SigLightLCLightOnOffGet class];
                break;
            case SigOpCode_LightLCLightOnOffSet:
                messageType = [SigLightLCLightOnOffSet class];
                break;
            case SigOpCode_LightLCLightOnOffSetUnacknowledged:
                messageType = [SigLightLCLightOnOffSetUnacknowledged class];
                break;
            case SigOpCode_LightLCLightOnOffStatus:
                messageType = [SigLightLCLightOnOffStatus class];
                break;
            case SigOpCode_LightLCPropertyGet:
                messageType = [SigLightLCPropertyGet class];
                break;
            case SigOpCode_LightLCPropertySet:
                messageType = [SigLightLCPropertySet class];
                break;
            case SigOpCode_LightLCPropertySetUnacknowledged:
                messageType = [SigLightLCPropertySetUnacknowledged class];
                break;
            case SigOpCode_LightLCPropertyStatus:
                messageType = [SigLightLCPropertyStatus class];
                break;
                
                // Firmware Update Messages
            case SigOpCode_FirmwareUpdateInformationGet:
                messageType = [SigFirmwareUpdateInformationGet class];
                break;
            case SigOpCode_FirmwareUpdateInformationStatus:
                messageType = [SigFirmwareUpdateInformationStatus class];
                break;
            case SigOpCode_FirmwareUpdateFirmwareMetadataCheck:
                messageType = [SigFirmwareUpdateFirmwareMetadataCheck class];
                break;
            case SigOpCode_FirmwareUpdateFirmwareMetadataStatus:
                messageType = [SigFirmwareUpdateFirmwareMetadataStatus class];
                break;
            case SigOpCode_FirmwareUpdateGet:
                messageType = [SigFirmwareUpdateGet class];
                break;
            case SigOpCode_FirmwareUpdateStart:
                messageType = [SigFirmwareUpdateStart class];
                break;
            case SigOpCode_FirmwareUpdateCancel:
                messageType = [SigFirmwareUpdateCancel class];
                break;
            case SigOpCode_FirmwareUpdateApply:
                messageType = [SigFirmwareUpdateApply class];
                break;
            case SigOpCode_FirmwareUpdateStatus:
                messageType = [SigFirmwareUpdateStatus class];
                break;
            case SigOpCode_FirmwareDistributionGet:
                messageType = [SigFirmwareDistributionGet class];
                break;
            case SigOpCode_FirmwareDistributionStart:
                messageType = [SigFirmwareDistributionStart class];
                break;
            case SigOpCode_FirmwareDistributionCancel:
                messageType = [SigFirmwareDistributionCancel class];
                break;
            case SigOpCode_FirmwareDistributionApply:
                messageType = [SigFirmwareDistributionApply class];
                break;
            case SigOpCode_FirmwareDistributionUploadGet:
                messageType = [SigFirmwareDistributionUploadGet class];
                break;
            case SigOpCode_FirmwareDistributionUploadStart:
                messageType = [SigFirmwareDistributionUploadStart class];
                break;
            case SigOpCode_FirmwareDistributionUploadOOBStart:
                messageType = [SigFirmwareDistributionUploadOOBStart class];
                break;
            case SigOpCode_FirmwareDistributionUploadCancel:
                messageType = [SigFirmwareDistributionUploadCancel class];
                break;
            case SigOpCode_FirmwareDistributionUploadStatus:
                messageType = [SigFirmwareDistributionUploadStatus class];
                break;
            case SigOpCode_FirmwareDistributionFirmwareGet:
                messageType = [SigFirmwareDistributionFirmwareGet class];
                break;
            case SigOpCode_FirmwareDistributionFirmwareGetByIndex:
                messageType = [SigFirmwareDistributionFirmwareGetByIndex class];
                break;
            case SigOpCode_FirmwareDistributionFirmwareDelete:
                messageType = [SigFirmwareDistributionFirmwareDelete class];
                break;
            case SigOpCode_FirmwareDistributionFirmwareDeleteAll:
                messageType = [SigFirmwareDistributionFirmwareDeleteAll class];
                break;
            case SigOpCode_FirmwareDistributionFirmwareStatus:
                messageType = [SigFirmwareDistributionFirmwareStatus class];
                break;
            case SigOpCode_FirmwareDistributionStatus:
                messageType = [SigFirmwareDistributionStatus class];
                break;
            case SigOpCode_FirmwareDistributionReceiversGet:
                messageType = [SigFirmwareDistributionReceiversGet class];
                break;
            case SigOpCode_FirmwareDistributionReceiversList:
                messageType = [SigFirmwareDistributionReceiversList class];
                break;
            case SigOpCode_FirmwareDistributionReceiversAdd:
                messageType = [SigFirmwareDistributionReceiversAdd class];
                break;
            case SigOpCode_FirmwareDistributionReceiversDeleteAll:
                messageType = [SigFirmwareDistributionReceiversDeleteAll class];
                break;
            case SigOpCode_FirmwareDistributionReceiversStatus:
                messageType = [SigFirmwareDistributionReceiversStatus class];
                break;
            case SigOpCode_FirmwareDistributionCapabilitiesGet:
                messageType = [SigFirmwareDistributionCapabilitiesGet class];
                break;
            case SigOpCode_FirmwareDistributionCapabilitiesStatus:
                messageType = [SigFirmwareDistributionCapabilitiesStatus class];
                break;
                
                // BLOB Transfer Messages
            case SigOpCode_BLOBTransferGet:
                messageType = [SigBLOBTransferGet class];
                break;
            case SigOpCode_BLOBTransferStart:
                messageType = [SigBLOBTransferStart class];
                break;
            case SigOpCode_BLOBTransferCancel:
                messageType = [SigObjectTransferCancel class];
                break;
            case SigOpCode_BLOBTransferStatus:
                messageType = [SigBLOBTransferStatus class];
                break;
            case SigOpCode_BLOBBlockStart:
                messageType = [SigBLOBBlockStart class];
                break;
            case SigOpCode_BLOBChunkTransfer:
                messageType = [SigBLOBChunkTransfer class];
                break;
            case SigOpCode_BLOBBlockGet:
                messageType = [SigBLOBBlockGet class];
                break;
            case SigOpCode_BLOBBlockStatus:
                messageType = [SigBLOBBlockStatus class];
                break;
            case SigOpCode_BLOBInformationGet:
                messageType = [SigBLOBInformationGet class];
                break;
            case SigOpCode_BLOBInformationStatus:
                messageType = [SigBLOBInformationStatus class];
                break;
            case SigOpCode_BLOBPartialBlockReport:
                messageType = [SigBLOBPartialBlockReport class];
                break;
                
                // On-demand proxy
            case SigOpCode_OnDemandPrivateProxyGet:
                messageType = [NSClassFromString(@"SigOnDemandPrivateProxyGet") class];
                break;
            case SigOpCode_OnDemandPrivateProxySet:
                messageType = [NSClassFromString(@"SigOnDemandPrivateProxySet") class];
                break;
            case SigOpCode_OnDemandPrivateProxyStatus:
                messageType = [NSClassFromString(@"SigOnDemandPrivateProxyStatus") class];
                break;
                
                // SAR configuration
            case SigOpCode_SARTransmitterGet:
                messageType = [NSClassFromString(@"SigSARTransmitterGet") class];
                break;
            case SigOpCode_SARTransmitterSet:
                messageType = [NSClassFromString(@"SigSARTransmitterSet") class];
                break;
            case SigOpCode_SARTransmitterStatus:
                messageType = [NSClassFromString(@"SigSARTransmitterStatus") class];
                break;
            case SigOpCode_SARReceiverGet:
                messageType = [NSClassFromString(@"SigSARReceiverGet") class];
                break;
            case SigOpCode_SARReceiverSet:
                messageType = [NSClassFromString(@"SigSARReceiverSet") class];
                break;
            case SigOpCode_SARReceiverStatus:
                messageType = [NSClassFromString(@"SigSARReceiverStatus") class];
                break;
                
                // Opcodes aggregator
            case SigOpCode_OpcodesAggregatorSequence:
                messageType = [NSClassFromString(@"SigOpcodesAggregatorSequence") class];
                break;
            case SigOpCode_OpcodesAggregatorStatus:
                messageType = [NSClassFromString(@"SigOpcodesAggregatorStatus") class];
                break;
                
                // Large Composition Data
            case SigOpCode_LargeCompositionDataGet:
                messageType = [NSClassFromString(@"SigLargeCompositionDataGet") class];
                break;
            case SigOpCode_LargeCompositionDataStatus:
                messageType = [NSClassFromString(@"SigLargeCompositionDataStatus") class];
                break;
            case SigOpCode_ModelsMetadataGet:
                messageType = [NSClassFromString(@"SigModelsMetadataGet") class];
                break;
            case SigOpCode_ModelsMetadataStatus:
                messageType = [NSClassFromString(@"SigModelsMetadataStatus") class];
                break;
                
                // 4.3.7 Solicitation PDU RPL Configuration messages
                // 以下3个opcode未定，未找到文档。
            case SigOpCode_SolicitationPduRplItemsClear:
                messageType = [NSClassFromString(@"SigSolicitationPduRplItemsClear") class];
                break;
            case SigOpCode_SolicitationPduRplItemsClearUnacknowledged:
                messageType = [NSClassFromString(@"SigSolicitationPduRplItemsClearUnacknowledged") class];
                break;
            case SigOpCode_SolicitationPduRplItemsStatus:
                messageType = [NSClassFromString(@"SigSolicitationPduRplItemsStatus") class];
                break;
                
                // subnet bridge Messages
            case SigOpCode_BridgingTableSizeGet:
                messageType = [NSClassFromString(@"SigBridgingTableSizeGet") class];
                break;
            case SigOpCode_BridgingTableSizeStatus:
                messageType = [NSClassFromString(@"SigBridgingTableSizeStatus") class];
                break;
            case SigOpCode_BridgeTableAdd:
                messageType = [NSClassFromString(@"SigBridgeTableAdd") class];
                break;
            case SigOpCode_BridgeTableGet:
                messageType = [NSClassFromString(@"SigBridgeTableGet") class];
                break;
            case SigOpCode_BridgeTableList:
                messageType = [NSClassFromString(@"SigBridgeTableList") class];
                break;
            case SigOpCode_BridgeTableRemove:
                messageType = [NSClassFromString(@"SigBridgeTableRemove") class];
                break;
            case SigOpCode_BridgeTableStatus:
                messageType = [NSClassFromString(@"SigBridgeTableStatus") class];
                break;
            case SigOpCode_BridgeSubnetsGet:
                messageType = [NSClassFromString(@"SigBridgeSubnetsGet") class];
                break;
            case SigOpCode_BridgeSubnetsList:
                messageType = [NSClassFromString(@"SigBridgeSubnetsList") class];
                break;
            case SigOpCode_SubnetBridgeGet:
                messageType = [NSClassFromString(@"SigSubnetBridgeGet") class];
                break;
            case SigOpCode_SubnetBridgeSet:
                messageType = [NSClassFromString(@"SigSubnetBridgeSet") class];
                break;
            case SigOpCode_SubnetBridgeStatus:
                messageType = [NSClassFromString(@"SigSubnetBridgeStatus") class];
                break;
                
                // Direct Forwarding
            case SigOpCode_DirectControlGet:
                messageType = [NSClassFromString(@"SigDirectControlGet") class];
                break;
            case SigOpCode_DirectControlSet:
                messageType = [NSClassFromString(@"SigDirectControlSet") class];
                break;
            case SigOpCode_DirectControlStatus:
                messageType = [NSClassFromString(@"SigDirectControlStatus") class];
                break;
            case SigOpCode_ForwardingTableAdd:
                messageType = [NSClassFromString(@"SigForwardingTableAdd") class];
                break;
            case SigOpCode_ForwardingTableDelete:
                messageType = [NSClassFromString(@"SigForwardingTableDelete") class];
                break;
            case SigOpCode_ForwardingTableStatus:
                messageType = [NSClassFromString(@"SigForwardingTableStatus") class];
                break;
            default:
                break;
        }
    }
    
    return messageType;
}

/// Get SigOpCodeType with hight byte opcode
/// @param opCode opcode of mesh message.
- (SigOpCodeType)getOpCodeTypeWithOpcode:(UInt8)opCode {
    if (opCode == SigOpCodeType_RFU) {
        return SigOpCodeType_RFU;
    }
    // 1-octet Opcodes.
    if ((opCode & 0x80) == 0) {
        return SigOpCodeType_sig1;
    }
    // 2-octet Opcodes.
    if ((opCode & 0x40) == 0) {
        return SigOpCodeType_sig2;
    }
    return SigOpCodeType_vendor3;
}

/// Get UInt8 OpCode Type
/// @param opCode 1-octet Opcodes,eg:0x00; 2-octet Opcodes,eg:0x8201; 3-octet Opcodes,eg:0xC11102
- (SigOpCodeType)getOpCodeTypeWithUInt32Opcode:(UInt32)opCode {
    if (opCode < 0x7F) {//1-octet Opcodes,Opcode Format:0xxxxxxx (excluding 01111111),eg:0x00
        return SigOpCodeType_sig1;
    }else if (opCode >= 0x8000 && opCode <= 0xBFFF) {//2-octet Opcodes,Opcode Format:10xxxxxx xxxxxxxx,eg:0x8201
        return SigOpCodeType_sig2;
    }else{//3-octet Opcodes,Opcode Format:11xxxxxx zzzzzzzz,eg:0xC11102
        return SigOpCodeType_vendor3;
    }
    return SigOpCodeType_RFU;
}

/// Get OpCode Data
/// @param opCode 1-octet Opcodes,eg:0x00; 2-octet Opcodes,eg:0x8201; 3-octet Opcodes,eg:0xC11102
- (NSData *)getOpCodeDataWithUInt32Opcode:(UInt32)opCode {
    UInt8 tem = 0;
    SigOpCodeType type = [self getOpCodeTypeWithUInt32Opcode:opCode];
    if (type == SigOpCodeType_sig1) {
        tem = (UInt8)opCode&0xFF;
        NSMutableData *data1 = [NSMutableData dataWithBytes:&tem length:1];
        return data1;
    } else if (type == SigOpCodeType_sig2) {
        tem = (UInt8)(0x80 | ((opCode >> 8) & 0x3F));
        NSMutableData *data1 = [NSMutableData dataWithBytes:&tem length:1];
        tem = (UInt8)(opCode & 0xFF);
        [data1 appendData:[NSData dataWithBytes:&tem length:1]];
        return data1;
    } else if (type == SigOpCodeType_vendor3) {
        tem = (UInt8)(0xC0 | ((opCode >> 16) & 0x3F));
        NSMutableData *data1 = [NSMutableData dataWithBytes:&tem length:1];
        tem = (UInt8)((opCode >> 8) & 0xFF);
        [data1 appendData:[NSData dataWithBytes:&tem length:1]];
        tem = (UInt8)(opCode & 0xFF);
        [data1 appendData:[NSData dataWithBytes:&tem length:1]];
        return data1;
    }
    return [NSData data];
}

/// opcode is encryption with deviceKey.
/// @param opCode 1-octet Opcodes,eg:0x00; 2-octet Opcodes,eg:0x8201; 3-octet Opcodes,eg:0xC11102
- (BOOL)isDeviceKeyOpCode:(UInt32)opCode {
    BOOL isDeviceKey = NO;
    //0x00~0x06 || 0x8000~0x805F
    if ((opCode >= SigOpCode_configAppKeyAdd && opCode <= SigOpCode_configHeartbeatPublicationStatus) || (opCode >= SigOpCode_configAppKeyDelete && opCode <= SigOpCode_remoteProvisioningPDUReport) || (opCode >= SigOpCode_SubnetBridgeGet && opCode <= SigOpCode_BridgingTableSizeStatus)) {
        isDeviceKey = YES;
    }
    return isDeviceKey;
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

/// Get Detail Of SigFirmwareUpdatePhaseType
/// @param phaseType The SigFirmwareUpdatePhaseType value.
- (NSString *)getDetailOfSigFirmwareUpdatePhaseType:(SigFirmwareUpdatePhaseType)phaseType {
    NSString *tem = @"";
    switch (phaseType) {
        case SigFirmwareUpdatePhaseType_idle:
            tem = @"idle";
            break;
        case SigFirmwareUpdatePhaseType_transferError:
            tem = @"transfer error";
            break;
        case SigFirmwareUpdatePhaseType_transferActive:
            tem = @"transfer active";
            break;
        case SigFirmwareUpdatePhaseType_verifyingUpdate:
            tem = @"verifying update";
            break;
        case SigFirmwareUpdatePhaseType_verificationSuccess:
            tem = @"verification success";
            break;
        case SigFirmwareUpdatePhaseType_verificationFailed:
            tem = @"verification failed";
            break;
        case SigFirmwareUpdatePhaseType_applyingUpdate:
            tem = @"applying update";
            break;
        case SigFirmwareUpdatePhaseType_prohibited:
            tem = @"prohibited";
            break;
        default:
            break;
    }
    return tem;
}

/// Get Detail Of SigFirmwareUpdateServerAndClientModelStatusType
/// @param statusType The SigFirmwareUpdateServerAndClientModelStatusType value.
- (NSString *)getDetailOfSigFirmwareUpdateServerAndClientModelStatusType:(SigFirmwareUpdateServerAndClientModelStatusType)statusType {
    NSString *tem = @"";
    switch (statusType) {
        case SigFirmwareUpdateServerAndClientModelStatusType_success:
            tem = @"success";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_insufficientResources:
            tem = @"insufficient resources";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_wrongPhase:
            tem = @"wrong phase";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_internalError:
            tem = @"internal error";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_wrongFirmwareIndex:
            tem = @"wrong firmware index";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_metadataCheckFailed:
            tem = @"metadata check failed";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_temporarilyUnavailable:
            tem = @"temporarily unavailable";
            break;
        case SigFirmwareUpdateServerAndClientModelStatusType_BLOBTransferBusy:
            tem = @"BLOB transfer busy";
            break;
        default:
            break;
    }
    return tem;
}

/// Get Detail Of SigBLOBTransferStatusType
/// @param statusType The SigBLOBTransferStatusType value.
- (NSString *)getDetailOfSigBLOBTransferStatusType:(SigBLOBTransferStatusType)statusType {
    NSString *tem = @"prohibited";
    switch (statusType) {
        case SigBLOBTransferStatusType_success:
            tem = @"success";
            break;
        case SigBLOBTransferStatusType_invalidBlockNumber:
            tem = @"invalid block number";
            break;
        case SigBLOBTransferStatusType_invalidBlockSize:
            tem = @"invalid block size";
            break;
        case SigBLOBTransferStatusType_invalidChunkSize:
            tem = @"invalid chunk size";
            break;
        case SigBLOBTransferStatusType_wrongPhase:
            tem = @"wrong phase";
            break;
        case SigBLOBTransferStatusType_invalidParameter:
            tem = @"invalid parameter";
            break;
        case SigBLOBTransferStatusType_wrongBLOBID:
            tem = @"wrong BLOBID";
            break;
        case SigBLOBTransferStatusType_BLOBTooLarge:
            tem = @"BLOB too large";
            break;
        case SigBLOBTransferStatusType_unsupportedTransferMode:
            tem = @"unsupported transfer mode";
            break;
        case SigBLOBTransferStatusType_internalError:
            tem = @"internal error";
            break;
        case SigBLOBTransferStatusType_informationUnavailable:
            tem = @"information unavailable";
            break;
        default:
            break;
    }
    return tem;
}

/// Get Detail Of SigDirectionsFieldValues
/// @param directions The SigDirectionsFieldValues value.
- (NSString *)getDetailOfSigDirectionsFieldValues:(SigDirectionsFieldValues)directions {
    NSString *tem = @"prohibited";
    switch (directions) {
        case SigDirectionsFieldValues_prohibited:
            tem = @"Prohibited";
            break;
        case SigDirectionsFieldValues_unidirectional:
            tem = @"Unidirectional";
            break;
        case SigDirectionsFieldValues_bidirectional:
            tem = @"Directional";
            break;
        default:
            break;
    }
    return tem;
}

/// Get Detail Of SigNodeFeaturesState
/// @param state The SigNodeFeaturesState value.
- (NSString *)getDetailOfSigNodeFeaturesState:(SigNodeFeaturesState)state {
    NSString *tem = @"not fount";
    switch (state) {
        case SigNodeFeaturesState_notEnabled:
            tem = @"disable";
            break;
        case SigNodeFeaturesState_enabled:
            tem = @"enable";
            break;
        case SigNodeFeaturesState_notSupported:
            tem = @"not support";
            break;
        default:
            break;
    }
    return tem;
}

/// Get Detail Of KeyRefreshPhase
/// @param phase The KeyRefreshPhase value.
- (NSString *)getDetailOfKeyRefreshPhase:(KeyRefreshPhase)phase {
    NSString *tem = @"unknown";
    switch (phase) {
        case normalOperation:
            tem = @"normalOperation";
            break;
        case distributingKeys:
            tem = @"distributingKeys";
            break;
        case finalizing:
            tem = @"finalizing";
            break;
        default:
            break;
    }
    return tem;
}

/// Get Detail Of SigNodeIdentityState
/// @param state The SigNodeIdentityState value.
- (NSString *)getDetailOfSigNodeIdentityState:(SigNodeIdentityState)state {
    NSString *tem = @"not fount";
    switch (state) {
        case SigNodeIdentityState_notEnabled:
            tem = @"stopped";
            break;
        case SigNodeIdentityState_enabled:
            tem = @"running";
            break;
        case SigNodeIdentityState_notSupported:
            tem = @"not supported";
            break;
        default:
            break;
    }
    return tem;
}

@end
