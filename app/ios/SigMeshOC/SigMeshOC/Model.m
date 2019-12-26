/********************************************************************************************************
 * @file     Model.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  Model.m
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "Model.h"

@implementation Model

@end

@implementation SIGModel

@end

@implementation ModelIDModel

-(instancetype)initWithModelGroup:(NSString *)modelGroup modelName:(NSString *)modelName sigModelID:(NSInteger)sigModelID{
    if (self = [super init]) {
        _modelGroup = modelGroup;
        _modelName = modelName;
        _sigModelID = sigModelID;
    }
    return self;
}

@end


@implementation ModelIDs

- (instancetype)init{
    if (self = [super init]) {
        //Generic
        _modelIDCount = 52;
        _model1 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic OnOff Server" sigModelID:SIG_MD_G_ONOFF_S];
        _model2 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic OnOff Client" sigModelID:SIG_MD_G_ONOFF_C];
        _model3 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Level Server" sigModelID:SIG_MD_G_LEVEL_S];
        _model4 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Level Client" sigModelID:SIG_MD_G_LEVEL_C];
        _model5 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Default Transition Time Server" sigModelID:SIG_MD_G_DEF_TRANSIT_TIME_S];
        _model6 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Default Transition Time Client" sigModelID:SIG_MD_G_DEF_TRANSIT_TIME_C];
        _model7 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Power OnOff Server" sigModelID:SIG_MD_G_POWER_ONOFF_S];
        _model8 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Power OnOff Setup Server" sigModelID:SIG_MD_G_POWER_ONOFF_SETUP_S];
        _model9 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Power OnOff Client" sigModelID:SIG_MD_G_POWER_ONOFF_C];
        _model10 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Power Level Server" sigModelID:SIG_MD_G_POWER_LEVEL_S];
        _model11 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Power Level Setup Server" sigModelID:SIG_MD_G_POWER_LEVEL_SETUP_S];
        _model12 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Power Level Client" sigModelID:SIG_MD_G_POWER_LEVEL_C];
        _model13 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Battery Server" sigModelID:SIG_MD_G_BAT_S];
        _model14 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Battery Client" sigModelID:SIG_MD_G_BAT_C];
        _model15 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Location Server" sigModelID:SIG_MD_G_LOCATION_S];
        _model16 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Location Setup Server" sigModelID:SIG_MD_G_LOCATION_SETUP_S];
        _model17 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Location Client" sigModelID:SIG_MD_G_LOCATION_C];
        _model18 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Admin Property Server" sigModelID:SIG_MD_G_ADMIN_PROP_S];
        _model19 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Manufacturer Property Server" sigModelID:SIG_MD_G_MFG_PROP_S];
        _model20 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic User Property Server" sigModelID:SIG_MD_G_USER_PROP_S];
        _model21 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Client Property Server" sigModelID:SIG_MD_G_CLIENT_PROP_S];
        _model22 = [[ModelIDModel alloc] initWithModelGroup:@"Generic" modelName:@"Generic Property Client" sigModelID:SIG_MD_G_PROP_C];
        //Sensors
        _model23 = [[ModelIDModel alloc] initWithModelGroup:@"Sensors" modelName:@"Sensor Server" sigModelID:SIG_MD_SENSOR_S];
        _model24 = [[ModelIDModel alloc] initWithModelGroup:@"Sensors" modelName:@"Sensor Setup Server" sigModelID:SIG_MD_SENSOR_SETUP_S];
        _model25 = [[ModelIDModel alloc] initWithModelGroup:@"Sensors" modelName:@"Sensor Client" sigModelID:SIG_MD_SENSOR_C];
        //Time and Scenes
        _model26 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Time and Scenes" sigModelID:SIG_MD_TIME_S];
        _model27 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Time Setup Server" sigModelID:SIG_MD_TIME_SETUP_S];
        _model28 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Time Client" sigModelID:SIG_MD_TIME_C];
        _model29 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Scene Server" sigModelID:SIG_MD_SCENE_S];
        _model30 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Scene Setup Server" sigModelID:SIG_MD_SCENE_SETUP_S];
        _model31 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Scene Client" sigModelID:SIG_MD_SCENE_C];
        _model32 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Scheduler Server" sigModelID:SIG_MD_SCHED_S];
        _model33 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Scheduler Setup Server" sigModelID:SIG_MD_SCHED_SETUP_S];
        _model34 = [[ModelIDModel alloc] initWithModelGroup:@"Time Server" modelName:@"Scheduler Client" sigModelID:SIG_MD_SCHED_C];
        //Lighting
        _model35 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light Lightness Server" sigModelID:SIG_MD_LIGHTNESS_S];
        _model36 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light Lightness Setup Server" sigModelID:SIG_MD_LIGHTNESS_SETUP_S];
        _model37 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light Lightness Client" sigModelID:SIG_MD_LIGHTNESS_C];
        _model38 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light CTL Server" sigModelID:SIG_MD_LIGHT_CTL_S];
        _model39 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light CTL Setup Server" sigModelID:SIG_MD_LIGHT_CTL_SETUP_S];
        _model40 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light CTL Client" sigModelID:SIG_MD_LIGHT_CTL_C];
        _model41 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light CTL Temperature Server" sigModelID:SIG_MD_LIGHT_CTL_TEMP_S];
        _model42 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light HSL Server" sigModelID:SIG_MD_LIGHT_HSL_S];
        _model43 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light HSL Setup Server" sigModelID:SIG_MD_LIGHT_HSL_SETUP_S];
        _model44 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light HSL Client" sigModelID:SIG_MD_LIGHT_HSL_C];
        _model45 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light HSL Hue Server" sigModelID:SIG_MD_LIGHT_HSL_HUE_S];
        _model46 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light HSL Saturation Server" sigModelID:SIG_MD_LIGHT_HSL_SAT_S];
        _model47 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light xyL Server" sigModelID:SIG_MD_LIGHT_XYL_S];
        _model48 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light xyL Setup Server" sigModelID:SIG_MD_LIGHT_XYL_SETUP_S];
        _model49 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light xyL Client" sigModelID:SIG_MD_LIGHT_XYL_C];
        _model50 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light LC Server" sigModelID:SIG_MD_LIGHT_LC_S];
        _model51 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light LC Setup Server" sigModelID:SIG_MD_LIGHT_LC_SETUP_S];
        _model52 = [[ModelIDModel alloc] initWithModelGroup:@"Lighting" modelName:@"Light LC Client" sigModelID:SIG_MD_LIGHT_LC_C];
        
        _modelIDs = @[_model1,_model2,_model3,_model4,_model5,_model6,_model7,_model8,_model9,_model10,_model11,_model12,_model13,_model14,_model15,_model16,_model17,_model18,_model19,_model20,_model21,_model22,_model23,_model24,_model25,_model26,_model27,_model28,_model29,_model30,_model31,_model32,_model33,_model34,_model35,_model36,_model37,_model38,_model39,_model40,_model41,_model42,_model43,_model44,_model45,_model46,_model47,_model48,_model49,_model50,_model51,_model52];
        //        _defaultModelIDs = @[_model1,_model3,_model4,_model35,_model36,_model38,_model39,_model41,_model50,_model51];//默认选中10个
        _defaultModelIDs = _modelIDs;//默认选中所有
        
    }
    return self;
}

@end


@implementation Groups

- (instancetype)init{
    if (self = [super init]) {
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


@implementation SchedulerModel

- (NSDictionary *)getDictionaryOfSchedulerModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"schedulerID"] = @(self.schedulerID);
    dict[@"sceneId"] = @(_sceneId);
    dict[@"schedulerData"] = [LibTools getUint64String:_schedulerData];
    return dict;
}

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

- (instancetype)init{
    if (self = [super init]) {
        _schedulerID = 0;
        _schedulerData = 0;
        _sceneId = 0;
        //set scheduler default time
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

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super init];
    if (self) {
        _schedulerID = [coder decodeIntegerForKey:kSchedulerID_key];
        _schedulerData = [coder decodeInt64ForKey:kSchedulerData_key];
        _sceneId = [coder decodeInt64ForKey:kSceneId_key];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeInteger:_schedulerID forKey:kSchedulerID_key];
    [coder encodeInt64:_schedulerData forKey:kSchedulerData_key];
    [coder encodeInt64:_sceneId forKey:kSceneId_key];
}

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SchedulerModel class]]) {
        return _schedulerID == ((SchedulerModel *)object).schedulerID;
    } else {
        return NO;
    }
}

- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    SchedulerModel *model = [[[self class] alloc] init];
    model.schedulerID = self.schedulerID;
    model.schedulerData = self.schedulerData;
    model.sceneId = self.sceneId;
    return model;
}

- (UInt64)valid_flag_or_idx{
    return (_schedulerData) & 0xF;
}

- (void)setValid_flag_or_idx:(UInt64)valid_flag_or_idx{
    UInt64 tem = 0xF;
    _schedulerData = (_schedulerData & (~(tem))) | ((valid_flag_or_idx & tem));
}

- (UInt64)year{
    return (_schedulerData >> 4) & 0x7F;
}

- (void)setYear:(UInt64)year{
    UInt64 tem = 0x7F;
    _schedulerData = (_schedulerData & (~(tem<<4))) | ((year & tem) << 4);
    
}

- (UInt64)month{
    return (_schedulerData >> 11) & 0xFFF;
}

- (void)setMonth:(UInt64)month{
    UInt64 tem = 0xFFF;
    _schedulerData = (_schedulerData & (~(tem<<11))) | ((month & tem) << 11);
}

- (UInt64)day{
    return (_schedulerData >> 23) & 0x1F;
}

- (void)setDay:(UInt64)day{
    UInt64 tem = 0x1F;
    _schedulerData = (_schedulerData & (~(tem<<23))) | ((day & tem) << 23);
}

- (UInt64)hour{
    return (_schedulerData >> 28) & 0x1F;
}

- (void)setHour:(UInt64)hour{
    UInt64 tem = 0x1F;
    _schedulerData = (_schedulerData & (~(tem<<28))) | ((hour & tem) << 28);
}

- (UInt64)minute{
    return (_schedulerData >> 33) & 0x3F;
}

- (void)setMinute:(UInt64)minute{
    UInt64 tem = 0x3F;
    _schedulerData = (_schedulerData & (~(tem<<33))) | ((minute & tem) << 33);
}

- (UInt64)second{
    return (_schedulerData >> 39) & 0x3F;
}

- (void)setSecond:(UInt64)second{
    UInt64 tem = 0x3F;
    _schedulerData = (_schedulerData & (~(tem<<39))) | ((second & tem) << 39);
}

- (UInt64)week{
    return (_schedulerData >> 45) & 0x7F;
}

- (void)setWeek:(UInt64)week{
    UInt64 tem = 0x7F;
    _schedulerData = (_schedulerData & (~(tem<<45))) | ((week & tem) << 45);
}

- (SchedulerType)action{
    return (_schedulerData >> 52) & 0xF;
}

- (void)setAction:(SchedulerType)action{
    UInt64 tem = 0xF;
    _schedulerData = (_schedulerData & (~(tem<<52))) | ((action & tem) << 52);
}

- (UInt64)transitionTime{
    return (_schedulerData >> 56) & 0xFF;
}

- (void)setTransitionTime:(UInt64)transitionTime{
    UInt64 tem = 0xFF;
    _schedulerData = (_schedulerData & (~tem<<56)) | ((transitionTime & tem) << 56);
}

@end


@implementation SigScanRspModel

- (instancetype)initWithCoder:(NSCoder *)coder
{
    if (self = [super init]) {
        _uuid = [coder decodeObjectForKey:kSigScanRspModel_uuid_key];
        _address = [coder decodeIntegerForKey:kSigScanRspModel_address_key];
        _macAddress = [coder decodeObjectForKey:kSigScanRspModel_mac_key];
        _CID = [coder decodeIntegerForKey:kSigScanRspModel_CID_key];
        _PID = [coder decodeIntegerForKey:kSigScanRspModel_PID_key];
        _networkIDData = [coder decodeObjectForKey:kSigScanRspModel_nodeIdentityData_key];
        _nodeIdentityData = [coder decodeObjectForKey:kSigScanRspModel_networkIDData_key];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_uuid forKey:kSigScanRspModel_uuid_key];
    [coder encodeInteger:_address forKey:kSigScanRspModel_address_key];
    [coder encodeObject:_macAddress forKey:kSigScanRspModel_mac_key];
    [coder encodeInteger:_CID forKey:kSigScanRspModel_CID_key];
    [coder encodeInteger:_PID forKey:kSigScanRspModel_PID_key];
    [coder encodeObject:_networkIDData forKey:kSigScanRspModel_nodeIdentityData_key];
    [coder encodeObject:_nodeIdentityData forKey:kSigScanRspModel_networkIDData_key];
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

 */
- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData{
    self = [super init];
    if (self) {
        _advertisementData = advertisementData;
        _uuid = peripheral.identifier.UUIDString;
//        TeLog(@"advertisementData=%@",advertisementData);
        if ([advertisementData.allKeys containsObject:CBAdvertisementDataManufacturerDataKey]) {
            NSData *allData = advertisementData[CBAdvertisementDataManufacturerDataKey];
            if (allData) {
                if (allData.length >= 6) {
                    _macAddress = [LibTools convertDataToHexStr:[LibTools turnOverData:[allData subdataWithRange:NSMakeRange(0, 6)]]];
                }
                if (allData.length >= 8) {
                    _address = [LibTools uint16From16String:[LibTools convertDataToHexStr:[LibTools turnOverData:[allData subdataWithRange:NSMakeRange(6, 2)]]]];
                }
            }
        }else{
            _macAddress = nil;
            _address = 0;
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
                    BOOL provisionAble = [suuidString  isEqualToString: kPBGATTService];
                    if (provisionAble) {
                        //default bound模式才有CID和PID，普通模式没有CID和PID。
                        if (advDataServiceData.length >= 2) {
                            _CID = [LibTools uint16FromBytes:[advDataServiceData subdataWithRange:NSMakeRange(0, 2)]];
                        }
                        if (advDataServiceData.length >= 4) {
                            _PID = [LibTools uint16FromBytes:[advDataServiceData subdataWithRange:NSMakeRange(2, 2)]];
                        }
                        _address = 0;//no provisioned node, address is zero.
                    } else {
                        UInt8 advType = [LibTools uint8From16String:[LibTools convertDataToHexStr:[LibTools turnOverData:[advDataServiceData subdataWithRange:NSMakeRange(0, 1)]]]];
                        if (advType == 0) {
                            if (advDataServiceData.length >= 9) {
                                _networkIDData = [advDataServiceData subdataWithRange:NSMakeRange(1, 8)];
                            }
                        }else if (advType == 1) {
                            if (advDataServiceData.length >= 17) {
                                _nodeIdentityData = [advDataServiceData subdataWithRange:NSMakeRange(1, 16)];
                            }
                        }
                    }
                }
            }
        }
    }
    return self;
}

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigScanRspModel class]]) {
//        return [_macAddress isEqualToString:[(SigScanRspModel *)object macAddress]];
        return [_uuid isEqualToString:[(SigScanRspModel *)object uuid]];
    } else {
        return NO;
    }
}

