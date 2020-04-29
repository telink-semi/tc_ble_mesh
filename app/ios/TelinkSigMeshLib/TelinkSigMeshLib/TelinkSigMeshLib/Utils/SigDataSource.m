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
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigDataSource.h"
#import "OpenSSLHelper.h"

@implementation SigEncryptedModel
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigEncryptedModel class]]) {
        return [_identityData isEqualToData:((SigEncryptedModel *)object).identityData];
    } else {
        return NO;
    }
}
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

        _ivIndex = @"11223344";
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
        netkey.minSecurity = @"high";
        _defaultNetKeyA = netkey;
        SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
        appkey.key = @"63964771734fbd76e3b474656c696e6b";
        appkey.index = 0;
        appkey.name = @"appkeyA";
        appkey.boundNetKey = 0;
        _defaultAppKeyA = appkey;
        _defaultIvIndexA = [[SigIvIndex alloc] initWithIndex:0x12345678 updateActive:NO];
        _needPublishTimeModel = NO;
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
    //The default use first appkey temporarily
    if (SigDataSource.share.appKeys.count > 0) {
        return SigDataSource.share.appKeys.firstObject;
    }
    return nil;
}

- (SigNetkeyModel *)curNetkeyModel{
    //The default use first netkey temporarily
    if (SigDataSource.share.netKeys.count > 0) {
        return SigDataSource.share.netKeys.firstObject;
    }
    return nil;
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
- (void)saveCurrentProvisionerUUID:(NSString *)uuid{
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
        //Attention: it will set _ivIndex to @"11223344" when mesh.json hasn't the key @"ivIndex"
        if (!_ivIndex || _ivIndex.length == 0) {
            _ivIndex = @"11223344";
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

- (void)initMeshData{
    NSString *timestamp = [LibTools getNowTimeTimestampFrome2000];
    //1.netKeys
    SigNetkeyModel *netkey = [[SigNetkeyModel alloc] init];
    netkey.oldKey = @"";
    netkey.index = 0;
    netkey.phase = 0;
    netkey.timestamp = timestamp;
    netkey.key = [LibTools convertDataToHexStr:[LibTools createNetworkKey]];
    netkey.name = @"";
    netkey.minSecurity = @"high";
    [_netKeys addObject:netkey];

    //2.appKeys
    SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
    appkey.oldKey = @"";
    appkey.key = [LibTools convertDataToHexStr:[LibTools initAppKey]];
    appkey.name = @"";
    appkey.boundNetKey = 0;
    appkey.index = 0;
    [_appKeys addObject:appkey];

    //3.provisioners
    SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerCount:0 andProvisionerUUID:[LibTools convertDataToHexStr:[LibTools initMeshUUID]]];
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
    _$schema = @"telink-semi.com";
    _meshName = @"Telink-Sig-Mesh";
    _version = LibTools.getSDKVersion;
    _timestamp = timestamp;
    _ivIndex = @"11223344";
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
    node.relayRetransmit.interval = 0;
    
    //添加本地节点的element
    NSMutableArray *elements = [NSMutableArray array];
    SigElementModel *element = [[SigElementModel alloc] init];
    element.name = @"Primary Element";
    element.location = 0;
    element.index = 0;
    NSMutableArray *models = [NSMutableArray array];
    NSArray *defaultModelIDs = @[@"0000",@"0001",@"0002",@"0003",@"0005",@"FE00",@"FE01",@"FE02",@"FE03",@"FF00",@"FF01",@"1202",@"1001",@"1003",@"1005",@"1008",@"1205",@"1208",@"1302",@"1305",@"1309",@"1311",@"1015",@"00010000"];
    for (NSString *modelID in defaultModelIDs) {
        SigModelIDModel *modelIDModel = [[SigModelIDModel alloc] init];
        modelIDModel.modelId = modelID;
        modelIDModel.subscribe = [NSMutableArray array];
        modelIDModel.bind = [NSMutableArray arrayWithArray:@[@(0)]];
        [models addObject:modelIDModel];
    }
    element.models = models;
    element.parentNode = node;
    [elements addObject:element];
    node.elements = elements;
    
    [self saveCurrentProvisionerUUID:provisioner.UUID];
    node.unicastAddress = [NSString stringWithFormat:@"%04X",(UInt16)provisioner.allocatedUnicastRange.firstObject.lowIntAddress];
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
        [SigDataSource.share setLocationSno:SigDataSource.share.getLocationSno + kSnoIncrement];
    }else{
        //don't exist location provisioner, create and add to SIGDataSource.provisioners, then save location.
        //Attention: the max location address is 0x7fff, so max provisioner's allocatedUnicastRange highAddress cann't bigger than 0x7fff.
        if (self.provisioners.count <= 0x7f) {
            SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerCount:[self getProvisionerCount] andProvisionerUUID:[self getCurrentProvisionerUUID]];
            [_provisioners addObject:provisioner];
            [self addLocationNodeWithProvisioner:provisioner];
            _timestamp = [LibTools getNowTimeTimestampFrome2000];
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
        if (model.nodeInfo.element_cnt > 1) {
            if (model.address <= address && model.address + model.nodeInfo.element_cnt - 1 >= address) {
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

- (void)updateIvIndexData:(NSData *)ivIndex{
    _ivIndex = [LibTools convertDataToHexStr:ivIndex];
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
        TeLogVerbose(@"更新，下一个可用的sequenceNumber=0x%x",sequenceNumber);
        [self setLocationSno:sequenceNumber];
        //sno无需存储json
//        if (sequenceNumber >= self.getLocationSno + kSnoIncrement) {
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
//    TeLogVerbose(@"sno=0x%x",(unsigned int)sno);
    [[NSUserDefaults standardUserDefaults] setObject:@(sno) forKey:kCurrenProvisionerSno_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
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
        if (self.curNetkeyModel.oldKey && self.curNetkeyModel.oldKey.length > 0 && ![self.curNetkeyModel.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
            encryptedData = [OpenSSLHelper.share calculateEvalueWithData:data andKey:self.curNetkeyModel.oldKeys.identityKey];
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

@end

@implementation SigNetkeyDerivaties

- (SigNetkeyDerivaties *)initWithNetkeyData:(NSData *)key helper:(OpenSSLHelper *)helper {
    if (self = [super init]) {
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

    }
    return self;
}

@end

@implementation SigNetkeyModel

- (NSDictionary *)getDictionaryOfSigNetkeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    dict[@"index"] = @(_index);
    if (_key) {
        dict[@"key"] = _key;
    }
    if (_oldKey) {
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

- (instancetype)init {
    if (self = [super init]) {
        SigIvIndex *ivIndex = [[SigIvIndex alloc] initWithIndex:kDefaultIvIndex updateActive:NO];
        _ivIndex = ivIndex;
    }
    return self;
}

- (SigIvIndex *)ivIndex {
    return [[SigIvIndex alloc] initWithIndex:[LibTools uint32From16String:SigDataSource.share.ivIndex] updateActive:NO];
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

- (SigNetkeyDerivaties *)keys {
    if (!_keys && self.key && self.key.length > 0 && ![self.key isEqualToString:@"00000000000000000000000000000000"]) {
        _keys = [[SigNetkeyDerivaties alloc] initWithNetkeyData:[LibTools nsstringToHex:self.key] helper:OpenSSLHelper.share];
    }
    return _keys;
}

- (SigNetkeyDerivaties *)oldKeys {
    if (!_oldKey && self.oldKey && self.oldKey.length > 0 && ![self.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
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

- (BOOL)isPrimary {
    return _index == 0;
}

@end


@implementation SigProvisionerModel

- (NSDictionary *)getDictionaryOfSigProvisionerModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_provisionerName) {
        dict[@"provisionerName"] = _provisionerName;
    }
    if (_UUID) {
        dict[@"UUID"] = _UUID;
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

- (void)setDictionaryToSigProvisionerModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"provisionerName"]) {
        _provisionerName = dictionary[@"provisionerName"];
    }
    if ([allKeys containsObject:@"UUID"]) {
        _UUID = dictionary[@"UUID"];
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
 create new provisioner by count of exist provisioners.
 
 @param count count of exist provisioners
 @param provisionerUUID new provisioner's uuid
 @return SigProvisionerModel model
 */
-(instancetype)initWithExistProvisionerCount:(UInt16)count andProvisionerUUID:(NSString *)provisionerUUID{
    if (self = [super init]) {
        self.allocatedGroupRange = [NSMutableArray array];
        SigRangeModel *range1 = [[SigRangeModel alloc] init];
        //做法1：不同的Provisioner使用不同的组地址范围
//        range1.lowAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeLowAddress + count*0x100];
//        range1.highAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeHighAddress + count*0x100];
        //做法2：不同的Provisioner都使用同一组组地址
        range1.lowAddress = [NSString stringWithFormat:@"%04lX",(long)kAllocatedGroupRangeLowAddress];
        range1.highAddress = [NSString stringWithFormat:@"%04lX",(long)kAllocatedGroupRangeHighAddress];
        [self.allocatedGroupRange addObject:range1];
        
        //短地址分配范围：1-0xff，0x0100-0x01ff，0x0200-0x02ff，0x0300-0x03ff， 。。。
        self.allocatedUnicastRange = [NSMutableArray array];
        SigRangeModel *range2 = [[SigRangeModel alloc] init];
        range2.lowAddress = [NSString stringWithFormat:@"%04X",kAllocatedUnicastRangeLowAddress + (count == 0 ? 0 : (count*(kAllocatedUnicastRangeHighAddress+1)-1))];
        range2.highAddress = [NSString stringWithFormat:@"%04X",kAllocatedUnicastRangeHighAddress + (count == 0 ? 0 : count*(kAllocatedUnicastRangeHighAddress+1))];
        [self.allocatedUnicastRange addObject:range2];
        
        self.allocatedSceneRange = [NSMutableArray array];
        SigSceneRangeModel *range3 = [[SigSceneRangeModel alloc] init];
        range3.lowAddress = kAllocatedSceneRangeLowAddress;
        range3.highAddress = kAllocatedSceneRangeHighAddress;
        [self.allocatedSceneRange addObject:range3];
                
        self.UUID = provisionerUUID;
        self.provisionerName = @"";
    }
    return self;
}


- (SigNodeModel *)node {
    SigNodeModel *tem = nil;
    NSArray *nodes = [NSArray arrayWithArray:SigDataSource.share.nodes];
    for (SigNodeModel *model in nodes) {
        if ([model.UUID isEqualToString:_UUID]) {
            tem = model;
            break;
        }
    }
    return tem;
}

@end


@implementation SigRangeModel

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

- (NSInteger)lowIntAddress{
    return [LibTools uint16From16String:self.lowAddress];
}

- (NSInteger)hightIntAddress{
    return [LibTools uint16From16String:self.highAddress];
}

@end


@implementation SigSceneRangeModel

- (NSDictionary *)getDictionaryOfSigSceneRangeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"lowAddress"] = @(_lowAddress);
    dict[@"highAddress"] = @(_highAddress);
    return dict;
}

- (void)setDictionaryToSigSceneRangeModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"lowAddress"]) {
        _lowAddress = [dictionary[@"lowAddress"] integerValue];
    }
    if ([allKeys containsObject:@"highAddress"]) {
        _highAddress = [dictionary[@"highAddress"] integerValue];
    }
}

@end


@implementation SigAppkeyModel

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

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigAppkeyModel class]]) {
        return [_key isEqualToString:((SigAppkeyModel *)object).key];
    } else {
        return NO;
    }
}

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

- (SigNetkeyModel *)getCurrentBoundNetKey {
    SigNetkeyModel *tem = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigDataSource.share.netKeys];
    for (SigNetkeyModel *model in netKeys) {
        if (model.index == _boundNetKey) {
            tem = model;
            break;
        }
    }
    return tem;
}

- (NSData *)getDataKey {
    if (_key != nil && _key.length > 0 && ![_key isEqualToString:@"00000000000000000000000000000000"]) {
        return [LibTools nsstringToHex:_key];
    }
    return nil;
}

- (NSData *)getDataOldKey {
    if (_oldKey != nil && _oldKey.length > 0 && ![_oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        return [LibTools nsstringToHex:_oldKey];
    }
    return nil;
}

@end


@implementation SigSceneModel

- (NSDictionary *)getDictionaryOfSigSceneModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    dict[@"number"] = @(_number);
    if (_addresses) {
        NSMutableArray *array = [NSMutableArray array];
        NSMutableArray *addresses = [NSMutableArray arrayWithArray:_addresses];
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

- (void)setDictionaryToSigSceneModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"name"]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"number"]) {
        _number = [dictionary[@"number"] integerValue];
    }
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
    }
}

- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigSceneModel class]]) {
        return _number == ((SigSceneModel *)object).number;
    } else {
        return NO;
    }
}

- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    SigSceneModel *model = [[[self class] alloc] init];
    model.addresses = [[NSMutableArray alloc] initWithArray:self.addresses];
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


@implementation SigGroupModel

- (instancetype)init {
    if (self = [super init]) {
        _groupBrightness = 100;
        _groupTempareture = 100;
    }
    return self;
}

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

- (UInt16)intAddress{
    return [LibTools uint16From16String:self.address];
}

- (BOOL)isOn{
    BOOL tem = NO;
    for (SigNodeModel *model in self.groupDevices) {
        if (model.state == DeviceStateOn) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (NSMutableArray <SigNodeModel *>*)groupDevices{
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    for (SigNodeModel *model in SigDataSource.share.curNodes) {
        if ([model.getGroupIDs containsObject:@(self.intAddress)]) {
            [tem addObject:model];
        }
    }
    return tem;
}

- (NSMutableArray <SigNodeModel *>*)groupOnlineDevices{
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    for (SigNodeModel *model in SigDataSource.share.curNodes) {
        if ([model.getGroupIDs containsObject:@(self.intAddress)] && model.state != DeviceStateOutOfLine) {
            [tem addObject:model];
        }
    }
    return tem;
}

@end


@implementation SigNodeModel{
    UInt16 _address;
}

@synthesize nodeInfo = _nodeInfo;

- (instancetype)init{
    if (self = [super init]) {
        _elements = [NSMutableArray array];
        _netKeys = [NSMutableArray array];
        _appKeys = [NSMutableArray array];

        _schedulerList = [[NSMutableArray alloc] init];
        _keyBindModelIDs = [[NSMutableArray alloc] init];

        _state = DeviceStateOutOfLine;
        _macAddress = @"";
        _name = @"";
        _features = [[SigNodeFeatures alloc] init];
        _relayRetransmit = [[SigRelayretransmitModel alloc] init];
        _networkTransmit = [[SigNetworktransmitModel alloc] init];
        
        SigNodeKeyModel *nodeNetkey = [[SigNodeKeyModel alloc] init];
        nodeNetkey.index = 0;
        [_netKeys addObject:nodeNetkey];
        _secureNetworkBeacon = NO;
        _configComplete = NO;
        _blacklisted = NO;
        _HSL_Hue = 0xffff-1;
        _HSL_Saturation = 0xffff;
        _HSL_Lightness = 0xffff;
//        _sno = @"00000000";
    }
    return self;
}


- (instancetype)initWithNode:(SigNodeModel *)node
{
    self = [super init];
    if (self) {
        _features = node.features;
        _unicastAddress = node.unicastAddress;
        _secureNetworkBeacon = node.secureNetworkBeacon;
        _relayRetransmit = node.relayRetransmit;
        _networkTransmit = node.networkTransmit;
        _configComplete = node.configComplete;
        _vid = node.vid;
        _cid = node.cid;
        _blacklisted = node.blacklisted;
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
//        _sno = node.sno;
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
    device.blacklisted = self.blacklisted;
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
//    device.sno = self.sno;
    
    return device;
}

//Attention: 1.it is use peripheralUUID to compare SigNodeModel when SigScanRspModel.macAddress is nil.
//Attention: 2.it is use macAddress to compare SigNodeModel when peripheralUUID is nil.
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[SigNodeModel class]]) {
        SigNodeModel *tem = (SigNodeModel *)object;
        if (self.peripheralUUID && self.peripheralUUID.length > 0 && tem.peripheralUUID && tem.peripheralUUID.length > 0) {
            return [self.peripheralUUID isEqualToString:tem.peripheralUUID];
        }else if (self.macAddress && self.macAddress.length > 0 && tem.macAddress && tem.macAddress.length > 0) {
            return [self.macAddress.uppercaseString isEqualToString:tem.macAddress.uppercaseString];
        }
        return NO;
    } else {
        return NO;
    }
}

