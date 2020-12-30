/********************************************************************************************************
 * @file     SigDataSource.m 
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
//  SigDataSource.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigDataSource.h"
#import "OpenSSLHelper.h"

@interface SigDataSource ()<SigDataSourceDelegate>
@property (nonatomic,assign) UInt32 sequenceNumberOnDelegate;//通过SigDataSourceDelegate回调的sequenceNumber值。
@end

@implementation SigDataSource

+ (SigDataSource *)share{
    static SigDataSource *shareDS = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareDS = [[SigDataSource alloc] init];
    });
    return shareDS;
}

- (instancetype)init{
    if (self = [super init]) {
        _provisioners = [NSMutableArray array];
        _nodes = [NSMutableArray array];
        _groups = [NSMutableArray array];
        _scenes = [NSMutableArray array];
        _netKeys = [NSMutableArray array];
        _appKeys = [NSMutableArray array];
        _scanList = [NSMutableArray array];
        _ivIndex = [NSString stringWithFormat:@"%08X",(unsigned int)kDefaultIvIndex];
        _encryptedArray = [NSMutableArray array];
        _defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:@[@(SIG_MD_G_ONOFF_S),@(SIG_MD_LIGHTNESS_S),@(SIG_MD_LIGHT_CTL_S),@(SIG_MD_LIGHT_CTL_TEMP_S),@(SIG_MD_LIGHT_HSL_S)]];
        _defaultNodeInfos = [NSMutableArray array];
        DeviceTypeModel *model1 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_Panel];
        DeviceTypeModel *model2 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_CT];
        [_defaultNodeInfos addObject:model1];
        [_defaultNodeInfos addObject:model2];
        SigNetkeyModel *netkey = [[SigNetkeyModel alloc] init];
        netkey.key = @"7dd7364cd842ad18c17c74656c696e6b";
        netkey.index = 0;
        netkey.name = @"netkeyA";
        netkey.minSecurity = @"secure";
        _defaultNetKeyA = netkey;
        SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
        appkey.key = @"63964771734fbd76e3b474656c696e6b";
        appkey.index = 0;
        appkey.name = @"appkeyA";
        appkey.boundNetKey = 0;
        _defaultAppKeyA = appkey;
        _defaultIvIndexA = [[SigIvIndex alloc] initWithIndex:0x12345678 updateActive:NO];
        _needPublishTimeModel = YES;
        _defaultUnsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
        
        //OOB
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSData *data = [defaults objectForKey:kOOBStoreKey];
        if (data) {
            NSArray *array = [NSKeyedUnarchiver unarchiveObjectWithData:data];
            if (array && array.count) {
                _OOBList = [NSMutableArray arrayWithArray:array];
            } else {
                _OOBList = [NSMutableArray array];
            }
        } else {
            _OOBList = [NSMutableArray array];
        }
        _addStaticOOBDevcieByNoOOBEnable = YES;
        _defaultRetryCount = 2;
        _defaultAllocatedUnicastRangeHighAddress = kAllocatedUnicastRangeHighAddress;
        _defaultSnoIncrement = kSnoIncrement;
    }
    return self;
}

- (NSDictionary *)getDictionaryFromDataSource {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_meshUUID) {
        dict[@"meshUUID"] = _meshUUID;
    }
    if (_meshName) {
        dict[@"meshName"] = _meshName;
    }
    if (_$schema) {
        dict[@"$schema"] = _$schema;
    }
    if (_version) {
        dict[@"version"] = _version;
    }
    if (_timestamp) {
        dict[@"timestamp"] = _timestamp;
    }
    if (_ivIndex) {
        dict[@"ivIndex"] = _ivIndex;
    }
    if (_netKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
        for (SigNetkeyModel *model in netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfSigNetkeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"netKeys"] = array;
    }
    if (_appKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *appKeys = [NSArray arrayWithArray:_appKeys];
        for (SigAppkeyModel *model in appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfSigAppkeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"appKeys"] = array;
    }
    if (_provisioners) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
        for (SigProvisionerModel *model in provisioners) {
            NSDictionary *provisionDict = [model getDictionaryOfSigProvisionerModel];
            [array addObject:provisionDict];
        }
        dict[@"provisioners"] = array;
    }
    if (_nodes) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (SigNodeModel *model in nodes) {
            NSDictionary *nodeDict = [model getDictionaryOfSigNodeModel];
            [array addObject:nodeDict];
        }
        dict[@"nodes"] = array;
    }
    if (_groups) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *groups = [NSArray arrayWithArray:_groups];
        for (SigGroupModel *model in groups) {
            NSDictionary *groupDict = [model getDictionaryOfSigGroupModel];
            [array addObject:groupDict];
        }
        dict[@"groups"] = array;
    }
    if (_scenes) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *scenes = [NSArray arrayWithArray:_scenes];
        for (SigSceneModel *model in scenes) {
            NSDictionary *sceneDict = [model getDictionaryOfSigSceneModel];
            [array addObject:sceneDict];
        }
        dict[@"scenes"] = array;
    }
    return dict;
}

- (void)setDictionaryToDataSource:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    _curNodes = nil;
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"meshUUID"]) {
        _meshUUID = dictionary[@"meshUUID"];
    }
    if ([allKeys containsObject:@"meshName"]) {
        _meshName = dictionary[@"meshName"];
    }
    if ([allKeys containsObject:@"$schema"]) {
        _$schema = dictionary[@"$schema"];
    }
    if ([allKeys containsObject:@"version"]) {
        _version = dictionary[@"version"];
    }
    if ([allKeys containsObject:@"timestamp"]) {
        _timestamp = dictionary[@"timestamp"];
    }
    if ([allKeys containsObject:@"ivIndex"]) {
        _ivIndex = dictionary[@"ivIndex"];
    }
    if ([allKeys containsObject:@"netKeys"]) {
        NSMutableArray *netKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"netKeys"];
        for (NSDictionary *netkeyDict in array) {
            SigNetkeyModel *model = [[SigNetkeyModel alloc] init];
            [model setDictionaryToSigNetkeyModel:netkeyDict];
            [netKeys addObject:model];
        }
        _netKeys = netKeys;
    }
    if ([allKeys containsObject:@"appKeys"]) {
        NSMutableArray *appKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"appKeys"];
        for (NSDictionary *appkeyDict in array) {
            SigAppkeyModel *model = [[SigAppkeyModel alloc] init];
            [model setDictionaryToSigAppkeyModel:appkeyDict];
            [appKeys addObject:model];
        }
        _appKeys = appKeys;
    }
    if ([allKeys containsObject:@"provisioners"]) {
        NSMutableArray *provisioners = [NSMutableArray array];
        NSArray *array = dictionary[@"provisioners"];
        for (NSDictionary *provisionDict in array) {
            SigProvisionerModel *model = [[SigProvisionerModel alloc] init];
            [model setDictionaryToSigProvisionerModel:provisionDict];
            [provisioners addObject:model];
        }
        _provisioners = provisioners;
    }
    if ([allKeys containsObject:@"nodes"]) {
        NSMutableArray *nodes = [NSMutableArray array];
        NSArray *array = dictionary[@"nodes"];
        for (NSDictionary *nodeDict in array) {
            SigNodeModel *model = [[SigNodeModel alloc] init];
            [model setDictionaryToSigNodeModel:nodeDict];
            [nodes addObject:model];
        }
        _nodes = nodes;
    }
    if ([allKeys containsObject:@"groups"]) {
        NSMutableArray *groups = [NSMutableArray array];
        NSArray *array = dictionary[@"groups"];
        for (NSDictionary *groupDict in array) {
            SigGroupModel *model = [[SigGroupModel alloc] init];
            [model setDictionaryToSigGroupModel:groupDict];
            [groups addObject:model];
        }
        _groups = groups;
    }
    if ([allKeys containsObject:@"scenes"]) {
        NSMutableArray *scenes = [NSMutableArray array];
        NSArray *array = dictionary[@"scenes"];
        for (NSDictionary *sceneDict in array) {
            SigSceneModel *model = [[SigSceneModel alloc] init];
            [model setDictionaryToSigSceneModel:sceneDict];
            [scenes addObject:model];
        }
        _scenes = scenes;
    }
    _curNetkeyModel = nil;
    _curAppkeyModel = nil;
}

- (NSDictionary *)getFormatDictionaryFromDataSource {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_meshUUID) {
        dict[@"meshUUID"] = _meshUUID;
    }
    if (_meshName) {
        dict[@"meshName"] = _meshName;
    }
    if (_$schema) {
        dict[@"$schema"] = _$schema;
    }
    if (_version) {
        dict[@"version"] = _version;
    }
    if (_timestamp) {
        dict[@"timestamp"] = _timestamp;
    }
    if (_ivIndex) {
        dict[@"ivIndex"] = _ivIndex;
    }
    if (_netKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
        for (SigNetkeyModel *model in netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfSigNetkeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"netKeys"] = array;
    }
    if (_appKeys) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *appKeys = [NSArray arrayWithArray:_appKeys];
        for (SigAppkeyModel *model in appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfSigAppkeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"appKeys"] = array;
    }
    if (_provisioners) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
        for (SigProvisionerModel *model in provisioners) {
            NSDictionary *provisionDict = [model getDictionaryOfSigProvisionerModel];
            [array addObject:provisionDict];
        }
        dict[@"provisioners"] = array;
    }
    if (_nodes) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (SigNodeModel *model in nodes) {
            NSDictionary *nodeDict = [model getFormatDictionaryOfSigNodeModel];
            [array addObject:nodeDict];
        }
        dict[@"nodes"] = array;
    }
    if (_groups) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *groups = [NSArray arrayWithArray:_groups];
        for (SigGroupModel *model in groups) {
            NSDictionary *groupDict = [model getDictionaryOfSigGroupModel];
            [array addObject:groupDict];
        }
        dict[@"groups"] = array;
    }
    if (_scenes) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *scenes = [NSArray arrayWithArray:_scenes];
        for (SigSceneModel *model in scenes) {
            NSDictionary *sceneDict = [model getFormatDictionaryOfSigSceneModel];
            [array addObject:sceneDict];
        }
        dict[@"scenes"] = array;
    }
    dict[@"id"] = @"http://www.bluetooth.com/specifications/assigned-numbers/mesh-profile/cdb-schema.json#";
    return dict;
}

- (UInt16)provisionAddress{
    if (!self.curProvisionerModel) {
        TeLogInfo(@"warning: Abnormal situation, there is not provisioner.");
        return kLocationAddress;
    } else {
        UInt16 maxAddr = self.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress;
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (SigNodeModel *node in nodes) {
            NSInteger curMax = node.address + node.elements.count - 1;
            if (curMax > maxAddr) {
                maxAddr = curMax;
            }
        }

        NSMutableDictionary *dict = [[NSUserDefaults standardUserDefaults] objectForKey:kCurrentMeshProvisionAddress_key];
        if (dict && [dict.allKeys containsObject:self.getCurrentProvisionerUUID]) {
            maxAddr = [dict[self.getCurrentProvisionerUUID] intValue];
        }else{
            if (dict == nil) {
                [self saveLocationProvisionAddress:maxAddr];
            }
        }

        //限制短地址的做法：
//        if (maxAddr + 1 <= self.curProvisionerModel.allocatedUnicastRange.firstObject.hightIntAddress) {
//            //Attention: location address is the smallest address of allocatedUnicastRange, app can add new node by use address from smallest address+1.
//            return maxAddr + 1;
//        } else {
//            TeLogInfo(@"warning: Abnormal situation, there is no more address can be use.");
//            return 0;
//        }
        //不限制短地址的做法：
        return maxAddr + 1;
    }
}

- (SigAppkeyModel *)curAppkeyModel{
    if (_curAppkeyModel == nil) {
        //The default use first appkey temporarily
        if (SigDataSource.share.appKeys.count > 0) {
            _curAppkeyModel = SigDataSource.share.appKeys.firstObject;
        }
    }
    return _curAppkeyModel;
}

- (SigNetkeyModel *)curNetkeyModel{
    if (_curNetkeyModel == nil) {
        //The default use first netkey temporarily
        if (SigDataSource.share.netKeys.count > 0) {
            _curNetkeyModel = SigDataSource.share.netKeys.firstObject;
        }
    }
    return _curNetkeyModel;
}

- (SigProvisionerModel *)curProvisionerModel{
    //Practice 1. Temporary default to the first provisioner
//    if (SigDataSource.share.provisioners.count > 0) {
//        return SigDataSource.share.provisioners.firstObject;
//    }
    //Practice 2. get provisioner by location node's uuid.
    NSString *curUUID = [self getCurrentProvisionerUUID];
    NSArray *provisioners = [NSArray arrayWithArray: _provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        if ([provisioner.UUID isEqualToString:curUUID]) {
            return provisioner;
        }
    }
    return nil;
}

- (NSData *)curNetKey{
    if (self.curNetkeyModel) {
        return [LibTools nsstringToHex:self.curNetkeyModel.key];
    }
    return nil;
}

- (NSData *)curAppKey{
    if (self.curAppkeyModel) {
        return [LibTools nsstringToHex:self.curAppkeyModel.key];
    }
    return nil;
}

- (SigNodeModel *)curLocationNodeModel{
    if (SigDataSource.share.curProvisionerModel) {
        NSArray *nodes = [NSArray arrayWithArray: self.nodes];
        for (SigNodeModel *model in nodes) {
            if ([model.UUID isEqualToString:SigDataSource.share.curProvisionerModel.UUID]) {
                return model;
            }
        }
    }
    return nil;
}

- (NSInteger)getOnlineDevicesNumber{
    NSInteger count = 0;
    NSArray *curNodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *model in curNodes) {
        if (model.state != DeviceStateOutOfLine) {
            count ++;
        }
    }
    return count;
}

- (BOOL)hasNodeExistTimeModelID {
    BOOL tem = NO;
    NSArray *curNodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *node in curNodes) {
        UInt32 option = SIG_MD_TIME_S;
        NSArray *elementAddresses = [node getAddressesWithModelID:@(option)];
        if (elementAddresses.count > 0) {
            tem = YES;
            break;
        }
    }
    return tem;
}

///Special handling: store the uuid of current provisioner.
- (void)saveCurrentProvisionerUUID:(NSString *)uuid {
    [[NSUserDefaults standardUserDefaults] setObject:uuid forKey:kCurrenProvisionerUUID_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

///Special handling: get the uuid of current provisioner.
- (NSString *)getCurrentProvisionerUUID{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *uuid = [defaults objectForKey:kCurrenProvisionerUUID_key];
    return uuid;
}

- (NSData *)getLocationMeshData {
    return [NSUserDefaults.standardUserDefaults objectForKey:kSaveLocationDataKey];
}

- (void)saveLocationMeshData:(NSData *)data {
    [NSUserDefaults.standardUserDefaults setObject:data forKey:kSaveLocationDataKey];
    [NSUserDefaults.standardUserDefaults synchronize];
}

/// Init SDK location Data(include create mesh.json, check provisioner, provisionLocation)
- (void)configData{
    NSData *locationData = [self getLocationMeshData];
    BOOL exist = locationData.length > 0;
    if (!exist) {
        //don't exist mesh.json, create and init mesh
        [self initMeshData];
        TeLogInfo(@"creat mesh_sample.json success");
        [self saveLocationData];
    }else{
        //exist mesh.json, load json
        NSData *data = [self getLocationMeshData];
        NSDictionary *meshDict = [LibTools getDictionaryWithJSONData:data];
        [SigDataSource.share setDictionaryToDataSource:meshDict];
        //Attention: it will set _ivIndex to kDefaultIvIndex when mesh.json hasn't the key @"ivIndex"
        if (!_ivIndex || _ivIndex.length == 0) {
            _ivIndex = [NSString stringWithFormat:@"%08X",(unsigned int)kDefaultIvIndex];
            [self saveLocationData];
        }
    }
    //check provisioner
    [self checkExistLocationProvisioner];
    //init SigScanRspModel list
    [self loadScanList];
    //init Bluetooth
    [SigBluetooth share];
}

- (void)initMeshData {
    NSString *timestamp = [LibTools getNowTimeStringOfJson];
    //1.netKeys
    SigNetkeyModel *netkey = [[SigNetkeyModel alloc] init];
    netkey.index = 0;
    netkey.phase = 0;
    netkey.timestamp = timestamp;
    netkey.oldKey = @"00000000000000000000000000000000";
    netkey.key = [LibTools convertDataToHexStr:[LibTools createNetworkKey]];
    netkey.name = @"";
    netkey.minSecurity = @"secure";
    [_netKeys addObject:netkey];

    //2.appKeys
    SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
    appkey.oldKey = @"00000000000000000000000000000000";
    appkey.key = [LibTools convertDataToHexStr:[LibTools initAppKey]];
    appkey.name = @"";
    appkey.boundNetKey = 0;
    appkey.index = 0;
    [_appKeys addObject:appkey];

    //3.provisioners
    SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerMaxHighAddressUnicast:0 andProvisionerUUID:[LibTools convertDataToHexStr:[LibTools initMeshUUID]]];
    [_provisioners addObject:provisioner];

    //4.add new provisioner to nodes
    [self addLocationNodeWithProvisioner:provisioner];

    //5.add default group
    Groups *defultGroup = [[Groups alloc] init];
    for (int i=0; i<defultGroup.groupCount; i++) {
        SigGroupModel *group = [[SigGroupModel alloc] init];
        group.address = [NSString stringWithFormat:@"%04X",0xc000+i];
        group.parentAddress = [NSString stringWithFormat:@"%04X",0];
        group.name = defultGroup.names[i];
        [_groups addObject: group];
    }

    _meshUUID = netkey.key;
    _$schema = @"http://json-schema.org/draft-04/schema#";
    _meshName = @"Telink-Sig-Mesh";
//    _version = LibTools.getSDKVersion;
    _version = @"1.0.0";
    _timestamp = timestamp;
    _ivIndex = [NSString stringWithFormat:@"%08X",(unsigned int)kDefaultIvIndex];
}

- (void)addLocationNodeWithProvisioner:(SigProvisionerModel *)provisioner{
    SigNodeModel *node = [[SigNodeModel alloc] init];

    //init defoult data
    node.UUID = provisioner.UUID;
    node.secureNetworkBeacon = YES;
    node.defaultTTL = TTL_DEFAULT;
    node.features.proxyFeature = SigNodeFeaturesState_notSupported;
    node.features.friendFeature = SigNodeFeaturesState_notEnabled;
    node.features.relayFeature = SigNodeFeaturesState_notSupported;
    node.relayRetransmit.count = 3;
    node.relayRetransmit.interval = 10;
    node.unicastAddress = [NSString stringWithFormat:@"%04X",(UInt16)provisioner.allocatedUnicastRange.firstObject.lowIntAddress];
    node.name = @"Telink iOS provisioner node";
    
    //添加本地节点的element
    NSMutableArray *elements = [NSMutableArray array];
    SigElementModel *element = [[SigElementModel alloc] init];
    element.name = @"Primary Element";
    element.location = @"0000";
    element.index = 0;
    NSMutableArray *models = [NSMutableArray array];
    NSArray *defaultModelIDs = @[@"0000",@"0001",@"0002",@"0003",@"0005",@"FE00",@"FE01",@"FE02",@"FE03",@"FF00",@"FF01",@"1202",@"1001",@"1003",@"1005",@"1008",@"1205",@"1208",@"1302",@"1305",@"1309",@"1311",@"1015",@"00010211"];
    for (NSString *modelID in defaultModelIDs) {
        SigModelIDModel *modelIDModel = [[SigModelIDModel alloc] init];
        modelIDModel.modelId = modelID;
        modelIDModel.subscribe = [NSMutableArray array];
        modelIDModel.bind = [NSMutableArray arrayWithArray:@[@(0)]];
        [models addObject:modelIDModel];
    }
    element.models = models;
    element.parentNodeAddress = node.address;
    [elements addObject:element];
    node.elements = elements;
    
    [self saveCurrentProvisionerUUID:provisioner.UUID];
    NSData *devicekeyData = [LibTools createRandomDataWithLength:16];
    node.deviceKey = [LibTools convertDataToHexStr:devicekeyData];
    SigAppkeyModel *appkey = [SigDataSource.share curAppkeyModel];
    SigNodeKeyModel *nodeAppkey = [[SigNodeKeyModel alloc] init];
    nodeAppkey.index = appkey.index;
    if (![node.appKeys containsObject:nodeAppkey]) {
        [node.appKeys addObject:nodeAppkey];
    }
    
    [_nodes addObject:node];
}

- (void)deleteNodeFromMeshNetworkWithDeviceAddress:(UInt16)deviceAddress{
    @synchronized(self) {
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (SigNodeModel *model in nodes) {
            if (model.address == deviceAddress) {
                [_nodes removeObject:model];
                break;
            }
        }
        NSArray *scenes = [NSArray arrayWithArray:_scenes];
        for (SigSceneModel *scene in scenes) {
//            for (NSString *actionAddress in scene.addresses) {
//                if (actionAddress.intValue == deviceAddress) {
//                    [scene.addresses removeObject:actionAddress];
//                    break;
//                }
//            }
            NSArray *actionList = [NSArray arrayWithArray:scene.actionList];
            for (ActionModel *action in actionList) {
                if (action.address == deviceAddress) {
                    [scene.actionList removeObject:action];
                    break;
                }
            }
        }
        [self saveLocationData];
        [self deleteScanRspModelWithAddress:deviceAddress];
        [self deleteSigEncryptedModelWithAddress:deviceAddress];
    }
}

/// check SigDataSource.provisioners, this api will auto create a provisioner when SigDataSource.provisioners hasn't provisioner corresponding to app's UUID.
- (void)checkExistLocationProvisioner{
    if (self.curProvisionerModel) {
        TeLogInfo(@"exist location provisioner, needn't create");
        //sno添加增量
        [SigDataSource.share setLocationSno:SigDataSource.share.getLocationSno + SigDataSource.share.defaultSnoIncrement];
    }else{
        //don't exist location provisioner, create and add to SIGDataSource.provisioners, then save location.
        //Attention: the max location address is 0x7fff, so max provisioner's allocatedUnicastRange highAddress cann't bigger than 0x7fff.
        if (self.provisioners.count <= 0x7f) {
            SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerMaxHighAddressUnicast:[self getMaxHighAddressUnicast] andProvisionerUUID:[self getCurrentProvisionerUUID]];
            [_provisioners addObject:provisioner];
            [self addLocationNodeWithProvisioner:provisioner];
            _timestamp = [LibTools getNowTimeStringOfJson];
            [self saveLocationData];
        }else{
            TeLogInfo(@"waring: count of provisioners is bigger than 0x7f, app allocates node address will be error.");
        }
    }
}

- (void)changeLocationProvisionerNodeAddressToAddress:(UInt16)address {
    SigNodeModel *node = SigDataSource.share.curLocationNodeModel;
    node.unicastAddress = [NSString stringWithFormat:@"%04X",address];
}

- (NSInteger)getProvisionerCount{
    NSInteger max = 0;
    NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        if (max < provisioner.allocatedUnicastRange.firstObject.hightIntAddress) {
            max = provisioner.allocatedUnicastRange.firstObject.hightIntAddress;
        }
    }
    NSInteger count = (max >> 8) + 1;
    return count;
}

- (UInt16)getMaxHighAddressUnicast {
    UInt16 max = 0;
    NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        if (max < provisioner.allocatedUnicastRange.firstObject.hightIntAddress) {
            max = provisioner.allocatedUnicastRange.firstObject.hightIntAddress;
        }
    }
    return max;
}

- (void)editGroupIDsOfDevice:(BOOL)add unicastAddress:(NSNumber *)unicastAddress groupAddress:(NSNumber *)groupAddress{
    @synchronized(self) {
        SigNodeModel *model = [self getDeviceWithAddress:[unicastAddress intValue]];
        if (model) {
            if (add) {
                if (![model.getGroupIDs containsObject:groupAddress]) {
                    [model addGroupID:groupAddress];
                    [self saveLocationData];
                } else {
                    TeLogInfo(@"add group model fail.");
                }
            } else {
                if (![model.getGroupIDs containsObject:groupAddress]) {
                    TeLogInfo(@"delete group model fail.");
                } else {
                    [model deleteGroupID:groupAddress];
                    [self saveLocationData];
                }
            }
        } else {
            TeLogInfo(@"edit group model fail, node no found.");
        }
    }
}

- (void)addAndSaveNodeToMeshNetworkWithDeviceModel:(SigNodeModel *)model{
    @synchronized(self) {
        if ([_nodes containsObject:model]) {
            NSInteger index = [_nodes indexOfObject:model];
            _nodes[index] = model;
        } else {
            [_nodes addObject:model];
        }
        [self saveLocationData];
    }
}

- (void)setAllDevicesOutline{
    @synchronized(self) {
        _curNodes = nil;
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (SigNodeModel *model in nodes) {
            model.state = DeviceStateOutOfLine;
        }
    }
}

- (void)saveLocationData{
    TeLogDebug(@"");
    @synchronized(self) {
        //sort
        [self.nodes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigNodeModel *)obj1 address] > [(SigNodeModel *)obj2 address];
        }];
        [self.groups sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigGroupModel *)obj1 intAddress] > [(SigGroupModel *)obj2 intAddress];
        }];
        [self.scenes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigSceneModel *)obj1 number] > [(SigSceneModel *)obj2 number];
        }];

        NSDictionary *meshDict = [SigDataSource.share getDictionaryFromDataSource];
        NSData *tempData = [LibTools getJSONDataWithDictionary:meshDict];
        [self saveLocationMeshData:tempData];
        saveMeshJsonData([LibTools getReadableJSONStringWithDictionary:meshDict]);
    }
}

///Special handling: store the uuid and MAC mapping relationship.
- (void)saveScanList{
    NSMutableArray *tem = [NSMutableArray array];
    NSArray *nodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *node in nodes) {
        SigScanRspModel *rsp = [self getScanRspModelWithAddress:node.address];
        if (rsp) {
            [tem addObject:rsp];
        }
    }
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:tem];
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:kScanList_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

///Special handling: load the uuid and MAC mapping relationship.
- (void)loadScanList{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *data = [defaults objectForKey:kScanList_key];
    if (data) {
        NSArray *array = [NSKeyedUnarchiver unarchiveObjectWithData:data];
        if (array && array.count) {
            [self.scanList addObjectsFromArray:array];
        }
    }
}

- (SigNodeModel *)getDeviceWithAddress:(UInt16)address{
    NSArray *curNodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *model in curNodes) {
        if (model.getElementCount > 1) {
            if (model.address <= address && model.address + model.getElementCount - 1 >= address) {
                return model;
            }
        } else {
            if (model.address == address) {
                return model;
            }
        }
    }
    return nil;
}

///nodes should show in HomeViewController
- (NSMutableArray<SigNodeModel *> *)curNodes{
    @synchronized(self) {
        if (_curNodes && _curNodes.count == _nodes.count - _provisioners.count) {
            return _curNodes;
        } else {
            _curNodes = [NSMutableArray array];
            NSArray *nodes = [NSArray arrayWithArray:_nodes];
            for (SigNodeModel *node in nodes) {
                BOOL isProvisioner = NO;
                NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
                for (SigProvisionerModel *provisioner in provisioners) {
                    if (node.UUID && [node.UUID isEqualToString:provisioner.UUID]) {
                        isProvisioner = YES;
                        break;
                    }
                }
                if (isProvisioner) {
                    continue;
                }
                [_curNodes addObject:node];
            }
            return _curNodes;
        }
    }
}

- (void)saveLocationProvisionAddress:(NSInteger)address{
    NSMutableDictionary *dict = [[NSUserDefaults standardUserDefaults] objectForKey:kCurrentMeshProvisionAddress_key];
    if (dict == nil) {
        dict = [NSMutableDictionary dictionary];
    }else{
        dict = [NSMutableDictionary dictionaryWithDictionary:dict];
    }
    [dict setObject:[NSNumber numberWithInteger:address] forKey:self.getCurrentProvisionerUUID];
    [[NSUserDefaults standardUserDefaults] setObject:dict forKey:kCurrentMeshProvisionAddress_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source {
    SigNodeModel *node = [self getDeviceWithAddress:source];
    if (responseMessage && node) {
        [node updateNodeStatusWithBaseMeshMessage:responseMessage source:source];
    }
}

- (UInt16)getNewSceneAddress{
    UInt16 address = 1;
    if (_scenes.count > 0) {
        [_scenes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigSceneModel *)obj1 number] > [(SigSceneModel *)obj2 number];
        }];
        address = _scenes.lastObject.number + 1;
    }
    return address;
}

- (void)saveSceneModelWithModel:(SigSceneModel *)model{
    @synchronized(self) {
        SigSceneModel *scene = [[SigSceneModel alloc] init];
        scene.number = model.number;
        scene.name = model.name;
        scene.actionList = [[NSMutableArray alloc] initWithArray:model.actionList];

        if ([self.scenes containsObject:scene]) {
            NSInteger index = [self.scenes indexOfObject:scene];
            self.scenes[index] = scene;
        } else {
            [self.scenes addObject:scene];
        }
        [self saveLocationData];
    }
}

- (void)delectSceneModelWithModel:(SigSceneModel *)model{
    @synchronized(self) {
        if ([self.scenes containsObject:model]) {
            [self.scenes removeObject:model];
            [self saveLocationData];
        }
    }
}

- (NSData *)getIvIndexData{
    return [LibTools nsstringToHex:_ivIndex];
}

- (void)updateIvIndexString:(NSString *)ivIndexString {
    _ivIndex = ivIndexString;
    [self saveLocationData];
}

- (int)getCurrentProvisionerIntSequenceNumber {
    if (self.curLocationNodeModel) {
        return [self getLocationSno];
    }
    TeLogInfo(@"get sequence fail.");
    return 0;
}

- (void)updateCurrentProvisionerIntSequenceNumber:(int)sequenceNumber {
    if (sequenceNumber < self.getCurrentProvisionerIntSequenceNumber) {
//        TeLogVerbose(@"更新sequenceNumber异常=0x%x",sequenceNumber);
////#warning 2019年12月30日17:11:46，暂时处理：将设备sequenceNumber赋值本地
////        [self setLocationSno:sequenceNumber];
////        [self saveLocationData];
        return;
    }
    if (self.curLocationNodeModel && sequenceNumber != self.getCurrentProvisionerIntSequenceNumber) {
//        TeLogVerbose(@"更新，下一个可用的sequenceNumber=0x%x",sequenceNumber);
        [self setLocationSno:sequenceNumber];
        //sno无需存储json
//        if (sequenceNumber >= self.getLocationSno + SigDataSource.share.defaultSnoIncrement) {
//            [self saveLocationData];
//        }
    }else{
        TeLogVerbose(@"set sequence=0x%x again.",sequenceNumber);
    }
}

- (UInt32)getLocationSno {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *sno = [defaults objectForKey:kCurrenProvisionerSno_key];
    if (!sno) {
        sno = @(0);
    }
//    TeLogVerbose(@"sno=0x%x",sno.intValue);
    return sno.intValue;
}

- (void)setLocationSno:(UInt32)sno {
    if ((sno - _sequenceNumberOnDelegate >= self.defaultSnoIncrement) || (sno < _sequenceNumberOnDelegate)) {
        self.sequenceNumberOnDelegate = sno;
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([weakSelf.delegate respondsToSelector:@selector(onSequenceNumberUpdate:ivIndexUpdate:)]) {
                [weakSelf.delegate onSequenceNumberUpdate:weakSelf.sequenceNumberOnDelegate ivIndexUpdate:[LibTools uint32From16String:SigDataSource.share.ivIndex]];
            }
        });
    }
    //    TeLogVerbose(@"sno=0x%x",(unsigned int)sno);
    [[NSUserDefaults standardUserDefaults] setObject:@(sno) forKey:kCurrenProvisionerSno_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)setIvIndex:(NSString *)ivIndex {
    if (![ivIndex isEqualToString:_ivIndex]) {
        _ivIndex = ivIndex;
        UInt32 newSequenceNumber = 0;
        _sequenceNumberOnDelegate = newSequenceNumber;
        [[NSUserDefaults standardUserDefaults] setObject:@(newSequenceNumber) forKey:kCurrenProvisionerSno_key];
        [[NSUserDefaults standardUserDefaults] synchronize];// save sequenceNumber
        [SigDataSource.share saveLocationData];// save ivIndex
        __block NSString *blockIv = _ivIndex;
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([weakSelf.delegate respondsToSelector:@selector(onSequenceNumberUpdate:ivIndexUpdate:)]) {
                [weakSelf.delegate onSequenceNumberUpdate:weakSelf.sequenceNumberOnDelegate ivIndexUpdate:[LibTools uint32From16String:blockIv]];
            }
        });
    }
}

- (SigEncryptedModel *)getSigEncryptedModelWithAddress:(UInt16)address {
    SigEncryptedModel *tem = nil;
    NSArray *encryptedArray = [NSArray arrayWithArray:_encryptedArray];
    for (SigEncryptedModel *model in encryptedArray) {
        if (model.address == address) {
            return model;
        }
    }
    return tem;
}

- (void)deleteSigEncryptedModelWithAddress:(UInt16)address {
    @synchronized(self) {
        NSArray *encryptedArray = [NSArray arrayWithArray:_encryptedArray];
        for (SigEncryptedModel *model in encryptedArray) {
            if (model.address == address) {
                [_encryptedArray removeObject:model];
                break;
            }
        }
    }
}

///Special handling: determine model whether exist current meshNetwork
- (BOOL)existScanRspModelOfCurrentMeshNetwork:(SigScanRspModel *)model{
    if (model.networkIDData && model.networkIDData.length > 0) {
        if (self.curNetkeyModel.networkId && self.curNetkeyModel.networkId.length > 0) {
            return [self.curNetkeyModel.networkId isEqualToData:model.networkIDData];
        }
        if (self.curNetkeyModel.oldNetworkId && self.curNetkeyModel.oldNetworkId.length > 0) {
            return [self.curNetkeyModel.oldNetworkId isEqualToData:model.networkIDData];
        }
    }else if (model.nodeIdentityData && model.nodeIdentityData.length == 16) {
        return [self matchsWithNodeIdentityData:model.nodeIdentityData peripheralUUIDString:model.uuid];
    }
    return NO;
}

///Special handling: determine peripheralUUIDString whether exist current meshNetwork
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString{
    SigNodeModel *node = [self getNodeWithUUID:peripheralUUIDString];
    return node != nil;
}

- (BOOL)existEncryptedWithNodeIdentityData:(NSData *)nodeIdentityData {
    SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
    tem.identityData = nodeIdentityData;
    return [_encryptedArray containsObject:tem];
}

- (BOOL)matchsWithNodeIdentityData:(NSData *)nodeIdentityData peripheralUUIDString:(NSString *)peripheralUUIDString {
    if ([self existEncryptedWithNodeIdentityData:nodeIdentityData]) {
        return YES;
    } else {
        NSData *hashData = [nodeIdentityData subdataWithRange:NSMakeRange(0, 8)];
        NSData *randomData = [nodeIdentityData subdataWithRange:NSMakeRange(8, 8)];
        return [self matchsWithHashData:hashData randomData:randomData peripheralUUIDString:peripheralUUIDString];
    }
}

- (BOOL)matchsWithHashData:(NSData *)hash randomData:(NSData *)random peripheralUUIDString:(NSString *)peripheralUUIDString {
    NSArray *curNodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *node in curNodes) {
        // Data are: 48 bits of Padding (0s), 64 bit Random and Unicast Address.
        Byte byte[6];
        memset(byte, 0, 6);
        NSData *data = [NSData dataWithBytes:byte length:6];
        NSMutableData *mData = [NSMutableData dataWithData:data];
        [mData appendData:random];
        // 把大端模式的数字Number转为本机数据存放模式
        UInt16 address = CFSwapInt16BigToHost(node.address);;
        data = [NSData dataWithBytes:&address length:2];
        [mData appendData:data];
//        NSLog(@"mdata=%@",mData);
        NSData *encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:self.curNetkeyModel.keys.identityKey];
        BOOL isExist = NO;
        if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
            isExist = YES;
        }
        // If the Key refresh procedure is in place, the identity might have been generated with the old key.
        if (!isExist && self.curNetkeyModel.oldKey && self.curNetkeyModel.oldKey.length > 0 && ![self.curNetkeyModel.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
            encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:self.curNetkeyModel.oldKeys.identityKey];
            if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
                isExist = YES;
            }
        }
        if (isExist) {
            NSMutableData *mData = [NSMutableData dataWithData:hash];
            [mData appendData:random];
            SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
            tem.identityData = mData;
            tem.hashData = hash;
            tem.randomData = random;
            tem.peripheralUUID = peripheralUUIDString;
            tem.encryptedData = encryptedData;
            tem.address = node.address;
            [self deleteSigEncryptedModelWithAddress:node.address];
            [self.encryptedArray addObject:tem];
            return YES;
        }
    }
    return NO;
}

- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model{
    @synchronized(self) {
        if (model.uuid) {
            if ([self.scanList containsObject:model]) {
                NSInteger index = [self.scanList indexOfObject:model];
                SigScanRspModel *oldModel = [self.scanList objectAtIndex:index];
                if (![oldModel.macAddress isEqualToString:model.macAddress]) {
                    if (!model.macAddress || model.macAddress.length != 12) {
                        model.macAddress = oldModel.macAddress;
                    }
                }
                if (oldModel.address != model.address && model.address == 0) {
                    model.address = oldModel.address;
                }
                if (model.provisioned) {
                    if (oldModel.networkIDData && oldModel.networkIDData.length == 8 && (model.networkIDData == nil || model.networkIDData.length != 8)) {
                        model.networkIDData = oldModel.networkIDData;
                    }
                    if (oldModel.nodeIdentityData && oldModel.nodeIdentityData.length == 16 && (model.nodeIdentityData == nil || model.nodeIdentityData.length != 16)) {
                        model.nodeIdentityData = oldModel.nodeIdentityData;
                    }
                }
                if (![oldModel.macAddress isEqualToString:model.macAddress] || oldModel.address != model.address || ![oldModel.networkIDData isEqualToData:model.networkIDData] || ![oldModel.nodeIdentityData isEqualToData:model.nodeIdentityData]) {
                    [self.scanList replaceObjectAtIndex:index withObject:model];
                    [self saveScanList];
                }
            } else {
                [self.scanList addObject:model];
                [self saveScanList];
            }
        }
    }
}

- (SigScanRspModel *)getScanRspModelWithUUID:(NSString *)uuid{
    NSArray *scanList = [NSArray arrayWithArray:_scanList];
    for (SigScanRspModel *model in scanList) {
        if ([model.uuid isEqualToString:uuid]) {
            return model;
        }
    }
    return nil;
}

- (SigScanRspModel *)getScanRspModelWithMac:(NSString *)mac{
    NSArray *scanList = [NSArray arrayWithArray:_scanList];
    for (SigScanRspModel *model in scanList) {
        if ([model.macAddress isEqualToString:mac]) {
            return model;
        }
    }
    return nil;
}

- (SigScanRspModel *)getScanRspModelWithAddress:(UInt16)address{
    NSArray *scanList = [NSArray arrayWithArray:_scanList];
    for (SigScanRspModel *model in scanList) {
        if (model.address == address) {
            return model;
        }
    }
    return nil;
}

- (void)deleteScanRspModelWithAddress:(UInt16)address{
    @synchronized(self) {
        NSArray *scanList = [NSArray arrayWithArray:_scanList];
        for (SigScanRspModel *model in scanList) {
            if (model.address == address) {
                [_scanList removeObject:model];
                break;
            }
        }
        [self saveScanList];
    }
}

- (SigNetkeyModel *)getNetkeyModelWithNetworkId:(NSData *)networkId {
    SigNetkeyModel *tem = nil;
    NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
    for (SigNetkeyModel *model in netKeys) {
        if (model.networkId && [model.networkId isEqualToData:networkId]) {
            tem = model;
            break;
        }else if (model.oldNetworkId && [model.oldNetworkId isEqualToData:networkId]) {
            tem = model;
            break;
        }
    }
    return tem;
}

- (SigNetkeyModel *)getNetkeyModelWithNetkeyIndex:(NSInteger)index {
    SigNetkeyModel *tem = nil;
    NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
    for (SigNetkeyModel *model in netKeys) {
        if (model.index == index) {
            tem = model;
            break;
        }
    }
    return tem;
}

- (SigAppkeyModel *)getAppkeyModelWithAppkeyIndex:(NSInteger)appkeyIndex {
    SigAppkeyModel *model = nil;
    NSArray *appKeys = [NSArray arrayWithArray:_appKeys];
    for (SigAppkeyModel *tem in appKeys) {
        if (tem.index == appkeyIndex) {
            model = tem;
            break;
        }
    }
    return model;
}

- (SigNodeModel *)getNodeWithUUID:(NSString *)uuid{
    NSArray *nodes = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *model in nodes) {
        if ([model.peripheralUUID isEqualToString:uuid]) {
            return model;
        }
    }
    return nil;
}

- (SigNodeModel *)getNodeWithAddress:(UInt16)address{
    NSArray *nodes = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *model in nodes) {
        if (model.elements.count > 1) {
            if (model.address <= address && model.address + model.elements.count - 1 >= address) {
                return model;
            }
        } else {
            if (model.address == address) {
                return model;
            }
        }
    }
    return nil;
}

- (SigNodeModel *)getDeviceWithMacAddress:(NSString *)macAddress{
    NSArray *nodes = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *model in nodes) {
        //peripheralUUID || location node's uuid
        if (macAddress && model.macAddress && [model.macAddress.uppercaseString isEqualToString:macAddress.uppercaseString]) {
            return model;
        }
    }
    return nil;
}

- (SigNodeModel *)getCurrentConnectedNode{
    SigNodeModel *node = [self getNodeWithAddress:self.unicastAddressOfConnected];
    return node;
}

- (ModelIDModel *)getModelIDModel:(NSNumber *)modelID{
    ModelIDs *modelIDs = [[ModelIDs alloc] init];
    NSArray *all = [NSArray arrayWithArray:modelIDs.modelIDs];
    for (ModelIDModel *model in all) {
        if (model.sigModelID == [modelID intValue]) {
            return model;
        }
    }
    return nil;
}

- (SigGroupModel *)getGroupModelWithGroupAddress:(UInt16)groupAddress {
    SigGroupModel *tem = nil;
    NSArray *groups = [NSArray arrayWithArray:_groups];
    for (SigGroupModel *model in groups) {
        if (model.intAddress == groupAddress) {
            tem = model;
            break;
        }
    }
    return tem;
}

- (DeviceTypeModel *)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID {
    DeviceTypeModel *model = nil;
    NSArray *defaultNodeInfos = [NSArray arrayWithArray:_defaultNodeInfos];
    for (DeviceTypeModel *tem in defaultNodeInfos) {
        if (tem.CID == CID && tem.PID == PID) {
            model = tem;
            break;
        }
    }
    return model;
}

#pragma mark - OOB存取相关

- (void)addAndUpdateSigOOBModel:(SigOOBModel *)oobModel {
    if ([self.OOBList containsObject:oobModel]) {
        NSInteger index = [self.OOBList indexOfObject:oobModel];
        [self.OOBList replaceObjectAtIndex:index withObject:oobModel];
    } else {
        [self.OOBList addObject:oobModel];
    }
    [self saveCurrentOobList];
}

- (void)addAndUpdateSigOOBModelList:(NSArray <SigOOBModel *>*)oobModelList {
    for (SigOOBModel *oobModel in oobModelList) {
        if ([self.OOBList containsObject:oobModel]) {
            NSInteger index = [self.OOBList indexOfObject:oobModel];
            [self.OOBList replaceObjectAtIndex:index withObject:oobModel];
        } else {
            [self.OOBList addObject:oobModel];
        }
    }
    [self saveCurrentOobList];
}

- (void)deleteSigOOBModel:(SigOOBModel *)oobModel {
    if ([self.OOBList containsObject:oobModel]) {
        [self.OOBList removeObject:oobModel];
        [self saveCurrentOobList];
    }
}

- (void)deleteAllSigOOBModel {
    [self.OOBList removeAllObjects];
    [self saveCurrentOobList];
}

- (SigOOBModel *)getSigOOBModelWithUUID:(NSString *)UUIDString {
    SigOOBModel *tem = nil;
    NSArray *OOBList = [NSArray arrayWithArray:self.OOBList];
    for (SigOOBModel *oobModel in OOBList) {
        if ([oobModel.UUIDString isEqualToString:UUIDString]) {
            tem = oobModel;
            break;
        }
    }
    return tem;
}

- (void)saveCurrentOobList {
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:self.OOBList];
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:kOOBStoreKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

@end