- (UInt16)address{
    //非telink的sig mesh设备，广播包是不会存在mac和address的。
    if (_address == 0 && _nodeIdentityData && _nodeIdentityData.length > 0 && _uuid && _uuid.length > 0) {
        for (SigEncryptedModel *model in SigDataSource.share.matchsNodeIdentityArray) {
            if ([model.peripheralUUID isEqualToString:_uuid]) {
                _address = model.address;
                break;
            }
        }
    }
    return _address;
}

@end


@implementation SigRemoteScanRspModel

- (instancetype)initWithPar:(u8 *)par len:(u8)len{
    self = [super init];
    if (self) {
        remote_prov_scan_report_win32 *p_repwin32 = (remote_prov_scan_report_win32 *)par;
        _unicastAddress = p_repwin32->unicast;
        _uuid = [NSData dataWithBytes:p_repwin32->scan_report.uuid length:16];
        _RSSI = p_repwin32->scan_report.rssi;
        _oob = p_repwin32->scan_report.oob;
    }
    return self;
}

- (NSString *)macAddress{
    NSString *tem = nil;
    if (_uuid && _uuid.length >= 6) {
        tem = [LibTools convertDataToHexStr:[LibTools turnOverData:[_uuid subdataWithRange:NSMakeRange(_uuid.length - 6, 6)]]];
    }
    return tem;
}

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigRemoteScanRspModel class]]) {
        return [_uuid isEqualToData:[(SigRemoteScanRspModel *)object uuid]];
    } else {
        return NO;
    }
}