- (BOOL)isSensor{
    return self.nodeInfo.cps.page0_head.pid == 0x201;
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
        [SigDataSource.share saveLocationData];
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
    return [self getAddressesWithModelID:@(SIG_MD_G_ONOFF_S)];
}

- (NSMutableArray<NSNumber *> *)levelAddresses{
    return [self getAddressesWithModelID:@(SIG_MD_G_LEVEL_S)];
}

- (NSMutableArray<NSNumber *> *)temperatureAddresses{
    return [self getAddressesWithModelID:@(SIG_MD_LIGHT_CTL_TEMP_S)];
}

- (NSMutableArray<NSNumber *> *)HSLAddresses{
    return [self getAddressesWithModelID:@(SIG_MD_LIGHT_HSL_S)];
}

- (NSMutableArray<NSNumber *> *)lightnessAddresses{
    return [self getAddressesWithModelID:@(SIG_MD_LIGHTNESS_S)];
}

- (NSMutableArray<NSNumber *> *)schedulerAddress{
    return [self getAddressesWithModelID:@(SIG_MD_SCHED_S)];
}

- (NSMutableArray<NSNumber *> *)sceneAddress{
    return [self getAddressesWithModelID:@(SIG_MD_SCENE_S)];
}

- (NSMutableArray<NSNumber *> *)publishAddress{
    return [self getAddressesWithModelID:@(self.publishModelID)];
}

