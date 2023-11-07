/********************************************************************************************************
 * @file     SigModel.m 
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

#import "SigModel.h"
#import "CBUUID+Hex.h"
#import "OpenSSLHelper.h"
#import "SDKLibCommand+directForwarding.h"

/// base model of SigLib.
@implementation SigModel

@end


@implementation ModelIDModel

/// Initialize ModelIDModel object.
/// @param modelGroup Group name of model ID.
/// @param modelName the Model ID name.
/// @param sigModelID The value of model ID.
/// @returns return `nil` when initialize ModelIDModel object fail.
-(instancetype)initWithModelGroup:(NSString *)modelGroup modelName:(NSString *)modelName sigModelID:(NSInteger)sigModelID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _modelGroup = modelGroup;
        _modelName = modelName;
        _sigModelID = sigModelID;
    }
    return self;
}

@end


@implementation ModelIDs

- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        //Generic
        ModelIDModel *model1 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericOnOffServer_Describe sigModelID:kSigModel_GenericOnOffServer_ID];
        ModelIDModel *model2 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericOnOffClient_Describe sigModelID:kSigModel_GenericOnOffClient_ID];
        ModelIDModel *model3 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericLevelServer_Describe sigModelID:kSigModel_GenericLevelServer_ID];
        ModelIDModel *model4 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericLevelClient_Describe sigModelID:kSigModel_GenericLevelClient_ID];
        ModelIDModel *model5 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericDefaultTransitionTimeServer_Describe sigModelID:kSigModel_GenericDefaultTransitionTimeServer_ID];
        ModelIDModel *model6 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericDefaultTransitionTimeClient_Describe sigModelID:kSigModel_GenericDefaultTransitionTimeClient_ID];
        ModelIDModel *model7 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPowerOnOffServer_Describe sigModelID:kSigModel_GenericPowerOnOffServer_ID];
        ModelIDModel *model8 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPowerOnOffSetupServer_Describe sigModelID:kSigModel_GenericPowerOnOffSetupServer_ID];
        ModelIDModel *model9 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPowerOnOffClient_Describe sigModelID:kSigModel_GenericPowerOnOffClient_ID];
        ModelIDModel *model10 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPowerLevelServer_Describe sigModelID:kSigModel_GenericPowerLevelServer_ID];
        ModelIDModel *model11 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPowerLevelSetupServer_Describe sigModelID:kSigModel_GenericPowerLevelSetupServer_ID];
        ModelIDModel *model12 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPowerLevelClient_Describe sigModelID:kSigModel_GenericPowerLevelClient_ID];
        ModelIDModel *model13 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericBatteryServer_Describe sigModelID:kSigModel_GenericBatteryServer_ID];
        ModelIDModel *model14 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericBatteryClient_Describe sigModelID:kSigModel_GenericBatteryClient_ID];
        ModelIDModel *model15 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericLocationServer_Describe sigModelID:kSigModel_GenericLocationServer_ID];
        ModelIDModel *model16 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericLocationSetupServer_Describe sigModelID:kSigModel_GenericLocationSetupServer_ID];
        ModelIDModel *model17 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericLocationClient_Describe sigModelID:kSigModel_GenericLocationClient_ID];
        ModelIDModel *model18 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericAdminPropertyServer_Describe sigModelID:kSigModel_GenericAdminPropertyServer_ID];
        ModelIDModel *model19 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericManufacturerPropertyServer_Describe sigModelID:kSigModel_GenericManufacturerPropertyServer_ID];
        ModelIDModel *model20 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericUserPropertyServer_Describe sigModelID:kSigModel_GenericUserPropertyServer_ID];
        ModelIDModel *model21 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericClientPropertyServer_Describe sigModelID:kSigModel_GenericClientPropertyServer_ID];
        ModelIDModel *model22 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Generic_Describe modelName:kSigModel_GenericPropertyClient_Describe sigModelID:kSigModel_GenericPropertyClient_ID];
        //Sensors
        ModelIDModel *model23 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Sensors_Describe modelName:kSigModel_SensorServer_Describe sigModelID:kSigModel_SensorServer_ID];
        ModelIDModel *model24 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Sensors_Describe modelName:kSigModel_SensorSetupServer_Describe sigModelID:kSigModel_SensorSetupServer_ID];
        ModelIDModel *model25 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Sensors_Describe modelName:kSigModel_SensorClient_Describe sigModelID:kSigModel_SensorClient_ID];
        //Time and Scenes
        ModelIDModel *model26 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_TimeServer_Describe sigModelID:kSigModel_TimeServer_ID];
        ModelIDModel *model27 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_TimeSetupServer_Describe sigModelID:kSigModel_TimeSetupServer_ID];
        ModelIDModel *model28 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_TimeClient_Describe sigModelID:kSigModel_TimeClient_ID];
        ModelIDModel *model29 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_SceneServer_Describe sigModelID:kSigModel_SceneServer_ID];
        ModelIDModel *model30 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_SceneSetupServer_Describe sigModelID:kSigModel_SceneSetupServer_ID];
        ModelIDModel *model31 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_SceneClient_Describe sigModelID:kSigModel_SceneClient_ID];
        ModelIDModel *model32 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_SchedulerServer_Describe sigModelID:kSigModel_SchedulerServer_ID];
        ModelIDModel *model33 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_SchedulerSetupServer_Describe sigModelID:kSigModel_SchedulerSetupServer_ID];
        ModelIDModel *model34 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_TimeServer_Describe modelName:kSigModel_SchedulerClient_Describe sigModelID:kSigModel_SchedulerClient_ID];
        //Lighting
        ModelIDModel *model35 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightLightnessServer_Describe sigModelID:kSigModel_LightLightnessServer_ID];
        ModelIDModel *model36 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightLightnessSetupServer_Describe sigModelID:kSigModel_LightLightnessSetupServer_ID];
        ModelIDModel *model37 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightLightnessClient_Describe sigModelID:kSigModel_LightLightnessClient_ID];
        ModelIDModel *model38 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightCTLServer_Describe sigModelID:kSigModel_LightCTLServer_ID];
        ModelIDModel *model39 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightCTLSetupServer_Describe sigModelID:kSigModel_LightCTLSetupServer_ID];
        ModelIDModel *model40 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightCTLClient_Describe sigModelID:kSigModel_LightCTLClient_ID];
        ModelIDModel *model41 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightCTLTemperatureServer_Describe sigModelID:kSigModel_LightCTLTemperatureServer_ID];
        ModelIDModel *model42 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightHSLServer_Describe sigModelID:kSigModel_LightHSLServer_ID];
        ModelIDModel *model43 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightHSLSetupServer_Describe sigModelID:kSigModel_LightHSLSetupServer_ID];
        ModelIDModel *model44 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightHSLClient_Describe sigModelID:kSigModel_LightHSLClient_ID];
        ModelIDModel *model45 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightHSLHueServer_Describe sigModelID:kSigModel_LightHSLHueServer_ID];
        ModelIDModel *model46 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightHSLSaturationServer_Describe sigModelID:kSigModel_LightHSLSaturationServer_ID];
        ModelIDModel *model47 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightxyLServer_Describe sigModelID:kSigModel_LightxyLServer_ID];
        ModelIDModel *model48 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightxyLSetupServer_Describe sigModelID:kSigModel_LightxyLSetupServer_ID];
        ModelIDModel *model49 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightxyLClient_Describe sigModelID:kSigModel_LightxyLClient_ID];
        ModelIDModel *model50 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightLCServer_Describe sigModelID:kSigModel_LightLCServer_ID];
        ModelIDModel *model51 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightLCSetupServer_Describe sigModelID:kSigModel_LightLCSetupServer_ID];
        ModelIDModel *model52 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Lighting_Describe modelName:kSigModel_LightLCClient_Describe sigModelID:kSigModel_LightLCClient_ID];

        ModelIDModel *model53 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Configuration_Describe modelName:kSigModel_ConfigurationServer_Describe sigModelID:kSigModel_ConfigurationServer_ID];
        ModelIDModel *model54 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Configuration_Describe modelName:kSigModel_ConfigurationClient_Describe sigModelID:kSigModel_ConfigurationClient_ID];
        ModelIDModel *model55 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Health_Describe modelName:kSigModel_HealthServer_Describe sigModelID:kSigModel_HealthServer_ID];
        ModelIDModel *model56 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_Health_Describe modelName:kSigModel_HealthClient_Describe sigModelID:kSigModel_HealthClient_ID];
        ModelIDModel *model57 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_RemoteProvision_Describe modelName:kSigModel_RemoteProvisionServer_Describe sigModelID:kSigModel_RemoteProvisionServer_ID];
        ModelIDModel *model58 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_RemoteProvision_Describe modelName:kSigModel_RemoteProvisionClient_Describe sigModelID:kSigModel_RemoteProvisionClient_ID];
        ModelIDModel *model59 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_DirectedForwardingConfiguration_Describe modelName:kSigModel_DirectedForwardingConfigurationServer_Describe sigModelID:kSigModel_DirectedForwardingConfigurationServer_ID];
        ModelIDModel *model60 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_DirectedForwardingConfiguration_Describe modelName:kSigModel_DirectedForwardingConfigurationClient_Describe sigModelID:kSigModel_DirectedForwardingConfigurationClient_ID];
        ModelIDModel *model61 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_BridgeConfiguration_Describe modelName:kSigModel_BridgeConfigurationServer_Describe sigModelID:kSigModel_BridgeConfigurationServer_ID];
        ModelIDModel *model62 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_BridgeConfiguration_Describe modelName:kSigModel_BridgeConfigurationClient_Describe sigModelID:kSigModel_BridgeConfigurationClient_ID];
        ModelIDModel *model63 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_MeshPrivateBeacon_Describe modelName:kSigModel_MeshPrivateBeaconServer_Describe sigModelID:kSigModel_MeshPrivateBeaconServer_ID];
        ModelIDModel *model64 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_MeshPrivateBeacon_Describe modelName:kSigModel_MeshPrivateBeaconClient_Describe sigModelID:kSigModel_MeshPrivateBeaconClient_ID];
        ModelIDModel *model65 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_OnDemandPrivateProxy_Describe modelName:kSigModel_OnDemandPrivateProxyServer_Describe sigModelID:kSigModel_OnDemandPrivateProxyServer_ID];
        ModelIDModel *model66 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_OnDemandPrivateProxy_Describe modelName:kSigModel_OnDemandPrivateProxyClient_Describe sigModelID:kSigModel_OnDemandPrivateProxyClient_ID];
        ModelIDModel *model67 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_SARConfiguration_Describe modelName:kSigModel_SARConfigurationServer_Describe sigModelID:kSigModel_SARConfigurationServer_ID];
        ModelIDModel *model68 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_SARConfiguration_Describe modelName:kSigModel_SARConfigurationClient_Describe sigModelID:kSigModel_SARConfigurationClient_ID];
        ModelIDModel *model69 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_OpcodesAggregator_Describe modelName:kSigModel_OpcodesAggregatorServer_Describe sigModelID:kSigModel_OpcodesAggregatorServer_ID];
        ModelIDModel *model70 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_OpcodesAggregator_Describe modelName:kSigModel_OpcodesAggregatorClient_Describe sigModelID:kSigModel_OpcodesAggregatorClient_ID];
        ModelIDModel *model71 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_LargeCompositionData_Describe modelName:kSigModel_LargeCompositionDataServer_Describe sigModelID:kSigModel_LargeCompositionDataServer_ID];
        ModelIDModel *model72 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_LargeCompositionData_Describe modelName:kSigModel_LargeCompositionDataClient_Describe sigModelID:kSigModel_LargeCompositionDataClient_ID];
        ModelIDModel *model73 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_SolicitationPDURPLConfiguration_Describe modelName:kSigModel_SolicitationPDURPLConfigurationServer_Describe sigModelID:kSigModel_SolicitationPDURPLConfigurationServer_ID];
        ModelIDModel *model74 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_SolicitationPDURPLConfiguration_Describe modelName:kSigModel_SolicitationPDURPLConfigurationClient_Describe sigModelID:kSigModel_SolicitationPDURPLConfigurationClient_ID];
        ModelIDModel *model75 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_BLOBTransfer_Describe modelName:kSigModel_BLOBTransferServer_Describe sigModelID:kSigModel_BLOBTransferServer_ID];
        ModelIDModel *model76 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_BLOBTransfer_Describe modelName:kSigModel_BLOBTransferClient_Describe sigModelID:kSigModel_BLOBTransferClient_ID];
        ModelIDModel *model77 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_FirmwareUpdate_Describe modelName:kSigModel_FirmwareUpdateServer_Describe sigModelID:kSigModel_FirmwareUpdateServer_ID];
        ModelIDModel *model78 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_FirmwareUpdate_Describe modelName:kSigModel_FirmwareUpdateClient_Describe sigModelID:kSigModel_FirmwareUpdateClient_ID];
        ModelIDModel *model79 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_FirmwareDistribution_Describe modelName:kSigModel_FirmwareDistributionServer_Describe sigModelID:kSigModel_FirmwareDistributionServer_ID];
        ModelIDModel *model80 = [[ModelIDModel alloc] initWithModelGroup:kSigModelGroup_FirmwareDistribution_Describe modelName:kSigModel_FirmwareDistributionClient_Describe sigModelID:kSigModel_FirmwareDistributionClient_ID];
        

        _modelIDs = @[model1,model2,model3,model4,model5,model6,model7,model8,model9,model10,model11,model12,model13,model14,model15,model16,model17,model18,model19,model20,model21,model22,model23,model24,model25,model26,model27,model28,model29,model30,model31,model32,model33,model34,model35,model36,model37,model38,model39,model40,model41,model42,model43,model44,model45,model46,model47,model48,model49,model50,model51,model52,model53,model54,model55,model56,model57,model58,model59,model60,model61,model62,model63,model64,model65,model66,model67,model68,model69,model70,model71,model72,model73,model74,model75,model76,model77,model78,model79,model80];
        //        _defaultModelIDs = @[model1,model3,model4,model35,model36,model38,model39,model41,model50,model51];//默认选中10个
        _defaultModelIDs = _modelIDs;//默认选中所有
        
    }
    return self;
}

@end


/// Default group information
@implementation Groups

- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _groupCount = 8;
        _name1 = @"Living room";
        _name2 = @"Kitchen";
        _name3 = @"Master bedroom";
        _name4 = @"Secondary bedroom";
        _name5 = @"Balcony";
        _name6 = @"Bathroom";
        _name7 = @"Hallway";
        _name8 = @"others";
        _names = @[_name1,_name2,_name3,_name4,_name5,_name6,_name7,_name8];
    }
    return self;
}

@end


/*
 5.1.4 Scheduler
 5.1.4.1 Scheduler overview
 Scheduler provides a means of autonomous change of states of a
 device based on the notion of UTC time and the ISO 8601 calendar
 [15] and a register of defined time points with associated state-
 changing actions. For example, a lamp may automatically turn off
 every day at 2AM, or a coffee machine may make coffee at 6:30AM.
 The scheduler is based on a register (see Section 5.1.4.2) that
 is capable of storing up to sixteen scheduled entries, each
 containing a starting point in local time, that may include values
 that represent multiple values, and an associated action to perform.
 */
/// The Schedule Register state is a 16-entry, zero-based, indexed array of 76-bit values
/// formatted as Scheduled Time. Each entry represents a state-changing event. Time and
/// date fields represent local time.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.139),
/// 5.1.4.2 Schedule Register.
/// @note   The Year, Month, Day, Hour, Minute, and Second fields represent local time (i.e.,
/// after the TAI-UTC Delta and Time Zone Offset have been applied). The fields have the
/// meaning defined in ISO 8601 [15] (which replicates the "Gregorian" calendar in common
/// use). Some of these values can either represent an exact value or a range of values when
/// the scheduled action is performed.
/// @note   The minute and second field values have an enumerated value that represents ‘at
/// a random’ value. This scheduled action shall be triggered once within the corresponding
/// hour or minute.
/// @note   5.1.4.2.1 Schedule Register behavior
/// When the current time is known to the device, which is indicated by a value of the TAI
/// Seconds state (see Section 5.1.1.1) that is greater than zero, the entries in the Schedule
/// Register are tracked. If there is a match between the Time state (see Section 5.1.1) and
/// an entry in the Schedule Register, a corresponding Action indicated by the values in the
/// Action and Scene fields shall be executed.
/// @note   Note:A recommended implementation of the Scheduler should calculate the value
/// of the TAI Seconds (see Section 5.1.1.1) of the next scheduled event and put it in a queue
/// of scheduled events sorted by time. Every second, the first event in the queue is compared
/// with the value of the Time state (see Section 5.1.1). The first event is executed if it is less than
/// or equal to the Time state and then removed from the queue. After execution, the Repeat Flag
/// shall be checked, and the next occurrence of the scheduled event is calculated and put in the
/// queue.
@implementation SchedulerModel

/// get dictionary from SchedulerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSchedulerModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"schedulerID"] = @(self.schedulerID);
    dict[@"sceneId"] = @(_sceneId);
    dict[@"schedulerData"] = [SigHelper.share getUint64String:_schedulerData];
    return dict;
}

/// Set dictionary to SchedulerModel object.
/// @param dictionary SchedulerModel dictionary object.
- (void)setDictionaryToSchedulerModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"schedulerID"]) {
        self.schedulerID = (UInt8)[dictionary[@"schedulerID"] intValue];
    }
    if ([allKeys containsObject:@"sceneId"]) {
        _sceneId = (UInt64)[dictionary[@"sceneId"] integerValue];
    }
    if ([allKeys containsObject:@"schedulerData"]) {
        _schedulerData = [LibTools uint64From16String:dictionary[@"schedulerData"]];
    }
}

/// Initialize SchedulerModel object.
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _schedulerData = 0;
        _sceneId = 0;
        //set scheduler default time
        [self setSchedulerID:0];
        [self setYear:0x64];
        [self setMonth:0xFFF];
        [self setDay:0];
        [self setHour:0x18];
        [self setMinute:0x3C];
        [self setSecond:0x3C];
        [self setWeek:0x7F];
    }
    return self;
}

/// Determine if the data of two SchedulerModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SchedulerModel class]]) {
        //schedulerID is the unique identifier of SchedulerModel.
        return self.schedulerID == ((SchedulerModel *)object).schedulerID;
    } else {
        //Two schedulerModel object is different.
        return NO;
    }
}

/// Serialization SchedulerModel Object
- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    SchedulerModel *model = [[[self class] alloc] init];
    model.schedulerID = self.schedulerID;
    model.schedulerData = self.schedulerData;
    model.sceneId = self.sceneId;
    return model;
}

/// get schedulerID from _schedulerData.
- (UInt64)schedulerID {
    return (_schedulerData) & 0xF;
}

/// set schedulerID to _schedulerData.
/// @param schedulerID schedulerID of SchedulerModel Object.
- (void)setSchedulerID:(UInt64)schedulerID {
    UInt64 tem = 0xF;
    _schedulerData = (_schedulerData & (~(tem))) | ((schedulerID & tem));
}

/// get year value from _schedulerData.
- (UInt64)year {
    return (_schedulerData >> 4) & 0x7F;
}

/// set value of year to _schedulerData.
/// @param year year value of SchedulerModel Object.
- (void)setYear:(UInt64)year {
    UInt64 tem = 0x7F;
    _schedulerData = (_schedulerData & (~(tem<<4))) | ((year & tem) << 4);
    
}

/// get month value from _schedulerData.
- (UInt64)month {
    return (_schedulerData >> 11) & 0xFFF;
}

/// set value of month to _schedulerData.
/// @param month month value of SchedulerModel Object.
- (void)setMonth:(UInt64)month {
    UInt64 tem = 0xFFF;
    _schedulerData = (_schedulerData & (~(tem<<11))) | ((month & tem) << 11);
}

/// get day value from _schedulerData.
- (UInt64)day {
    return (_schedulerData >> 23) & 0x1F;
}

/// set value of day to _schedulerData.
/// @param day day value of SchedulerModel Object.
- (void)setDay:(UInt64)day {
    UInt64 tem = 0x1F;
    _schedulerData = (_schedulerData & (~(tem<<23))) | ((day & tem) << 23);
}

/// get hour value from _schedulerData.
- (UInt64)hour {
    return (_schedulerData >> 28) & 0x1F;
}

/// set value of hour to _schedulerData.
/// @param hour hour value of SchedulerModel Object.
- (void)setHour:(UInt64)hour {
    UInt64 tem = 0x1F;
    _schedulerData = (_schedulerData & (~(tem<<28))) | ((hour & tem) << 28);
}

/// get minute value from _schedulerData.
- (UInt64)minute {
    return (_schedulerData >> 33) & 0x3F;
}

/// set value of minute to _schedulerData.
/// @param minute minute value of SchedulerModel Object.
- (void)setMinute:(UInt64)minute {
    UInt64 tem = 0x3F;
    _schedulerData = (_schedulerData & (~(tem<<33))) | ((minute & tem) << 33);
}

/// get second value from _schedulerData.
- (UInt64)second {
    return (_schedulerData >> 39) & 0x3F;
}

/// set value of second to _schedulerData.
/// @param second second value of SchedulerModel Object.
- (void)setSecond:(UInt64)second {
    UInt64 tem = 0x3F;
    _schedulerData = (_schedulerData & (~(tem<<39))) | ((second & tem) << 39);
}

/// get week value from _schedulerData.
- (UInt64)week {
    return (_schedulerData >> 45) & 0x7F;
}

/// set value of week to _schedulerData.
/// @param week week value of SchedulerModel Object.
- (void)setWeek:(UInt64)week {
    UInt64 tem = 0x7F;
    _schedulerData = (_schedulerData & (~(tem<<45))) | ((week & tem) << 45);
}

/// get action value from _schedulerData.
- (SchedulerType)action {
    return (_schedulerData >> 52) & 0xF;
}

/// set value of action to _schedulerData.
/// @param action action value of SchedulerModel Object.
- (void)setAction:(SchedulerType)action {
    UInt64 tem = 0xF;
    _schedulerData = (_schedulerData & (~(tem<<52))) | ((action & tem) << 52);
}

/// get transitionTime value from _schedulerData.
- (UInt64)transitionTime {
    return (_schedulerData >> 56) & 0xFF;
}

/// set value of transitionTime to _schedulerData.
/// @param transitionTime transitionTime value of SchedulerModel Object.
- (void)setTransitionTime:(UInt64)transitionTime {
    UInt64 tem = 0xFF;
    _schedulerData = (_schedulerData & (~tem<<56)) | ((transitionTime & tem) << 56);
}

@end


/// The model for caching Bluetooth scan callbacks, with uuid (peripheral. identifier. UUIDString) as the unique identifier.
@implementation SigScanRspModel

- (instancetype)initWithCoder:(NSCoder *)coder
{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _advertisementDataServiceData = [coder decodeObjectForKey:kSigScanRspModel_advertisementDataServiceData_key];
        _uuid = [coder decodeObjectForKey:kSigScanRspModel_uuid_key];
        _address = [coder decodeIntegerForKey:kSigScanRspModel_address_key];
        _macAddress = [coder decodeObjectForKey:kSigScanRspModel_mac_key];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_advertisementDataServiceData forKey:kSigScanRspModel_advertisementDataServiceData_key];
    [coder encodeObject:_uuid forKey:kSigScanRspModel_uuid_key];
    [coder encodeInteger:_address forKey:kSigScanRspModel_address_key];
    [coder encodeObject:_macAddress forKey:kSigScanRspModel_mac_key];
}

/*蓝牙scan_rsp的结构体，iOS实际可以获取到mac_adr及之后的数据。
 typedef struct{
 u8 len;
 u8 type;            // 0xFF: manufacture data
 u8 mac_adr[6];
 u16 adr_primary;
 u8 rsv_telink[10];  // not for user
 u8 rsv_user[11];
 }mesh_scan_rsp_t;
 
 iOS12 实际数据：
 固件v3.2.1及之前版本的广播包(kCBAdvDataManufacturerData=mac+address)
 1827 Printing description of advertisementData:
 {
 kCBAdvDataIsConnectable = 1;
 kCBAdvDataManufacturerData = <00f5b973 cdab0075 00000000 00000000 00000102 03040506 0708090a 0b>;
 kCBAdvDataServiceData =     {
 1827 = <11020700 32376900 070000f5 b973cdab 0000>;
 };
 kCBAdvDataServiceUUIDs =     (
 1827
 );
 }
 
 
 1828 Printing description of advertisementData:
 {
 kCBAdvDataIsConnectable = 1;
 kCBAdvDataManufacturerData = <00f5b973 cdab0101 00000000 00000000 00000102 03040506 0708090a 0b>;
 kCBAdvDataServiceData =     {
 1828 = <01fcd9c0 59b9593b 48dc6ab3 aece7547 be>;
 };
 kCBAdvDataServiceUUIDs =     (
 1828
 );
 }
 
 固件v3.2.2及之后版本的广播包(kCBAdvDataManufacturerData=CID+mac+address)
  1827 Printing description of advertisementData={
      kCBAdvDataIsConnectable = 1;
      kCBAdvDataManufacturerData = {length = 29, bytes = 0x110228c1 d738c1a4 28410000 00000000 ... 04050607 08090a0b };
      kCBAdvDataRxPrimaryPHY = 0;
      kCBAdvDataRxSecondaryPHY = 0;
      kCBAdvDataServiceData =     {
          1827 = {length = 18, bytes = 0x446d6f6f098414309e8328c1d738c1a40000};
      };
      kCBAdvDataServiceUUIDs =     (
          1827
      );
      kCBAdvDataTimestamp = "617765262.924383";
  }
 
 1828 Printing description of advertisementData:={
     kCBAdvDataIsConnectable = 1;
     kCBAdvDataManufacturerData = {length = 29, bytes = 0x110228c1 d738c1a4 06000000 00000000 ... 04050607 08090a0b };
     kCBAdvDataRxPrimaryPHY = 0;
     kCBAdvDataRxSecondaryPHY = 0;
     kCBAdvDataServiceData =     {
         1828 = {length = 9, bytes = 0x007e7a9e258796c955};
     };
     kCBAdvDataServiceUUIDs =     (
         1828
     );
     kCBAdvDataTimestamp = "617766058.0931309";
 }

 */