- (NSString *)description{
    return [NSString stringWithFormat:@"unicastAddress=%@,uuid=%@,macAddress=%@,RSSI=%@,OOB=%@",@(self.unicastAddress),_uuid,self.macAddress,@(_RSSI),@(_oob)];
}

@end


@implementation AddDeviceModel
@end


@implementation PublishResponseModel

- (instancetype)initWithResponseData:(NSData *)rspData{
    if (self = [super init]) {
        Byte *pu = (Byte *)rspData.bytes;
        unsigned int temp = 0;
        
        memcpy(&temp, pu + 8+1, 1);
        _status = temp;
        memcpy(&temp, pu + 9+1, 2);
        _elementAddress = temp;
        memcpy(&temp, pu + 11+1, 2);
        _publishAddress = temp;
        memcpy(&temp, pu + 13+1, 2);
        _appKeyIndex = temp>>4;
        memcpy(&temp, pu + 14+1, 1);
        _credentialFlag = (temp>>3)&0b1;
        memcpy(&temp, pu + 14+1, 1);
        _RFU = temp&0b111;
        memcpy(&temp, pu + 15+1, 1);
        _publishTTL = temp;
        memcpy(&temp, pu + 16+1, 1);
        _publishPeriod = temp;
        memcpy(&temp, pu + 17+1, 1);
        _publishRetransmitCount = (temp>>5)&0b111;
        memcpy(&temp, pu + 17+1, 1);
        _publishRetransmitIntervalSteps = temp&0b11111;
        _isVendorModelID = rspData.length > 20;
        memcpy(&temp, pu + 18+1, _isVendorModelID?4:2);
        _modelIdentifier = temp;
    }
    return self;
}