///publish首选SIG_MD_LIGHT_CTL_S，次选SIG_MD_LIGHT_HSL_S，SIG_MD_LIGHTNESS_S，SIG_MD_G_ONOFF_S
- (UInt16)publishModelID{
    UInt16 tem = 0;
    if ([self getAddressesWithModelID:@(SIG_MD_LIGHT_CTL_S)].count > 0) {
        tem = (UInt16)SIG_MD_LIGHT_CTL_S;
    } else if ([self getAddressesWithModelID:@(SIG_MD_LIGHT_HSL_S)].count > 0){
        tem = (UInt16)SIG_MD_LIGHT_HSL_S;
    } else if ([self getAddressesWithModelID:@(SIG_MD_LIGHTNESS_S)].count > 0){
        tem = (UInt16)SIG_MD_LIGHTNESS_S;
    } else if ([self getAddressesWithModelID:@(SIG_MD_G_ONOFF_S)].count > 0){
        tem = (UInt16)SIG_MD_G_ONOFF_S;
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
                if (modelID.getIntModelIdentifier == sigModelID.intValue) {
                    [array addObject:@(self.address+i)];
                    break;
                }
            }
        }
    }
    return array;
}

- (NSString *)peripheralUUID{
    if (self.address == SigDataSource.share.curLocationNodeModel.address) {
        //location node's uuid
        return _UUID;
    }
    //new code:use in v3.0.0 and later
    SigEncryptedModel *model = [SigDataSource.share getSigEncryptedModelWithAddress:self.address];
    _peripheralUUID = model.peripheralUUID;
    if ((!_peripheralUUID || _peripheralUUID.length == 0) && self.address != 0) {
        SigScanRspModel *rspModel = [SigDataSource.share getScanRspModelWithAddress:self.address];
        _peripheralUUID = rspModel.uuid;
        if ((!_peripheralUUID || _peripheralUUID.length == 0) && self.macAddress != nil && self.macAddress.length > 0) {
            rspModel = [SigDataSource.share getScanRspModelWithMac:self.macAddress];
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
        SigScanRspModel *model = [SigDataSource.share getScanRspModelWithUUID:_peripheralUUID];
        if (model) {
            tem = model.macAddress;
        }
    }
    if (tem == nil) {
        if (self.address != 0) {
            SigScanRspModel *model = [SigDataSource.share getScanRspModelWithAddress:self.address];
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
    NSArray *netKeys = [NSArray arrayWithArray:SigDataSource.share.netKeys];
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

- (UInt16)lastUnicastAddress {
    // Provisioner may not have any elements
    UInt16 allocatedAddresses = _elements.count > 0 ? _elements.count : 1;
    return self.address + allocatedAddresses - 1;
}

- (BOOL)hasAllocatedAddr:(UInt16)addr {
    return addr >= self.address && addr <= self.lastUnicastAddress;
}

- (SigModelIDModel *)getModelIDModelWithModelID:(UInt16)modelID {
    SigModelIDModel *model = nil;
    NSArray *elements = [NSArray arrayWithArray:self.elements];
    for (SigElementModel *element in elements) {
        element.parentNode = self;
        NSArray *all = [NSArray arrayWithArray:element.models];
        for (SigModelIDModel *tem in all) {
            tem.parentElement = element;
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

- (SigModelIDModel *)getModelIDModelWithModelID:(UInt16)modelID andElementAddress:(UInt16)elementAddress {
    SigModelIDModel *model = nil;
    NSArray *elements = [NSArray arrayWithArray:self.elements];
    for (SigElementModel *element in elements) {
        element.parentNode = self;
        if (element.unicastAddress == elementAddress) {
            NSArray *all = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *tem in all) {
                tem.parentElement = element;
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
    dict[@"blacklisted"] = [NSNumber numberWithBool:_blacklisted];
    if (_UUID) {
        dict[@"UUID"] = [LibTools meshUUIDToUUID:_UUID];
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
//    if (_sno) {
//        dict[@"sno"] = _sno;
//    }
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
    if (_schedulerList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *schedulerList = [NSArray arrayWithArray:_schedulerList];
        for (SchedulerModel *model in schedulerList) {
            NSDictionary *schedulerDict = [model getDictionaryOfSchedulerModel];
            [array addObject:schedulerDict];
        }
        dict[@"schedulerList"] = array;
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
    if ([allKeys containsObject:@"blacklisted"]) {
        _blacklisted = [dictionary[@"blacklisted"] boolValue];
    }
    if ([allKeys containsObject:@"UUID"]) {
        _UUID = [LibTools meshUUIDToUUID:dictionary[@"UUID"]];
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
//    if ([allKeys containsObject:@"sno"]) {
//        _sno = dictionary[@"sno"];
//    }

    if ([allKeys containsObject:@"elements"]) {
        NSMutableArray *elements = [NSMutableArray array];
        NSArray *array = dictionary[@"elements"];
        for (NSDictionary *elementDict in array) {
            SigElementModel *model = [[SigElementModel alloc] init];
            [model setDictionaryToSigElementModel:elementDict];
            if (model.index != elements.count) {
                model.index = elements.count;
            }
            model.parentNode = self;
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
}

- (void)setCompositionData:(SigPage0 *)compositionData {
    _nodeInfo.node_adr = 0;//if node_adr is 0, nodeInfo get will reCalculation _nodeInfo.
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
        element.parentNode = self;
        NSArray *all = [NSArray arrayWithArray:element.models];
        for (SigModelIDModel *modelID in all) {
            [self setBindSigNodeKeyModel:self.appKeys.firstObject toSigModelIDModel:modelID];
        }
        [array addObject:element];
    }
    self.elements = array;
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

- (UInt16)address{
    if (_address == 0) {
        _address = [LibTools uint16From16String:self.unicastAddress];
    }
    return _address;
}

- (void)setAddress:(UInt16)address{
    _address = address;
    self.unicastAddress = [NSString stringWithFormat:@"%04X",address];
}

//- (int)getIntSNO{
//    if (self.sno) {
//        return [LibTools uint32From16String :self.sno];
//    } else {
//        return 0;
//    }
//}
//
//- (void)setIntSno:(UInt32)intSno{
//    self.sno = [NSString stringWithFormat:@"%08X",(unsigned int)intSno];
//}

- (VC_node_info_t)nodeInfo{
    if (self.appKeys.count > 0) {
        if (_nodeInfo.node_adr != 0) {
            return _nodeInfo;
        }
    }
    
    VC_node_info_t node_info = {};
    //_nodeInfo默认赋值ff
    memset(&node_info, 0xff, sizeof(VC_node_info_t));
    node_info.node_adr = [LibTools uint16From16String:self.unicastAddress];
    node_info.element_cnt = self.elements.count;//即使keyBind失败，也通过json发送的无效elements数组的元素个数获取element_cnt，防止加灯出现address重叠的现象。
    node_info.rsv = 0xff;
    memcpy(&node_info.dev_key, [LibTools nsstringToHex:self.deviceKey].bytes, 16);
    NSMutableArray *eleModelIDArray = [NSMutableArray array];
    NSInteger cpsLength = 2+8;
    if (self.appKeys.count > 0) {
        //keyBind成功
        NSArray *elements = [NSArray arrayWithArray:self.elements];
        for (SigElementModel *element in elements) {
            NSMutableArray *sigModelIDs = [NSMutableArray array];
            NSMutableArray *vendorModelIDs = [NSMutableArray array];
            NSArray *all = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *modelIDModel in all) {
                if (modelIDModel.modelId.length <= 4) {
                    [sigModelIDs addObject:@([LibTools uint16From16String:modelIDModel.modelId])];
                } else {
                    [vendorModelIDs addObject:@([LibTools uint32From16String:modelIDModel.modelId])];
                }
            }
            [eleModelIDArray addObject:@{@"modelIDs":sigModelIDs,@"vendorIDs":vendorModelIDs}];
            if (sigModelIDs.count > 0) {
                cpsLength += 2 * sigModelIDs.count;
            }
            if (vendorModelIDs.count > 0) {
                cpsLength += 4 * vendorModelIDs.count;
            }
            if ([self.elements indexOfObject:element] != self.elements.count - 1) {
                cpsLength += 2+1+1;
            }
        }
    }else{
        //keybind失败，模拟生成element_cnt个element，每个element值有一个modelID，为“0000”。
        NSMutableArray *sigModelIDs = [NSMutableArray arrayWithObject:@(0)];
        NSMutableArray *vendorModelIDs = [NSMutableArray array];
        [eleModelIDArray addObject:@{@"modelIDs":sigModelIDs,@"vendorIDs":vendorModelIDs}];
        if (sigModelIDs.count > 0) {
            cpsLength += 2 * sigModelIDs.count;
        }
        if (vendorModelIDs.count > 0) {
            cpsLength += 4 * vendorModelIDs.count;
        }
    }
    node_info.cps.len_cps = cpsLength;
    if (self.cid && self.cid.length > 0) {
        node_info.cps.page0_head.cid = (UInt16)[LibTools uint16From16String:self.cid];
    } else {
        node_info.cps.page0_head.cid = kCompanyID;
    }
    if (self.pid && self.pid.length > 0) node_info.cps.page0_head.pid = [LibTools uint16From16String:self.pid];
    if (self.vid && self.vid.length > 0) node_info.cps.page0_head.vid = (UInt16)[LibTools uint16From16String:self.vid];
    if (self.crpl && self.crpl.length > 0) node_info.cps.page0_head.crpl = (UInt16)[LibTools uint16From16String:self.crpl];
    node_info.cps.page0_head.feature.relay = self.features.relayFeature == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.proxy = self.features.proxyFeature == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.frid = self.features.friendFeature == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.low_power = self.features.lowPowerFeature == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.rfu = 0;
    node_info.cps.local = 0;
    if (eleModelIDArray.count > 0) {
        node_info.cps.model_id.nums = [(NSArray *)eleModelIDArray.firstObject[@"modelIDs"] count];
        node_info.cps.model_id.numv = [(NSArray *)eleModelIDArray.firstObject[@"vendorIDs"] count];
        NSMutableData *modelIDData = [NSMutableData data];
        UInt16 locat = 0;
        UInt8 nums = 0;
        UInt8 numv = 0;
        for (NSDictionary *dict in eleModelIDArray) {
            NSMutableArray *sigModelIDs = dict[@"modelIDs"];
            NSMutableArray *vendorModelIDs = dict[@"vendorIDs"];
            nums = sigModelIDs.count;
            numv = vendorModelIDs.count;
            [modelIDData appendData:[NSData dataWithBytes:&locat length:2]];
            [modelIDData appendData:[NSData dataWithBytes:&nums length:1]];
            [modelIDData appendData:[NSData dataWithBytes:&numv length:1]];
            for (NSNumber *modelID in sigModelIDs) {
                UInt16 mID = modelID.intValue;
                [modelIDData appendData:[NSData dataWithBytes:&mID length:2]];
            }
            for (NSNumber *vendorID in vendorModelIDs) {
                UInt32 vID = vendorID.intValue;
                [modelIDData appendData:[NSData dataWithBytes:&vID length:4]];
            }
        }
        memcpy(&node_info.cps.model_id.id, modelIDData.bytes+4, modelIDData.length-4);
    }

    _nodeInfo = node_info;
    return _nodeInfo;
}

- (void)setNodeInfo:(VC_node_info_t)nodeInfo{
    if (nodeInfo.node_adr != self.address) {
        return;
    }
    _nodeInfo = nodeInfo;
    [self.elements removeAllObjects];
    if (self.appKeys.count > 0) {
        //keyBind成功
        if (nodeInfo.element_cnt != 0 && nodeInfo.element_cnt != 0xff) {
            UInt8 numS = (UInt8)nodeInfo.cps.model_id.nums;
            UInt8 numV = (UInt8)nodeInfo.cps.model_id.numv;
            UInt8 *index = nodeInfo.cps.model_id.id;
            UInt16 location = nodeInfo.cps.local;
            
            while (numS != 0xff && numV != 0xff) {
                SigElementModel *eleModel = [[SigElementModel alloc] init];
                eleModel.name = @"";
                eleModel.location = [NSString stringWithFormat:@"%04X",location];
                for (int i=0; i<numS; i++) {
                    NSData *data = [NSData dataWithBytes:index+i*2 length:2];
                    UInt16 modelId = [LibTools uint16FromBytes:data];
                    SigModelIDModel *model = [[SigModelIDModel alloc] init];
                    model.modelId = [NSString stringWithFormat:@"%04X",modelId];
                    model.bind = [NSMutableArray arrayWithObject:@(0)];
                    [eleModel.models addObject:model];
                }
                for (int i=0; i<numV; i++) {
                    NSData *data = [NSData dataWithBytes:index+numS*2+i*4 length:4];
                    //做法1：生成的JSON数据中vendorID为“00000211”
//                    NSMutableData *temData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(2, 2)]];
//                    [temData appendData:[data subdataWithRange:NSMakeRange(0, 2)]];
                    //做法2：生成的JSON数据中vendorID为“02110000”
                    NSMutableData *temData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(0, 2)]];
                    [temData appendData:[data subdataWithRange:NSMakeRange(2, 2)]];
                    UInt32 modelId = [LibTools uint32FromBytes:temData];
                    SigModelIDModel *model = [[SigModelIDModel alloc] init];
                    model.modelId = [NSString stringWithFormat:@"%08X",(unsigned int)modelId];
                    model.bind = [NSMutableArray arrayWithObject:@(0)];
                    [eleModel.models addObject:model];
                }
                [_elements addObject:eleModel];
                
                UInt16 nextLoc = [LibTools uint16FromBytes:[NSData dataWithBytes:index+numS*2+numV*4 length:2]];
                if (nextLoc != 0xff) {
                    location = nextLoc;
                    index = index+numS*2+numV*4+2;
                    numS = [LibTools uint8FromBytes:[NSData dataWithBytes:index length:1]];
                    numV = [LibTools uint8FromBytes:[NSData dataWithBytes:index+1 length:1]];
                    index = index+2;
                    if (numS == 0 && numV == 0) {
                        break;
                    }
                }else{
                    break;
                }
            }
        }
    } else {
        //keyBind失败
        if (nodeInfo.element_cnt != 0 && nodeInfo.element_cnt != 0xff) {
            for (int i=0; i<nodeInfo.element_cnt; i++) {
                SigElementModel *ele = [[SigElementModel alloc] init];
                SigModelIDModel *modelID = [[SigModelIDModel alloc] init];
                modelID.modelId = @"0000";
                ele.models = [NSMutableArray arrayWithObject:modelID];
                ele.name = @"";
                ele.location = @"0000";
                ele.index = i;
                [self.elements addObject:ele];
            }
        }
    }
    
    self.features.proxyFeature = nodeInfo.cps.page0_head.feature.proxy;
    self.features.friendFeature = nodeInfo.cps.page0_head.feature.frid;
    self.features.relayFeature = nodeInfo.cps.page0_head.feature.relay;
    self.features.lowPowerFeature = nodeInfo.cps.page0_head.feature.low_power;
    //设备默认已经打开所有feature，值应该为1，如果值为0说明不支持该功能(而json中：0关闭，1打开，2不支持)
    if (self.features.proxyFeature == 0) {
        self.features.proxyFeature = 2;
    }
    if (self.features.friendFeature == 0) {
        self.features.friendFeature = 2;
    }
    if (self.features.relayFeature == 0) {
        self.features.relayFeature = 2;
    }
    if (self.features.lowPowerFeature == 0) {
        self.features.lowPowerFeature = 2;
    }
    
    self.cid = [NSString stringWithFormat:@"%04X",nodeInfo.cps.page0_head.cid];
    self.pid = [NSString stringWithFormat:@"%04X",nodeInfo.cps.page0_head.pid];
    self.vid = [NSString stringWithFormat:@"%04X",nodeInfo.cps.page0_head.vid];
    self.crpl = [NSString stringWithFormat:@"%04X",nodeInfo.cps.page0_head.crpl];

}

///获取该设备的所有组号
- (NSMutableArray <NSNumber *>*)getGroupIDs{
    @synchronized (self) {
        NSMutableArray *tem = [NSMutableArray array];
        NSArray *allOptions = SigDataSource.share.defaultGroupSubscriptionModels;
        for (NSNumber *modelID in allOptions) {
            BOOL hasOption = NO;
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                BOOL shouldBreak = NO;
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //[NSString]->[NSNumber]
                        NSArray *subscribe = [NSArray arrayWithArray:modelIDModel.subscribe];
                        for (NSString *groupIDString in subscribe) {
                            [tem addObject:@([LibTools uint16From16String:groupIDString])];
                        }
                        hasOption = YES;
                        shouldBreak = YES;
                        break;
                    }
                }
                if (shouldBreak) {
                    break;
                }
            }
            if (hasOption) {
                break;
            }
        }
        return tem;
    }
}

///新增设备的组号
- (void)addGroupID:(NSNumber *)groupID{
    @synchronized (self) {
        NSArray *allOptions = SigDataSource.share.defaultGroupSubscriptionModels;
        for (NSNumber *modelID in allOptions) {
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //[NSString]->[NSNumber]
                        NSMutableArray *tem = [NSMutableArray array];
                        NSArray *subscribe = [NSArray arrayWithArray:modelIDModel.subscribe];
                        for (NSString *groupIDString in subscribe) {
                            [tem addObject:@([LibTools uint16From16String:groupIDString])];
                        }
                        if (![tem containsObject:groupID]) {
                            [modelIDModel.subscribe addObject:[NSString stringWithFormat:@"%04X",groupID.intValue]];
                        }
                    }
                }
            }
        }
    }
}

///删除设备的组号
- (void)deleteGroupID:(NSNumber *)groupID{
    @synchronized (self) {
        NSArray *allOptions = SigDataSource.share.defaultGroupSubscriptionModels;
        for (NSNumber *modelID in allOptions) {
            NSArray *elements = [NSArray arrayWithArray:self.elements];
            for (SigElementModel *element in elements) {
                NSArray *models = [NSArray arrayWithArray:element.models];
                for (SigModelIDModel *modelIDModel in models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //[NSString]->[NSNumber]
                        NSMutableArray *tem = [NSMutableArray array];
                        NSArray *subscribe = [NSArray arrayWithArray:modelIDModel.subscribe];
                        for (NSString *groupIDString in subscribe) {
                            [tem addObject:@([LibTools uint16From16String:groupIDString])];
                        }
                        if ([tem containsObject:groupID]) {
                            [modelIDModel.subscribe removeObjectAtIndex:[tem indexOfObject:groupID]];
                        }
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
                        retransmit.count = 2;
                        retransmit.interval = 5;
                        sigModelIDModel.publish.index = 0;
                        sigModelIDModel.publish.credentials = 0;
                        sigModelIDModel.publish.ttl = 0xff;
                        //json数据中，period为publish周期的毫秒数据
                        sigModelIDModel.publish.period = 1000 * kPublishInterval;
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
        UInt8 lum = [SigHelper.share getUInt8LumFromSInt16Level:level];
        if (source == self.address) {
            //lum
            UInt16 lightness = [SigHelper.share getUint16LightnessFromUInt8Lum:lum];
            self.brightness = lightness;
            self.state = self.brightness != 0 ? DeviceStateOn : DeviceStateOff;
        } else if (source == self.temperatureAddresses.firstObject.intValue) {
            //temp
            UInt16 temperature = [SigHelper.share getUint16TemperatureFromUInt8Temperature100:lum];
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
    } else {
//        TeLogWarn(@"Node response status Model is Undefined.");
    }
}

@end


@implementation SigRelayretransmitModel

- (NSDictionary *)getDictionaryOfSigRelayretransmitModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"count"] = @(_count);
    dict[@"interval"] = @(_interval);
    return dict;
}

- (void)setDictionaryToSigRelayretransmitModel:(NSDictionary *)dictionary {
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

- (UInt8)steps {
    return (UInt8)(_interval / 10) - 1;
}

@end


@implementation SigNetworktransmitModel

- (NSDictionary *)getDictionaryOfSigNetworktransmitModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"count"] = @(_count);
    dict[@"interval"] = @(_interval);
    return dict;
}

- (void)setDictionaryToSigNetworktransmitModel:(NSDictionary *)dictionary {
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

/// Number of 10-millisecond steps between transmissions.
- (UInt8)septs {
    return (UInt8)(_interval / 10) - 1;
}

/// The interval in as `TimeInterval` in seconds.
- (NSTimeInterval)timeInterval {
    return (NSTimeInterval)_interval / 1000.0;
}

- (instancetype)initWithSigConfigNetworkTransmitSet:(SigConfigNetworkTransmitSet *)request {
    if (self = [super init]) {
        _count = request.count + 1;
        _interval = (UInt16)(request.steps + 1) * 10;
    }
    return self;
}

- (instancetype)initWithSigConfigNetworkTransmitStatus:(SigConfigNetworkTransmitStatus *)status {
    if (self = [super init]) {
        _count = status.count + 1;
        _interval = (UInt16)(status.steps + 1) * 10;
    }
    return self;
}

@end


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

- (instancetype)init {
    if (self = [super init]) {
        _relayFeature = 2;
        _proxyFeature = 2;
        _friendFeature = 2;
        _lowPowerFeature = 2;
    }
    return self;
}

- (instancetype)initWithRawValue:(UInt16)rawValue {
    if (self = [super init]) {
        _relayFeature = (rawValue & 0x01) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_notEnabled;
        _proxyFeature = (rawValue & 0x02) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_notEnabled;
        _friendFeature = (rawValue & 0x04) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_notEnabled;
        _lowPowerFeature = (rawValue & 0x08) == 0 ? SigNodeFeaturesState_notSupported : SigNodeFeaturesState_notEnabled;
    }
    return self;
}

- (instancetype)initWithRelay:(SigNodeFeaturesState)relayFeature proxy:(SigNodeFeaturesState)proxyFeature friend:(SigNodeFeaturesState)friendFeature lowPower:(SigNodeFeaturesState)lowPowerFeature {
    if (self = [super init]) {
        _relayFeature = relayFeature;
        _proxyFeature = proxyFeature;
        _friendFeature = friendFeature;
        _lowPowerFeature = lowPowerFeature;
    }
    return self;
}

- (NSDictionary *)getDictionaryOfSigFeatureModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"proxy"] = @(_proxyFeature);
    dict[@"friend"] = @(_friendFeature);
    dict[@"relay"] = @(_relayFeature);
    dict[@"lowPower"] = @(_lowPowerFeature);
    return dict;
}

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


@implementation SigNodeKeyModel

- (instancetype)initWithIndex:(UInt16)index updated:(bool)updated {
    if (self = [super init]) {
        _index = index;
        _updated = updated;
    }
    return self;
}

- (NSDictionary *)getDictionaryOfSigNodeKeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"index"] = @(_index);
    dict[@"updated"] = [NSNumber numberWithBool:_updated];
    return dict;
}

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

- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigNodeKeyModel class]]) {
        return _index == ((SigNodeKeyModel *)object).index;
    } else {
        return NO;
    }
}