/// Initialize SigScanRspModel object.
/// @param peripheral the CBPeripheral object of BLE device.
/// @param advertisementData Complete data of Bluetooth broadcast package.
/// @returns return `nil` when initialize SigScanRspModel object fail.
- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _peripheral = peripheral;
        _advertisementData = advertisementData;
        _uuid = peripheral.identifier.UUIDString;
        _advName = advertisementData[CBAdvertisementDataLocalNameKey];
        if ([advertisementData.allKeys containsObject:CBAdvertisementDataManufacturerDataKey]) {
            NSData *allData = advertisementData[CBAdvertisementDataManufacturerDataKey];
            if (allData) {
                Byte *byte = (Byte *)allData.bytes;
                UInt16 tem16 = 0;
                if (allData.length >= 2) {
                    memcpy(&tem16, byte, 2);
                    _CID = tem16;
                }
                if (allData.length >= 8) {
                    _macAddress = [LibTools convertDataToHexStr:[LibTools turnOverData:[allData subdataWithRange:NSMakeRange(2, 6)]]];
                }
                if (allData.length >= 10) {
                    memcpy(&tem16, byte+8, 2);
                    _address = tem16;
                }
            }
        }

        if ([advertisementData.allKeys containsObject:CBAdvertisementDataServiceDataKey]) {
            NSData *advDataServiceData = [(NSDictionary *)advertisementData[CBAdvertisementDataServiceDataKey] allValues].firstObject;
            if (advDataServiceData) {
                if ([advertisementData.allKeys containsObject:CBAdvertisementDataServiceUUIDsKey]) {
                    NSArray *suuids = advertisementData[CBAdvertisementDataServiceUUIDsKey];
                    if (!suuids || suuids.count == 0) {
                        NSCAssert(YES, @"library can't determine whether the node can be provisioned");
                        return self;
                    }
                    NSString *suuidString = ((CBUUID *)suuids.firstObject).UUIDString;
                    BOOL provisionAble = [suuidString  isEqualToString: kPBGATTService] || [suuidString  isEqualToString:[LibTools change16BitsUUIDTO128Bits:kPBGATTService]];
                    _provisioned = !provisionAble;
                    if (provisionAble) {
                        // 未入网
                        _address = 0;//this address of unprovision node is invalid.
                        //且还需要支持fast bind的功能才有CID、PID。普通固件不广播该参数。
                        if (advDataServiceData.length >= 2) {
                            _CID = [LibTools uint16FromBytes:[advDataServiceData subdataWithRange:NSMakeRange(0, 2)]];
                        }
                        if (advDataServiceData.length >= 4) {
                            _PID = [LibTools uint16FromBytes:[advDataServiceData subdataWithRange:NSMakeRange(2, 2)]];
                        }
                        if (advDataServiceData.length >= 16) {
                            _advUuid = [LibTools convertDataToHexStr:[advDataServiceData subdataWithRange:NSMakeRange(0, 16)]];
                            if (_macAddress == nil) {
                                //当kCBAdvDataManufacturerData未返回时，暂时使用这个位置的数据作为MacAddress。（只有Telink的支持RP的设备才会在这里包含MacAddress）
                                NSData *macAddress1 = [LibTools turnOverData:[advDataServiceData subdataWithRange:NSMakeRange(10, 6)]];
                                NSData *uuid1 = [LibTools calcUuidByMac:macAddress1];
                                if ([uuid1 isEqualToData:[advDataServiceData subdataWithRange:NSMakeRange(0, 16)]]) {
                                    _macAddress = [LibTools convertDataToHexStr:macAddress1];
                                }
                            }
                        }
                        if (advDataServiceData.length >= 18) {
                            struct OobInformation oob = {};
                            oob.value = [LibTools uint16FromBytes:[advDataServiceData subdataWithRange:NSMakeRange(16, 2)]];
                            _advOobInformation = oob;
                        }
                        _advertisementDataServiceData = [NSData dataWithData:advDataServiceData];
                    } else {
                        // 已入网
                        _advertisementDataServiceData = [NSData dataWithData:advDataServiceData];
//                        UInt8 advType = [LibTools uint8From16String:[LibTools convertDataToHexStr:[LibTools turnOverData:[advDataServiceData subdataWithRange:NSMakeRange(0, 1)]]]];
//                        if (advType == SigIdentificationType_networkID) {
//                            if (advDataServiceData.length >= 9) {
//                                _networkIDData = [advDataServiceData subdataWithRange:NSMakeRange(1, 8)];
//                            }
//                        }else if (advType == SigIdentificationType_nodeIdentity) {
//                            if (advDataServiceData.length >= 17) {
//                                _nodeIdentityData = [advDataServiceData subdataWithRange:NSMakeRange(1, 16)];
//                            }
//                        }
                    }
                }
            }
        }
    }
    return self;
}

/// return the identification type of provisioned node.
- (SigIdentificationType)getIdentificationType {
    SigIdentificationType advType = 0xFF;
    if (_advertisementDataServiceData && _advertisementDataServiceData.length) {
        advType = [LibTools uint8From16String:[LibTools convertDataToHexStr:[LibTools turnOverData:[_advertisementDataServiceData subdataWithRange:NSMakeRange(0, 1)]]]];
    }
    return advType;
}

/// Determine if the data of two SigScanRspModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigScanRspModel class]]) {
        SigScanRspModel *tem = (SigScanRspModel *)object;
        //uuid is the unique identifier of SigScanRspModel.
        return [_uuid isEqualToString:tem.uuid];
    } else {
        //Two SigScanRspModel object is different.
        return NO;
    }
}

@end


/// The model for caching Remote scan callbacks, with reportNodeUUID as the unique identifier.
@implementation SigRemoteScanRspModel

/// Initialize SigRemoteScanRspModel object.
/// @param parameters Complete data of SigRemoteScanRspModel object.
/// @returns return `nil` when initialize SigRemoteScanRspModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length <19) {
            return nil;
        }
        SInt8 stem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&stem8, dataByte, 1);
        _RSSI = stem8;
        if (parameters.length >= 17) {
            _reportNodeUUID = [parameters subdataWithRange:NSMakeRange(1, 16)];
            if (parameters.length >= 19) {
                UInt16 tem16 = 0;
                memcpy(&tem16, dataByte+17, 2);
                struct OobInformation oob = {};
                oob.value = tem16;
                _oob = oob;
            }
        }
    }
    return self;
}

/// get MacAddress from _reportNodeUUID.
- (NSString *)macAddress {
    NSString *tem = @"";
    if (_reportNodeUUID && _reportNodeUUID.length >= 6) {
        tem = [LibTools convertDataToHexStr:[LibTools turnOverData:[_reportNodeUUID subdataWithRange:NSMakeRange(_reportNodeUUID.length - 6, 6)]]];
    }
    return tem;
}

/// Determine if the data of two SigRemoteScanRspModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigRemoteScanRspModel class]]) {
        //reportNodeUUID is the unique identifier of SigRemoteScanRspModel.
        return [_reportNodeUUID isEqualToData:[(SigRemoteScanRspModel *)object reportNodeUUID]];
    } else {
        //Two SigRemoteScanRspModel object is different.
        return NO;
    }
}

/// change the log string of SigRemoteScanRspModel.
- (NSString *)description {
    return [NSString stringWithFormat:@"unicastAddress=%@,uuid=%@,macAddress=%@,RSSI=%@,OOB=%@",@(self.reportNodeAddress),_reportNodeUUID,self.macAddress,@(_RSSI),@(_oob.value)];
}

@end


/// the model use for add device VC
@implementation AddDeviceModel

/// Initialize AddDeviceModel object.
/// @param scanRemoteModel SigRemoteScanRspModel object.
/// @returns return `nil` when initialize AddDeviceModel object fail.
- (instancetype)initWithRemoteScanRspModel:(SigRemoteScanRspModel *)scanRemoteModel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _state = AddDeviceModelStateScanned;
        SigScanRspModel *model = [[SigScanRspModel alloc] init];
        model.address = scanRemoteModel.reportNodeAddress;
        model.uuid = [LibTools convertDataToHexStr:scanRemoteModel.reportNodeUUID];
        model.macAddress = scanRemoteModel.macAddress;
        model.advOobInformation = scanRemoteModel.oob;
        _scanRspModel = model;
    }
    return self;
}

/// Determine if the data of two AddDeviceModel is the same
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[AddDeviceModel class]]) {
        if (_scanRspModel.advUuid == nil && _scanRspModel.macAddress != nil) {
            //macAddress is the unique identifier of AddDeviceModel when provision is fastProvision.
            return [_scanRspModel.macAddress isEqualToString:[(AddDeviceModel *)object scanRspModel].macAddress];
        } else {
            //advUuid is the unique identifier of AddDeviceModel.
            return [_scanRspModel.advUuid isEqualToString:[(AddDeviceModel *)object scanRspModel].advUuid];
        }
    } else {
        //Two AddDeviceModel object is different.
        return NO;
    }
}

@end


/// the action model of scene
@implementation ActionModel

/// Initialize ActionModel object.
/// @param node SigNodeModel object.
/// @returns return `nil` when initialize ActionModel object fail.
- (instancetype)initWithNode:(SigNodeModel *)node{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = node.address;
        _state = node.state;
        _trueBrightness = node.trueBrightness;
        _trueTemperature = node.trueTemperature;
    }
    return self;
}

/// Initialize ActionModel object.
/// @param node SigNodeModel object.
/// @param elementAddress elementAddress of node.
/// @returns return `nil` when initialize ActionModel object fail.
- (instancetype)initWithNode:(SigNodeModel *)node elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = elementAddress;
        _state = node.state;
        _trueBrightness = node.trueBrightness;
        _trueTemperature = node.trueTemperature;
    }
    return self;
}

/// Compare two actions to see if they are the same.
/// @param action ActionModel object
/// @returns YES means same, NO means different.
- (BOOL)isSameActionWithAction:(ActionModel *)action{
    if (self.state == action.state && self.trueBrightness == action.trueBrightness && self.trueTemperature == action.trueTemperature) {
        return YES;
    }else{
        return NO;
    }
}

/// get dictionary from ActionModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfActionModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"address"] = @(_address);
    dict[@"state"] = @(_state);
    dict[@"trueBrightness"] = @(_trueBrightness);
    dict[@"trueTemperature"] = @(_trueTemperature);
    return dict;
}

/// Set dictionary to ActionModel object.
/// @param dictionary ActionModel dictionary object.
- (void)setDictionaryToActionModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"address"]) {
        _address = (UInt16)[dictionary[@"address"] intValue];
    }
    if ([allKeys containsObject:@"state"]) {
        _state = (DeviceState)[dictionary[@"state"] intValue];
    }
    if ([allKeys containsObject:@"trueBrightness"]) {
        _trueBrightness = (UInt8)[dictionary[@"trueBrightness"] intValue];
    }
    if ([allKeys containsObject:@"trueTemperature"]) {
        _trueTemperature = (UInt8)[dictionary[@"trueTemperature"] intValue];
    }
}

/// Determine if the data of two ActionModel is the same.
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[ActionModel class]]) {
        //address is the unique identifier of ActionModel.
        return _address == ((ActionModel *)object).address;
    } else {
        //Two SigRemoteScanRspModel object is different.
        return NO;
    }
}

@end


/// The DeviceTypeModel caching CID、PID and defaultCompositionData, use for fast provision and default bound.
@implementation DeviceTypeModel

/// Initialize DeviceTypeModel object.
/// @param cid company ID
/// @param pid product ID
/// @param cpsData composition data
/// @returns return `nil` when initialize DeviceTypeModel object fail.
- (instancetype)initWithCID:(UInt16)cid PID:(UInt16)pid compositionData:(nullable NSData *)cpsData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _CID = cid;
        _PID = pid;
        SigPage0 *compositionData = [[SigPage0 alloc] init];
        UInt8 temPage = 0;
        NSData *pageData = [NSData dataWithBytes:&temPage length:1];
        NSMutableData *mData = [NSMutableData dataWithData:pageData];
        if (cpsData && cpsData.length > 0) {
            [mData appendData:cpsData];
            compositionData = [[SigPage0 alloc] initWithParameters:mData];
        } else {
            NSData *data = [self getDefaultCompositionDataWithPid:pid];
            if (data && data.length > 0) {
                [mData appendData:data];
                compositionData = [[SigPage0 alloc] initWithParameters:mData];
            }
        }
        _defaultCompositionData = compositionData;
    }
    return self;
}

/// Get Default Composition Data With Pid.
/// @param pid product ID.
- (NSData *)getDefaultCompositionDataWithPid:(UInt16)pid {
    NSData *data = nil;
    if (pid == SigNodePID_Panel) {
        //set default compositionData of panel
        data = [NSData dataWithBytes:PanelByte length:sizeof(PanelByte)];
    }else if (pid == SigNodePID_HSL) {
        //set default compositionData of HSL
        data = [NSData dataWithBytes:HSLByte length:sizeof(HSLByte)];
    }else if (pid == SigNodePID_CT) {
        //set default compositionData of CT
        data = [NSData dataWithBytes:CTByte length:sizeof(CTByte)];
    }else if (pid == SigNodePID_LPN) {
        //set default compositionData of LPN
        data = [NSData dataWithBytes:LPNByte length:sizeof(LPNByte)];
    }
    return data;
}

/// Set composition Data to DeviceTypeModel.
/// @param compositionData composition data
- (void)setCompositionData:(NSData *)compositionData {
    UInt8 temPage = 0;
    NSData *pageData = [NSData dataWithBytes:&temPage length:1];
    NSMutableData *mData = [NSMutableData dataWithData:pageData];
    [mData appendData:compositionData];
    _defaultCompositionData = [[SigPage0 alloc] initWithParameters:mData];
}

@end


/// The config parameters of add device.
@implementation SigAddConfigModel

/// Initialize SigAddConfigModel object.
/// - Parameters:
///   - peripheral: The CBPeripheral object of BLE device.
///   - networkKey: The mesh network key.
///   - netkeyIndex: The mesh network index.
///   - appKey: The mesh application key.
///   - appkeyIndex: The mesh application index.
///   - staticOOBData: The static OOB data for static OOB provision.
///   - keyBindType: The type of keybind, KeyBindType_Normal or KeyBindType_Fast.
///   - productID: The product ID of unprovision node.
///   - cpsData: The composition data of unprovision node.
/// - returns:
/// return `nil` when initialize SigAddConfigModel object fail.
- (instancetype)initWithCBPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appKey:(NSData *)appKey appkeyIndex:(UInt16)appkeyIndex staticOOBData:(NSData * _Nullable)staticOOBData keyBindType:(KeyBindType)keyBindType productID:(UInt16)productID cpsData:(NSData * _Nullable)cpsData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _peripheral = peripheral;
        _networkKey = networkKey;
        _netkeyIndex = netkeyIndex;
        _appKey = appKey;
        _appkeyIndex = appkeyIndex;
        _staticOOBData = staticOOBData;
        _keyBindType = keyBindType;
        _productID = productID;
        _cpsData = cpsData;
    }
    return self;
}

@end


/// The ini command for developer.
/// sig model struct of firmware: mesh_bulk_cmd_par_t.
/// vendor model struct of firmware: mesh_vendor_par_ini_t.
/// It is sig model command when vendorId is 0, and It is vendor model command when vendorId isn't 0.
/// sig model config: netkeyIndex, appkeyIndex, retryCount, responseMax, address, opcode, commandData.
@implementation IniCommandModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self setDefaultEncryptKey];
    }
    return self;
}

/// Initialize sig model IniCommandModel object.
/// - Parameters:
///   - netkeyIndex: The mesh network index.
///   - appkeyIndex: The mesh application index.
///   - retryCount: The retry count of command, only for reliable command.
///   - responseMax: The response max count of command, only for reliable command.
///   - address: The destination address.
///   - opcode: The opcode of command.
/// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203.
/// VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
///   - commandData: The Hex command data.
///   eg: SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}.
///   eg: SigGenericOnOffSet: commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}
/// - returns:
/// return `nil` when initialize sig model IniCommandModel object fail.
- (instancetype)initSigModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt16)opcode commandData:(NSData *)commandData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _netkeyIndex = netkeyIndex;
        _appkeyIndex = appkeyIndex;
        _retryCount = retryCount;
        _responseMax = responseMax;
        _address = address;
        _opcode = opcode;
        _commandData = commandData;
        [self setDefaultEncryptKey];
    }
    return self;
}

/// Initialize vendor model IniCommandModel object.
/// - Parameters:
///   - netkeyIndex: The mesh network index.
///   - appkeyIndex: The mesh application index.
///   - retryCount: The retry count of command, only for reliable command.
///   - responseMax: The response max count of command, only for reliable command.
///   - address: The destination address.
///   - opcode: The opcode of command.
/// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203.
/// VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
///   - vendorId: The vendor ID of command.
/// 0 means sig model command, other means vendor model command.
///   - responseOpcode: The response opcode of command, only for reliable command.
/// eg: The response opcode of VendorOnOffSet is 0xC4.
///   - needTid: The position of tid in the ini command.
///   when needTid is NO, parameter tidPosition and tid is invalid.
///   - tidPosition: The position of tid in the ini command.
///   - tid: The value of tid in the ini command.
///   - commandData: The Hex command data.
///   eg: SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}.
///   eg: SigGenericOnOffSet: commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}
/// - returns:
/// return `nil` when initialize vendor model IniCommandModel object fail.
- (instancetype)initVendorModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt8)opcode vendorId:(UInt16)vendorId responseOpcode:(UInt8)responseOpcode needTid:(BOOL)needTid tidPosition:(UInt8)tidPosition tid:(UInt8)tid commandData:(nullable NSData *)commandData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _netkeyIndex = netkeyIndex;
        _appkeyIndex = appkeyIndex;
        _retryCount = retryCount;
        _responseMax = responseMax;
        _address = address;
        _opcode = opcode;
        _vendorId = vendorId;
        _responseOpcode = responseOpcode;
        _needTid = needTid;
        _tidPosition = tidPosition;
        _tid = tid;
        _commandData = commandData;
        [self setDefaultEncryptKey];
    }
    return self;
}

/// Initialize IniCommandModel object with complete Hex iniCommand data.
/// - Parameter iniCommandData: complete Hex iniCommand data.
/// eg: "a3ff000000000200ffffc21102c4020100"
/// - returns:
/// return `nil` when initialize IniCommandModel object fail.
- (instancetype)initWithIniCommandData:(NSData *)iniCommandData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (iniCommandData.length < 11) {
            return nil;
        }
        [self setDefaultEncryptKey];
        Byte *pu = (Byte *)[iniCommandData bytes];
        UInt16 tem16 = 0;
        UInt8 tem8 = 0;
        memcpy(&tem16, pu+2, 2);
        _netkeyIndex = tem16;
        memcpy(&tem16, pu+4, 2);
        _appkeyIndex = tem16;
        memcpy(&tem8, pu+6, 1);
        _retryCount = tem8;
        memcpy(&tem8, pu+7, 1);
        _responseMax = tem8;
        memcpy(&tem16, pu+8, 2);
        _address = tem16;
        _opcode = [self getOpcodeFromIniCommandData:iniCommandData opcodeIndex:10];
        UInt32 size_op = [self getOpcodeLengthFromIniCommandData:iniCommandData opcodeIndex:10];
        if (size_op > 2) {
            //vendor model
            _opcode = _opcode & 0xFF;
            memcpy(&tem16, pu+11, 2);
            _vendorId = tem16;
            memcpy(&tem8, pu+13, 1);
            _responseOpcode = tem8;
            memcpy(&tem8, pu+14, 1);
            _needTid = tem8 != 0;
            _tidPosition = tem8;
            if (tem8 != 0) {
                if (iniCommandData.length >= 15+tem8) {
                    _commandData = [iniCommandData subdataWithRange:NSMakeRange(15, iniCommandData.length-15)];
                    memcpy(&tem8, pu+15+tem8-1, 1);
                    _tid = tem8;
                }
            }else{
                if (iniCommandData.length > 15) {
                    _commandData = [iniCommandData subdataWithRange:NSMakeRange(15, iniCommandData.length-15)];
                }
            }
        } else {
            //sig model
            if (iniCommandData.length > 10+size_op) {
                _commandData = [iniCommandData subdataWithRange:NSMakeRange(10+size_op, iniCommandData.length-(10+size_op))];
            }
            if (size_op == 1) {
                _isEncryptByDeviceKey = [SigHelper.share isDeviceKeyOpCode:_opcode];
            } else if (size_op == 2) {
                if (CFSwapInt16BigToHost(_opcode) == SigOpCode_OpcodesAggregatorSequence) {
                    SigOpcodesAggregatorSequence *sequence = [[SigOpcodesAggregatorSequence alloc] initWithParameters:_commandData];
                    if (sequence.items && sequence.items.count > 0) {
                        _isEncryptByDeviceKey = [SigHelper.share isDeviceKeyOpCode:sequence.items.firstObject.getSigMeshMessage.opCode];
                    }
                } else {
                    _isEncryptByDeviceKey = [SigHelper.share isDeviceKeyOpCode:CFSwapInt16BigToHost(_opcode)];
                }
            }
        }
    }
    return self;
}

/// Get Opcode Length
/// - Parameters:
///   - iniCommandData: iniCommand Hex Data
///   - opcodeIndex: opcode Index
/// - returns:
/// return Opcode Length. length is SigOpCodeType_sig1、SigOpCodeType_sig2 or SigOpCodeType_vendor3.
- (UInt8)getOpcodeLengthFromIniCommandData:(NSData *)iniCommandData opcodeIndex:(int)opcodeIndex {
    Byte *pu = (Byte *)[iniCommandData bytes];
    UInt8 temp = 0;
    memcpy(&temp, pu+opcodeIndex, 1);
    UInt8 op_type = [SigHelper.share getOpCodeTypeWithOpcode:temp];
    return op_type;
}

/// Get opcode
/// - Parameters:
///   - iniCommandData: iniCommand Hex Data
///   - opcodeIndex: opcode Index
/// - returns:
/// return Opcode value.
- (int)getOpcodeFromIniCommandData:(NSData *)iniCommandData opcodeIndex:(int)opcodeIndex {
    int opcode = 0;
    Byte *pu = (Byte *)[iniCommandData bytes];
    UInt8 op_type = [self getOpcodeLengthFromIniCommandData:iniCommandData opcodeIndex:opcodeIndex];
    memcpy(&opcode, pu+opcodeIndex, op_type);
    return opcode;
}

/// Set Default Encrypt Key.
- (void)setDefaultEncryptKey {
    _isEncryptByDeviceKey = NO;
    _curNetkey = SigMeshLib.share.dataSource.curNetkeyModel;
    _curAppkey = SigMeshLib.share.dataSource.curAppkeyModel;
    _curIvIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
}

@end


/// The Receivers List field shall contain entries from the Distribution Receivers List state.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.98),
/// 8.4.2.5 Firmware Distribution Receivers List.
@implementation SigUpdatingNodeEntryModel