@end


@implementation ActionModel

- (NSDictionary *)getDictionaryOfActionModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"address"] = @(_address);
    dict[@"state"] = @(_state);
    dict[@"trueBrightness"] = @(_trueBrightness);
    dict[@"trueTemperature"] = @(_trueTemperature);
    return dict;
}

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

- (instancetype)initWithNode:(SigNodeModel *)node{
    if (self = [super init]) {
        _address = node.address;
        _state = node.state;
        _trueBrightness = node.trueBrightness;
        _trueTemperature = node.trueTemperature;
    }
    return self;
}

- (BOOL)isSameActionWithAction:(ActionModel *)action{
    if (self.state == action.state && self.trueBrightness == action.trueBrightness && self.trueTemperature == action.trueTemperature) {
        return YES;
    }else{
        return NO;
    }
}

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[ActionModel class]]) {
        return _address == ((ActionModel *)object).address;
    } else {
        return NO;
    }
}

@end


@interface BLEStore()
@property (nonatomic,strong) NSMutableDictionary <CBPeripheral *,NSMutableArray <NSNumber *>*>*RSSICalculate;
@end

@implementation BLEStore

- (instancetype)init
{
    self = [super init];
    if (self) {
        _RSSICalculate = [[NSMutableDictionary alloc] init];
        _scanedPeripherals = [[NSMutableArray alloc] init];
    }
    return self;
}