@end


@implementation SigElementModel

- (instancetype)init{
    if (self = [super init]) {
        _models = [NSMutableArray array];
    }
    return self;
}

- (instancetype)initWithLocation:(SigLocation)location {
    if (self = [super init]) {
        [self setSigLocation:location];
        _models = [NSMutableArray array];
        
        // Set temporary index.
        // Final index will be set when Element is added to the Node.
        _index = 0;
    }
    return self;
}

- (instancetype)initWithCompositionData:(NSData *)compositionData offset:(int *)offset {
    if (self = [super init]) {
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
            modelID.parentElement = self;
            [self addModel:modelID];
        }
        *offset += sigModelsByteCount;
        for (int i=0; i<numV; i++) {
            memcpy(&tem16, dataByte+*offset+i*4, 2);
            UInt16 companyId = tem16;
            memcpy(&tem16, dataByte+*offset+i*4+2, 2);
            UInt16 modelId = tem16;
            UInt32 vendorModelId = (UInt32)companyId << 16 | (UInt32)modelId;
            SigModelIDModel *modelID = [[SigModelIDModel alloc] initWithVendorModelId:vendorModelId];
            modelID.parentElement = self;
            [self addModel:modelID];
        }
        *offset += vendorModelsByteCount;
    }
    return self;
}