/// Initialize SigUpdatingNodeEntryModel object.
/// - Parameters:
///   - address: least significant bits of the unicast address of the Updating node. Size is 15 bits.
/// The Address field shall indicate the address of the Updating node.
///   - retrievedUpdatePhase: Retrieved Update Phase state of the Updating node. Size is 4 bits.
/// The Retrieved Update Phase field shall indicate the Update Phase state of the Firmware
/// Update Server identified by the Address field. The values for this field are defined in Table 8.8.
///   - updateStatus: Status of the last operation with the Firmware Update Server. Size is 3 bits.
/// The Update Status field shall indicate the status of the last operation between the Firmware
/// Update Client and the Firmware Update Server identified by the Address field. The values
/// for this field are defined in Table 8.23.
///   - transferStatus: Status of the last operation with the BLOB Transfer Server. Size is 4 bits.
/// The Transfer Status field shall indicate the status of the last operation between the BLOB
/// Transfer Client and the BLOB Transfer Server identified by the Address field. The values
/// for this field are defined in Table 7.17.
///   - transferProgress: Progress of the BLOB transfer in 2 percent increments. Size is 6 bits.
/// The Transfer Progress field shall indicate the progress of the firmware image transfer.
/// The value is multiplied by 2 to calculate the percentage of the firmware image that has
/// been delivered and confirmed. The range of allowed values for the Transfer Progress
/// field is 0x0 to 0x32. The values 0x33 to 0x3F are prohibited.
///   - updateFirmwareImageIndex: Index of the firmware image on the Firmware Information List state that is being
/// updated. Size is 8 bits.
/// The Update Firmware Image Index field shall identify the index of the firmware
/// image on the Firmware Information List state of the Updating node that is being updated.
/// - returns:
/// return `nil` when initialize SigUpdatingNodeEntryModel object fail.
- (instancetype)initWithAddress:(UInt16)address retrievedUpdatePhase:(SigFirmwareUpdatePhaseType)retrievedUpdatePhase updateStatus:(SigFirmwareUpdateServerAndClientModelStatusType)updateStatus transferStatus:(SigBLOBTransferStatusType)transferStatus transferProgress:(UInt8)transferProgress updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = address;
        _retrievedUpdatePhase = retrievedUpdatePhase;
        _updateStatus = updateStatus;
        _transferStatus = transferStatus;
        _transferProgress = transferProgress;
        _updateFirmwareImageIndex = updateFirmwareImageIndex;
        UInt64 tem64 = 0;
        UInt64 address64 = address;
        UInt64 retrievedUpdatePhase64 = retrievedUpdatePhase;
        UInt64 updateStatus64 = updateStatus;
        UInt64 transferStatus64 = transferStatus;
        UInt64 transferProgress64 = transferProgress;
        UInt64 updateFirmwareImageIndex64 = updateFirmwareImageIndex;
        tem64 = (address64 & 0x7FFF) | ((retrievedUpdatePhase64 & 0xF) << 15) | ((updateStatus64 & 0x7) << 19) | ((transferStatus64 & 0xF) << 22) | ((transferProgress64 & 0x3F) << 26) | ((updateFirmwareImageIndex64 & 0xFF) << 32);
        self.parameters = [NSData dataWithBytes:&tem64 length:5];
    }
    return self;
}

/// Initialize SigUpdatingNodeEntryModel object with Hex data.
/// - Parameter parameters: Hex data of SigUpdatingNodeEntryModel object.
/// - returns:
/// return `nil` when initialize SigUpdatingNodeEntryModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length == 5) {
            UInt64 tem64 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem64, dataByte, 5);
            _address = tem64&0x7FFF;
            _retrievedUpdatePhase = (tem64 >> 15) & 0xF;
            _updateStatus = (tem64 >> 19) & 0x7;
            _transferStatus = (tem64 >> 22) & 0xF;
            _transferProgress = (tem64 >> 26) & 0x3F;
            _updateFirmwareImageIndex = (tem64 >> 32) & 0xFF;
            _parameters = [NSData dataWithData:parameters];
        }
    }
    return self;
}

/// Get SigUpdatingNodeEntryModel object describe string.
- (NSString *)getDetailString {
    NSString *tem = [NSString stringWithFormat:@"Address:(0x%X),Phase:(0x%X,\"%@\"),update:(0x%X,\"%@\"),transfer:(0x%X,\"%@\"),Progress:(%d%%),index:(0x%X)",_address,_retrievedUpdatePhase,[SigHelper.share getDetailOfSigFirmwareUpdatePhaseType:_retrievedUpdatePhase],_updateStatus,[SigHelper.share getDetailOfSigFirmwareUpdateServerAndClientModelStatusType:_updateStatus],_transferStatus,[SigHelper.share getDetailOfSigBLOBTransferStatusType:_transferStatus],_transferProgress,_updateFirmwareImageIndex];
    return tem;
}

/// Determine if the data of two SigUpdatingNodeEntryModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigUpdatingNodeEntryModel class]]) {
        //address is the unique identifier of SigUpdatingNodeEntryModel.
        return _address == ((SigUpdatingNodeEntryModel *)object).address;
    } else {
        //Two SigUpdatingNodeEntryModel object is different.
        return NO;
    }
}

@end


/// The Firmware Information Entry field shall identify the information for a
/// firmware subsystem on the node from the Firmware Information List state.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.92),
/// 8.4.1.2 Firmware Update Information Status.
@implementation SigFirmwareInformationEntryModel

/// Initialize SigFirmwareInformationEntryModel object with Hex data.
/// - Parameter parameters: Hex data of SigFirmwareInformationEntryModel object.
/// - returns:
/// return `nil` when initialize SigFirmwareInformationEntryModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 3) {
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _currentFirmwareIDLength = tem8;
            if (_currentFirmwareIDLength > 0 && parameters.length >= _currentFirmwareIDLength+1+1) {
                _currentFirmwareID = [parameters subdataWithRange:NSMakeRange(1, _currentFirmwareIDLength)];
                memcpy(&tem8, dataByte+1+_currentFirmwareIDLength, 1);
                _updateURILength = tem8;
                if (_updateURILength > 0) {
                    if (parameters.length >= 1+_currentFirmwareID.length+1+_updateURILength) {
                        _updateURL = [parameters subdataWithRange:NSMakeRange(1+_currentFirmwareIDLength+1, _updateURILength)];
                        _parameters = [parameters subdataWithRange:NSMakeRange(0, 1+_currentFirmwareIDLength+1+_updateURILength)];
                    } else {
                        return nil;
                    }
                } else {
                    _parameters = [parameters subdataWithRange:NSMakeRange(0, 1+_currentFirmwareIDLength+1)];
                }
            } else {
                return nil;
            }
        } else {
            return nil;
        }
    }
    return self;
}

/// Get FirmwareID String
- (NSString *)getFirmwareIDString {
    NSString *tem = @"";
    if (_currentFirmwareID && _currentFirmwareID.length > 0) {
        tem = [LibTools convertDataToHexStr:_currentFirmwareID];
    }
    return tem;
}

/// Get UpdateURI String
- (NSString *)getUpdateURIString {
    NSString *tem = @"";
    if (_updateURL && _updateURL.length > 0) {
        tem = [LibTools convertDataToHexStr:_updateURL];
    }
    return tem;
}

@end


/// The Receiver Entry field
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.1 Firmware Distribution Receivers Add.
/// Table 8.34: The format of the Receiver Entry field
@implementation SigReceiverEntryModel

/// Initialize SigReceiverEntryModel object.
/// - Parameters:
///     - address: The unicast address of the Updating node.
/// The Address field shall contain the unicast address of an Updating node.
///     - updateFirmwareImageIndex: The index of the firmware image in the Firmware Information List state to be updated.
/// The Update Firmware Image Index field shall identify the index of a firmware image in
/// the Firmware Information List state of the Updating node to be updated.
/// - returns:
/// return `nil` when initialize SigReceiverEntryModel object fail.
- (instancetype)initWithAddress:(UInt16)address updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = address;
        _updateFirmwareImageIndex = updateFirmwareImageIndex;
        UInt16 tem16 = _address;
        UInt8 tem8 = _updateFirmwareImageIndex;
        NSMutableData *mData = [NSMutableData data];
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
        [mData appendData:[NSData dataWithBytes:&tem8 length:1]];
        _parameters = mData;
    }
    return self;
}

/// Initialize SigReceiverEntryModel object with hex data.
/// - Parameters:
///     - parameters: The Hex data of SigReceiverEntryModel.
/// - returns:
/// return `nil` when initialize SigReceiverEntryModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 3) {
            _parameters = parameters;
            UInt8 tem8 = 0;
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _address = tem16;
            memcpy(&tem8, dataByte+2, 1);
            _updateFirmwareImageIndex = (BOOL)tem8;
        } else {
            return nil;
        }
    }
    return self;
}

@end


/// The Generic Default Transition Time state determines how long an element shall take to transition
/// from a present state to a new state (see Section 1.4.1.1). This is a 1-octet value that consists of two
/// fields: a 2-bit bit field representing the step resolution and a 6-bit bit field representing the number
/// of transition steps.
/// @note   This mechanism covers a wide range of times that may be required by different applications:
/// • For 100 millisecond step resolution, the range is 0 through 6.2 seconds.
/// • For 1 second step resolution, the range is 0 through 62 seconds.
/// • For 10 seconds step resolution, the range is 0 through 620 seconds (10.5 minutes).
/// • For 10 minutes step resolution, the range is 0 through 620 minutes (10.5 hours). The Generic Default
/// Transition Time is calculated using the following formula:
/// Generic Default Transition Time = Default Transition Step Resolution * Default Transition Number of Steps.
/// Default values for the Generic Default Transition Step Resolution and the Default Transition Number of
/// Steps are implementation-specific and are defined by a device manufacturer.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.29),
/// 3.1.3 Generic Default Transition Time.
/// Table 3.3: Generic Default Transition Time state format
@implementation SigTransitionTime

/// Initialize SigTransitionTime object.
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        // The value = 0x3F is unknown. The state cannot be set to this value,
        // but an element may report an unknown value if a transition is higher than 0x3E or not determined.
        _steps = 0x3F;
        _stepResolution = SigStepResolution_hundredsOfMilliseconds;
    }
    return self;
}

/// Initialize SigTransitionTime object.
/// @param rawValue The hex data of SigTransitionTime.
- (instancetype)initWithRawValue:(UInt8)rawValue {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _steps = rawValue & 0x3F;
        _stepResolution = (SigStepResolution)rawValue >> 6;
    }
    return self;
}

/// Initialize SigTransitionTime object.
/// - parameter steps: The number of Steps, 6-bit value.
/// The Default Transition Number of Steps field is a 6-bit value representing the number of transition steps.
/// The field values represent the states defined in the following table.
/// - parameter stepResolution: The resolution of the Default Transition Number of Steps field.
/// The Default Transition Step Resolution field is a 2-bit bit field that determines the resolution of the Generic
/// Default Transition Time state. The field values represent the states defined in the following table.
- (instancetype)initWithSetps:(UInt8)steps stepResolution:(SigStepResolution)stepResolution {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _steps = MIN(steps, 0x3E);
        _stepResolution = stepResolution;
    }
    return self;
}

/// The transition time in milliseconds.
- (int)milliseconds {
    return [SigHelper.share getMillisecondsOfTransitionIntervalWithPeriodSteps:_steps & 0x3F periodResolution:_stepResolution];
}

/// The transition time as `TimeInterval` in seconds.
- (NSTimeInterval)interval {
    return (NSTimeInterval)[self milliseconds]/1000.0;
}

/// The hex data of SigTransitionTime.
- (UInt8)rawValue {
    return (_steps & 0x3F) | (_stepResolution << 6);
}

/// Returns whether the transition time is known.
- (BOOL)isKnown {
    return _steps < 0x3F;
}

@end


@implementation SigMeshAddress

- (instancetype)initWithHex:(NSString *)hex {
    if (hex.length == 4) {
        return [self initWithAddress:[LibTools uint16From16String:hex]];
    }else{
        CBUUID *virtualLabel = [[CBUUID alloc] initWithHex:hex];
        if (virtualLabel) {
            return [self initWithVirtualLabel:virtualLabel];
        }
    }
    return nil;
}

/// Creates a Mesh Address. For virtual addresses use `initWithVirtualLabel:` instead.
/// @param address address
-(instancetype)initWithAddress:(UInt16)address {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = address;
        _virtualLabel = nil;
    }
    return self;
}

/// 3.4.2.3 Virtual address
/// Creates a Mesh Address based on the virtual label.
- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _virtualLabel = virtualLabel;
        
        // Calculate the 16-bit virtual address based on the 128-bit label.
        NSData *salt = [OpenSSLHelper.share calculateSalt:[@"vtad" dataUsingEncoding:kCFStringEncodingASCII]];
        NSData *hash = [OpenSSLHelper.share calculateCMAC:[LibTools nsstringToHex:[LibTools meshUUIDToUUID:_virtualLabel.UUIDString]] andKey:salt];
        UInt16 address = CFSwapInt16HostToBig([LibTools uint16FromBytes:[hash subdataWithRange:NSMakeRange(14, 2)]]);
        address |= 0x8000;
        address &= 0xBFFF;
        _address = address;
    }
    return self;
}

- (NSString *)getHex {
    if (_virtualLabel != nil) {
        return _virtualLabel.getHex;
    }
    return [NSString stringWithFormat:@"%04d",_address];
}

/// Determine if the data of two SigMeshAddress is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigMeshAddress class]]) {
        //address is the unique identifier of SigMeshAddress.
        return _address == ((SigMeshAddress *)object).address;
    } else {
        //Two SigMeshAddress object is different.
        return NO;
    }
}

@end


/// The object is used to describe the number of times a message is published and the interval between
/// retransmissions of the published message.
@implementation SigRetransmit

/// Initialize SigRetransmit object.
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _count = 0;
        _interval = 50;
    }
    return self;
}

/// Initialize SigRetransmit object.
/// - Parameters:
///   - publishRetransmitCount: Number of retransmissions for network messages. The value is in range from 0 to 7, where 0 means no retransmissions.
///   - intervalSteps: The interval (in milliseconds) between retransmissions (50...3200 with step 50).
/// - returns:
/// return `nil` when initialize SigRetransmit object fail.
- (instancetype)initWithPublishRetransmitCount:(UInt8)publishRetransmitCount intervalSteps:(UInt8)intervalSteps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _count = publishRetransmitCount;
        // Interval is in 50 ms steps.
        _interval = (UInt16)(intervalSteps + 1) * 50;// ms
    }
    return self;
}

/// Retransmission steps, from 0 to 31. Use `interval` to get the interval in ms.
- (UInt8)steps {
    return (UInt8)((_interval / 50) - 1);
}

@end


/// @note   - seeAlso: Mesh_v1.0.pdf (page.158), 4.3.2.16 Config Model Publication Set.
@implementation SigPublish

/// Creates an instance of Publish object.
/// @param stringDestination The publication address.
/// @param keyIndex The Application Key that will be used to send messages.
/// @param friendshipCredentialsFlag `True`, to use Friendship Security Material, `false` to use Master Security Material.
/// @param ttl Time to live. Use 0xFF to use Node's default TTL.
/// @param periodSteps Period steps, together with `periodResolution` are used to calculate period interval. Value can be in range 0...63. Value 0 disables periodic publishing.
/// @param periodResolution The period resolution, used to calculate interval. Use `._100_milliseconds` when periodic publishing is disabled.
/// @param retransmit The retransmission data. See `Retransmit` for details.
- (instancetype)initWithStringDestination:(NSString *)stringDestination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = stringDestination;
        _index = keyIndex;
        _credentials = friendshipCredentialsFlag ? 1 : 0;
        _ttl = ttl;
        _periodSteps = periodSteps;
        _periodResolution = periodResolution;
        _period = [SigHelper.share getMillisecondsOfTransitionIntervalWithPeriodSteps:_periodSteps periodResolution:_periodResolution];
        _retransmit = retransmit;
    }
    return self;
}

/// This initializer should be used to remove the publication from a Model.
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = @"0000";
        _index = 0;
        _credentials = 0;
        _ttl = 0;
        _periodSteps = 0;
        _periodResolution = SigStepResolution_hundredsOfMilliseconds;
        _period = 0;
        _retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:0 intervalSteps:0];
    }
    return self;
}

/// Creates an instance of Publish object.
/// @param destination The publication address.
/// @param keyIndex The Application Key that will be used to send messages.
/// @param friendshipCredentialsFlag `True`, to use Friendship Security Material, `false` to use Master Security Material.
/// @param ttl Time to live. Use 0xFF to use Node's default TTL.
/// @param periodSteps Period steps, together with `periodResolution` are used to calculate period interval. Value can be in range 0...63. Value 0 disables periodic publishing.
/// @param periodResolution The period resolution, used to calculate interval. Use `._100_milliseconds` when periodic publishing is disabled.
/// @param retransmit The retransmission data. See `Retransmit` for details.
- (instancetype)initWithDestination:(UInt16)destination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = [SigHelper.share getUint16String:destination];
        _index = keyIndex;
        _credentials = friendshipCredentialsFlag;
        _ttl = ttl;
        _periodSteps = periodSteps;
        _periodResolution = periodResolution;
        _period = [SigHelper.share getMillisecondsOfTransitionIntervalWithPeriodSteps:_periodSteps periodResolution:_periodResolution];
        _retransmit = retransmit;
    }
    return self;
}

/// Returns the interval between subsequent publications in seconds.
- (NSTimeInterval)publicationInterval {
    return (NSTimeInterval)_period / 1000.0;
}

/// Returns the interval between subsequent publications in seconds.
- (BOOL)isUsingMasterSecurityMaterial {
    return _credentials == 0;
}

/// Returns whether friendship security materials are used.
- (BOOL)isUsingFriendshipSecurityMaterial {
    return _credentials == 1;
}

/// Publication address for the model.
- (SigMeshAddress *)publicationAddress {
    return [[SigMeshAddress alloc] initWithHex:_address];
}

@end


/// 5.2.1.2 Time Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
@implementation SigTimeModel

/// Initialize SigTimeModel object.
/// - Parameters:
///   - TAISeconds: 40 bits, The current TAI time in seconds.
/// The TAI Seconds field identifies the TAI Seconds state (see Section 5.1.1.1).
/// The TAI Seconds state is the current TAI time in seconds after the epoch
/// 2000-01-01T00:00:00 TAI (1999-12-31T23:59:28 UTC).
/// For example, the value 0x20E5369D represents the 2017-06-27T15:30:37 TAI (15:30:00 UTC).
/// When an element does not know the time, a special value of 0x0000000000 is used.
///   - subSeconds: The sub-second time in units of 1/256th second.
/// The Subsecond field identifies the Subsecond state (see Section 5.1.1.2).
/// The Subsecond is a fractional part of the TAI time, in units of 1/256th seconds. An implementation
/// may increment this field by more than one unit (i.e. the mechanism it uses may have a larger
/// granularity) and/or by different amounts at each increment.
///   - uncertainty: The estimated uncertainty in 10 millisecond steps.
/// The Uncertainty field identifies the Time Uncertainty state (see Section 5.1.1.3).
/// The Uncertainty field represents the accumulated uncertainty of the Mesh Timestamp in 10- millisecond steps.
/// It includes the Uncertainty of the time source and the accumulated uncertainty resulting from the local clock drift
/// since the last update. Value 255 represents uncertainty of 2.55 seconds or more.
///   - timeAuthority: 1 bit, 0 = NO Time Authority, 1 = Time Authority.
/// The Time Authority field identifies the Time Authority state (see Section 5.1.1.4).
/// The Time Authority bit represents whether the element has a reliable source of TAI, such as a GPS receiver or an
/// NTP-synchronized clock. Time Authority = 1 is when the device itself has access to a reliable and trusted time
/// source, such as NTP or GPS, or has been given the TAI time by a Provisioner (using the Time Setup Server model
/// and a DevKey – see Section 5.3.2).
/// Note: Many time sources do not provide TAI directly, but it can be derived from UTC (if the current TAI-UTC Delta
/// is known) or from GPS time (which is always 19 seconds behind TAI).
///   - TAI_UTC_Delta: 15 bits, Current difference between TAI and UTC in seconds.
/// The TAI-UTC Delta field identifies the TAI-UTC Delta Current state (see Section 5.1.1.8).
/// The TAI-UTC Delta Current state represents the value: current_TAI minus current_UTC. For example, on 2017-01-19,
/// this value equals +37. The valid range is -255 through +32512 (i.e., 0x00FF represents a value of 0 and 0x7FFF
/// represents a value of 32512).
///   - timeZoneOffset: The local time zone offset in 15-minute increments.
/// The Time Zone Offset field shall be set to the Time Zone Offset Current state (see Section 5.1.1.5).
/// The Time Zone Offset Current field represents the current zone offset in 15-minute increments. The value is the number
/// of 15-minute increments from UTC. Positive numbers are eastwards. The state is a uint8 value representing the valid
/// range of -64 through +191 (i.e., 0x40 represents a value of 0 and 0xFF represents a value of 191).
/// Note: The offset representation with a range -64 through +191 (-16 through +47.75 hours) is in anticipation of proposals
/// dealing with the leap seconds issue by replacing the leap seconds with local zone changes, which means that by the
/// year 5000 the UK will be in zone +8 in the winter while New Zealand will be +21 in the summer with some places +22 or more.
/// - returns:
/// return `nil` when initialize SigTimeModel object fail.
- (instancetype)initWithTAISeconds:(UInt64)TAISeconds subSeconds:(UInt8)subSeconds uncertainty:(UInt8)uncertainty timeAuthority:(UInt8)timeAuthority TAI_UTC_Delta:(UInt16)TAI_UTC_Delta timeZoneOffset:(UInt8)timeZoneOffset {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _TAISeconds = TAISeconds;
        _subSeconds = subSeconds;
        _uncertainty = uncertainty;
        _timeAuthority = timeAuthority;
        _TAI_UTC_Delta = TAI_UTC_Delta;
        _timeZoneOffset = timeZoneOffset;
    }
    return self;
}

/// Initialize SigTimeModel object.
/// - Parameter parameters: The hex of SigTimeModel object.
/// - returns:
/// return `nil` when initialize SigTimeModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        UInt64 tem64 = 0;
        UInt16 tem16 = 0;
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        if (parameters.length >= 5) {
            memcpy(&tem64, dataByte, 5);
            _TAISeconds = tem64;
        }
        if (parameters.length >= 6) {
            memcpy(&tem8, dataByte+5, 1);
            _subSeconds = tem8;
        }
        if (parameters.length >= 7) {
            memcpy(&tem8, dataByte+6, 1);
            _uncertainty = tem8;
        }
        if (parameters.length >= 9) {
            memcpy(&tem16, dataByte+7, 2);
            _timeAuthority = (tem16 >> 15) & 0b1;
            _TAI_UTC_Delta = tem16 & 0x7FFF;
        }
        if (parameters.length >= 10) {
            memcpy(&tem8, dataByte+9, 1);
            _timeZoneOffset = tem8;
        }
    }
    return self;
}

/// Get hex data of SigTimeModel object.
- (NSData *)getTimeParameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem16 = 0;
    UInt64 tem64 = _TAISeconds & 0xFFFFFFFF;
    NSData *data = [NSData dataWithBytes:&tem64 length:8];
    [mData appendData:[data subdataWithRange:NSMakeRange(0, 5)]];
    tem8 = _subSeconds;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _uncertainty;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem16 = ((UInt16)_timeAuthority << 15) | _subSeconds;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _timeZoneOffset;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


/// The Sensor Descriptor state represents the attributes describing the sensor data.
/// This state does not change throughout the lifetime of an element.
/// 4.1.1 Sensor Descriptor
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.105)
@implementation SigSensorDescriptorModel

/// Initialize SigSensorDescriptorModel object.
/// - Parameter parameters: The hex of SigSensorDescriptorModel object.
/// - returns:
/// return `nil` when initialize SigSensorDescriptorModel object fail.
- (instancetype)initWithDescriptorParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters != nil && (parameters.length == 2 || parameters.length == 8)) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            if (parameters.length > 2) {
                memcpy(&tem16, dataByte+2, 2);
                _positiveTolerance = tem16;
                memcpy(&tem16, dataByte+4, 2);
                _negativeTolerance = tem16;
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+5, 1);
                _samplingFunction = tem8;
                memcpy(&tem8, dataByte+6, 1);
                _measurementPeriod = tem8;
                memcpy(&tem8, dataByte+7, 1);
                _updateInterval = tem8;
            }
        }else{
            return nil;
        }
    }
    return self;
}

/// Get Sensor Descriptor hex data.
- (NSData *)getDescriptorParameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _propertyID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_positiveTolerance != 0) {
        tem16 = _positiveTolerance;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _negativeTolerance;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _samplingFunction;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _measurementPeriod;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _updateInterval;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


@implementation SigEncryptedModel

/// Determine if the data of two SigEncryptedModel is the same
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigEncryptedModel class]]) {
        //advertisementDataServiceData is the unique identifier of SigEncryptedModel.
        return [_advertisementDataServiceData isEqualToData:((SigEncryptedModel *)object).advertisementDataServiceData];
    } else {
        //Two SigEncryptedModel object is different.
        return NO;
    }
}

@end


@implementation SigNetkeyDerivaties

- (SigNetkeyDerivaties *)initWithNetkeyData:(NSData *)key helper:(OpenSSLHelper *)helper {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        // Calculate Identity Key and Beacon Key.
        uint8_t byte[6] = {0x69, 0x64, 0x31, 0x32, 0x38, 0x01};//"id128" || 0x01
        NSData *P = [NSData dataWithBytes:&byte length:6];
        NSData *saltIK = [helper calculateSalt:[@"nkik" dataUsingEncoding:NSASCIIStringEncoding]];
        _identityKey = [helper calculateK1WithN:key salt:saltIK andP:P];
        NSData *saltBK = [helper calculateSalt:[@"nkbk" dataUsingEncoding:NSASCIIStringEncoding]];
        _beaconKey = [helper calculateK1WithN:key salt:saltBK andP:P];
        // Calculate Encryption Key and Privacy Key.
        byte[0] = 0x00;
        P = [NSData dataWithBytes:&byte length:1];
        NSData *hash = [helper calculateK2WithN:key andP:P];
        // NID was already generated in Network Key below and is ignored here.
        _encryptionKey = [hash subdataWithRange:NSMakeRange(1, 16)];
        _privacyKey = [hash subdataWithRange:NSMakeRange(17, 16)];
        // Calculate The directed security material Encryption Key and Privacy Key.
        byte[0] = 0x02;
        P = [NSData dataWithBytes:&byte length:1];
        hash = [helper calculateK2WithN:key andP:P];
        _directedSecurityEncryptionKey = [hash subdataWithRange:NSMakeRange(1, 16)];
        _directedSecurityPrivacyKey = [hash subdataWithRange:NSMakeRange(17, 16)];
    }
    return self;
}