/// which means finish the step of scan Characteristics, and you can do other thing ,
/// why need judge this
/// if you do't finish scan the Characteristics , and do something others, it'll appear something error unknow
- (BOOL)isWorkNormal{
    if (self.OTACharacteristic != nil && self.PBGATT_InCharacteristic != nil && self.PROXY_InCharacteristic != nil && self.PBGATT_OutCharacteristic != nil && self.PROXY_OutCharacteristic != nil) {
        return YES;
    }
    return NO;
}

- (void)saveToLocal:(CBPeripheral *)tempPeripheral{
    if (![self.scanedPeripherals containsObject:tempPeripheral]) {
        [self.scanedPeripherals addObject:tempPeripheral];
    }
}

- (void)addRSSIWithPeripheral:(CBPeripheral *)peripheral RSSI:(int)rssi{
    if ([self.RSSICalculate.allKeys containsObject:peripheral]) {
        NSMutableArray *tem = self.RSSICalculate[peripheral];
        [tem addObject:@(rssi)];
    } else {
        self.RSSICalculate[peripheral] = [[NSMutableArray alloc] initWithObjects:@(rssi), nil];
    }
}

- (void)clearRecord{
    [self.RSSICalculate removeAllObjects];
}

- (CBPeripheral *)hightestRSSI{
    if (!self.RSSICalculate || self.RSSICalculate.count == 0) {
        return nil;
    }
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    for (CBPeripheral *key in self.RSSICalculate.allKeys) {
        NSMutableArray *sortRSSIs = self.RSSICalculate[key];
        [sortRSSIs sortUsingComparator:^NSComparisonResult(NSNumber *obj1, NSNumber *obj2) {
            return obj1.intValue < obj2.intValue;
        }];
        int low = (int)(floor(0.2 * (double)(sortRSSIs.count)));
        int hight = (int)(floor(0.1 * (double)(sortRSSIs.count)));
        [sortRSSIs removeObjectsInRange:NSMakeRange(0, low)];
        [sortRSSIs removeObjectsInRange:NSMakeRange(sortRSSIs.count - hight, hight)];
        int rv = 0;
        for (NSNumber *r in sortRSSIs) {
            rv += r.intValue;
        }
        float averageRSSI = rv / (float)sortRSSIs.count;
        NSMutableArray *keyValue = [[NSMutableArray alloc] init];
        [keyValue addObject:key];
        [keyValue addObject:@(averageRSSI)];
        [tem addObject:keyValue];
    }
    [tem sortUsingComparator:^NSComparisonResult(NSMutableArray *obj1, NSMutableArray *obj2) {
        return ((NSNumber *)obj1.lastObject).floatValue < ((NSNumber *)obj2.lastObject).floatValue;
    }];
    return ((NSMutableArray *)tem.firstObject).firstObject;
}

- (void)ressetParameters {
    self.OTACharacteristic = nil;
    self.PBGATT_InCharacteristic = nil;
    self.PBGATT_OutCharacteristic = nil;
    self.PROXY_InCharacteristic = nil;
    self.PROXY_OutCharacteristic = nil;
    [self clearRecord];
}

- (void)libBeaconBack:(BOOL)available{
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self.beaconBackCallBack) {
            self.beaconBackCallBack(available);
        }
    });
}

- (void)writeForPROXYIn:(NSData *)d{
    if (self.PROXY_InCharacteristic) {
        TeLog(@"app PROXY writeValue:%@",d);
        [self.peripheral writeValue:d forCharacteristic:self.PROXY_InCharacteristic type:CBCharacteristicWriteWithoutResponse];
    }else{
        TeLog(@"app don't found PROXY_InCharacteristic");
    }
}