- (void)addModel:(SigModelIDModel *)model {
    model.parentElement = self;
    [_models addObject:model];
}

- (UInt16)unicastAddress {
    return (UInt16)_index + _parentNode.address;
}

- (SigLocation)getSigLocation {
    return [LibTools uint16From16String:self.location];
}

- (void)setSigLocation:(SigLocation)sigLocation {
    self.location = [SigHelper.share getNodeAddressString:sigLocation];
}

- (NSDictionary *)getDictionaryOfSigElementModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_location) {
        dict[@"location"] = _location;
    }
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
            model.parentElement = self;
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
        tem16 = model.modelIdentifier;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
    }
    for (SigModelIDModel *model in vendorModels) {
        tem16 = model.companyIdentifier;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
        tem16 = model.modelIdentifier;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
    }
    return mData;
}

@end


@implementation SigModelIDModel

- (instancetype)init{
    self = [super init];
    if (self) {
        _bind = [NSMutableArray array];
        _subscribe = [NSMutableArray array];
    }
    return self;
}

///返回整形的modelID
- (int)getIntModelID{
    int modelID = 0;
    if (self.modelId.length == 4) {
        modelID = [LibTools uint16From16String:self.modelId];
    } else {
        modelID = [LibTools uint32From16String:self.modelId];
    }
    return modelID;
}