@end

/// The network key object represents the state of the mesh network key
/// that is used for securing communication at the network layer [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.10),
/// 2.1.2 Network key object.
@implementation SigNetkeyModel

/// Get dictionary from SigNetkeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigNetkeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    dict[@"index"] = @(_index);
    if (_key) {
        dict[@"key"] = _key;
    }
    if (_oldKey && _phase != normalOperation) {
        dict[@"oldKey"] = _oldKey;
    }
    dict[@"phase"] = @(_phase);
    if (_minSecurity) {
        dict[@"minSecurity"] = _minSecurity;
    }
    if (_timestamp) {
        dict[@"timestamp"] = _timestamp;
    }
    return dict;
}

/// Set dictionary to SigNetkeyModel object.
/// @param dictionary SigNetkeyModel dictionary object.
- (void)setDictionaryToSigNetkeyModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"index"]) {
        _index = (UInt16)[dictionary[@"index"] intValue];
    }
    if ([allKeys containsObject:@"key"]) {
        _key = dictionary[@"key"];
    }
    if ([allKeys containsObject:@"oldKey"]) {
        _oldKey = dictionary[@"oldKey"];
    }
    if ([allKeys containsObject:@"phase"]) {
        _phase = (KeyRefreshPhase)[dictionary[@"phase"] intValue];
    }
    if ([allKeys containsObject:@"minSecurity"]) {
        _minSecurity = dictionary[@"minSecurity"];
    }
    if ([allKeys containsObject:@"timestamp"]) {
        _timestamp = dictionary[@"timestamp"];
    }
}

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        SigIvIndex *ivIndex = [[SigIvIndex alloc] initWithIndex:kDefaultIvIndex updateActive:NO];
        _ivIndex = ivIndex;
    }
    return self;
}

- (SigIvIndex *)ivIndex {
    if (_ivIndex) {
        return _ivIndex;
    }
    _ivIndex = [[SigIvIndex alloc] initWithIndex:[LibTools uint32From16String:SigMeshLib.share.dataSource.ivIndex] updateActive:NO];
    return _ivIndex;
}

- (UInt8)nid {
    if (!_nid && self.key && self.key.length > 0 && ![self.key isEqualToString:@"00000000000000000000000000000000"]) {
        // Calculate NID.
        UInt8 tem = 0;
        NSData *temData = [NSData dataWithBytes:&tem length:1];
        NSData *hash = [OpenSSLHelper.share calculateK2WithN:[LibTools nsstringToHex:_key] andP:temData];
        Byte *byte = (Byte *)hash.bytes;
        memcpy(&tem, byte, 1);
        _nid = tem & 0x7F;
    }
    return _nid;
}

- (UInt8)directedSecurityNid {
    if (!_directedSecurityNid && self.key && self.key.length > 0 && ![self.key isEqualToString:@"00000000000000000000000000000000"]) {
        // Calculate NID.
        UInt8 tem = 2;
        NSData *temData = [NSData dataWithBytes:&tem length:1];
        NSData *hash = [OpenSSLHelper.share calculateK2WithN:[LibTools nsstringToHex:_key] andP:temData];
        Byte *byte = (Byte *)hash.bytes;
        memcpy(&tem, byte, 1);
        _directedSecurityNid = tem & 0x7F;
    }
    return _directedSecurityNid;
}

- (UInt8)oldNid {
    if (!_oldNid && self.oldKey && self.oldKey.length > 0 && ![self.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        // Calculate NID.
        UInt8 tem = 0;
        NSData *temData = [NSData dataWithBytes:&tem length:1];
        NSData *hash = [OpenSSLHelper.share calculateK2WithN:[LibTools nsstringToHex:_oldKey] andP:temData];
        Byte *byte = (Byte *)hash.bytes;
        memcpy(&tem, byte, 1);
        _oldNid = tem & 0x7F;
    }
    return _oldNid;
}

- (UInt8)directedSecurityOldNid {
    if (!_directedSecurityOldNid && self.oldKey && self.oldKey.length > 0 && ![self.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        // Calculate NID.
        UInt8 tem = 2;
        NSData *temData = [NSData dataWithBytes:&tem length:1];
        NSData *hash = [OpenSSLHelper.share calculateK2WithN:[LibTools nsstringToHex:_oldKey] andP:temData];
        Byte *byte = (Byte *)hash.bytes;
        memcpy(&tem, byte, 1);
        _directedSecurityOldNid = tem & 0x7F;
    }
    return _directedSecurityOldNid;
}

- (SigNetkeyDerivaties *)keys {
    if (!_keys && self.key && self.key.length > 0 && ![self.key isEqualToString:@"00000000000000000000000000000000"]) {
        _keys = [[SigNetkeyDerivaties alloc] initWithNetkeyData:[LibTools nsstringToHex:self.key] helper:OpenSSLHelper.share];
    }
    return _keys;
}

- (SigNetkeyDerivaties *)oldKeys {
    if (!_oldKeys && self.oldKey && self.oldKey.length > 0 && ![self.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        _oldKeys = [[SigNetkeyDerivaties alloc] initWithNetkeyData:[LibTools nsstringToHex:self.oldKey] helper:OpenSSLHelper.share];
    }
    return _oldKeys;
}

- (NSData *)networkId {
    if (!_networkId && self.key && self.key.length > 0 && ![self.key isEqualToString:@"00000000000000000000000000000000"]) {
        _networkId = [OpenSSLHelper.share calculateK3WithN:[LibTools nsstringToHex:self.key]];
    }
    return _networkId;
}

- (NSData *)oldNetworkId {
    if (!_oldNetworkId && self.oldKey && self.oldKey.length > 0 && ![self.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        _oldNetworkId = [OpenSSLHelper.share calculateK3WithN:[LibTools nsstringToHex:self.oldKey]];
    }
    return _oldNetworkId;
}

- (SigNetkeyDerivaties *)transmitKeys {
    if (_phase == distributingKeys) {
        return self.oldKeys;
    }
    return self.keys;
}

/// Returns whether the Network Key is the Primary Network Key.
/// The Primary key is the one which Key Index is equal to 0.
///
/// A Primary Network Key may not be removed from the mesh network.
- (BOOL)isPrimary {
    return _index == 0;
}

/// Get detail string of SigNetkeyModel object.
- (NSString *)getNetKeyDetailString {
    return [NSString stringWithFormat:@"name:%@\tindex:0x%04lX\tkey:0x%@\toldKey:0x%@\tphase:%d\tminSecurity:%@\ttimestamp:%@",_name,(long)_index,_key,_oldKey,_phase,_minSecurity,_timestamp];
}

@end


@implementation SigIvIndex

- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _index = 0;
        _updateActive = NO;
    }
    return self;
}

- (instancetype)initWithIndex:(UInt32)index updateActive:(BOOL)updateActive {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _index = index;
        _updateActive = updateActive;
    }
    return self;
}

@end


/// A Provisioner is a mesh node that is capable of provisioning a device to the
/// mesh network as described in [1], and is represented by a provisioner object
/// in the Mesh Configuration Database.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.8),
/// 2.1.1 Provisioner object.
@implementation SigProvisionerModel

/// Get dictionary from SigProvisionerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigProvisionerModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_provisionerName) {
        dict[@"provisionerName"] = _provisionerName;
    }
    if (_UUID) {
        if (_UUID.length == 32) {
            dict[@"UUID"] = [LibTools UUIDToMeshUUID:_UUID];
        } else if (_UUID.length == 36) {
            dict[@"UUID"] = _UUID;
        }
    }
    if (_allocatedUnicastRange) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *allocatedUnicastRange = [NSMutableArray arrayWithArray:_allocatedUnicastRange];
        for (SigRangeModel *model in allocatedUnicastRange) {
            NSDictionary *rangeDict = [model getDictionaryOfSigRangeModel];
            [array addObject:rangeDict];
        }
        dict[@"allocatedUnicastRange"] = array;
    }
    if (_allocatedGroupRange) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *allocatedGroupRange = [NSMutableArray arrayWithArray:_allocatedGroupRange];
        for (SigRangeModel *model in allocatedGroupRange) {
            NSDictionary *rangeDict = [model getDictionaryOfSigRangeModel];
            [array addObject:rangeDict];
        }
        dict[@"allocatedGroupRange"] = array;
    }
    if (_allocatedSceneRange) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *allocatedSceneRange = [NSMutableArray arrayWithArray:_allocatedSceneRange];
        for (SigSceneRangeModel *model in allocatedSceneRange) {
            NSDictionary *sceneRangeDict = [model getDictionaryOfSigSceneRangeModel];
            [array addObject:sceneRangeDict];
        }
        dict[@"allocatedSceneRange"] = array;
    }
    return dict;
}

/// Set dictionary to SigProvisionerModel object.
/// @param dictionary SigProvisionerModel dictionary object.
- (void)setDictionaryToSigProvisionerModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"provisionerName"]) {
        _provisionerName = dictionary[@"provisionerName"];
    }
    if ([allKeys containsObject:@"UUID"]) {
        NSString *str = [dictionary[@"UUID"] uppercaseString];
        if (str.length == 32) {
            _UUID = [LibTools UUIDToMeshUUID:str];
        } else if (str.length == 36) {
            _UUID = str;
        }
    }
    if ([allKeys containsObject:@"allocatedUnicastRange"]) {
        NSMutableArray *allocatedUnicastRange = [NSMutableArray array];
        NSArray *array = dictionary[@"allocatedUnicastRange"];
        for (NSDictionary *rangeDict in array) {
            SigRangeModel *model = [[SigRangeModel alloc] init];
            [model setDictionaryToSigRangeModel:rangeDict];
            [allocatedUnicastRange addObject:model];
        }
        _allocatedUnicastRange = allocatedUnicastRange;
    }
    if ([allKeys containsObject:@"allocatedGroupRange"]) {
        NSMutableArray *allocatedGroupRange = [NSMutableArray array];
        NSArray *array = dictionary[@"allocatedGroupRange"];
        for (NSDictionary *rangeDict in array) {
            SigRangeModel *model = [[SigRangeModel alloc] init];
            [model setDictionaryToSigRangeModel:rangeDict];
            [allocatedGroupRange addObject:model];
        }
        _allocatedGroupRange = allocatedGroupRange;
    }
    if ([allKeys containsObject:@"allocatedSceneRange"]) {
        NSMutableArray *allocatedSceneRange = [NSMutableArray array];
        NSArray *array = dictionary[@"allocatedSceneRange"];
        for (NSDictionary *sceneRangeDict in array) {
            SigSceneRangeModel *model = [[SigSceneRangeModel alloc] init];
            [model setDictionaryToSigSceneRangeModel:sceneRangeDict];
            [allocatedSceneRange addObject:model];
        }
        _allocatedSceneRange = allocatedSceneRange;
    }
}

/**
 create new provisioner by count of exist provisioners.（已弃用，请使用'initWithExistProvisionerMaxHighAddressUnicast:andProvisionerUUID:'）

 @param count count of exist provisioners
 @param provisionerUUID new provisioner's uuid
 @return SigProvisionerModel model
 */
- (instancetype)initWithExistProvisionerCount:(UInt16)count andProvisionerUUID:(NSString *)provisionerUUID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.allocatedGroupRange = [NSMutableArray array];
        SigRangeModel *range1 = [[SigRangeModel alloc] init];
        //做法1：不同的Provisioner使用不同的组地址范围
//        range1.lowAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeLowAddress + count*0x100];
//        range1.highAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeHighAddress + count*0x100];
        //做法2：不同的Provisioner都使用同一组组地址
        range1.lowAddress = [NSString stringWithFormat:@"%04lX",(long)kAllocatedGroupRangeLowAddress];
        range1.highAddress = [NSString stringWithFormat:@"%04lX",(long)kAllocatedGroupRangeHighAddress];
        [self.allocatedGroupRange addObject:range1];
        
        //源码版本v3.2.2前，间隔255，短地址分配范围：1-0xff，0x0100-0x01ff，0x0200-0x02ff，0x0300-0x03ff， 。。。
        //源码版本v3.2.3及以后，间隔1024，短地址分配范围：1~1024，1025~2048，2049~3072，3073~4096， 。。。
        //源码版本v3.3.0及以后，间隔0x0400，短地址分配范围：0x0001~0x03FF，0x0400~0x07FF，0x0800~0x0BFF，0x0C00~0x0FFF,...,0x7C00~0x7FFF.
        self.allocatedUnicastRange = [NSMutableArray array];
        SigRangeModel *range2 = [[SigRangeModel alloc] init];
        range2.lowAddress = [NSString stringWithFormat:@"%04X",kAllocatedUnicastRangeLowAddress + (count == 0 ? 0 : (count*(SigMeshLib.share.dataSource.defaultAllocatedUnicastRangeHighAddress+1)-1))];
        range2.highAddress = [NSString stringWithFormat:@"%04X",SigMeshLib.share.dataSource.defaultAllocatedUnicastRangeHighAddress + (count == 0 ? 0 : count*(SigMeshLib.share.dataSource.defaultAllocatedUnicastRangeHighAddress+1))];
        [self.allocatedUnicastRange addObject:range2];
        
        self.allocatedSceneRange = [NSMutableArray array];
        SigSceneRangeModel *range3 = [[SigSceneRangeModel alloc] init];
        range3.firstScene = [NSString stringWithFormat:@"%04lX",(long)kAllocatedSceneRangeFirstAddress];
        range3.lastScene = [NSString stringWithFormat:@"%04lX",(long)kAllocatedSceneRangeLastAddress];
        [self.allocatedSceneRange addObject:range3];
                
        self.UUID = provisionerUUID;
        self.provisionerName = @"Telink iOS provisioner";
    }
    return self;
}

/// Initialize SigProvisionerModel object.
/// @param maxHighAddressUnicast The max hight unicastAddress of `allocatedUnicastRange`.
/// @param provisionerUUID The UUID of new provisioner.
/// @returns return `nil` when initialize SigProvisionerModel object fail.
- (instancetype)initWithExistProvisionerMaxHighAddressUnicast:(UInt16)maxHighAddressUnicast andProvisionerUUID:(NSString *)provisionerUUID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.allocatedGroupRange = [NSMutableArray array];
        SigRangeModel *range1 = [[SigRangeModel alloc] init];
        //做法1：不同的Provisioner使用不同的组地址范围
//        range1.lowAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeLowAddress + count*0x100];
//        range1.highAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeHighAddress + count*0x100];
        //做法2：不同的Provisioner都使用同一组组地址
        range1.lowAddress = [NSString stringWithFormat:@"%04lX",(long)kAllocatedGroupRangeLowAddress];
        range1.highAddress = [NSString stringWithFormat:@"%04lX",(long)kAllocatedGroupRangeHighAddress];
        [self.allocatedGroupRange addObject:range1];
        
        //源码版本v3.2.2前，间隔255，短地址分配范围：1-0xff，0x0100-0x01ff，0x0200-0x02ff，0x0300-0x03ff， 。。。
        //源码版本v3.2.3及以后，间隔1024，短地址分配范围：1~1024，1025~2048，2049~3072，3073~4096， 。。。
        //源码版本v3.3.0及以后，间隔0x0400，短地址分配范围：0x0001~0x03FF，0x0400~0x07FF，0x0800~0x0BFF，0x0C00~0x0FFF,...,0x7C00~0x7FFF.
        self.allocatedUnicastRange = [NSMutableArray array];
        SigRangeModel *range2 = [[SigRangeModel alloc] init];
        range2.lowAddress = [NSString stringWithFormat:@"%04X",maxHighAddressUnicast + 1];
        UInt16 highAddress = maxHighAddressUnicast + SigMeshLib.share.dataSource.defaultAllocatedUnicastRangeHighAddress - (maxHighAddressUnicast == 0 ? 1 : 0);
        if (highAddress > 0x7FFF) {
            highAddress = 0x7FFF;
        }
        range2.highAddress = [NSString stringWithFormat:@"%04X",highAddress];
        [self.allocatedUnicastRange addObject:range2];
        
        self.allocatedSceneRange = [NSMutableArray array];
        SigSceneRangeModel *range3 = [[SigSceneRangeModel alloc] init];
        range3.firstScene = [NSString stringWithFormat:@"%04lX",(long)kAllocatedSceneRangeFirstAddress];
        range3.lastScene = [NSString stringWithFormat:@"%04lX",(long)kAllocatedSceneRangeLastAddress];
        [self.allocatedSceneRange addObject:range3];
                
        self.UUID = provisionerUUID;
        self.provisionerName = @"Telink iOS provisioner";
    }
    return self;
}

/// Get SigNodeModel object of current provisioner.
- (nullable SigNodeModel *)node {
    SigNodeModel *tem = nil;
    NSArray *nodes = [NSArray arrayWithArray:SigMeshLib.share.dataSource.nodes];
    for (SigNodeModel *model in nodes) {
        if ([model.UUID isEqualToString:_UUID]) {
            tem = model;
            break;
        }
    }
    return tem;
}

@end


/// The unicast range object represents the range of unicast addresses that the
/// Provisioner can allocate to new devices when they are provisioned onto the
/// mesh network, without needing to coordinate the node additions with other
/// Provisioners. The unicast range object contains the properties defined in Table 2.3.
/// The group range object represents the range of group addresses that the
/// Provisioner can allocate to newly created groups, without needing to
/// coordinate the group additions with other Provisioners.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.9),
/// 2.1.1.1 Unicast range object and 2.1.1.2 Group range object.
@implementation SigRangeModel

/// Get dictionary from SigRangeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigRangeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_lowAddress) {
        dict[@"lowAddress"] = _lowAddress;
    }
    if (_highAddress) {
        dict[@"highAddress"] = _highAddress;
    }
    return dict;
}

/// Set dictionary to SigRangeModel object.
/// @param dictionary SigRangeModel dictionary object.
- (void)setDictionaryToSigRangeModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"lowAddress"]) {
        _lowAddress = dictionary[@"lowAddress"];
    }
    if ([allKeys containsObject:@"highAddress"]) {
        _highAddress = dictionary[@"highAddress"];
    }
}

/// Get int number of low address.
- (NSInteger)lowIntAddress{
    return [LibTools uint16From16String:self.lowAddress];
}

/// Get int number of hight address.
- (NSInteger)hightIntAddress{
    return [LibTools uint16From16String:self.highAddress];
}

/// Initialize SigRangeModel object.
/// @param maxHighAddressUnicast The max hight unicastAddress of `allocatedUnicastRange`.
/// @returns return `nil` when initialize SigRangeModel object fail.
- (instancetype)initWithMaxHighAddressUnicast:(UInt16)maxHighAddressUnicast {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        //源码版本v3.3.0及以后，间隔0x0400，短地址分配范围：0x0001~0x03FF，0x0400~0x07FF，0x0800~0x0BFF，0x0C00~0x0FFF,...,0x7C00~0x7FFF.
        _lowAddress = [NSString stringWithFormat:@"%04X",maxHighAddressUnicast + 1];
        UInt16 highAddress = maxHighAddressUnicast + SigMeshLib.share.dataSource.defaultAllocatedUnicastRangeHighAddress - (maxHighAddressUnicast == 0 ? 1 : 0);
        if (highAddress > 0x7FFF) {
            highAddress = 0x7FFF;
        }
        _highAddress = [NSString stringWithFormat:@"%04X",highAddress];
    }
    return self;
}

@end


/// The scene range object represents the range of scene numbers that the
/// Provisioner can use to register new scenes in the mesh network, without
/// needing to coordinate the allocated scene numbers with other Provisioners.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.10),
/// 2.1.1.3 Scene range object.
@implementation SigSceneRangeModel

/// Get dictionary from SigSceneRangeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigSceneRangeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_firstScene) {
        dict[@"firstScene"] = _firstScene;
    }
    if (_lastScene) {
        dict[@"lastScene"] = _lastScene;
    }
    return dict;
}

/// Set dictionary to SigSceneRangeModel object.
/// @param dictionary SigSceneRangeModel dictionary object.
- (void)setDictionaryToSigSceneRangeModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"firstScene"]) {
        _firstScene = dictionary[@"firstScene"];
    }
    if ([allKeys containsObject:@"lastScene"]) {
        _lastScene = dictionary[@"lastScene"];
    }
}

@end


/// The application key object represents the state of a mesh application key
/// that is used for securing communication at the access layer [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.11),
/// 2.1.3 Application key object.
@implementation SigAppkeyModel

/// Get dictionary from SigAppkeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigAppkeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_key) {
        dict[@"key"] = _key;
    }
    if (_oldKey) {
        dict[@"oldKey"] = _oldKey;
    }
    dict[@"index"] = @(_index);
    dict[@"boundNetKey"] = @(_boundNetKey);
    return dict;
}

/// Set dictionary to SigAppkeyModel object.
/// @param dictionary SigAppkeyModel dictionary object.
- (void)setDictionaryToSigAppkeyModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"key"]) {
        _key = dictionary[@"key"];
    }
    if ([allKeys containsObject:@"oldKey"]) {
        _oldKey = dictionary[@"oldKey"];
    }
    if ([allKeys containsObject:@"index"]) {
        _index = (UInt16)[dictionary[@"index"] intValue];
    }
    if ([allKeys containsObject:@"boundNetKey"]) {
        _boundNetKey = [dictionary[@"boundNetKey"] integerValue];
    }
}

//- (BOOL)isEqual:(id)object{
//    if ([object isKindOfClass:[SigAppkeyModel class]]) {
//        return [_key isEqualToString:((SigAppkeyModel *)object).key];
//    } else {
//        return NO;
//    }
//}

- (UInt8)aid {
    if (_aid == 0 && _key && _key.length > 0) {
        _aid = [OpenSSLHelper.share calculateK4WithN:[LibTools nsstringToHex:_key]];
    }
    return _aid;
}

- (UInt8)oldAid {
    if (_oldKey && _oldKey.length > 0 && _oldAid == 0) {
        _oldAid = [OpenSSLHelper.share calculateK4WithN:[LibTools nsstringToHex:_oldKey]];
    }
    return _oldAid;
}

/// Get curent bound network key.
- (nullable SigNetkeyModel *)getCurrentBoundNetKey {
    SigNetkeyModel *tem = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *model in netKeys) {
        if (model.index == _boundNetKey) {
            tem = model;
            break;
        }
    }
    return tem;
}

/// Get hex data appkey of SigAppkeyModel object.
- (nullable NSData *)getDataKey {
    if (_key != nil && _key.length > 0 && ![_key isEqualToString:@"00000000000000000000000000000000"]) {
        return [LibTools nsstringToHex:_key];
    }
    return nil;
}

/// Get hex data old appkey of SigAppkeyModel object.
- (nullable NSData *)getDataOldKey {
    if (_oldKey != nil && _oldKey.length > 0 && ![_oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        return [LibTools nsstringToHex:_oldKey];
    }
    return nil;
}

/// Get detail string of SigAppkeyModel object.
- (NSString *)getAppKeyDetailString {
    return [NSString stringWithFormat:@"name:%@\tindex:0x%04lX\tboundNetKey:0x%04lX\tkey:0x%@\toldKey:0x%@",_name,(long)_index,(long)_boundNetKey,_key,_oldKey];
}

@end


@implementation SigExclusionListObjectModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _addresses = [NSMutableArray array];
    }
    return self;
}

/// Get dictionary from SigExclusionListObjectModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigExclusionListObjectModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"ivIndex"] = @(_ivIndex);
    if (self.addresses) {
        dict[@"addresses"] = self.addresses;
    }
    return dict;
}

/// Set dictionary to SigExclusionListObjectModel object.
/// @param dictionary SigExclusionListObjectModel dictionary object.
- (void)setDictionaryToSigExclusionListObjectModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"ivIndex"]) {
        _ivIndex = (UInt16)[dictionary[@"ivIndex"] intValue];
    }
    if ([allKeys containsObject:@"addresses"]) {
        _addresses = [NSMutableArray arrayWithArray:dictionary[@"addresses"]];
    }
}

/// Determine if the data of two SigExclusionListObjectModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigExclusionListObjectModel class]]) {
        //ivIndex is the unique identifier of SigExclusionListObjectModel.
        return _ivIndex == ((SigExclusionListObjectModel *)object).ivIndex;
    } else {
        //Two SigExclusionListObjectModel object is different.
        return NO;
    }
}

- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    SigExclusionListObjectModel *model = [[[self class] alloc] init];
    model.addresses = [[NSMutableArray alloc] initWithArray:self.addresses];
    model.ivIndex = self.ivIndex;
    return model;
}