- (void)writeForPBGATTIn:(NSData *)d{
    if (self.PBGATT_InCharacteristic) {
        TeLog(@"app PBGATT writeValue:%@",d);
        [self.peripheral writeValue:d forCharacteristic:self.PBGATT_InCharacteristic type:CBCharacteristicWriteWithoutResponse];
    }else{
        TeLog(@"app don't found PBGATT_InCharacteristic");
    }
}

- (void)writeForOnlineStatus:(NSData *)d{
    if (self.OnlineStatusCharacteristic) {
        TeLog(@"app's OnlineStatusCharacteristic writeValue:%@",d);
        [self.peripheral writeValue:d forCharacteristic:self.OnlineStatusCharacteristic type:CBCharacteristicWriteWithResponse];
    }else{
        TeLog(@"app don't found OnlineStatusCharacteristic");
    }
}

- (void)writeOTAData:(NSData *)d{
    if (self.OTACharacteristic) {
//        TeLog(@"app writeValue:%@",d);
        [self.peripheral writeValue:d forCharacteristic:self.OTACharacteristic type:CBCharacteristicWriteWithoutResponse];
    }else{
        TeLog(@"app don't found OTACharacteristic");
    }
}

- (void)writeMeshOTAData:(NSData *)d{
    if (self.MeshOTACharacteristic) {
        //        TeLog(@"app meshOTA writeValue:%@",d);
        [self.peripheral writeValue:d forCharacteristic:self.MeshOTACharacteristic type:CBCharacteristicWriteWithoutResponse];
    }else{
        TeLog(@"app don't found MeshOTACharacteristic");
    }
}

- (void)readOTACharachteristicForContinue{
    if (self.OTACharacteristic) {
//        TeLog(@"app readOTACharachteristicForContinue");
        [self.peripheral readValueForCharacteristic:self.OTACharacteristic];
    }else{
        TeLog(@"app don't found OTACharacteristic");
    }
}

@end


@implementation RangeModel

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super init];
    if (self) {
        _lowAddress = [coder decodeObjectForKey:kJsonRangeLow_key];
        _highAddress = [coder decodeObjectForKey:kJsonRangeHigh_key];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_lowAddress forKey:kJsonRangeLow_key];
    [coder encodeObject:_highAddress forKey:kJsonRangeHigh_key];
}

@end

@implementation DeviceTypeModel

static Byte PanelByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x07, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00
    , (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff
    , (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12
    , (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12
    , (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10
    , (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00};
static Byte CTByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00
    , (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff
    , (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10
    , (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13
    , (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};

- (instancetype)initWithCID:(UInt16)cid PID:(SigNodePID)pid{
    if (self = [super init]) {
        _CID = cid;
        _PID = pid;
        if (cid == kCompanyID) {
            VC_node_info_t node_info = {};
            memset(&node_info, 0xff, sizeof(VC_node_info_t));
            
            if (pid == SigNodePID_Panel) {
                //set default VC_node_info_t of panel
                memcpy(&node_info.cps.page0_head.cid, PanelByte, sizeof(PanelByte));
            }else if (pid == SigNodePID_CT) {
                //set default VC_node_info_t of CT
                memcpy(&node_info.cps.page0_head.cid, CTByte, sizeof(CTByte));
            }
            _defultNodeInfo = node_info;
        }
    }
    return self;
}

- (void)setDefultNodeInfoData:(NSData *)data {
    VC_node_info_t node_info = {};
    memset(&node_info, 0xff, sizeof(VC_node_info_t));
    Byte *byte = (Byte *)data.bytes;
    int length = (int)data.length;
    memcpy(&node_info.cps.page0_head.cid, byte, length);
    _defultNodeInfo = node_info;
}

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[DeviceTypeModel class]]) {
        return _CID == ((DeviceTypeModel *)object).CID && _PID == ((DeviceTypeModel *)object).PID;
    } else {
        return NO;
    }
}

@end