- (UInt16)getIntModelIdentifier {
    UInt16 tem = 0;
    if (self.modelId.length == 4) {
        tem = [LibTools uint16From16String:self.modelId];
    } else if (self.modelId.length == 8) {
        tem = [LibTools uint16From16String:[self.modelId substringFromIndex:4]];
    }
    return tem;
}

- (UInt16)getIntCompanyIdentifier {
    UInt16 tem = 0;
    if (self.modelId.length == 8) {
        tem = [LibTools uint16From16String:[self.modelId substringToIndex:4]];
    }
    return tem;
}

- (instancetype)initWithSigModelId:(UInt16)sigModelId {
    return [self initWithVendorModelId:(UInt32)sigModelId];
}

- (instancetype)initWithVendorModelId:(UInt32)vendorModelId {
    if (self = [super init]) {
        _modelId = [SigHelper.share getUint32String:vendorModelId];
        _subscribe = [NSMutableArray array];
        _bind = [NSMutableArray array];
        _delegate = nil;
    }
    return self;
}

- (UInt16)modelIdentifier {
    return (UInt16)([self getIntModelID] & 0x0000FFFF);
}

- (UInt16)companyIdentifier {
    if ([self getIntModelID] > 0xFFFF) {
        return (UInt16)([self getIntModelID] >> 16);
    }
    return 0;
}