@end


/// The scene object represents a configured state of a mesh scene.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.22),
/// 2.1.6 Scene object
@implementation SigSceneModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _addresses = [NSMutableArray array];
        _actionList = [NSMutableArray array];
    }
    return self;
}

/// Get dictionary from SigSceneModel object.(save local)
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigSceneModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_number) {
        dict[@"number"] = _number;
    }
    if (self.addresses) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *addresses = [NSMutableArray arrayWithArray:self.addresses];
        for (NSString *str in addresses) {
            [array addObject:str];
        }
        dict[@"addresses"] = array;
    }
    if (_actionList) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *actionList = [NSMutableArray arrayWithArray:_actionList];
        for (ActionModel *model in actionList) {
            NSDictionary *actionDict = [model getDictionaryOfActionModel];
            [array addObject:actionDict];
        }
        dict[@"actionList"] = array;
    }
    return dict;
}

/// Set dictionary to SigSceneModel object.
/// @param dictionary SigSceneModel dictionary object.
- (void)setDictionaryToSigSceneModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"number"]) {
        _number = dictionary[@"number"];
    }
//    if ([allKeys containsObject:@"number"]) {
//        if ([dictionary[@"number"] isKindOfClass:[NSString class]]) {
//            _number = [LibTools uint16From16String:dictionary[@"number"]];
//        }
//    }
    if ([allKeys containsObject:@"addresses"]) {
        NSMutableArray *addresses = [NSMutableArray array];
        NSArray *array = dictionary[@"addresses"];
        for (NSString *str in array) {
            [addresses addObject:str];
        }
        _addresses = addresses;
    }
    if ([allKeys containsObject:@"actionList"]) {
        NSMutableArray *actionList = [NSMutableArray array];
        NSArray *array = dictionary[@"actionList"];
        for (NSDictionary *actionDict in array) {
            ActionModel *model = [[ActionModel alloc] init];
            [model setDictionaryToActionModel:actionDict];
            [actionList addObject:model];
        }
        _actionList = actionList;
    } else {
        NSMutableArray *actionList = [NSMutableArray array];
        for (NSString *numberStr in _addresses) {
            ActionModel *model = [[ActionModel alloc] init];
            model.address = [LibTools uint16From16String:numberStr];
            [actionList addObject:model];
        }
        _actionList = actionList;
    }
}

/// Get format dictionary from SigSceneModel object.(Mesh Spec format)
/// @returns return dictionary object.
- (NSDictionary *)getFormatDictionaryOfSigSceneModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_number) {
        dict[@"number"] = _number;
    }
    if (self.addresses) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *addresses = [NSMutableArray arrayWithArray:self.addresses];
        for (NSString *str in addresses) {
            [array addObject:str];
        }
        dict[@"addresses"] = array;
    }
    return dict;
}

/// Determine if the data of two SigSceneModel is the same
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigSceneModel class]]) {
        //number is the unique identifier of SigSceneModel.
        return _number == ((SigSceneModel *)object).number;
    } else {
        //Two SigSceneModel object is different.
        return NO;
    }
}

- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    SigSceneModel *model = [[[self class] alloc] init];
    model.addresses = [[NSMutableArray alloc] initWithArray:self.addresses];
    model.actionList = [[NSMutableArray alloc] initWithArray:self.actionList];
    model.name = self.name;
    model.number = self.number;
    return model;
}

- (NSMutableArray<NSString *> *)addresses{
    if (self.actionList && self.actionList.count > 0) {
        NSMutableArray *tem = [NSMutableArray array];
        NSMutableArray *actionList = [NSMutableArray arrayWithArray:_actionList];
        for (ActionModel *action in actionList) {
            [tem addObject:[NSString stringWithFormat:@"%04X",action.address]];
        }
        return tem;
    } else {
        return _addresses;
    }
}

@end


/// If a node is part of a Group, at least one model of the node is subscribed
/// to the Group’s group address.
/// @note   A Group may have a Parent Group. In this case, all the models
/// of a node that are subscribed to the Group’s address are also subscribed
/// to the Parent Group’s address.
/// @note   For example, the Second-Floor Group is a parent of the Bedroom
/// Group and the Guest Bedroom Group. In this case, at least one model of all
/// the nodes of the Bedroom Group is subscribed to a group address or virtual
/// label of the Bedroom Group and Second-Floor Group; and at least one model
/// of all the nodes of the Guest Bedroom Group is subscribed to the group
/// address or virtual label of the Guest Bedroom Group and the Second-Floor Group.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.21),
/// 2.1.5 Group object.
@implementation SigGroupModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _groupBrightness = 100;
        _groupTempareture = 100;
    }
    return self;
}

/// Get dictionary from SigGroupModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigGroupModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_address) {
        dict[@"address"] = _address;
    }
    if (_parentAddress) {
        dict[@"parentAddress"] = _parentAddress;
    }
    return dict;
}

/// Set dictionary to SigGroupModel object.
/// @param dictionary SigGroupModel dictionary object.
- (void)setDictionaryToSigGroupModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"address"]) {
        _address = dictionary[@"address"];
    }
    if ([allKeys containsObject:@"parentAddress"]) {
        _parentAddress = dictionary[@"parentAddress"];
    }
    _groupBrightness = 100;
    _groupTempareture = 100;
}

- (SigMeshAddress *)meshAddress {
    if (!_meshAddress) {
        _meshAddress = [[SigMeshAddress alloc] initWithHex:_address];
    }
    return _meshAddress;
}

- (UInt16)intAddress {
    if (_address && _address.length == 4) {
        return [LibTools uint16From16String:self.address];
    } else {
        return [self intVirtualAddress];
    }
}

- (UInt16)intVirtualAddress {
    UInt16 tem = 0;
    if (_address && _address.length == 16) {
        //message22
        //16字节的Label UUID：0073e7e4d8b9440faf8415df4c56c0e1转2字节的DST (Virtual Address)：b529
        
    }
    return tem;
}

- (BOOL)isOn{
    BOOL tem = NO;
    NSArray *groupDevices = [NSArray arrayWithArray:self.groupDevices];
    for (SigNodeModel *model in groupDevices) {
        if (model.state == DeviceStateOn) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (NSMutableArray <SigNodeModel *>*)groupDevices{
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    NSArray *curNodes = [NSArray arrayWithArray:SigMeshLib.share.dataSource.curNodes];
    for (SigNodeModel *model in curNodes) {
        if ([model.getGroupIDs containsObject:@(self.intAddress)]) {
            [tem addObject:model];
        }
    }
    return tem;
}

- (NSMutableArray <SigNodeModel *>*)groupOnlineDevices{
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    NSArray *curNodes = [NSArray arrayWithArray:SigMeshLib.share.dataSource.curNodes];
    for (SigNodeModel *model in curNodes) {
        if ([model.getGroupIDs containsObject:@(self.intAddress)] && model.state != DeviceStateOutOfLine) {
            [tem addObject:model];
        }
    }
    return tem;
}

- (BOOL)hasLevelCapability {
    BOOL tem = NO;
    NSArray *groupNodes = [NSArray arrayWithArray:self.groupDevices];
    for (SigNodeModel *node in groupNodes) {
        if (node.hasLevelCapability) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (BOOL)hasLumLevelCapability {
    BOOL tem = NO;
    NSArray *groupNodes = [NSArray arrayWithArray:self.groupDevices];
    for (SigNodeModel *node in groupNodes) {
        if (node.hasLumLevelCapability) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (BOOL)hasTempLevelCapability {
    BOOL tem = NO;
    NSArray *groupNodes = [NSArray arrayWithArray:self.groupDevices];
    for (SigNodeModel *node in groupNodes) {
        if (node.hasTempLevelCapability) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (BOOL)hasHueLevelCapability {
    BOOL tem = NO;
    NSArray *groupNodes = [NSArray arrayWithArray:self.groupDevices];
    for (SigNodeModel *node in groupNodes) {
        if (node.hasHueLevelCapability) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (BOOL)hasSatLevelCapability {
    BOOL tem = NO;
    NSArray *groupNodes = [NSArray arrayWithArray:self.groupDevices];
    for (SigNodeModel *node in groupNodes) {
        if (node.hasSatLevelCapability) {
            tem = YES;
            break;
        }
    }
    return tem;
}

@end


/// The node object represents a configured state of a mesh node.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.12),
/// 2.1.4 Node object
@implementation SigNodeModel{
    UInt16 _address;
}


- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _elements = [NSMutableArray array];
        _netKeys = [NSMutableArray array];
        _appKeys = [NSMutableArray array];

        _schedulerList = [[NSMutableArray alloc] init];

        _state = DeviceStateOutOfLine;
        _macAddress = @"";
        _name = @"";
        _security = @"secure";
        _features = [[SigNodeFeatures alloc] init];
        _relayRetransmit = [[SigRelayretransmitModel alloc] init];
        _networkTransmit = [[SigNetworktransmitModel alloc] init];
        
        _defaultTTL = 10;
        _secureNetworkBeacon = YES;
        _configComplete = NO;
        _HSL_Hue = 0;
        _HSL_Saturation = 0;
        _HSL_Lightness = 0;
        _heartbeatPub = nil;
        _heartbeatSub = nil;
        _subnetBridgeList = [[NSMutableArray alloc] init];
        _subnetBridgeEnable = NO;
        _directControlStatus = nil;
        _excluded = false;
    }
    return self;
}


- (instancetype)initWithNode:(SigNodeModel *)node {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _features = node.features;
        _unicastAddress = node.unicastAddress;
        _secureNetworkBeacon = node.secureNetworkBeacon;
        _relayRetransmit = node.relayRetransmit;
        _networkTransmit = node.networkTransmit;
        _configComplete = node.configComplete;
        _vid = node.vid;
        _cid = node.cid;
        _peripheralUUID = node.peripheralUUID;
        _security = node.security;
        _crpl = node.crpl;
        _defaultTTL = node.defaultTTL;
        _pid = node.pid;
        _name = node.name;
        _deviceKey = node.deviceKey;
        _macAddress = node.macAddress;
        
        _elements = [NSMutableArray arrayWithArray:node.elements];
        _netKeys = [NSMutableArray arrayWithArray:node.netKeys];
        _appKeys = [NSMutableArray arrayWithArray:node.appKeys];
        
        _state = node.state;
        _brightness = node.brightness;
        _temperature = node.temperature;
        _schedulerList = [NSMutableArray arrayWithArray:node.schedulerList];
        _heartbeatPub = node.heartbeatPub;
        _heartbeatSub = node.heartbeatSub;
        _subnetBridgeList = [[NSMutableArray alloc] initWithArray:node.subnetBridgeList];
        _subnetBridgeEnable = node.subnetBridgeEnable;
        _excluded = node.excluded;
        _directControlStatus = node.directControlStatus;
    }
    return self;
}

- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    SigNodeModel *device = [[[self class] alloc] init];
    device.features = self.features;
    device.unicastAddress = self.unicastAddress;
    device.secureNetworkBeacon = self.secureNetworkBeacon;
    device.relayRetransmit = self.relayRetransmit;
    device.networkTransmit = self.networkTransmit;
    device.configComplete = self.configComplete;
    device.vid = self.vid;
    device.cid = self.cid;
    device.peripheralUUID = self.peripheralUUID;
    device.security = self.security;
    device.crpl = self.crpl;
    device.defaultTTL = self.defaultTTL;
    device.pid = self.pid;
    device.name = self.name;
    device.deviceKey = self.deviceKey;
    device.macAddress = self.macAddress;
    
    device.elements = [NSMutableArray arrayWithArray:self.elements];
    device.netKeys = [NSMutableArray arrayWithArray:self.netKeys];
    device.appKeys = [NSMutableArray arrayWithArray:self.appKeys];
    
    device.state = self.state;
    device.brightness = self.brightness;
    device.temperature = self.temperature;
    device.schedulerList = [NSMutableArray arrayWithArray:self.schedulerList];
    device.heartbeatPub = self.heartbeatPub;
    device.heartbeatSub = self.heartbeatSub;
    device.subnetBridgeList = [[NSMutableArray alloc] initWithArray:self.subnetBridgeList];
    device.subnetBridgeEnable = self.subnetBridgeEnable;
    device.excluded = self.excluded;
    device.directControlStatus = self.directControlStatus;
    return device;
}

//Attention: 1.it is use peripheralUUID to compare SigNodeModel when SigScanRspModel.macAddress is nil.
//Attention: 2.it is use macAddress to compare SigNodeModel when peripheralUUID is nil.
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigNodeModel class]]) {
        SigNodeModel *tem = (SigNodeModel *)object;
        if (self.peripheralUUID && self.peripheralUUID.length > 0 && tem.peripheralUUID && tem.peripheralUUID.length > 0 && ![tem.peripheralUUID isEqual:@"00000000000000000000000000000000"]) {
            return [self.peripheralUUID isEqualToString:tem.peripheralUUID];
        }else if (self.macAddress && self.macAddress.length > 0 && tem.macAddress && tem.macAddress.length > 0 && ![tem.macAddress isEqual:@"000000000000"]) {
            return [self.macAddress.uppercaseString isEqualToString:tem.macAddress.uppercaseString];
        }
        return NO;
    } else {
        return NO;
    }
}

- (BOOL)isSensor{
    return self.features.lowPowerFeature == SigNodeFeaturesState_enabled;
//    return [LibTools uint16From16String:self.cid] == 0x201;
}

- (BOOL)isRemote {
    return self.getTelinkPID.majorProductType == MajorProductType_switch;
//    return [LibTools uint16From16String:self.pid] == 0x301;
}

- (struct TelinkPID)getTelinkPID {
    struct TelinkPID telinkPid = {};
    telinkPid.value = [LibTools uint16From16String:self.pid];
    return telinkPid;
}

- (UInt8)HSL_Hue100{
    return [LibTools lightnessToLum:self.HSL_Hue];
}

- (UInt8)HSL_Saturation100{
    return [LibTools lightnessToLum:self.HSL_Saturation];
}

- (UInt8)HSL_Lightness100{
    return [LibTools lightnessToLum:self.HSL_Lightness];
}

///return node true brightness, range is 0~100
- (UInt8)trueBrightness{
    return [LibTools lightnessToLum:self.brightness];
}

///return node true color temperture, range is 0~100
- (UInt8)trueTemperature{
    return [LibTools tempToTemp100:self.temperature];
}

///update node status from api getOnlineStatusFromUUIDWithCompletation
- (void)updateOnlineStatusWithDeviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100{
    _state = state;
    if (state == DeviceStateOutOfLine) {
        return;
    }
    _brightness = [LibTools lumToLightness:bright100];
    _temperature = [LibTools temp100ToTemp:temperature100];
}

- (UInt16)getNewSchedulerID{
    UInt16 schedulerId = 0;
    if (_schedulerList.count > 0) {
        [_schedulerList sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SchedulerModel *)obj1 schedulerID] > [(SchedulerModel *)obj2 schedulerID];
        }];
        schedulerId = _schedulerList.lastObject.schedulerID + 1;
    }
    return schedulerId;
}

- (void)saveSchedulerModelWithModel:(SchedulerModel *)scheduler{
    @synchronized(self) {
        SchedulerModel *model = [[SchedulerModel alloc] init];
        model.schedulerID = scheduler.schedulerID;
        model.schedulerData = scheduler.schedulerData;
        model.sceneId = scheduler.sceneId;
        
        if ([self.schedulerList containsObject:scheduler]) {
            NSInteger index = [self.schedulerList indexOfObject:scheduler];
            self.schedulerList[index] = model;
        } else {
            [self.schedulerList addObject:model];
        }
        [SigMeshLib.share.dataSource saveLocationData];
    }
}

- (UInt8)getElementCount{
    UInt8 tem = 2;
    if (self.elements && self.elements.count != 0) {
        tem = (UInt8)self.elements.count;
    }
    return tem;
}

- (BOOL)isKeyBindSuccess{
    if (self.appKeys && self.appKeys.count > 0) {
        return YES;
    }
    return NO;
}

- (NSMutableArray<NSNumber *> *)onoffAddresses{
    return [self getAddressesWithModelID:@(kSigModel_GenericOnOffServer_ID)];
}

- (NSMutableArray<NSNumber *> *)levelAddresses{
    return [self getAddressesWithModelID:@(kSigModel_GenericLevelServer_ID)];
}

- (NSMutableArray<NSNumber *> *)temperatureAddresses{
    return [self getAddressesWithModelID:@(kSigModel_LightCTLTemperatureServer_ID)];
}

- (NSMutableArray<NSNumber *> *)HSLAddresses{
    return [self getAddressesWithModelID:@(kSigModel_LightHSLServer_ID)];
}

- (NSMutableArray<NSNumber *> *)lightnessAddresses{
    return [self getAddressesWithModelID:@(kSigModel_LightLightnessServer_ID)];
}

- (NSMutableArray<NSNumber *> *)schedulerAddress{
    return [self getAddressesWithModelID:@(kSigModel_SchedulerServer_ID)];
}

- (NSMutableArray<NSNumber *> *)subnetBridgeServerAddress {
    return [self getAddressesWithModelID:@(kSigModel_BridgeConfigurationServer_ID)];
}

- (NSMutableArray<NSNumber *> *)sceneAddress{
    return [self getAddressesWithModelID:@(kSigModel_SceneServer_ID)];
}

- (NSMutableArray<NSNumber *> *)publishAddress{
    return [self getAddressesWithModelID:@(self.publishModelID)];
}

///publish首选SIG_MD_LIGHT_CTL_S，次选SIG_MD_LIGHT_HSL_S，SIG_MD_LIGHTNESS_S，SIG_MD_G_ONOFF_S
- (UInt16)publishModelID{
    UInt16 tem = 0;
    if ([self getAddressesWithModelID:@(kSigModel_LightCTLServer_ID)].count > 0) {
        tem = (UInt16)kSigModel_LightCTLServer_ID;
    } else if ([self getAddressesWithModelID:@(kSigModel_LightHSLServer_ID)].count > 0){
        tem = (UInt16)kSigModel_LightHSLServer_ID;
    } else if ([self getAddressesWithModelID:@(kSigModel_LightLightnessServer_ID)].count > 0){
        tem = (UInt16)kSigModel_LightLightnessServer_ID;
    } else if ([self getAddressesWithModelID:@(kSigModel_GenericOnOffServer_ID)].count > 0){
        tem = (UInt16)kSigModel_GenericOnOffServer_ID;
    }
    return tem;
}

- (NSMutableArray *)getAddressesWithModelID:(NSNumber *)sigModelID{
    NSMutableArray *array = [NSMutableArray array];
    if (self.elements.count > 0) {
        for (int i=0; i<self.elements.count; i++) {
            SigElementModel *ele = self.elements[i];
            NSArray *all = [NSArray arrayWithArray:ele.models];
            for (SigModelIDModel *modelID in all) {
                if (modelID.getIntModelID == sigModelID.intValue) {
                    [array addObject:@(self.address+i)];
                    break;
                }
            }
        }
    }
    return array;
}

- (NSString *)peripheralUUID{
    if (self.address == SigMeshLib.share.dataSource.curLocationNodeModel.address) {
        //local node's uuid
        return _UUID;
    }
    //new code:use in v3.0.0 and later
    SigEncryptedModel *model = [SigMeshLib.share.dataSource getSigEncryptedModelWithAddress:self.address];
    _peripheralUUID = model.peripheralUUID;
    if ((!_peripheralUUID || _peripheralUUID.length == 0) && self.address != 0) {
        SigScanRspModel *rspModel = [SigMeshLib.share.dataSource getScanRspModelWithUnicastAddress:self.address];
        _peripheralUUID = rspModel.uuid;
        if ((!_peripheralUUID || _peripheralUUID.length == 0) && self.macAddress != nil && self.macAddress.length > 0) {
            rspModel = [SigMeshLib.share.dataSource getScanRspModelWithMacAddress:self.macAddress];
            _peripheralUUID = rspModel.uuid;
        }
    }
    
    //show in HomeViewController node's peripheralUUID
    return _peripheralUUID;

}

- (NSString *)macAddress{
    if (_macAddress && _macAddress.length > 0) {
        return _macAddress;
    }
    NSString *tem = nil;
    if (_peripheralUUID && _peripheralUUID.length > 0) {
        SigScanRspModel *model = [SigMeshLib.share.dataSource getScanRspModelWithUUID:_peripheralUUID];
        if (model) {
            tem = model.macAddress;
        }
    }
    if (tem == nil) {
        if (self.address != 0) {
            SigScanRspModel *model = [SigMeshLib.share.dataSource getScanRspModelWithUnicastAddress:self.address];
            if (model) {
                tem = model.macAddress;
            }
        }
    }
    _macAddress = tem;
    return _macAddress;
}

/// Returns list of Network Keys known to this Node.
- (NSArray <SigNetkeyModel *>*)getNetworkKeys {
    NSMutableArray *tem = [NSMutableArray array];
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *key in netKeys) {
        BOOL has = NO;
        NSArray *all = [NSArray arrayWithArray:_netKeys];
        for (SigNodeKeyModel *nodeKey in all) {
            if (nodeKey.index == key.index) {
                has = YES;
                break;
            }
        }
        if (has) {
            [tem addObject:key];
        }
    }
    return tem;
}

/// The last unicast address allocated to this node. Each node's element
/// uses its own subsequent unicast address. The first (0th) element is identified
/// by the node's unicast address. If there are no elements, the last unicast address
/// is equal to the node's unicast address.
- (UInt16)lastUnicastAddress {
    // Provisioner may not have any elements
    UInt16 allocatedAddresses = _elements.count > 0 ? _elements.count : 1;
    return self.address + allocatedAddresses - 1;
}

/// Returns whether the address uses the given unicast address for one
/// of its elements.
///
/// - parameter addr: Address to check.
/// - returns: `True` if any of node's elements (or the node itself) was assigned
///            the given address, `false` otherwise.
- (BOOL)hasAllocatedAddr:(UInt16)addr {
    return addr >= self.address && addr <= self.lastUnicastAddress;
}

- (nullable SigModelIDModel *)getModelIDModelWithModelID:(UInt32)modelID {
    SigModelIDModel *model = nil;
    NSArray *elements = [NSArray arrayWithArray:self.elements];
    for (SigElementModel *element in elements) {
        element.parentNodeAddress = self.address;
        NSArray *all = [NSArray arrayWithArray:element.models];
        for (SigModelIDModel *tem in all) {
            if (tem.getIntModelID == modelID) {
                model = tem;
                break;
            }
        }
        if (model) {
            break;
        }
    }
    return model;
}

- (nullable SigModelIDModel *)getModelIDModelWithModelID:(UInt32)modelID andElementAddress:(UInt16)elementAddress {
    SigModelIDModel *model = nil;
    NSArray *elements = [NSArray arrayWithArray:self.elements];
    for (SigElementModel *element in elements) {
        element.parentNodeAddress = self.address;
        if (element.unicastAddress == elementAddress) {
            NSArray *all = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *tem in all) {
                if (tem.getIntModelID == modelID) {
                    model = tem;
                    break;
                }
            }
            if (model) {
                break;
            }
        }
    }
    return model;
}

- (NSDictionary *)getDictionaryOfSigNodeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (kSaveMacAddressToJson) {
        if (self.macAddress) {
            dict[@"macAddress"] = self.macAddress;
        }
    }
    if (_features) {
        dict[@"features"] = [_features getDictionaryOfSigFeatureModel];
    }
    if (_unicastAddress) {
        dict[@"unicastAddress"] = _unicastAddress;
    }
    dict[@"secureNetworkBeacon"] = [NSNumber numberWithBool:_secureNetworkBeacon];
    if (_relayRetransmit) {
        dict[@"relayRetransmit"] = [_relayRetransmit getDictionaryOfSigRelayretransmitModel];
    }
    if (_networkTransmit) {
        dict[@"networkTransmit"] = [_networkTransmit getDictionaryOfSigNetworktransmitModel];
    }
    dict[@"configComplete"] = [NSNumber numberWithBool:_configComplete];
    if (_vid) {
        dict[@"vid"] = _vid;
    }
    if (_cid) {
        dict[@"cid"] = _cid;
    }
    if (_UUID) {
        if (_UUID.length == 32) {
            dict[@"UUID"] = [LibTools UUIDToMeshUUID:_UUID];
        } else if (_UUID.length == 36) {
            dict[@"UUID"] = _UUID;
        }
    }
    if (_security) {
        dict[@"security"] = _security;
    }
    if (_crpl) {
        dict[@"crpl"] = _crpl;
    }
    dict[@"defaultTTL"] = @(_defaultTTL);
    if (_pid) {
        dict[@"pid"] = _pid;
    }
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_deviceKey) {
        dict[@"deviceKey"] = _deviceKey;
    }
    dict[@"excluded"] = [NSNumber numberWithBool:_excluded];
    if (_heartbeatPub) {
        dict[@"heartbeatPub"] = [_heartbeatPub getDictionaryOfSigHeartbeatPubModel];
    }
    if (_heartbeatSub) {
        dict[@"heartbeatSub"] = [_heartbeatSub getDictionaryOfSigHeartbeatSubModel];
    }
    if (_elements) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *elements = [NSArray arrayWithArray:_elements];
        for (SigElementModel *model in elements) {
            NSDictionary *elementDict = [model getDictionaryOfSigElementModel];
            [array addObject:elementDict];
        }
        dict[@"elements"] = array;
    }
    if (_netKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
        for (SigNodeKeyModel *model in netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfSigNodeKeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"netKeys"] = array;
    }
    if (_appKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *appKeys = [NSArray arrayWithArray:_appKeys];
        for (SigNodeKeyModel *model in appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfSigNodeKeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"appKeys"] = array;
    }
    if (_schedulerList && _schedulerList.count > 0) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *schedulerList = [NSArray arrayWithArray:_schedulerList];
        for (SchedulerModel *model in schedulerList) {
            NSDictionary *schedulerDict = [model getDictionaryOfSchedulerModel];
            [array addObject:schedulerDict];
        }
        dict[@"schedulerList"] = array;
    }
    if (_subnetBridgeList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *subnetBridgeList = [NSArray arrayWithArray:_subnetBridgeList];
        for (SigSubnetBridgeModel *model in subnetBridgeList) {
            NSDictionary *subnetBridgeDict = [model getDictionaryOfSubnetBridgeModel];
            [array addObject:subnetBridgeDict];
        }
        dict[@"subnetBridgeList"] = array;
    }
    dict[@"subnetBridgeEnable"] = [NSNumber numberWithBool:_subnetBridgeEnable];
    if (_directControlStatus) {
        dict[@"directControlStatus"] = [_directControlStatus getDictionaryOfSigDirectControlStatus];
    }
    return dict;
}

- (void)setDictionaryToSigNodeModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"macAddress"]) {
        _macAddress = dictionary[@"macAddress"];
    }
    if ([allKeys containsObject:@"features"]) {
        SigNodeFeatures *features = [[SigNodeFeatures alloc] init];
        [features setDictionaryToSigFeatureModel:dictionary[@"features"]];
        _features = features;
    }
    if ([allKeys containsObject:@"unicastAddress"]) {
        _unicastAddress = dictionary[@"unicastAddress"];
    }
    if ([allKeys containsObject:@"secureNetworkBeacon"]) {
        _secureNetworkBeacon = [dictionary[@"secureNetworkBeacon"] boolValue];
    }
    if ([allKeys containsObject:@"relayRetransmit"]) {
        SigRelayretransmitModel *relayRetransmit = [[SigRelayretransmitModel alloc] init];
        [relayRetransmit setDictionaryToSigRelayretransmitModel:dictionary[@"relayRetransmit"]];
        _relayRetransmit = relayRetransmit;
    }
    if ([allKeys containsObject:@"networkTransmit"]) {
        SigNetworktransmitModel *networkTransmit = [[SigNetworktransmitModel alloc] init];
        [networkTransmit setDictionaryToSigNetworktransmitModel:dictionary[@"networkTransmit"]];
        _networkTransmit = networkTransmit;
    }
    if ([allKeys containsObject:@"configComplete"]) {
        _configComplete = [dictionary[@"configComplete"] boolValue];
    }
    if ([allKeys containsObject:@"vid"]) {
        _vid = dictionary[@"vid"];
    }
    if ([allKeys containsObject:@"cid"]) {
        _cid = dictionary[@"cid"];
    }
    if ([allKeys containsObject:@"UUID"]) {
        NSString *str = [dictionary[@"UUID"] uppercaseString];
        if (str.length == 32) {
            _UUID = [LibTools UUIDToMeshUUID:str];
        } else if (str.length == 36) {
            _UUID = str;
        }
    }
    if ([allKeys containsObject:@"security"]) {
        _security = dictionary[@"security"];
    }
    if ([allKeys containsObject:@"crpl"]) {
        _crpl = dictionary[@"crpl"];
    }
    if ([allKeys containsObject:@"defaultTTL"]) {
        _defaultTTL = [dictionary[@"defaultTTL"] integerValue];
    }
    if ([allKeys containsObject:@"pid"]) {
        _pid = dictionary[@"pid"];
    }
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"deviceKey"]) {
        _deviceKey = dictionary[@"deviceKey"];
    }
    if ([allKeys containsObject:@"excluded"]) {
        _excluded = [dictionary[@"excluded"] boolValue];
    }
    if ([allKeys containsObject:@"heartbeatPub"]) {
        _heartbeatPub = [[SigHeartbeatPubModel alloc] init];
        [_heartbeatPub setDictionaryToSigHeartbeatPubModel:dictionary[@"heartbeatPub"]];
    }
    if ([allKeys containsObject:@"heartbeatSub"]) {
        _heartbeatSub = [[SigHeartbeatSubModel alloc] init];
        [_heartbeatSub setDictionaryToSigHeartbeatSubModel:dictionary[@"heartbeatSub"]];
    }

    if ([allKeys containsObject:@"elements"]) {
        NSMutableArray *elements = [NSMutableArray array];
        NSArray *array = dictionary[@"elements"];
        for (NSDictionary *elementDict in array) {
            SigElementModel *model = [[SigElementModel alloc] init];
            [model setDictionaryToSigElementModel:elementDict];
            if (model.index != elements.count) {
                model.index = elements.count;
            }
            model.parentNodeAddress = self.address;
            [elements addObject:model];
        }
        _elements = elements;
    }
    if ([allKeys containsObject:@"netKeys"]) {
        NSMutableArray *netKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"netKeys"];
        for (NSDictionary *netkeyDict in array) {
            SigNodeKeyModel *model = [[SigNodeKeyModel alloc] init];
            [model setDictionaryToSigNodeKeyModel:netkeyDict];
            [netKeys addObject:model];
        }
        _netKeys = netKeys;
    }
    if ([allKeys containsObject:@"appKeys"]) {
        NSMutableArray *appKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"appKeys"];
        for (NSDictionary *appkeyDict in array) {
            SigNodeKeyModel *model = [[SigNodeKeyModel alloc] init];
            [model setDictionaryToSigNodeKeyModel:appkeyDict];
            [appKeys addObject:model];
        }
        _appKeys = appKeys;
    }
    if ([allKeys containsObject:@"schedulerList"]) {
        NSMutableArray *schedulerList = [NSMutableArray array];
        NSArray *array = dictionary[@"schedulerList"];
        for (NSDictionary *schedulerDict in array) {
            SchedulerModel *model = [[SchedulerModel alloc] init];
            [model setDictionaryToSchedulerModel:schedulerDict];
            [schedulerList addObject:model];
        }
        _schedulerList = schedulerList;
    }
    if ([allKeys containsObject:@"subnetBridgeList"]) {
        NSMutableArray *subnetBridgeList = [NSMutableArray array];
        NSArray *array = dictionary[@"subnetBridgeList"];
        for (NSDictionary *subnetBridgeDict in array) {
            SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] init];
            [model setDictionaryToSubnetBridgeModel:subnetBridgeDict];
            [subnetBridgeList addObject:model];
        }
        _subnetBridgeList = subnetBridgeList;
    }
    if ([allKeys containsObject:@"subnetBridgeEnable"]) {
        _subnetBridgeEnable = [dictionary[@"subnetBridgeEnable"] boolValue];
    }
    if ([allKeys containsObject:@"directControlStatus"]) {
        SigDirectControlStatus *tem = [[SigDirectControlStatus alloc] init];
        [tem setDictionaryToSigDirectControlStatus:dictionary[@"directControlStatus"]];
        _directControlStatus = tem;
    }
}

- (NSDictionary *)getFormatDictionaryOfSigNodeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_features) {
        dict[@"features"] = [_features getDictionaryOfSigFeatureModel];
    }
    if (_unicastAddress) {
        dict[@"unicastAddress"] = _unicastAddress;
    }
    dict[@"secureNetworkBeacon"] = [NSNumber numberWithBool:_secureNetworkBeacon];
    if (_relayRetransmit) {
        dict[@"relayRetransmit"] = [_relayRetransmit getDictionaryOfSigRelayretransmitModel];
    }
    if (_networkTransmit) {
        dict[@"networkTransmit"] = [_networkTransmit getDictionaryOfSigNetworktransmitModel];
    }
    dict[@"configComplete"] = [NSNumber numberWithBool:_configComplete];
    if (_vid) {
        dict[@"vid"] = _vid;
    }
    if (_cid) {
        dict[@"cid"] = _cid;
    }
    if (_UUID) {
        if (_UUID.length == 32) {
            dict[@"UUID"] = [LibTools UUIDToMeshUUID:_UUID];
        } else if (_UUID.length == 36) {
            dict[@"UUID"] = _UUID;
        }
    }
    if (_security) {
        dict[@"security"] = _security;
    }
    if (_crpl) {
        dict[@"crpl"] = _crpl;
    }
    dict[@"defaultTTL"] = @(_defaultTTL);
    if (_pid) {
        dict[@"pid"] = _pid;
    }
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_deviceKey) {
        dict[@"deviceKey"] = _deviceKey;
    }
    dict[@"excluded"] = [NSNumber numberWithBool:_excluded];
    if (_heartbeatPub) {
        dict[@"heartbeatPub"] = [_heartbeatPub getDictionaryOfSigHeartbeatPubModel];
    }
    if (_heartbeatSub) {
        dict[@"heartbeatSub"] = [_heartbeatSub getDictionaryOfSigHeartbeatSubModel];
    }
    if (_elements) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *elements = [NSArray arrayWithArray:_elements];
        for (SigElementModel *model in elements) {
            NSDictionary *elementDict = [model getDictionaryOfSigElementModel];
            [array addObject:elementDict];
        }
        dict[@"elements"] = array;
    }
    if (_netKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
        for (SigNodeKeyModel *model in netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfSigNodeKeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"netKeys"] = array;
    }
    if (_appKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *appKeys = [NSArray arrayWithArray:_appKeys];
        for (SigNodeKeyModel *model in appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfSigNodeKeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"appKeys"] = array;
    }
    return dict;
}

- (void)setCompositionData:(SigPage0 *)compositionData {
    self.cid = [SigHelper.share getUint16String:compositionData.companyIdentifier];
    self.pid = [SigHelper.share getUint16String:compositionData.productIdentifier];
    self.vid = [SigHelper.share getUint16String:compositionData.versionIdentifier];
    self.crpl = [SigHelper.share getUint16String:compositionData.minimumNumberOfReplayProtectionList];
    SigNodeFeatures *features = [[SigNodeFeatures alloc] init];
    features.proxyFeature = compositionData.features.proxyFeature;
    features.friendFeature = compositionData.features.friendFeature;
    features.relayFeature = compositionData.features.relayFeature;
    features.lowPowerFeature = compositionData.features.lowPowerFeature;
    self.features = features;
    NSMutableArray *array = [NSMutableArray array];
    NSArray *elements = [NSArray arrayWithArray:compositionData.elements];
    for (SigElementModel *element in elements) {
        element.parentNodeAddress = self.address;
        NSArray *all = [NSArray arrayWithArray:element.models];
        for (SigModelIDModel *modelID in all) {
            [self setBindSigNodeKeyModel:self.appKeys.firstObject toSigModelIDModel:modelID];
        }
        [array addObject:element];
    }
    BOOL modelChange = NO;
    if (self.elements.count != elements.count) {
        modelChange = YES;
    }
    if (!modelChange) {
        for (int i=0; i<self.elements.count; i++) {
            SigElementModel *oldElement = self.elements[i];
            if (array.count > i) {
                SigElementModel *newElement = array[i];
                if (oldElement.models.count != newElement.models.count) {
                    modelChange = YES;
                } else {
                    for (int j=0; j<oldElement.models.count; j++) {
                        SigModelIDModel *oldModelID = oldElement.models[j];
                        if (newElement.models.count > j) {
                            SigModelIDModel *newModelID = newElement.models[j];
                            if (![oldModelID.modelId isEqualToString:newModelID.modelId]) {
                                modelChange = YES;
                            }
                        } else {
                            modelChange = YES;
                        }
                        if (modelChange) {
                            break;
                        }
                    }
                }
            } else {
                modelChange = YES;
            }
            if (modelChange) {
                break;
            }
        }
    }
    if (modelChange) {
        self.elements = array;
    }
}

- (SigPage0 *)compositionData {
    SigPage0 *page0 = [[SigPage0 alloc] init];
    page0.companyIdentifier = [LibTools uint16From16String:self.cid];
    page0.productIdentifier = [LibTools uint16From16String:self.pid];
    page0.versionIdentifier = [LibTools uint16From16String:self.vid];
    page0.minimumNumberOfReplayProtectionList = [LibTools uint16From16String:self.crpl];
    SigNodeFeatures *features = [[SigNodeFeatures alloc] init];
    features.proxyFeature = self.features.proxyFeature;
    features.friendFeature = self.features.friendFeature;
    features.relayFeature = self.features.relayFeature;
    features.lowPowerFeature = self.features.lowPowerFeature;
    page0.features = features;
    page0.elements = [NSMutableArray arrayWithArray:self.elements];
    return page0;
}

- (void)setAddSigAppkeyModelSuccess:(SigAppkeyModel *)appkey {
    if (!_appKeys) {
        _appKeys = [NSMutableArray array];
    }
    SigNodeKeyModel *model = [[SigNodeKeyModel alloc] initWithIndex:appkey.index updated:false];
    if (![_appKeys containsObject:model]) {
        [_appKeys addObject:model];
    }
}

- (void)setBindSigNodeKeyModel:(SigNodeKeyModel *)appkey toSigModelIDModel:(SigModelIDModel *)modelID {
    NSNumber *bindIndex = @(appkey.index);
    if (!modelID.bind) {
        modelID.bind = [NSMutableArray array];
    }
    if (![modelID.bind containsObject:bindIndex]) {
        [modelID.bind addObject:bindIndex];
    }
}

- (void)setUnicastAddress:(NSString *)unicastAddress {
    _unicastAddress = unicastAddress;
    _address = [LibTools uint16From16String:unicastAddress];
}

- (UInt16)address {
    if (_address == 0) {
        _address = [LibTools uint16From16String:self.unicastAddress];
    }
    return _address;
}

- (void)setAddress:(UInt16)address{
    _address = address;
    self.unicastAddress = [NSString stringWithFormat:@"%04X",address];
    if (_elements && _elements.count) {
        NSArray *array = [NSArray arrayWithArray:_elements];
        for (SigElementModel *element in array) {
            element.parentNodeAddress = address;
        }
    }
}

///获取该设备的所有组号
- (NSMutableArray <NSNumber *>*)getGroupIDs{
    @synchronized (self) {
        NSMutableArray *tem = [NSMutableArray array];
        NSArray *elements = [NSArray arrayWithArray:self.elements];
        for (SigElementModel *element in elements) {
            NSArray *models = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *modelIDModel in models) {
                //[NSString]->[NSNumber]
                NSArray *subscribe = [NSArray arrayWithArray:modelIDModel.subscribe];
                for (NSString *groupIDString in subscribe) {
                    NSNumber *groupNumber = nil;
                    if (groupIDString.length == 4) {
                        groupNumber = @([LibTools uint16From16String:groupIDString]);
                    } else {
                        groupNumber = @([LibTools getVirtualAddressOfLabelUUID:groupIDString]);
                    }
                    if (![tem containsObject:groupNumber]) {
                        [tem addObject:groupNumber];
                    }
                }
            }
        }
        return tem;
    }
}

///新增设备的组号
- (void)addGroupID:(NSNumber *)groupID{
    @synchronized (self) {
        NSArray *allOptions = SigMeshLib.share.dataSource.defaultGroupSubscriptionModels;
        for (NSNumber *modelID in allOptions) {
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //计算组地址
                        UInt16 groupAddress = groupID.intValue;
                        if (modelID.intValue == kSigModel_GenericLevelServer_ID) {
                            if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightCTLServer_ID]] || [element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightLightnessServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress];
                            } else if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightCTLTemperatureServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 1;
                            } else if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightHSLHueServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 2;
                            } else if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightHSLSaturationServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 3;
                            }
                        }
                        
                        //[NSString]->[NSNumber]，因为如果存储的组号不是标准的长度为4的字符串，就会出现匹配组号移除的情况。
                        NSMutableArray *tem = [NSMutableArray array];
                        NSArray *subscribe = [NSArray arrayWithArray:modelIDModel.subscribe];
                        for (NSString *groupIDString in subscribe) {
                            [tem addObject:@([LibTools uint16From16String:groupIDString])];
                        }
                        if (![tem containsObject:groupID]) {
                            [modelIDModel.subscribe addObject:[SigHelper.share getUint16String:groupAddress]];
                        }
                        break;
                    }
                }
            }
        }
    }
}

///删除设备的组号
- (void)deleteGroupID:(NSNumber *)groupID{
    @synchronized (self) {
        NSArray *allOptions = SigMeshLib.share.dataSource.defaultGroupSubscriptionModels;
        for (NSNumber *modelID in allOptions) {
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //计算组地址
                        UInt16 groupAddress = groupID.intValue;
                        if (modelID.intValue == kSigModel_GenericLevelServer_ID) {
                            if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightCTLServer_ID]] || [element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightLightnessServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress];
                            } else if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightCTLTemperatureServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 1;
                            } else if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightHSLHueServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 2;
                            } else if ([element hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightHSLSaturationServer_ID]]) {
                                groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 3;
                            }
                        }
                        
                        //[NSString]->[NSNumber]，因为如果存储的组号不是标准的长度为4的字符串，就会出现匹配组号移除的情况。
                        NSMutableArray *tem = [NSMutableArray array];
                        NSArray *subscribe = [NSArray arrayWithArray:modelIDModel.subscribe];
                        for (NSString *groupIDString in subscribe) {
                            [tem addObject:@([LibTools uint16From16String:groupIDString])];
                        }
                        if ([tem containsObject:@(groupAddress)]) {
                            [modelIDModel.subscribe removeObjectAtIndex:[tem indexOfObject:@(groupAddress)]];
                        }
                        break;
                    }
                }
            }
        }
    }
}

///打开publish功能
- (void)openPublish{
    @synchronized (self) {
        if (self.hasPublishFunction) {
            //存在publish功能
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                BOOL hasPublish = NO;
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *sigModelIDModel in models) {
                    if (sigModelIDModel.getIntModelID == self.publishModelID) {
                        hasPublish = YES;
                        sigModelIDModel.publish = [[SigPublishModel alloc] init];
                        SigRetransmitModel *retransmit = [[SigRetransmitModel alloc] init];
                        retransmit.count = 5;
                        retransmit.interval = (2+1)*50;//4.2.2.7 Publish Retransmit Interval Steps
                        sigModelIDModel.publish.index = 0;
                        sigModelIDModel.publish.credentials = 0;
                        sigModelIDModel.publish.ttl = 0xff;
                        //json数据中，period为publish周期的毫秒数据，默认20秒
                        sigModelIDModel.publish.period.numberOfSteps = SigMeshLib.share.dataSource.defaultPublishPeriodModel.numberOfSteps;
                        sigModelIDModel.publish.period.resolution = SigMeshLib.share.dataSource.defaultPublishPeriodModel.resolution;
                        sigModelIDModel.publish.retransmit = retransmit;
                        sigModelIDModel.publish.address = [NSString stringWithFormat:@"%04lX",(long)kMeshAddress_allNodes];
                        break;
                    }
                }
                if (hasPublish) {
                    break;
                }
            }
        }
    }
}

///关闭publish功能
- (void)closePublish{
    @synchronized (self) {
        if (self.hasPublishFunction) {
            //存在publish功能
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                BOOL hasPublish = NO;
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *sigModelIDModel in models) {
                    if (sigModelIDModel.getIntModelID == self.publishModelID) {
                        sigModelIDModel.publish = nil;
                        hasPublish = YES;
                        break;
                    }
                }
                if (hasPublish) {
                    break;
                }
            }
        }
    }
}

///返回是否支持publish功能
- (BOOL)hasPublishFunction{
    return self.publishModelID != 0;
}

///返回是否支持Direct Forwarding功能
- (BOOL)hasDirectForwardingFunction {
    return [self getAddressesWithModelID:@(kSigModel_DirectedForwardingConfigurationServer_ID)].count >= 0;
}

///返回是否打开了publish功能
- (BOOL)hasOpenPublish{
    @synchronized(self) {
        BOOL tem = NO;
        if (self.hasPublishFunction) {
            //存在publish功能
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                BOOL hasPublish = NO;
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == self.publishModelID) {
                        hasPublish = YES;
                        if (modelIDModel.publish != nil && [LibTools uint16From16String:modelIDModel.publish.address] == 0xffff) {
                            tem = YES;
                        }
                        break;
                    }
                }
                if (hasPublish) {
                    break;
                }
            }
        }
        return tem;
    }
}

///publish是否存在周期上报功能。
- (BOOL)hasPublishPeriod{
    @synchronized(self) {
        BOOL tem = NO;
        if (self.hasPublishFunction) {
            //存在publish功能
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                BOOL hasPublish = NO;
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == self.publishModelID) {
                        hasPublish = YES;
                        if (modelIDModel.publish != nil && [LibTools uint16From16String:modelIDModel.publish.address] == 0xffff) {
                            //注意：period=0时，设备状态改变主动上报；period=1时，设备状态改变主动上报且按周期上报。
                            if (modelIDModel.publish.period != 0) {
                                tem = YES;
                            }
                        }
                        break;
                    }
                }
                if (hasPublish) {
                    break;
                }
            }
        }
        return tem;
    }
}

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source {
    if ([responseMessage isMemberOfClass:[SigGenericOnOffStatus class]]) {
        SigGenericOnOffStatus *message = (SigGenericOnOffStatus *)responseMessage;
        if (message.remainingTime) {
            self.state = message.targetState ? DeviceStateOn : DeviceStateOff;
        } else {
            self.state = message.isOn ? DeviceStateOn : DeviceStateOff;
        }
    } else if ([responseMessage isMemberOfClass:[SigLightLightnessStatus class]]) {
        SigLightLightnessStatus *message = (SigLightLightnessStatus *)responseMessage;
        if (message.remainingTime) {
            self.brightness = message.targetLightness;
        } else {
            self.brightness = message.presentLightness;
        }
        self.state = self.brightness != 0 ? DeviceStateOn : DeviceStateOff;
    } else if ([responseMessage isMemberOfClass:[SigLightCTLStatus class]]) {
        SigLightCTLStatus *message = (SigLightCTLStatus *)responseMessage;
        if (message.remainingTime) {
            self.brightness = message.targetCTLLightness;
            self.temperature = message.targetCTLTemperature;
        } else {
            self.brightness = message.presentCTLLightness;
            self.temperature = message.presentCTLTemperature;
        }
        self.state = self.brightness != 0 ? DeviceStateOn : DeviceStateOff;
    } else if ([responseMessage isMemberOfClass:[SigGenericLevelStatus class]]) {
        SigGenericLevelStatus *message = (SigGenericLevelStatus *)responseMessage;
        SInt16 level = 0;
        if (message.remainingTime) {
            level = message.targetLevel;
        } else {
            level = message.level;
        }
        UInt8 lum = [LibTools levelToLum:level];
        if (source == self.address) {
            //lum
            UInt16 lightness = [LibTools lumToLightness:lum];
            self.brightness = lightness;
            self.HSL_Lightness = lightness;//对于HSL设备，lum改变则HSL_Lightness也需要改变。
            self.state = self.brightness != 0 ? DeviceStateOn : DeviceStateOff;
        } else if (source == self.temperatureAddresses.firstObject.intValue) {
            //temp
            UInt16 temperature = [LibTools temp100ToTemp:lum];
            self.temperature = temperature;
        } else {
            TeLogWarn(@"source address is Undefined.");
        }
    } else if ([responseMessage isMemberOfClass:[SigLightCTLTemperatureStatus class]]) {
        SigLightCTLTemperatureStatus *message = (SigLightCTLTemperatureStatus *)responseMessage;
        if (message.remainingTime) {
            self.temperature = message.targetCTLTemperature;
        } else {
            self.temperature = message.presentCTLTemperature;
        }
    } else if ([responseMessage isMemberOfClass:[SigLightHSLStatus class]]) {
        SigLightHSLStatus *message = (SigLightHSLStatus *)responseMessage;
        self.HSL_Lightness = message.HSLLightness;
        self.HSL_Hue = message.HSLHue;
        self.HSL_Saturation = message.HSLSaturation;
    } else if ([responseMessage isMemberOfClass:[SigTelinkOnlineStatusMessage class]]) {
        SigTelinkOnlineStatusMessage *message = (SigTelinkOnlineStatusMessage *)responseMessage;
        self.state = message.state;
        self.brightness = [LibTools lumToLightness:message.brightness];
        self.temperature = [LibTools temp100ToTemp:message.temperature];
    } else if ([responseMessage isMemberOfClass:[SigLightLightnessLastStatus class]]) {
        SigLightLightnessLastStatus *message = (SigLightLightnessLastStatus *)responseMessage;
        self.brightness = message.lightness;
        self.state = self.brightness != 0 ? DeviceStateOn : DeviceStateOff;
    } else if ([responseMessage isMemberOfClass:[SigLightXyLStatus class]]) {
        SigLightXyLStatus *message = (SigLightXyLStatus *)responseMessage;
        self.brightness = message.xyLLightness;
        self.state = self.brightness != 0 ? DeviceStateOn : DeviceStateOff;
    } else if ([responseMessage isMemberOfClass:[SigLightLCLightOnOffStatus class]]) {
        SigLightLCLightOnOffStatus *message = (SigLightLCLightOnOffStatus *)responseMessage;
        if (message.remainingTime) {
            self.state = message.targetLightOnOff;
        } else {
            self.state = message.presentLightOnOff;
        }
    } else {
//        TeLogWarn(@"Node response status Model is Undefined.");
    }
}

- (void)addDefaultPublicAddressToRemote {
    TeLogInfo(@"addDefaultPublicAddressToRemote");
    for (int i=0; i < self.elements.count; i++) {
        SigElementModel *element = self.elements[i];
        for (SigModelIDModel *model in element.models) {
            if (model.getIntModelID == kSigModel_GenericOnOffClient_ID) {
                SigPublishModel *pM = [[SigPublishModel alloc] init];
                pM.index = SigMeshLib.share.dataSource.curAppkeyModel.index;
                pM.credentials = 0;
                pM.ttl = 0xFF;
                SigRetransmitModel *rM = [[SigRetransmitModel alloc] init];
                rM.count = 5;
                rM.interval = 2;
                pM.retransmit = rM;
                SigPeriodModel *per = [[SigPeriodModel alloc] init];
                per.numberOfSteps = 0;
                per.resolution = 100;
                pM.period = per;
                pM.address = [NSString stringWithFormat:@"%04X",0xC000+i];
                model.publish = pM;
                break;
            }
        }
    }
}

- (BOOL)hasLevelCapability {
    return [self getModelIDModelWithModelID:kSigModel_GenericLevelServer_ID] != nil;
}

- (BOOL)hasLumLevelCapability {
    return [self getElementModelWithModelIds:@[@(kSigModel_LightCTLServer_ID), @(kSigModel_GenericLevelServer_ID)]] || [self getElementModelWithModelIds:@[@(kSigModel_LightLightnessServer_ID), @(kSigModel_GenericLevelServer_ID)]];
}

- (BOOL)hasTempLevelCapability {
    return [self getElementModelWithModelIds:@[@(kSigModel_LightCTLTemperatureServer_ID), @(kSigModel_GenericLevelServer_ID)]];
}

- (BOOL)hasHueLevelCapability {
    return [self getElementModelWithModelIds:@[@(kSigModel_LightHSLHueServer_ID), @(kSigModel_GenericLevelServer_ID)]];
}

- (BOOL)hasSatLevelCapability {
    return [self getElementModelWithModelIds:@[@(kSigModel_LightHSLSaturationServer_ID), @(kSigModel_GenericLevelServer_ID)]];
}

- (SigElementModel *)getElementModelWithModelIds:(NSArray <NSNumber *>*)modelIds {
    SigElementModel *tem = nil;
    NSArray *elements = [NSArray arrayWithArray:self.elements];
    for (SigElementModel *ele in elements) {
        NSMutableArray *mArray = [NSMutableArray array];
        NSArray *models = [NSArray arrayWithArray:ele.models];
        for (SigModelIDModel *model in models) {
            [mArray addObject:@(model.getIntModelID)];
        }
        BOOL mate = YES;
        for (NSNumber *m in modelIds) {
            if (![mArray containsObject:m]) {
                mate = NO;
                break;
            }
        }
        if (mate) {
            tem = ele;
            break;
        }
    }
    return tem;
}

- (void)deleteNetKeyDataFromNode:(SigNodeKeyModel *)netKey {
    //need delete netkey in _netKeys and delete appkey bound this netkey in _appKeys.
    [_netKeys removeObject:netKey];
    NSArray *appKeys = [NSArray arrayWithArray:_appKeys];
    for (SigNodeKeyModel *key in appKeys) {
        SigAppkeyModel *appkey = [SigDataSource.share getAppkeyModelWithAppkeyIndex:key.index];
        if (appkey.boundNetKey == netKey.index) {
            [_appKeys removeObject:key];
        }
    }
}

@end


/// The relay retransmit object represents the parameters of the retransmissions of network layer
/// messages relayed by a mesh node [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.16),
/// 2.1.4.4 Relay retransmit object.
@implementation SigRelayretransmitModel

/// get dictionary from SigRelayretransmitModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigRelayretransmitModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"count"] = @(_relayRetransmitCount);
    dict[@"interval"] = @([self getIntervalOfJsonFile]);
    return dict;
}

/// Set dictionary to SigRelayretransmitModel object.
/// @param dictionary SigRelayretransmitModel dictionary object.
- (void)setDictionaryToSigRelayretransmitModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"count"]) {
        _relayRetransmitCount = [dictionary[@"count"] integerValue];
    }
    if ([allKeys containsObject:@"interval"]) {
        [self setIntervalOfJsonFile:[dictionary[@"interval"] integerValue]];
    }
}

- (UInt8)getIntervalOfJsonFile {
    return (UInt8)(10 * (_relayRetransmitIntervalSteps + 1));
}

- (void)setIntervalOfJsonFile:(UInt8)intervalOfJsonFile {
    _relayRetransmitIntervalSteps = (UInt8)(intervalOfJsonFile / 10) - 1;
}

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _relayRetransmitCount = 5;
        _relayRetransmitIntervalSteps = 2;
    }
    return self;
}

@end


/// The network transmit object represents the parameters of the transmissions of network
/// layer messages originating from a mesh node [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.16),
/// 2.1.4.3 Network transmit object.
@implementation SigNetworktransmitModel

/// get dictionary from SigNetworktransmitModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigNetworktransmitModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"count"] = @(_networkTransmitCount);
    dict[@"interval"] = @([self getIntervalOfJsonFile]);
    return dict;
}

/// Set dictionary to SigNetworktransmitModel object.
/// @param dictionary SigNetworktransmitModel dictionary object.
- (void)setDictionaryToSigNetworktransmitModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"count"]) {
        _networkTransmitCount = [dictionary[@"count"] integerValue];
    }
    if ([allKeys containsObject:@"interval"]) {
        [self setIntervalOfJsonFile:[dictionary[@"interval"] integerValue]];
    }
}

- (UInt8)getIntervalOfJsonFile {
    return (UInt8)(10 * (_networkTransmitIntervalSteps + 1));
}

- (void)setIntervalOfJsonFile:(UInt8)intervalOfJsonFile {
    _networkTransmitIntervalSteps = (UInt8)(intervalOfJsonFile / 10) - 1;
}

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _networkTransmitCount = 5;
        _networkTransmitIntervalSteps = 2;
    }
    return self;
}

@end


/// The features object represents the functionality of a mesh node that is determined by the set
/// features that the node supports [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.15),
/// 2.1.4.2 Features object.
@implementation SigNodeFeatures

- (UInt16)rawValue {
    UInt16 bitField = 0;
    if (_relayFeature != SigNodeFeaturesState_notSupported) {
        bitField |= 0x01;
    }
    if (_proxyFeature != SigNodeFeaturesState_notSupported) {
        bitField |= 0x02;
    }
    if (_friendFeature != SigNodeFeaturesState_notSupported) {
        bitField |= 0x04;
    }
    if (_lowPowerFeature != SigNodeFeaturesState_notSupported) {
        bitField |= 0x08;
    }
    return bitField;
}

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _relayFeature = 2;
        _proxyFeature = 2;
        _friendFeature = 2;
        _lowPowerFeature = 2;
    }
    return self;
}

- (instancetype)initWithRawValue:(UInt16)rawValue {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _relayFeature = (rawValue & 0x01) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_enabled;
        _proxyFeature = (rawValue & 0x02) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_enabled;
        _friendFeature = (rawValue & 0x04) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_enabled;
        _lowPowerFeature = (rawValue & 0x08) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_enabled;
    }
    return self;
}

- (instancetype)initWithRelay:(SigNodeFeaturesState)relayFeature proxy:(SigNodeFeaturesState)proxyFeature friend:(SigNodeFeaturesState)friendFeature lowPower:(SigNodeFeaturesState)lowPowerFeature {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _relayFeature = relayFeature;
        _proxyFeature = proxyFeature;
        _friendFeature = friendFeature;
        _lowPowerFeature = lowPowerFeature;
    }
    return self;
}

/// get dictionary from SigFeatureModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigFeatureModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"proxy"] = @(_proxyFeature);
    dict[@"friend"] = @(_friendFeature);
    dict[@"relay"] = @(_relayFeature);
    dict[@"lowPower"] = @(_lowPowerFeature);
    return dict;
}

/// Set dictionary to SigFeatureModel object.
/// @param dictionary SigFeatureModel dictionary object.
- (void)setDictionaryToSigFeatureModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"proxy"]) {
        _proxyFeature = [dictionary[@"proxy"] integerValue];
    }
    if ([allKeys containsObject:@"friend"]) {
        _friendFeature = [dictionary[@"friend"] integerValue];
    }
    if ([allKeys containsObject:@"relay"]) {
        _relayFeature = [dictionary[@"relay"] integerValue];
    }
    if ([allKeys containsObject:@"lowPower"]) {
        _lowPowerFeature = [dictionary[@"lowPower"] integerValue];
    }
}

@end


/// The node network key object represents the state of a network key distributed to a mesh
/// node by a Mesh Manager.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.15),
/// 2.1.4.1 Node network key object.
@implementation SigNodeKeyModel

- (instancetype)initWithIndex:(UInt16)index updated:(bool)updated {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _index = index;
        _updated = updated;
    }
    return self;
}

/// get dictionary from SigNodeKeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigNodeKeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"index"] = @(_index);
    dict[@"updated"] = [NSNumber numberWithBool:_updated];
    return dict;
}

/// Set dictionary to SigNodeKeyModel object.
/// @param dictionary SigNodeKeyModel dictionary object.
- (void)setDictionaryToSigNodeKeyModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"index"]) {
        _index = (UInt16)[dictionary[@"index"] intValue];
    }
    if ([allKeys containsObject:@"updated"]) {
        _updated = [dictionary[@"updated"] boolValue];
    }
}

/// Determine if the data of two SigNodeKeyModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigNodeKeyModel class]]) {
        //index is the unique identifier of SigNodeKeyModel.
        return _index == ((SigNodeKeyModel *)object).index;
    } else {
        //Two SigNodeKeyModel object is different.
        return NO;
    }
}

@end


/// The element object represents a mesh element that is defined as an addressable entity within
/// a mesh node [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.17),
/// 2.1.4.6 Element object.
@implementation SigElementModel

- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _models = [NSMutableArray array];
    }
    return self;
}

- (instancetype)initWithLocation:(SigLocation)location {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self setSigLocation:location];
        _models = [NSMutableArray array];
        
        // Set temporary index.
        // Final index will be set when Element is added to the Node.
        _index = 0;
    }
    return self;
}

- (instancetype)initWithCompositionData:(NSData *)compositionData offset:(int *)offset {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        // Composition Data must have at least 4 bytes: 2 for Location and one for NumS and NumV.
        if (compositionData && compositionData.length < *offset + 4) {
            TeLogError(@"Composition Data must have at least 4 bytes.");
            return nil;
        }

        // Read location.
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)compositionData.bytes;
        memcpy(&tem16, dataByte+*offset, 2);
        [self setSigLocation:tem16];
        
        // Read NumS and NumV.
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2+*offset, 1);
        UInt8 numS = tem8;
        int sigModelsByteCount = numS * 2;// SIG Model ID is 16-bit long.
        memcpy(&tem8, dataByte+3+*offset, 1);
        UInt8 numV = tem8;
        int vendorModelsByteCount = numV * 4;// Vendor Model ID is 32-bit long.
        
        // Ensure the Composition Data have enough data.
        if (compositionData.length < *offset + 3 + sigModelsByteCount + vendorModelsByteCount) {
            TeLogError(@"the Composition Data have not enough data.");
            return nil;
        }
        
        // 4 bytes have been read.
        *offset += 4;

        // Set temporary index.
        // Final index will be set when Element is added to the Node.
        _index = 0;

        // Read models.
        _models = [NSMutableArray array];
        for (int i=0; i<numS; i++) {
            memcpy(&tem16, dataByte+*offset+i*2, 2);
            SigModelIDModel *modelID = [[SigModelIDModel alloc] initWithSigModelId:tem16];
            [self addModel:modelID];
        }
        *offset += sigModelsByteCount;
        for (int i=0; i<numV; i++) {
            memcpy(&tem16, dataByte+*offset+i*4, 2);
            UInt32 companyId = tem16;
            memcpy(&tem16, dataByte+*offset+i*4+2, 2);
            UInt32 modelId = tem16;
            UInt32 vendorModelId = (UInt32)modelId << 16 | (UInt32)companyId;
            SigModelIDModel *modelID = [[SigModelIDModel alloc] initWithVendorModelId:vendorModelId];
            [self addModel:modelID];
        }
        *offset += vendorModelsByteCount;
    }
    return self;
}

- (void)addModel:(SigModelIDModel *)model {
    [_models addObject:model];
}

- (UInt16)unicastAddress {
    return (UInt16)_index + _parentNodeAddress;
}

- (SigNodeModel * _Nullable)getParentNode {
    return [SigMeshLib.share.dataSource getNodeWithAddress:_parentNodeAddress];
}

- (SigLocation)getSigLocation {
    return [LibTools uint16From16String:self.location];
}

- (void)setSigLocation:(SigLocation)sigLocation {
    self.location = [SigHelper.share getNodeAddressString:sigLocation];
}

/// get dictionary from SigElementModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigElementModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    dict[@"location"] = _location;
    dict[@"index"] = @(_index);
    if (_models) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *models = [NSArray arrayWithArray:_models];
        for (SigModelIDModel *model in models) {
            NSDictionary *modelIDDict = [model getDictionaryOfSigModelIDModel];
            [array addObject:modelIDDict];
        }
        dict[@"models"] = array;
    }
    return dict;
}

/// Set dictionary to SigElementModel object.
/// @param dictionary SigElementModel dictionary object.
- (void)setDictionaryToSigElementModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"location"]) {
        _location = dictionary[@"location"];
    }
    if ([allKeys containsObject:@"index"]) {
        _index = (UInt8)[dictionary[@"index"] intValue];
    }
    if ([allKeys containsObject:@"models"]) {
        NSMutableArray *models = [NSMutableArray array];
        NSArray *array = dictionary[@"models"];
        for (NSDictionary *modelIDDict in array) {
            SigModelIDModel *model = [[SigModelIDModel alloc] init];
            [model setDictionaryToSigModelIDModel:modelIDDict];
            [models addObject:model];
        }
        _models = models;
    }
}

- (NSData *)getElementData {
    UInt16 tem16 = (UInt16)self.getSigLocation;
    NSMutableData *mData = [NSMutableData dataWithBytes:&tem16 length:2];
    NSMutableArray <SigModelIDModel *>*sigModels = [NSMutableArray array];
    NSMutableArray <SigModelIDModel *>*vendorModels = [NSMutableArray array];
    NSArray *models = [NSArray arrayWithArray:self.models];
    for (SigModelIDModel *modelID in models) {
        if (modelID.isBluetoothSIGAssigned) {
            [sigModels addObject:modelID];
        } else {
            [vendorModels addObject:modelID];
        }
    }
    UInt8 tem8 = (UInt8)sigModels.count;
    [mData appendData:[NSData dataWithBytes:&tem8 length:1]];
    tem8 = (UInt8)vendorModels.count;
    [mData appendData:[NSData dataWithBytes:&tem8 length:1]];
    for (SigModelIDModel *model in sigModels) {
        tem16 = model.getIntModelIdentifier;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
    }
    for (SigModelIDModel *model in vendorModels) {
        tem16 = model.getIntCompanyIdentifier;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
        tem16 = model.getIntModelIdentifier;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
    }
    return mData;
}

- (BOOL)hasModelIdString:(NSString *)modelIdString {
    BOOL tem = NO;
    for (SigModelIDModel *model in self.models) {
        if ([model.modelId isEqualToString:modelIdString]) {
            tem = YES;
            break;
        }
    }
    return tem;
}

@end


/// The model object represents a configured state of a mesh model.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.17),
/// 2.1.4.6.1 Model object.
@implementation SigModelIDModel

- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _bind = [NSMutableArray array];
        _subscribe = [NSMutableArray array];
    }
    return self;
}

///返回整形的modelID
- (int)getIntModelID{
    int modelID = 0;
    if (self.isBluetoothSIGAssigned) {
        modelID = [LibTools uint16From16String:self.modelId];
    } else {
        modelID = [LibTools uint32From16String:self.modelId];
    }
    return modelID;
}

- (UInt16)getIntModelIdentifier {
    //sig model:1306 vendor mdoel:00010211
    //ModelID+CID = vendorModelID
    UInt16 tem = 0;
    if (self.isBluetoothSIGAssigned) {
        tem = [LibTools uint16From16String:self.modelId];
    } else if (self.isVendorModelID) {
        tem = [LibTools uint16From16String:[self.modelId substringToIndex:4]];
    }
    return tem;
}

- (UInt16)getIntCompanyIdentifier {
    //ModelID+CID = vendorModelID
    //sig model:1306 vendor mdoel:00010211
    UInt16 tem = 0;
    if (self.isVendorModelID) {
        tem = [LibTools uint16From16String:[self.modelId substringFromIndex:4]];
    }
    return tem;
}

- (instancetype)initWithSigModelId:(UInt16)sigModelId {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _modelId = [SigHelper.share getUint16String:sigModelId];
        _subscribe = [NSMutableArray array];
        _bind = [NSMutableArray array];
        _delegate = nil;
    }
    return self;
}

- (instancetype)initWithVendorModelId:(UInt32)vendorModelId {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _modelId = [SigHelper.share getUint32String:vendorModelId];
        _subscribe = [NSMutableArray array];
        _bind = [NSMutableArray array];
        _delegate = nil;
    }
    return self;
}

- (BOOL)isBluetoothSIGAssigned {
    return _modelId.length == 4;
}

- (BOOL)isVendorModelID {
    return _modelId.length == 8;
}

- (NSArray <SigGroupModel *>*)subscriptions {
    return @[];
}

- (BOOL)isConfigurationServer {
    return [self getIntModelID] == kSigModel_ConfigurationServer_ID;
}

- (BOOL)isConfigurationClient {
    return [self getIntModelID] == kSigModel_ConfigurationClient_ID;
}

- (BOOL)isHealthServer {
    return [self getIntModelID] == kSigModel_HealthServer_ID;
}

- (BOOL)isHealthClient {
    return [self getIntModelID] == kSigModel_HealthClient_ID;
}

/// 返回是否是强制使用deviceKey加解密的modelID，是则无需进行keyBind操作。
- (BOOL)isDeviceKeyModelID {
    BOOL tem = NO;
    int modelID = [self getIntModelID];
    if (modelID == kSigModel_ConfigurationServer_ID ||
        modelID == kSigModel_ConfigurationClient_ID ||
        modelID == kSigModel_RemoteProvisionServer_ID ||
        modelID == kSigModel_RemoteProvisionClient_ID ||
        modelID == kSigModel_DirectedForwardingConfigurationServer_ID ||
        modelID == kSigModel_DirectedForwardingConfigurationClient_ID ||
        modelID == kSigModel_BridgeConfigurationServer_ID ||
        modelID == kSigModel_BridgeConfigurationClient_ID ||
        modelID == kSigModel_MeshPrivateBeaconServer_ID ||
        modelID == kSigModel_MeshPrivateBeaconClient_ID ||
        modelID == kSigModel_OnDemandPrivateProxyServer_ID ||
        modelID == kSigModel_OnDemandPrivateProxyClient_ID ||
        modelID == kSigModel_SARConfigurationServer_ID ||
        modelID == kSigModel_SARConfigurationClient_ID ||
        modelID == kSigModel_OpcodesAggregatorServer_ID ||
        modelID == kSigModel_OpcodesAggregatorClient_ID ||
        modelID == kSigModel_LargeCompositionDataServer_ID ||
        modelID == kSigModel_LargeCompositionDataClient_ID) {
        tem = YES;
    }
    return tem;
}

/// Adds the given Application Key Index to the bound keys.
///
/// - paramter applicationKeyIndex: The Application Key index to bind.
- (void)bindApplicationKeyWithIndex:(UInt16)applicationKeyIndex {
    if ([_bind containsObject:@(applicationKeyIndex)]) {
        TeLogInfo(@"bind exist appIndex.");
        return;
    }
    [_bind addObject:@(applicationKeyIndex)];
}

/// Removes the Application Key binding to with the given Key Index
/// and clears the publication, if it was set to use the same key.
///
/// - parameter applicationKeyIndex: The Application Key index to unbind.
- (void)unbindApplicationKeyWithIndex:(UInt16)applicationKeyIndex {
    if (![_bind containsObject:@(applicationKeyIndex)]) {
        TeLogInfo(@"unbind not exist appIndex.");
        return;
    }
    [_bind removeObject:@(applicationKeyIndex)];
    // If this Application Key was used for publication, the publication has been cancelled.
    if (_publish && _publish.index == applicationKeyIndex) {
        _publish = nil;
    }
}

/// Adds the given Group to the list of subscriptions.
///
/// - parameter group: The new Group to be added.
- (void)subscribeToGroup:(SigGroupModel *)group {
    NSString *address = group.address;
    if ([_subscribe containsObject:address]) {
        TeLogInfo(@"subscribe exist appIndex.");
        return;
    }
    [_subscribe addObject:address];
}

/// Removes the given Group from list of subscriptions.
///
/// - parameter group: The Group to be removed.
- (void)unsubscribeFromGroup:(SigGroupModel *)group {
    NSString *address = group.address;
    if (![_subscribe containsObject:address]) {
        TeLogInfo(@"unsubscribe not exist appIndex.");
        return;
    }
    [_subscribe removeObject:address];
}

/// Removes the given Address from list of subscriptions.
///
/// - parameter address: The Address to be removed.
- (void)unsubscribeFromAddress:(UInt16)address {
    NSString *addressStr = [SigHelper.share getNodeAddressString:address];
    if (![_subscribe containsObject:addressStr]) {
        TeLogInfo(@"unsubscribe not exist appIndex.");
        return;
    }
    [_subscribe removeObject:addressStr];
}

/// Removes all subscribtions from this Model.
- (void)unsubscribeFromAll {
    [_subscribe removeAllObjects];
}

/// Whether the given Application Key is bound to this Model.
///
/// - parameter applicationKey: The key to check.
/// - returns: `True` if the key is bound to this Model,
///            otherwise `false`.
- (BOOL)isBoundToApplicationKey:(SigAppkeyModel *)applicationKey {
    return [self.bind containsObject:@(applicationKey.index)];
}

/// Returns whether the given Model is compatible with the one.
///
/// A compatible Models create a Client-Server pair. I.e., the
/// Generic On/Off Client is compatible to Generic On/Off Server,
/// and vice versa. The rule is that the Server Model has an even
/// Model ID and the Client Model has Model ID greater by 1.
///
/// - parameter model: The Model to compare to.
/// - returns: `True`, if the Models are compatible, `false` otherwise.
- (BOOL)isCompatibleToModel:(SigModelIDModel *)model {
    UInt32 compatibleModelId = ([self getIntModelID] % 2 == 0) ? ([self getIntModelID] + 1) : ([self getIntModelID] - 1);
    return model.getIntModelID == compatibleModelId;
}

/// Returns whether the Model is subscribed to the given Group.
///
/// This method may also return `true` if the Group is not known
/// to the local Provisioner and is not returned using `subscriptions`
/// property.
///
/// - parameter group: The Group to check subscription to.
/// - returns: `True` if the Model is subscribed to the Group,
///            `false` otherwise.
- (BOOL)isSubscribedToGroup:(SigGroupModel *)group {
    return [_subscribe containsObject:group.address];
}

/// Returns whether the Model is subscribed to the given address.
///
/// This method may also return `true` if the address is not known
/// to the local Provisioner and is a Group with this address is
/// not returned using `subscriptions` property.
/// Moreover, if a Virtual Label of a Group is not known, but the
/// 16-bit address is known, and the given address contains the Virtual
/// Label, with the same 16-bit address, this method will return `false`,
/// as it may not guarantee that the labels are the same.
///
/// - parameter address: The address to check subscription to.
/// - returns: `True` if the Model is subscribed to a Group with given,
///            address, `false` otherwise.
- (BOOL)isSubscribedToAddress:(SigMeshAddress *)address {
    BOOL has = NO;
    NSArray *subscriptions = [NSArray arrayWithArray:self.subscriptions];
    for (SigGroupModel *model in subscriptions) {
        if (model.intAddress == address.address) {
            has = YES;
            break;
        }
    }
    return has;
}