- (BOOL)isBluetoothSIGAssigned {
    return [self getIntModelID] <= 0xFFFF;
}

- (NSArray <SigGroupModel *>*)subscriptions {
//    return parentElement.parentNode?.meshNetwork?.groups.filter({ subscribe.contains($0._address )}) ?? []
    return @[];
}

- (BOOL)isConfigurationServer {
    return [self getIntModelID] == kConfigurationServerModelId;
}

- (BOOL)isConfigurationClient {
    return [self getIntModelID] == kConfigurationClientModelId;
}

- (BOOL)isHealthServer {
    return [self getIntModelID] == kHealthServerModelId;
}

- (BOOL)isHealthClient {
    return [self getIntModelID] == kHealthClientModelId;
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

/// List of Application Keys bound to this Model.
///
/// The list will not contain unknown Application Keys bound
/// to this Model, possibly bound by other Provisioner.
- (NSMutableArray <SigAppkeyModel *>*)boundApplicationKeys {
    NSMutableArray *tem = [NSMutableArray array];
    NSArray *appKeys = [NSArray arrayWithArray:self.parentElement.parentNode.appKeys];
    for (SigAppkeyModel *key in appKeys) {
        if ([self.bind containsObject:@(key.index)]) {
            [tem addObject:key];
        }
    }
    return tem;
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


@implementation SigPublishModel

- (instancetype)init
{
    self = [super init];
    if (self) {
        _index = 0;
        _credentials = 0;
        _ttl = 0;
        SigRetransmitModel *retransmit = [[SigRetransmitModel alloc] init];
        retransmit.count = 0;
        retransmit.interval = 0;
        _retransmit = retransmit;
        _period = 0;
        _address = @"0000";
    }
    return self;
}

- (NSDictionary *)getDictionaryOfSigPublishModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_address) {
        dict[@"address"] = _address;
    }
    dict[@"index"] = @(_index);
    dict[@"credentials"] = @(_credentials);
    dict[@"ttl"] = @(_ttl);
    dict[@"period"] = @(_period);
    if (_retransmit) {
        dict[@"retransmit"] = [_retransmit getDictionaryOfSigRetransmitModel];
    }
    return dict;
}

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
        _period = [dictionary[@"period"] integerValue];
    }
    if ([allKeys containsObject:@"retransmit"]) {
        SigRetransmitModel *retransmit = [[SigRetransmitModel alloc] init];
        [retransmit setDictionaryToSigRetransmitModel:dictionary[@"retransmit"]];
        _retransmit = retransmit;
    }
}

@end


@implementation SigRetransmitModel

- (NSDictionary *)getDictionaryOfSigRetransmitModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"count"] = @(_count);
    dict[@"interval"] = @(_interval);
    return dict;
}

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