/// get dictionary from SigModelIDModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigModelIDModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_modelId) {
        dict[@"modelId"] = _modelId;
    }
    if (_publish) {
        dict[@"publish"] = [_publish getDictionaryOfSigPublishModel];
    }
    if (_bind) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *bind = [NSArray arrayWithArray:_bind];
        for (NSNumber *num in bind) {
            [array addObject:num];
        }
        dict[@"bind"] = array;
    }
    if (_subscribe) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *subscribe = [NSArray arrayWithArray:_subscribe];
        for (NSString *str in subscribe) {
            [array addObject:str];
        }
        dict[@"subscribe"] = array;
    }
    return dict;
}

/// Set dictionary to SigModelIDModel object.
/// @param dictionary SigModelIDModel dictionary object.
- (void)setDictionaryToSigModelIDModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"modelId"]) {
        _modelId = dictionary[@"modelId"];
    }
    if ([allKeys containsObject:@"publish"]) {
        SigPublishModel *publish = [[SigPublishModel alloc] init];
        [publish setDictionaryToSigPublishModel:dictionary[@"publish"]];
        _publish = publish;
    }
    if ([allKeys containsObject:@"bind"]) {
        NSMutableArray *bind = [NSMutableArray array];
        NSArray *array = dictionary[@"bind"];
        for (NSNumber *num in array) {
            [bind addObject:num];
        }
        _bind = bind;
    }
    if ([allKeys containsObject:@"subscribe"]) {
        NSMutableArray *subscribe = [NSMutableArray array];
        NSArray *array = dictionary[@"subscribe"];
        for (NSString *str in array) {
            [subscribe addObject:str];
        }
        _subscribe = subscribe;
    }
}

@end


/// The publish object represents parameters that define how the messages are published
/// by a mesh model [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.18),
/// 2.1.4.6.1.1 Publish object.
@implementation SigPublishModel

- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _index = 0;
        _credentials = 0;
        _ttl = 0;
        SigRetransmitModel *retransmit = [[SigRetransmitModel alloc] init];
        retransmit.count = 0;
        retransmit.interval = 0;
        _retransmit = retransmit;
        _period = [[SigPeriodModel alloc] init];
        _address = @"0000";
    }
    return self;
}

/// get dictionary from SigPublishModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigPublishModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_address) {
        dict[@"address"] = _address;
    }
    dict[@"index"] = @(_index);
    dict[@"credentials"] = @(_credentials);
    dict[@"ttl"] = @(_ttl);
    if (_period) {
        dict[@"period"] = [_period getDictionaryOfSigPeriodModel];
    }
    if (_retransmit) {
        dict[@"retransmit"] = [_retransmit getDictionaryOfSigRetransmitModel];
    }
    return dict;
}

/// Set dictionary to SigPublishModel object.
/// @param dictionary SigPublishModel dictionary object.
- (void)setDictionaryToSigPublishModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"address"]) {
        _address = dictionary[@"address"];
    }
    if ([allKeys containsObject:@"index"]) {
        _index = [dictionary[@"index"] integerValue];
    }
    if ([allKeys containsObject:@"credentials"]) {
        _credentials = [dictionary[@"credentials"] integerValue];
    }
    if ([allKeys containsObject:@"ttl"]) {
        _ttl = [dictionary[@"ttl"] integerValue];
    }
    if ([allKeys containsObject:@"period"]) {
        SigPeriodModel *period = [[SigPeriodModel alloc] init];
        if ([dictionary[@"period"] isKindOfClass:[NSDictionary class]]) {
            [period setDictionaryToSigPeriodModel:dictionary[@"period"]];
            _period = period;
        }
    }
    if ([allKeys containsObject:@"retransmit"]) {
        SigRetransmitModel *retransmit = [[SigRetransmitModel alloc] init];
        [retransmit setDictionaryToSigRetransmitModel:dictionary[@"retransmit"]];
        _retransmit = retransmit;
    }
}

@end


/// The retransmit object is used to describe the number of times a message is published and the
/// interval between retransmissions of the published messages.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.19)x  xXx,
/// 2.1.4.6.1.3 Retransmit object.
@implementation SigRetransmitModel

/// get dictionary from SigRetransmitModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigRetransmitModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"count"] = @(_count);
    dict[@"interval"] = @(_interval);
    return dict;
}

/// Set dictionary to SigRetransmitModel object.
/// @param dictionary SigRetransmitModel dictionary object.
- (void)setDictionaryToSigRetransmitModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"count"]) {
        _count = [dictionary[@"count"] integerValue];
    }
    if ([allKeys containsObject:@"interval"]) {
        _interval = [dictionary[@"interval"] integerValue];
    }
}

@end


/// The publish period object determines the interval at which messages are published by a
/// model and is defined by two values: the number of steps and step resolution.
/// by a mesh model [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.19)x  xXx,
/// 2.1.4.6.1.2 Publish period object.
@implementation SigPeriodModel

/// get dictionary from SigPeriodModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigPeriodModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"numberOfSteps"] = @(_numberOfSteps);
    dict[@"resolution"] = @(_resolution);
    return dict;
}

/// Set dictionary to SigPeriodModel object.
/// @param dictionary SigPeriodModel dictionary object.
- (void)setDictionaryToSigPeriodModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"numberOfSteps"]) {
        _numberOfSteps = [dictionary[@"numberOfSteps"] integerValue];
    }
    if ([allKeys containsObject:@"resolution"]) {
        _resolution = [dictionary[@"resolution"] integerValue];
    }
}

@end


/// The heartbeat publication object represents parameters that define the sending of periodic
/// Heartbeat transport control messages [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.20)x  xXx,
/// 2.1.4.6.1.3 Retransmit object.
@implementation SigHeartbeatPubModel

/// get dictionary from SigHeartbeatPubModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigHeartbeatPubModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_address) {
        dict[@"address"] = _address;
    }
    dict[@"period"] = @(_period);
    dict[@"ttl"] = @(_ttl);
    dict[@"index"] = @(_index);
    if (_features) {
        dict[@"features"] = _features;
    }
    return dict;
}

/// Set dictionary to SigHeartbeatPubModel object.
/// @param dictionary SigHeartbeatPubModel dictionary object.
- (void)setDictionaryToSigHeartbeatPubModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"address"]) {
        _address = dictionary[@"address"];
    }
    if ([allKeys containsObject:@"period"]) {
        _period = [dictionary[@"period"] integerValue];
    }
    if ([allKeys containsObject:@"ttl"]) {
        _ttl = [dictionary[@"ttl"] integerValue];
    }
    if ([allKeys containsObject:@"index"]) {
        _index = [dictionary[@"index"] integerValue];
    }
    if ([allKeys containsObject:@"features"]) {
        _features = dictionary[@"features"];
    }
}

@end


/// The heartbeat subscription object represents parameters that define the receiving of periodical
/// Heartbeat transport control messages [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.21)x  xXx,
/// 2.1.4.8 Heartbeat subscription object.
@implementation SigHeartbeatSubModel

/// get dictionary from SigHeartbeatSubModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigHeartbeatSubModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_source) {
        dict[@"source"] = _source;
    }
    if (_destination) {
        dict[@"destination"] = _destination;
    }
//    dict[@"period"] = @(_period);
    return dict;
}

/// Set dictionary to SigHeartbeatSubModel object.
/// @param dictionary SigHeartbeatSubModel dictionary object.
- (void)setDictionaryToSigHeartbeatSubModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"source"]) {
        _source = dictionary[@"source"];
    }
    if ([allKeys containsObject:@"destination"]) {
        _destination = dictionary[@"destination"];
    }
//    if ([allKeys containsObject:@"period"]) {
//        _period = [dictionary[@"period"] integerValue];
//    }
}

@end


@implementation SigOOBModel

- (instancetype)initWithSourceType:(OOBSourceType)sourceType UUIDString:(NSString *)UUIDString OOBString:(NSString *)OOBString {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _sourceType = sourceType;
        _UUIDString = UUIDString;
        _OOBString = OOBString;
        _lastEditTimeString = [LibTools getNowTimeTimeString];
    }
    return self;
}

- (void)updateWithUUIDString:(NSString *)UUIDString OOBString:(NSString *)OOBString {
    _UUIDString = UUIDString;
    _OOBString = OOBString;
    _lastEditTimeString = [LibTools getNowTimeTimeString];
}

/// Determine if the data of two SigOOBModel is the same
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigOOBModel class]]) {
        //UUIDString is the unique identifier of SigOOBModel.
        return [_UUIDString isEqualToString:((SigOOBModel *)object).UUIDString];
    } else {
        //Two SigOOBModel object is different.
        return NO;
    }
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _sourceType = [coder decodeIntegerForKey:kSigOOBModel_sourceType_key];
        _UUIDString = [coder decodeObjectForKey:kSigOOBModel_UUIDString_key];
        _OOBString = [coder decodeObjectForKey:kSigOOBModel_OOBString_key];
        _lastEditTimeString = [coder decodeObjectForKey:kSigOOBModel_lastEditTimeString_key];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeInteger:_sourceType forKey:kSigOOBModel_sourceType_key];
    [coder encodeObject:_UUIDString forKey:kSigOOBModel_UUIDString_key];
    [coder encodeObject:_OOBString forKey:kSigOOBModel_OOBString_key];
    [coder encodeObject:_lastEditTimeString forKey:kSigOOBModel_lastEditTimeString_key];
}

@end


@implementation SigSubnetBridgeModel

/// get dictionary from SubnetBridgeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSubnetBridgeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"directions"] = [NSNumber numberWithBool:self.directions];
    dict[@"netKeyIndex1"] = [NSString stringWithFormat:@"%04X",self.netKeyIndex1];
    dict[@"netKeyIndex2"] = [NSString stringWithFormat:@"%04X",self.netKeyIndex2];
    dict[@"address1"] = [NSString stringWithFormat:@"%04X",self.address1];
    dict[@"address2"] = [NSString stringWithFormat:@"%04X",self.address2];
    return dict;
}

/// Set dictionary to SubnetBridgeModel object.
/// @param dictionary SubnetBridgeModel dictionary object.
- (void)setDictionaryToSubnetBridgeModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"directions"]) {
        self.directions = (SigDirectionsFieldValues)[dictionary[@"directions"] intValue];
    }
    if ([allKeys containsObject:@"netKeyIndex1"]) {
        _netKeyIndex1 = [LibTools uint16From16String:dictionary[@"netKeyIndex1"]];
    }
    if ([allKeys containsObject:@"netKeyIndex2"]) {
        _netKeyIndex2 = [LibTools uint16From16String:dictionary[@"netKeyIndex2"]];
    }
    if ([allKeys containsObject:@"address1"]) {
        _address1 = [LibTools uint16From16String:dictionary[@"address1"]];
    }
    if ([allKeys containsObject:@"address2"]) {
        _address2 = [LibTools uint16From16String:dictionary[@"address2"]];
    }
}

- (instancetype)initWithDirections:(SigDirectionsFieldValues)directions netKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 address1:(UInt16)address1 address2:(UInt16)address2 {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _directions = directions;
        _netKeyIndex1 = netKeyIndex1;
        _netKeyIndex2 = netKeyIndex2;
        _address1 = address1;
        _address2 = address2;
    }
    return self;
}

/**
 * @brief   Initialize SigSubnetBridgeModel object.
 * @param   parameters    the hex data of SigSubnetBridgeModel.
 * @return  return `nil` when initialize SigSubnetBridgeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 8) {
            UInt8 tem8 = 0;
            UInt16 tem16 = 0;
            UInt32 tem32 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _directions = tem8;
            memcpy(&tem32, dataByte+1, 3);
            _netKeyIndex2 = (tem32 & 0xFFFFFF) >> 12;
            _netKeyIndex1 = (tem32 & 0xFFFFFF) & 0xFFF;
            memcpy(&tem16, dataByte+4, 2);
            _address1 = tem16;
            memcpy(&tem16, dataByte+6, 2);
            _address2 = tem16;
        }
    }
    return self;
}

/// Get description string.
- (NSString *)getDescription {
    NSString *tem = [NSString stringWithFormat:@"Direction:%d(%@)\nNetKeyIndex1:0x%04X\nNetKeyIndex2:0x%04X\nAddress1:0x%04X\nAddress2:0x%04X",_directions,[SigHelper.share getDetailOfSigDirectionsFieldValues:_directions],_netKeyIndex1,_netKeyIndex2,_address1,_address2];
    
    return tem;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem16 = 0;
    UInt32 tem32 = 0;
    tem8 = _directions;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem32 = _netKeyIndex2;
    tem32 = (tem32 << 12) | _netKeyIndex1;
    data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:[data subdataWithRange:NSMakeRange(0, 3)]];
    
    tem16 = _address1;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _address2;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


@implementation SigBridgeSubnetModel

- (instancetype)initWithNetKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _netKeyIndex1 = netKeyIndex1;
        _netKeyIndex2 = netKeyIndex2;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeSubnetModel object.
 * @param   parameters    the hex data of SigBridgeSubnetModel.
 * @return  return `nil` when initialize SigBridgeSubnetModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 3) {
            UInt32 tem32 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem32, dataByte, 3);
            _netKeyIndex2 = (tem32 & 0xFFFFFF) >> 12;
            _netKeyIndex1 = (tem32 & 0xFFFFFF) & 0xFFF;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = 0;
    tem32 = _netKeyIndex2;
    tem32 = (tem32 << 12) | _netKeyIndex1;
    NSData *data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:[data subdataWithRange:NSMakeRange(0, 3)]];
    return mData;
}

@end


@implementation SigBridgedAddressesModel

- (instancetype)initWithAddress1:(UInt16)address1 address2:(UInt16)address2 directions:(SigDirectionsFieldValues)directions {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address1 = address1;
        _address2 = address2;
        _directions = directions;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgedAddressesModel object.
 * @param   parameters    the hex data of SigBridgedAddressesModel.
 * @return  return `nil` when initialize SigBridgedAddressesModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 8) {
            UInt8 tem8 = 0;
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _address1 = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _address2 = tem16;
            memcpy(&tem8, dataByte+4, 1);
            _directions = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem16 = 0;
    tem16 = _address1;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _address2;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _directions;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


@implementation SigOpcodesAggregatorItemModel

- (instancetype)initWithLengthFormat:(BOOL)lengthFormat lengthShort:(UInt8)lengthShort lengthLong:(UInt8)lengthLong opcodeAndParameters:(NSData *)opcodeAndParameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _lengthFormat = lengthFormat;
        _lengthShort = lengthShort;
        _lengthLong = lengthLong;
        _opcodeAndParameters = [NSData dataWithData:opcodeAndParameters];
    }
    return self;
}

- (instancetype)initWithSigMeshMessage:(SigMeshMessage *)meshMessage {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        NSInteger parametersLength = meshMessage.parameters.length;
        NSInteger opcodeAndParametersLength = parametersLength;
        if (meshMessage.opCode <= 0xFF) {
            opcodeAndParametersLength += 1;
        } else if (meshMessage.opCode <= 0xFFFF) {
            opcodeAndParametersLength += 2;
        } else {
            TeLogError(@"meshMessage.opCode is invalid!");
            return nil;
        }
        _lengthFormat = opcodeAndParametersLength > 0x7F;
        if (_lengthFormat) {
            _lengthLong = opcodeAndParametersLength;
        } else {
            _lengthShort = opcodeAndParametersLength;
        }
        
        NSMutableData *mData = [NSMutableData data];
        NSData *opcodeData = [SigHelper.share getOpCodeDataWithUInt32Opcode:meshMessage.opCode];
        [mData appendData:opcodeData];
        [mData appendData:meshMessage.parameters];
        _opcodeAndParameters = mData;
    }
    return self;
}

- (instancetype)initWithOpcodeAndParameters:(NSData *)opcodeAndParameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (opcodeAndParameters && opcodeAndParameters.length > 0) {
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)opcodeAndParameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _lengthFormat = tem8 & 1;
            if (_lengthFormat == NO) {
                _lengthShort = (tem8 & 0b11111110) >> 1;
            } else {
                UInt16 tem16 = 0;
                memcpy(&tem16, dataByte, 2);
                _lengthLong = (tem16 & 0x7FFF) >> 1;
            }
            NSInteger length = _lengthFormat == NO ? (_lengthShort+1) : (_lengthLong+2);
            if (opcodeAndParameters.length < length) {
                TeLogError(@"opcodeAndParameters is invalid!");
                return nil;
            } else {
                _opcodeAndParameters = [NSData dataWithData:[opcodeAndParameters subdataWithRange:NSMakeRange(_lengthFormat == NO ? 1 : 2, _lengthFormat == NO ? _lengthShort : _lengthLong)]];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem16 = 0;
    NSData *data = nil;
    if (_lengthFormat == NO) {
        tem8 = _lengthShort << 1;
        data = [NSData dataWithBytes:&tem8 length:1];
    } else {
        tem16 = (_lengthLong << 1) | 1;
        data = [NSData dataWithBytes:&tem16 length:2];
    }
    [mData appendData:data];
    [mData appendData:_opcodeAndParameters];
    return mData;
}

- (SigMeshMessage *)getSigMeshMessage {
    SigOpCodeAndParametersModel *model = [[SigOpCodeAndParametersModel alloc] initWithOpCodeAndParameters:_opcodeAndParameters];
    return model.getSigMeshMessage;
}

@end


@implementation SigOpCodeAndParametersModel

- (instancetype)initWithOpCodeAndParameters:(NSData *)opCodeAndParameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        // At least 1 octet is required.
        if (opCodeAndParameters == nil || opCodeAndParameters.length == 0) {
            TeLogError(@"opCodeAndParameters has not data.");
            return nil;
        }
        UInt8 octet0 = 0;
        Byte *dataByte = (Byte *)opCodeAndParameters.bytes;
        memcpy(&octet0, dataByte, 1);

        if (octet0 == 0b01111111) {
            TeLogError(@"Opcode 0b01111111 is reseved for future use.");
            return nil;
        }

        _opCodeAndParameters = [NSData dataWithData:opCodeAndParameters];
        // 1-octet Opcodes.
        if ((octet0 & 0x80) == 0) {
            _opCodeSize = 1;
            _opCode = (UInt32)octet0;
            _parameters = [opCodeAndParameters subdataWithRange:NSMakeRange(1, opCodeAndParameters.length - 1)];
            return self;
        }
        // 2-octet Opcodes.
        if ((octet0 & 0x40) == 0) {
            // At least 2 octets are required.
            if (opCodeAndParameters.length < 2) {
                TeLogError(@"opCodeAndParameters is error.");
                return nil;
            }
            _opCodeSize = 2;
            UInt8 octet1 = 0;
            memcpy(&octet1, dataByte+1, 1);
            _opCode = (UInt32)octet0 << 8 | (UInt32)octet1;
            _parameters = [opCodeAndParameters subdataWithRange:NSMakeRange(2, opCodeAndParameters.length - 2)];
            return self;
        }
        // 3-octet Opcodes.
        // At least 3 octets are required.
        if (opCodeAndParameters.length < 3) {
            TeLogError(@"opCodeAndParameters is error.");
            return nil;
        }
        _opCodeSize = 3;
        UInt8 octet1 = 0;
        UInt8 octet2 = 0;
        memcpy(&octet1, dataByte+1, 1);
        memcpy(&octet2, dataByte+2, 1);
        _opCode = (UInt32)octet0 << 16 | (UInt32)octet1 << 8 | (UInt32)octet2;
        _parameters = [opCodeAndParameters subdataWithRange:NSMakeRange(3, opCodeAndParameters.length - 3)];
    }
    return self;
}

- (nullable SigMeshMessage *)getSigMeshMessage {
    Class MessageType = [SigHelper.share getMeshMessageWithOpCode:_opCode];
    if (MessageType != nil) {
        SigMeshMessage *msg = [[MessageType alloc] initWithParameters:_parameters];
        return msg;
    }
    return nil;
}

@end


@implementation GattDateTimeModel

/**
 * @brief   Initialize GattDateTimeModel object.
 * @param   parameters    the hex data of GattDateTimeModel.
 * @return  return `nil` when initialize GattDateTimeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length > 0) {
            UInt8 tem8 = 0;
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            if (parameters.length >= 2) {
                memcpy(&tem16, dataByte, 2);
                _year = tem16;
            }
            if (parameters.length >= 3) {
                memcpy(&tem8, dataByte+2, 1);
                _month = tem8;
            }
            if (parameters.length >= 4) {
                memcpy(&tem8, dataByte+3, 1);
                _day = tem8;
            }
            if (parameters.length >= 5) {
                memcpy(&tem8, dataByte+4, 1);
                _hours = tem8;
            }
            if (parameters.length >= 6) {
                memcpy(&tem8, dataByte+5, 1);
                _minutes = tem8;
            }
            if (parameters.length >= 7) {
                memcpy(&tem8, dataByte+6, 1);
                _seconds = tem8;
            }
        }
    }
    return self;
}

- (instancetype)initWithDate:(NSDate *)date {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:@"yyyy"];
        NSInteger currentYear = [[formatter stringFromDate:date] integerValue];
        [formatter setDateFormat:@"MM"];
        NSInteger currentMonth = [[formatter stringFromDate:date] integerValue];
        [formatter setDateFormat:@"dd"];
        NSInteger currentDay = [[formatter stringFromDate:date] integerValue];
        [formatter setDateFormat:@"HH"];
        NSInteger currentHours = [[formatter stringFromDate:date] integerValue];
        [formatter setDateFormat:@"mm"];
        NSInteger currentMinutes = [[formatter stringFromDate:date] integerValue];
        [formatter setDateFormat:@"ss"];
        NSInteger currentSeconds = [[formatter stringFromDate:date] integerValue];
        NSLog(@"currentDate = %@ ,year = %ld, month=%ld, day=%ld, hours=%ld, minutes=%ld, seconds=%ld", date, currentYear, currentMonth, currentDay, currentHours, currentMinutes, currentSeconds);
        _year = currentYear;
        _month = currentMonth;
        _day = currentDay;
        _hours = currentHours;
        _minutes = currentMinutes;
        _seconds = currentSeconds;
    }
    return self;
}

- (instancetype)initWithYear:(UInt16)year month:(UInt8)month day:(UInt8)day hours:(UInt8)hours minutes:(UInt8)minutes seconds:(UInt8)seconds {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _year = year;
        _month = month;
        _day = day;
        _hours = hours;
        _minutes = minutes;
        _seconds = seconds;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem16 = _year;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _month;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _day;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _hours;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _minutes;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _seconds;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


@implementation GattDayDateTimeModel

/**
 * @brief   Initialize GattDayDateTimeModel object.
 * @param   parameters    the hex data of GattDayDateTimeModel.
 * @return  return `nil` when initialize GattDayDateTimeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length > 0) {
            _dateTime = [[GattDateTimeModel alloc] initWithParameters:parameters];
            if (parameters.length >= 8) {
                UInt8 tem8 = 0;
                Byte *dataByte = (Byte *)parameters.bytes;
                memcpy(&tem8, dataByte+7, 1);
                _dayOfWeek = tem8;
            }
        }
    }
    return self;
}

- (instancetype)initWithDate:(NSDate *)date {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _dateTime = [[GattDateTimeModel alloc] initWithDate:date];
        NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
        NSInteger unitFlags = NSCalendarUnitWeekday ;
        NSDateComponents *comps = [calendar components:unitFlags fromDate:date];
        NSInteger week = [comps weekday];
        NSLog(@"week = %zd",week);
        if (week == 1) {
            _dayOfWeek = GattDayOfWeek_Sunday;
        } else {
            _dayOfWeek = week - 1;
        }
    }
    return self;
}

- (instancetype)initWithYear:(UInt16)year month:(UInt8)month day:(UInt8)day hours:(UInt8)hours minutes:(UInt8)minutes seconds:(UInt8)seconds dayOfWeek:(GattDayOfWeek)dayOfWeek {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _dateTime = [[GattDateTimeModel alloc] initWithYear:year month:month day:day hours:hours minutes:minutes seconds:seconds];
        _dayOfWeek = dayOfWeek;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    if (_dateTime && _dateTime.parameters) {
        [mData appendData:_dateTime.parameters];
    }
    UInt8 tem8 = _dayOfWeek;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


@implementation SigProxyFilterModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _filterType = SigProxyFilerType_whitelist;
        _addressList = [NSMutableArray arrayWithArray:@[@(0xFFFF)]];
    }
    return self;
}

/// get dictionary from SigProxyFilterModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigProxyFilterModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"filterType"] = @(_filterType);
    if (_addressList) {
        dict[@"addressList"] = _addressList;
    }
    return dict;
}

/// Set dictionary to SigProxyFilterModel object.
/// @param dictionary SigProxyFilterModel dictionary object.
- (void)setDictionaryToSigProxyFilterModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"filterType"]) {
        _filterType = [dictionary[@"filterType"] intValue];
    }
    if ([allKeys containsObject:@"addressList"]) {
        _addressList = [NSMutableArray arrayWithArray:dictionary[@"addressList"]];
    }
}

@end


@implementation SigNodeSeqZeroModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _seqAuths = [NSMutableArray array];
    }
    return self;
}

- (instancetype)initWithValue:(NSNumber *)value seqAuth:(NSNumber *)seqAuth {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _value = value;
        _seqAuths = [NSMutableArray arrayWithObject:seqAuth];
    }
    return self;
}

@end


@implementation SigNodeSequenceNumberCacheModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
    }
    return self;
}

@end
