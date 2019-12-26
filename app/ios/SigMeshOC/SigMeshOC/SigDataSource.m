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
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/9/29.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SigDataSource.h"
#import "OpenSSLHelper.h"

@implementation SigEncryptedModel

- (instancetype)initWithCoder:(NSCoder *)coder
{
    if (self = [super init]) {
        _identityData = [coder decodeObjectForKey:kSigEncryptedModel_identityData_key];
        _hashData = [coder decodeObjectForKey:kSigEncryptedModel_hashData_key];
        _randomData = [coder decodeObjectForKey:kSigEncryptedModel_randomData_key];
        _peripheralUUID = [coder decodeObjectForKey:kSigEncryptedModel_peripheralUUID_key];
        _encryptedData = [coder decodeObjectForKey:kSigEncryptedModel_encryptedData_key];
        _address = [coder decodeIntegerForKey:kSigEncryptedModel_address_key];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_identityData forKey:kSigEncryptedModel_identityData_key];
    [coder encodeObject:_hashData forKey:kSigEncryptedModel_hashData_key];
    [coder encodeObject:_randomData forKey:kSigEncryptedModel_randomData_key];
    [coder encodeObject:_peripheralUUID forKey:kSigEncryptedModel_peripheralUUID_key];
    [coder encodeObject:_encryptedData forKey:kSigEncryptedModel_encryptedData_key];
    [coder encodeInteger:_address forKey:kSigEncryptedModel_address_key];
}

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
        struct ProvisionInfo tem = {{}, {0x00,0x00}, 0, {0x11,0x22,0x33,0x44}, {}};
        _provsionInfo = tem;
        _hasWriteDataSourceToLib = NO;
        _matchsNodeIdentityArray = [NSMutableArray array];
        _noMatchsNodeIdentityArray = [NSMutableArray array];
        _defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:kSubscriptionModels];
        _defaultNodeInfos = [NSMutableArray array];
        DeviceTypeModel *model1 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_Panel];
        DeviceTypeModel *model2 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_CT];
        [_defaultNodeInfos addObject:model1];
        [_defaultNodeInfos addObject:model2];
    }
    return self;
}

- (DeviceTypeModel *)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID {
    DeviceTypeModel *model = nil;
    for (DeviceTypeModel *tem in _defaultNodeInfos) {
        if (tem.CID == CID && tem.PID == PID) {
            model = tem;
            break;
        }
    }
    return model;
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
        for (SigNetkeyModel *model in _netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfSigNetkeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"netKeys"] = array;
    }
    if (_appKeys) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigAppkeyModel *model in _appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfSigAppkeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"appKeys"] = array;
    }
    if (_provisioners) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigProvisionerModel *model in _provisioners) {
            NSDictionary *provisionDict = [model getDictionaryOfSigProvisionerModel];
            [array addObject:provisionDict];
        }
        dict[@"provisioners"] = array;
    }
    if (_nodes) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigNodeModel *model in _nodes) {
            NSDictionary *nodeDict = [model getDictionaryOfSigNodeModel];
            [array addObject:nodeDict];
        }
        dict[@"nodes"] = array;
    }
    if (_groups) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigGroupModel *model in _groups) {
            NSDictionary *groupDict = [model getDictionaryOfSigGroupModel];
            [array addObject:groupDict];
        }
        dict[@"groups"] = array;
    }
    if (_scenes) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigSceneModel *model in _scenes) {
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
        TeLog(@"creat mesh_sample.json success");
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
    //init SigScanRspModel list
    [self loadEncryptedNodeIdentityList];
    //init Bluetooth
    [Bluetooth share];
}

- (void)initMeshData{
    //1.netKeys
    SigNetkeyModel *netkey = [[SigNetkeyModel alloc] init];
    netkey.oldKey = @"";
    netkey.index = 0;
    netkey.phase = 0;
    netkey.timestamp = [LibTools getNowTimeTimestampFrome2000];
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
    _timestamp = [LibTools getNowTimeTimestampFrome2000];
}

- (void)addLocationNodeWithProvisioner:(SigProvisionerModel *)provisioner{
    SigNodeModel *node = [[SigNodeModel alloc] init];
    
    //init defoult data
    node.UUID = provisioner.UUID;
    node.secureNetworkBeacon = YES;
    node.defaultTTL = TTL_DEFAULT;
    node.features.proxy = 2;
    node.features.friend = 0;
    node.features.relay = 2;
    node.relayRetransmit.count = 3;
    node.relayRetransmit.interval = 0;
    node.networkTransmit.count = 5;
    node.networkTransmit.interval = 2;
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

    VC_node_info_t node_info = {};
    //_nodeInfo默认赋值ff
    memset(&node_info, 0xff, sizeof(VC_node_info_t));
    node_info.node_adr = [LibTools uint16From16String:node.unicastAddress];
    node_info.element_cnt = 1;
    node_info.cps.len_cps = SIZE_OF_PAGE0_LOCAL;
    memcpy(&node_info.cps.page0_head, gp_page0, SIZE_OF_PAGE0_LOCAL);
    node.nodeInfo = node_info;
    
    [_nodes addObject:node];
}

/// write SigDataSource To test.bin of lib
- (void)writeDataSourceToLib{
    //1.set netkey and iv_index to test.bin of lib.
    provisionLocal([LibTools nsstringToHex:self.curNetkeyModel.key], self.curLocationNodeModel.address,self.curNetkeyModel.index);
//    TeLog(@"当前APP使用的本地地址为：%d",self.curLocationNodeModel.address);
    
    //2.set appkey to test.bin of lib.
    u8 p_appkey[16] = {};
    memcpy(&p_appkey, [LibTools nsstringToHex:self.curAppkeyModel.key].bytes, 16);
    mesh_app_key_set(APPKEY_ADD, (u8 *)&p_appkey, 0, 0, 1);

    //3.set dev_key to test.bin of lib.
    for (SigNodeModel *model in self.curNodes) {
        u8 dev_key[16] = {};
        memcpy(&dev_key, [LibTools nsstringToHex:model.deviceKey].bytes, 16);
        //keyBind失败时，device.getElementCount != 0是正确的，device.elements.count=0是正确的。
        VC_node_dev_key_save(model.address,(u8 *)&dev_key,model.getElementCount);
        VC_node_info_t info = model.nodeInfo;
        VC_node_cps_save((mesh_page0_t *)&info.cps.page0_head, model.address, SIZE_OF_PAGE0_LOCAL);
    }
    
    //4.clean cache(slove bug of init different meshs that has same address node. )
    cache_init(ADR_ALL_NODES);
    
    //5.reset busyTimer
    [Bluetooth.share resetMeshBusyTimer];
    
    self.hasWriteDataSourceToLib = YES;
    _curNodes = [NSMutableArray array];

}

- (void)removeModelWithDeviceAddress:(UInt16)deviceAddress{
    @synchronized(self) {
        for (SigNodeModel *model in self.nodes) {
            if (model.address == deviceAddress) {
                [_nodes removeObject:model];
                break;
            }
        }
        for (SigSceneModel *scene in self.scenes) {
            for (NSString *actionAddress in scene.addresses) {
                if (actionAddress.intValue == deviceAddress) {
                    [scene.addresses removeObject:actionAddress];
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
        TeLog(@"exist location provisioner, needn't create");
    }else{
        //don't exist location provisioner, create and add to SIGDataSource.provisioners, then save location.
        //Attention: the max location address is 0x7fff, so max provisioner's allocatedUnicastRange highAddress cann't bigger than 0x7fff.
        if (self.provisioners.count <= 0x7f) {
            SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerCount:[self getProvisionerCount] andProvisionerUUID:[self getCurrentProvisionerUUID]];
            [_provisioners addObject:provisioner];
            [self addLocationNodeWithProvisioner:provisioner];
            [self saveLocationData];
        }else{
            TeLog(@"waring: count of provisioners is bigger than 0x7f, app allocates node address will be error.");
        }
    }
    
    //update iv_Index to SIGDataSource.provsionInfo
    NSData *data = [self getIvIndexData];
    Byte *ivIndexByte = (Byte *)[data bytes];
    struct ProvisionInfo tem = self.provsionInfo;
    memcpy(tem.prov_iv_index,ivIndexByte,4);
    self.provsionInfo = tem;
}

- (void)changeLocationProvisionerNodeAddressToAddress:(UInt16)address {
    SigNodeModel *node = SigDataSource.share.curLocationNodeModel;
    node.unicastAddress = [NSString stringWithFormat:@"%04X",address];
}

- (NSInteger)getProvisionerCount{
    NSInteger max = 0;
    for (SigProvisionerModel *provisioner in self.provisioners) {
        if (max < provisioner.allocatedUnicastRange.firstObject.hightIntAddress) {
            max = provisioner.allocatedUnicastRange.firstObject.hightIntAddress;
        }
    }
    NSInteger count = (max >> 8) + 1;
    return count;
}

- (void)editGroupIDsOfDevice:(BOOL)add device_address:(NSNumber *)device_address group_address:(NSNumber *)group_address{
    @synchronized(self) {
        SigNodeModel *model = [self getDeviceWithAddress:[device_address intValue]];
        if (model) {
            if (add) {
                if (![model.getGroupIDs containsObject:group_address]) {
                    [model addGroupID:group_address];
                    [self saveLocationData];
                } else {
                    TeLog(@"add group model fail.");
                }
            } else {
                if (![model.getGroupIDs containsObject:group_address]) {
                    TeLog(@"delete group model fail.");
                } else {
                    [model deleteGroupID:group_address];
                    [self saveLocationData];
                }
            }
        } else {
            TeLog(@"edit group model fail, node no found.");
        }
    }
}

- (void)saveDeviceWithDeviceModel:(SigNodeModel *)model{
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
        for (SigNodeModel *model in _nodes) {
            model.state = DeviceStateOutOfLine;
        }
    }
}

- (BOOL)updateResponseModelWithResponse:(ResponseModel *)response{
    SigNodeModel *model = [self getDeviceWithAddress:(UInt16)response.address];
    if (model.isKeyBindSuccess) {
        return [model update:response];
    }
    return NO;
}

- (void)saveLocationData{
//    TeLog(@"");
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
    for (SigNodeModel *node in self.curNodes) {
        SigScanRspModel *rsp = [self getScanRspModelWithAddress:node.address];
        if (rsp) {
            [tem addObject:rsp];
        }
    }
    if (tem.count == 0) {
        TeLog(@"scanList is empty.");
    }
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:tem];
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:kScanList_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

///Special handling: load the uuid and MAC mapping relationship.
- (void)loadScanList{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *data = [defaults objectForKey:kScanList_key];
    NSArray *array = [NSKeyedUnarchiver unarchiveObjectWithData:data];
    if (array && array.count) {
        [self.scanList addObjectsFromArray:array];
    }
}

///Special handling: store the matchs nodeIdentity and uuid mapping relationship.
- (void)saveEncryptedNodeIdentityList{
    NSData *matchsListData = [NSKeyedArchiver archivedDataWithRootObject:self.matchsNodeIdentityArray];
    [[NSUserDefaults standardUserDefaults] setObject:matchsListData forKey:kMatchsList_key];
    NSData *noMatchsListData = [NSKeyedArchiver archivedDataWithRootObject:self.noMatchsNodeIdentityArray];
    [[NSUserDefaults standardUserDefaults] setObject:noMatchsListData forKey:kNoMatchsList_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

///Special handling: load the matchs nodeIdentity and uuid mapping relationship.
- (void)loadEncryptedNodeIdentityList{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *data1 = [defaults objectForKey:kMatchsList_key];
    NSArray *array1 = [NSKeyedUnarchiver unarchiveObjectWithData:data1];
    if (array1 && array1.count) {
        [self.matchsNodeIdentityArray addObjectsFromArray:array1];
    }
    NSData *data2 = [defaults objectForKey:kNoMatchsList_key];
    NSArray *array2 = [NSKeyedUnarchiver unarchiveObjectWithData:data2];
    if (array2 && array2.count) {
        [self.noMatchsNodeIdentityArray addObjectsFromArray:array2];
    }
}

///Special handling: update the uuid and MAC mapping relationship.
- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model{
    @synchronized(self) {
        if ([self.scanList containsObject:model]) {
            NSInteger index = [self.scanList indexOfObject:model];
            SigScanRspModel *oldModel = [self.scanList objectAtIndex:index];
            if ((oldModel.address != model.address && model.address != 0) || (oldModel.PID != model.PID && model.PID != 0) || (oldModel.CID != model.CID && model.CID != 0) || (![oldModel.macAddress isEqualToString:model.macAddress] && model.macAddress != nil && model.macAddress.length > 0) || (![oldModel.nodeIdentityData isEqualToData:model.nodeIdentityData] && model.nodeIdentityData != nil && model.nodeIdentityData.length > 0) || (![oldModel.networkIDData isEqualToData:model.networkIDData] && model.networkIDData != nil && model.networkIDData.length > 0)) {
                [self.scanList replaceObjectAtIndex:index withObject:model];
                [self saveScanList];
            }
        } else {
//            BOOL add = NO;
//            if (Bluetooth.share.state == StateAddDevice_provision) {
//                add = YES;
//            }else{
//                if (model.address != 0) {
//                    SigNodeModel *node = [self getDeviceWithAddress:model.address];
//                    if (node) {
//                        add = YES;
//                    }
//                }
//            }
//            if (add) {
                [self.scanList addObject:model];
                [self saveScanList];
                TeLog(@"新增缓存UUID=%@ address=%d macAddress=%@ self.scanList.count=%lu",model.uuid,model.address,model.macAddress,(unsigned long)self.scanList.count);
//            }
        }
    }
}

///Special handling: determine model whether exist current meshNetwork
- (BOOL)existScanRspModel:(SigScanRspModel *)model{
    if (model.networkIDData && model.networkIDData.length > 0) {
        if (self.curNetkeyModel.networkID && self.curNetkeyModel.networkID.length > 0) {
            return [self.curNetkeyModel.networkID isEqualToData:model.networkIDData];
        }
        if (self.curNetkeyModel.oldNetworkID && self.curNetkeyModel.oldNetworkID.length > 0) {
            return [self.curNetkeyModel.oldNetworkID isEqualToData:model.networkIDData];
        }
    }else if (model.nodeIdentityData && model.nodeIdentityData.length == 16) {
        return [self matchsWithNodeIdentityData:model.nodeIdentityData peripheralUUIDString:model.uuid];
    }
    return NO;
}

///Special handling: determine peripheralUUIDString whether exist current meshNetwork
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString{
    for (SigEncryptedModel *model in self.matchsNodeIdentityArray) {
        if ([model.peripheralUUID isEqualToString:peripheralUUIDString]) {
            return YES;
        }
    }
    return NO;
}

- (BOOL)existMatchsWithNodeIdentityData:(NSData *)nodeIdentityData {
    SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
    tem.identityData = nodeIdentityData;
    return [_matchsNodeIdentityArray containsObject:tem];
}

- (BOOL)existNoMatchsWithNodeIdentityData:(NSData *)nodeIdentityData {
    SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
    tem.identityData = nodeIdentityData;
    return [_noMatchsNodeIdentityArray containsObject:tem];
}

- (BOOL)matchsWithNodeIdentityData:(NSData *)nodeIdentityData peripheralUUIDString:(NSString *)peripheralUUIDString {
    if ([self existMatchsWithNodeIdentityData:nodeIdentityData]) {
        return YES;
    } else if ([self existNoMatchsWithNodeIdentityData:nodeIdentityData]) {
        return NO;
    } else {
        NSData *hashData = [nodeIdentityData subdataWithRange:NSMakeRange(0, 8)];
        NSData *randomData = [nodeIdentityData subdataWithRange:NSMakeRange(8, 8)];
        return [self matchsWithHashData:hashData randomData:randomData peripheralUUIDString:peripheralUUIDString];
    }
}

- (BOOL)matchsWithHashData:(NSData *)hash randomData:(NSData *)random peripheralUUIDString:(NSString *)peripheralUUIDString {
    BOOL matchs = NO;
    NSMutableData *mData = [NSMutableData dataWithData:hash];
    [mData appendData:random];
    SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
    tem.identityData = mData;
    tem.hashData = hash;
    tem.randomData = random;
    tem.peripheralUUID = peripheralUUIDString;

    for (SigNodeModel *node in self.curNodes) {
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
        NSData *encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:self.curNetkeyModel.keys.identityKey];
        if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
            matchs = YES;
        }
        // If the Key refresh procedure is in place, the identity might have been generated with the old key.
        if (self.curNetkeyModel.oldKey && self.curNetkeyModel.oldKey.length > 0 && ![self.curNetkeyModel.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
            encryptedData = [OpenSSLHelper.share calculateEvalueWithData:data andKey:self.curNetkeyModel.oldKeys.identityKey];
            if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
                matchs = YES;
            }
        }
        if (matchs) {
            tem.encryptedData = encryptedData;
            tem.address = node.address;
            [self.matchsNodeIdentityArray addObject:tem];
            [self saveEncryptedNodeIdentityList];
            break;
        }
    }
    if (!matchs) {
        [self.noMatchsNodeIdentityArray addObject:tem];
        [self saveEncryptedNodeIdentityList];
    }
    return matchs;
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

- (UInt32)getLocationSno {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *sno = [defaults objectForKey:kCurrenProvisionerSno_key];
    if (!sno) {
        sno = @(0);
    }
    return sno.intValue;
}

- (void)setLocationSno:(UInt32)sno {
    [[NSUserDefaults standardUserDefaults] setObject:@(sno) forKey:kCurrenProvisionerSno_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (NSInteger)getOnlineDevicesNumber{
    NSInteger count = 0;
    for (SigNodeModel *model in self.curNodes) {
        if (model.state != DeviceStateOutOfLine) {
            count ++;
        }
    }
    return count;
}

- (SigNodeModel *)getDeviceWithUUID:(NSString *)uuid{
    for (SigNodeModel *model in self.nodes) {
        //peripheralUUID || location node's uuid
        if ([model.peripheralUUID isEqualToString:uuid] || [model.UUID isEqualToString:uuid]) {
            return model;
        }
    }
    return nil;
}

- (SigNodeModel *)getDeviceWithMacAddress:(NSString *)macAddress{
    for (SigNodeModel *model in self.nodes) {
        //peripheralUUID || location node's uuid
        if (macAddress && model.macAddress && [model.macAddress.uppercaseString isEqualToString:macAddress.uppercaseString]) {
            return model;
        }
    }
    return nil;
}

- (SigNodeModel *)getDeviceWithAddress:(UInt16)address{
    @synchronized(self) {
        for (SigNodeModel *model in self.curNodes) {
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
}

- (ModelIDModel *)getModelIDModel:(NSNumber *)modelID{
    ModelIDs *modelIDs = [[ModelIDs alloc] init];
    for (ModelIDModel *model in modelIDs.modelIDs) {
        if (model.sigModelID == [modelID intValue]) {
            return model;
        }
    }
    return nil;
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

- (SigScanRspModel *)getScanRspModelWithUUID:(NSString *)uuid{
    @synchronized(self) {
        for (SigScanRspModel *model in _scanList) {
            if ([model.uuid isEqualToString:uuid]) {
                return model;
            }
        }
        return nil;
    }
}

- (SigScanRspModel *)getScanRspModelWithMac:(NSString *)mac{
    @synchronized(self) {
        for (SigScanRspModel *model in _scanList) {
            if ([model.macAddress isEqualToString:mac]) {
                return model;
            }
        }
        return nil;
    }
}

- (SigScanRspModel *)getScanRspModelWithAddress:(UInt16)address{
    @synchronized(self) {
        for (SigScanRspModel *model in _scanList) {
            if (model.address == address) {
                return model;
            }
        }
        return nil;
    }
}

- (void)deleteScanRspModelWithAddress:(UInt16)address{
    for (SigScanRspModel *model in _scanList) {
        if (model.address == address) {
            [_scanList removeObject:model];
            break;
        }
    }
    [self saveScanList];
}

- (SigEncryptedModel *)getSigEncryptedModelWithAddress:(UInt16)address {
    SigEncryptedModel *tem = nil;
    for (SigEncryptedModel *model in _matchsNodeIdentityArray) {
        if (model.address == address) {
            return model;
        }
    }
    return tem;
}

- (void)deleteSigEncryptedModelWithAddress:(UInt16)address {
    for (SigEncryptedModel *model in _matchsNodeIdentityArray) {
        if (model.address == address) {
            [_matchsNodeIdentityArray removeObject:model];
            break;
        }
    }
}

- (SigNetkeyModel *)curNetkeyModel{
    //The default use first netkey temporarily
    if (SigDataSource.share.netKeys.count > 0) {
        return SigDataSource.share.netKeys.firstObject;
    }
    return nil;
}

- (SigAppkeyModel *)curAppkeyModel{
    //The default use first appkey temporarily
    if (SigDataSource.share.appKeys.count > 0) {
        return SigDataSource.share.appKeys.firstObject;
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
    for (SigProvisionerModel *provisioner in SigDataSource.share.provisioners) {
        if ([provisioner.UUID isEqualToString:curUUID]) {
            return provisioner;
        }
    }
    return nil;
}

- (SigNodeModel *)curLocationNodeModel{
    if (SigDataSource.share.provisioners.count > 0) {
        return [self getDeviceWithUUID:self.curProvisionerModel.UUID];
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

///nodes should show in HomeViewController
- (NSMutableArray<SigNodeModel *> *)curNodes{
    @synchronized(self) {
        if (_curNodes && _curNodes.count == self.nodes.count - self.provisioners.count) {
            return _curNodes;
        } else {
            _curNodes = [NSMutableArray array];
            for (SigNodeModel *node in self.nodes) {
                BOOL isProvisioner = NO;
                for (SigProvisionerModel *provisioner in self.provisioners) {
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

- (UInt16)provisionAddress{
    if (!self.curProvisionerModel) {
        TeLog(@"warning: Abnormal situation, there is not provisioner.");
        return kLocationAddress;
    } else {
        UInt16 maxAddr = self.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress;
        for (SigNodeModel *node in SigDataSource.share.nodes) {
            NSInteger curMax = node.address + node.elements.count - 1;
            if (curMax > maxAddr) {
                maxAddr = curMax;
            }
        }
        
        NSMutableDictionary *dict = [[NSUserDefaults standardUserDefaults] objectForKey:kCurrentMeshProvisionAddress_key];
        if (dict && [dict.allKeys containsObject:self.getCurrentProvisionerUUID] && maxAddr < [dict[self.getCurrentProvisionerUUID] intValue]) {
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
//            TeLog(@"warning: Abnormal situation, there is no more address can be use.");
//            return 0;
//        }
        //不限制短地址的做法：
        TeLog(@"provisionAddress=%d",maxAddr + 1);
        return maxAddr + 1;
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

- (BOOL)hasNodeExistTimeModelID {
    BOOL tem = NO;
    for (SigNodeModel *node in self.curNodes) {
        UInt32 option = SIG_MD_TIME_S;
        NSArray *elementAddresses = [node getAddressesWithModelID:@(option)];
        if (elementAddresses.count > 0) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (SigNodeModel *)getNodeWithUUID:(NSString *)uuid{
    @synchronized(self) {
        for (SigNodeModel *model in SigDataSource.share.nodes) {
            if ([model.peripheralUUID isEqualToString:uuid]) {
                return model;
            }
        }
        return nil;
    }
}

- (SigNodeModel *)getNodeWithAddress:(UInt16)address{
    @synchronized(self) {
        for (SigNodeModel *model in SigDataSource.share.nodes) {
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
}

- (SigNodeModel *)getCurrentConnectedNode{
    NSString *uuid = Bluetooth.share.currentPeripheral.identifier.UUIDString;
    return [self getDeviceWithUUID:uuid];
}

- (NSData *)getIvIndexData{
    return [LibTools nsstringToHex:_ivIndex];
}

- (void)updateIvIndexData:(NSData *)ivIndex{
    _ivIndex = [LibTools convertDataToHexStr:ivIndex];
    Byte *ivbyte = (Byte *)[ivIndex bytes];
    struct ProvisionInfo provsionInfo = SigDataSource.share.provsionInfo;
    memcpy(provsionInfo.prov_iv_index, ivbyte, 4);
    SigDataSource.share.provsionInfo = provsionInfo;

    [self saveLocationData];
}

- (UInt8)getElementCountOfPid:(UInt16)pid {
    UInt8 node_ele_cnt = 1;
    switch (pid) {
        case SigNodePID_CT:
            node_ele_cnt = 2;
            break;
        case SigNodePID_Panel:
            node_ele_cnt = 8;
            break;
        default:
            TeLog(@"[ERROR]:please fix the code of pid:0x%04x",pid);
            break;
    }
    return node_ele_cnt;
}

- (SigGroupModel *)createNewGroupModelWithGroupName:(NSString *)groupName groupAddress:(UInt16)groupAddress {
    SigGroupModel *group = [[SigGroupModel alloc] initWithGroupName:groupName groupAddress:groupAddress];
    [_groups addObject:group];
    [self saveLocationData];
    return group;
}

- (void)delectGroupModelWithGroupAddress:(UInt16)groupAddress {
    for (SigGroupModel *model in _groups) {
        if ([model intAddress] == groupAddress) {
            [_groups removeObject:model];
            break;
        }
    }
    [self saveLocationData];
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
        _phase = [dictionary[@"phase"] intValue];
    }
    if ([allKeys containsObject:@"minSecurity"]) {
        _minSecurity = dictionary[@"minSecurity"];
    }
    if ([allKeys containsObject:@"timestamp"]) {
        _timestamp = dictionary[@"timestamp"];
    }
}

- (NSData *)networkID{
    if (!_networkID && self.key.length > 0 && ![self.key isEqualToString:@"00000000000000000000000000000000"]) {
        _networkID = [OpenSSLHelper.share calculateK3WithN:[LibTools nsstringToHex:self.key]];
    }
    return _networkID;
}

- (NSData *)oldNetworkID{
    if (!_oldNetworkID && self.oldKey.length > 0 && ![self.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        _oldNetworkID = [OpenSSLHelper.share calculateK3WithN:[LibTools nsstringToHex:self.oldKey]];
    }
    return _oldNetworkID;
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
        for (SigRangeModel *model in _allocatedUnicastRange) {
            NSDictionary *rangeDict = [model getDictionaryOfSigRangeModel];
            [array addObject:rangeDict];
        }
        dict[@"allocatedUnicastRange"] = array;
    }
    if (_allocatedGroupRange) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigRangeModel *model in _allocatedGroupRange) {
            NSDictionary *rangeDict = [model getDictionaryOfSigRangeModel];
            [array addObject:rangeDict];
        }
        dict[@"allocatedGroupRange"] = array;
    }
    if (_allocatedSceneRange) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigSceneRangeModel *model in _allocatedSceneRange) {
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
        range1.lowAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeLowAddress];
        range1.highAddress = [NSString stringWithFormat:@"%04X",kAllocatedGroupRangeHighAddress];
        [self.allocatedGroupRange addObject:range1];
        
        //v3.1.0前，间隔255，短地址分配范围：1-0xff，0x0100-0x01ff，0x0200-0x02ff，0x0300-0x03ff， 。。。
        //v3.1.0后，间隔1024，短地址分配范围：1~1024，1025~2048，2049~3072，3073~4096， 。。。
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
        for (NSString *str in _addresses) {
            [array addObject:str];
        }
        dict[@"addresses"] = array;
    }
    if (_actionList) {
        NSMutableArray *array = [NSMutableArray array];
        for (ActionModel *model in _actionList) {
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
        for (ActionModel *action in self.actionList) {
            [tem addObject:[NSString stringWithFormat:@"%04X",action.address]];
        }
        return tem;
    } else {
        return _addresses;
    }
}

@end


@implementation SigGroupModel

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
}

- (instancetype)initWithGroupName:(NSString *)groupName groupAddress:(UInt16)groupAddress {
    if (self = [super init]) {
        _name = groupName;
        _address = [NSString stringWithFormat:@"%04X",groupAddress];
    }
    return self;
}

- (UInt16)intAddress{
    return [LibTools uint16From16String:self.address];
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
        _features = [[SigFeatureModel alloc] init];
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
    return lightness2_lum(self.HSL_Hue);
}

- (UInt8)HSL_Saturation100{
    return lightness2_lum(self.HSL_Saturation);
}

- (UInt8)HSL_Lightness100{
    return lightness2_lum(self.HSL_Lightness);
}

///return node true brightness, range is 0~100
- (UInt8)trueBrightness{
    return lightness2_lum(self.brightness);
}

///return node true color temperture, range is 0~100
- (UInt8)trueTemperature{
    return temp_to_temp100(self.temperature);
}

///update node status, YES means status had changed, NO means status hadn't changed.
- (BOOL)update:(ResponseModel *)m{
    Opcode op = m.opcode;
    BOOL change = NO;
    
    Byte *pu = (Byte *)[m.rspData bytes];
    UInt16 a=0,b=0,c=0;

    switch (op) {
        case OpcodeOnOffStatusResponse:
        {
            DeviceState tem = m.currentState ? DeviceStateOn : DeviceStateOff;
            if (self.state != tem) {
                self.state = tem;
                change = YES;
            }
        }
            break;
        case OpcodeBrightnessGetResponse:
        {
            if (_brightness != m.currentValue) {
                _brightness = m.currentValue;
                change = YES;
            }
            DeviceState tem = m.currentState ? DeviceStateOn : DeviceStateOff;
            if (_state != tem) {
                _state = tem;
                change = YES;
            }
        }
            break;
        case OpcodeCurrentCTLResponse:
        {
            if (_brightness != m.currentValue) {
                _brightness = m.currentValue;
                change = YES;
            }
            if (_temperature != m.pointValue) {
                _temperature = m.pointValue;
                change = YES;
            }
            DeviceState tem = m.currentState ? DeviceStateOn : DeviceStateOff;
            if (_state != tem) {
                _state = tem;
                change = YES;
            }
        }
            break;
        case OpcodeChangeLevelResponse:
        {
            if (m.address == self.address) {
                //lum
                UInt8 lum = level2lum(m.currentValue);
                if (_brightness != lum2_lightness(lum)) {
                    _brightness = lum2_lightness(lum);
                    change = YES;
                }
                if (lum > 0) {
                    //Attention: node's state is on when lum is bigger than 0.
                    _state = DeviceStateOn;
                }
            } else if (m.address == self.temperatureAddresses.firstObject.intValue){
                //temp
                //                UInt8 temp = level_to_temp100(m.currentValue);
                UInt8 lum = level2lum(m.currentValue);
                _temperature = temp100_to_temp(lum);
                if (_temperature != temp100_to_temp(lum)) {
                    _temperature = temp100_to_temp(lum);
                    change = YES;
                }
                //注意：lum不为0时，灯不一定是开的。
//                if (lum > 0) {
//                    _state = DeviceStateOn;
//                }
            }
        }
            break;
        case OpcodeCTLTemperatureStatusResponse:
        {
            if (_temperature != m.currentValue) {
                _temperature = m.currentValue;
                change = YES;
            }
        }
            break;
        case OpcodeHSLNotifyResponse:
        {
            if (m.rspData.length >= 14+1) {
                memcpy(&a, pu + 8+1, 2);
                memcpy(&b, pu + 10+1, 2);
                memcpy(&c, pu + 12+1, 2);
                if (_HSL_Lightness != a) {
                    _HSL_Lightness = a;
                    change = YES;
                }
                if (_HSL_Hue != b) {
                    _HSL_Hue = b;
                    change = YES;
                }
                if (_HSL_Saturation != c) {
                    _HSL_Saturation = c;
                    change = YES;
                }
            }
        }
            break;

        default:
            NSLog(@"opcode:%lu",(unsigned long)op);
            saveLogData([NSString stringWithFormat:@"opcode:%lu",(unsigned long)op]);
            break;
    }
    return change;
}

- (void)updateOnlineStatusWithDeviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100{
    _state = state;
    if (state == DeviceStateOutOfLine) {
        return;
    }
    _brightness = lum2_lightness(bright100);
    _temperature = temp100_to_temp(temperature100);
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
- (UInt32)publishModelID{
    UInt32 tem = 0;
    if ([self getAddressesWithModelID:@(SIG_MD_LIGHT_CTL_S)].count > 0) {
        tem = (UInt32)SIG_MD_LIGHT_CTL_S;
    } else if ([self getAddressesWithModelID:@(SIG_MD_LIGHT_HSL_S)].count > 0){
        tem = (UInt32)SIG_MD_LIGHT_HSL_S;
    } else if ([self getAddressesWithModelID:@(SIG_MD_LIGHTNESS_S)].count > 0){
        tem = (UInt32)SIG_MD_LIGHTNESS_S;
    } else if ([self getAddressesWithModelID:@(SIG_MD_G_ONOFF_S)].count > 0){
        tem = (UInt32)SIG_MD_G_ONOFF_S;
    }
    return tem;
}

- (NSMutableArray *)getAddressesWithModelID:(NSNumber *)sigModelID{
    NSMutableArray *array = [NSMutableArray array];
    if (self.elements.count > 0) {
        for (int i=0; i<self.elements.count; i++) {
            SigElementModel *ele = self.elements[i];
            for (SigModelIDModel *modelID in ele.models) {
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
    if (self.address == SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress) {
        //location node's uuid
        return _UUID;
    }

    //old code:use in v2.9.0 and before
//    NSString *tem = nil;
//    if (_macAddress && _macAddress.length > 0) {
//        for (SigScanRspModel *model in SigDataSource.share.scanList) {
//            if (model.macAddress && [model.macAddress.uppercaseString isEqualToString:_macAddress.uppercaseString]) {
//                tem = model.uuid;
//                break;
//            }
//        }
//    }
//
//    if (tem != nil) {
//        _peripheralUUID = tem;
//    } else {
//        if (kAddNotAdvertisementMac && self.address != 0) {
//            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:self.address];
//            if (node && node.UUID && ![node.UUID isEqualToString:@"00000000000000000000000000000000"]) {
//                _peripheralUUID = node.UUID;
//            }
//        }
//    }
    
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
        for (SigScanRspModel *model in SigDataSource.share.scanList) {
            if ([model.uuid isEqualToString:_peripheralUUID]) {
                tem = model.macAddress;
                break;
            }
        }
    }
    if (tem == nil) {
        if (self.address != 0) {
            for (SigScanRspModel *model in SigDataSource.share.scanList) {
                if (model.address == self.address) {
                    tem = model.macAddress;
                    break;
                }
            }
        }
    }
    _macAddress = tem;
    return _macAddress;
}

- (NSDictionary *)getDictionaryOfSigNodeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    //Attention:macAddress is unreliable, macAddress will remove from json since v3.0.0
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
        dict[@"UUID"] = _UUID;
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
    if (_elements) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigElementModel *model in _elements) {
            NSDictionary *elementDict = [model getDictionaryOfSigElementModel];
            [array addObject:elementDict];
        }
        dict[@"elements"] = array;
    }
    if (_netKeys) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigNodeKeyModel *model in _netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfSigNodeKeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"netKeys"] = array;
    }
    if (_appKeys) {
        NSMutableArray *array = [NSMutableArray array];
        for (SigNodeKeyModel *model in _appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfSigNodeKeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"appKeys"] = array;
    }
    if (_schedulerList) {
        NSMutableArray *array = [NSMutableArray array];
        for (SchedulerModel *model in _schedulerList) {
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
        SigFeatureModel *features = [[SigFeatureModel alloc] init];
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
        _UUID = dictionary[@"UUID"];
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

    if ([allKeys containsObject:@"elements"]) {
        NSMutableArray *elements = [NSMutableArray array];
        NSArray *array = dictionary[@"elements"];
        for (NSDictionary *elementDict in array) {
            SigElementModel *model = [[SigElementModel alloc] init];
            [model setDictionaryToSigElementModel:elementDict];
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
        for (SigElementModel *element in self.elements) {
            NSMutableArray *sigModelIDs = [NSMutableArray array];
            NSMutableArray *vendorModelIDs = [NSMutableArray array];
            for (SigModelIDModel *modelIDModel in element.models) {
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
        node_info.cps.page0_head.cid = (u16)[LibTools uint16From16String:self.cid];
    } else {
        node_info.cps.page0_head.cid = kCompanyID;
    }
    if (self.pid && self.pid.length > 0) node_info.cps.page0_head.pid = [LibTools uint16From16String:self.pid];
    if (self.vid && self.vid.length > 0) node_info.cps.page0_head.vid = (u16)[LibTools uint16From16String:self.vid];
    if (self.crpl && self.crpl.length > 0) node_info.cps.page0_head.crpl = (u16)[LibTools uint16From16String:self.crpl];
    node_info.cps.page0_head.feature.relay = self.features.relay == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.proxy = self.features.proxy == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.frid = self.features.friend == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.low_power = self.features.lowPower == 1 ? 1 : 0;
    node_info.cps.page0_head.feature.rfu = 0;
    node_info.cps.local = 0;
    if (eleModelIDArray.count > 0) {
        node_info.cps.model_id.nums = [eleModelIDArray.firstObject[@"modelIDs"] count];
        node_info.cps.model_id.numv = [eleModelIDArray.firstObject[@"vendorIDs"] count];
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
            u8 *index = nodeInfo.cps.model_id.id;
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
                if (nodeInfo.element_cnt == _elements.count) {
                    break;
                }
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
    
    self.features.proxy = nodeInfo.cps.page0_head.feature.proxy;
    self.features.friend = nodeInfo.cps.page0_head.feature.frid;
    self.features.relay = nodeInfo.cps.page0_head.feature.relay;
    self.features.lowPower = nodeInfo.cps.page0_head.feature.low_power;
    //设备默认已经打开所有feature，值应该为1，如果值为0说明不支持该功能(而json中：0关闭，1打开，2不支持)
    if (self.features.proxy == 0) {
        self.features.proxy = 2;
    }
    if (self.features.friend == 0) {
        self.features.friend = 2;
    }
    if (self.features.relay == 0) {
        self.features.relay = 2;
    }
    if (self.features.lowPower == 0) {
        self.features.lowPower = 2;
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
            for (SigElementModel *element in self.elements) {
                BOOL shouldBreak = NO;
                for (SigModelIDModel *modelIDModel in element.models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //[NSString]->[NSNumber]
                        for (NSString *groupIDString in modelIDModel.subscribe) {
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
            for (SigElementModel *element in self.elements) {
                for (SigModelIDModel *modelIDModel in element.models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //[NSString]->[NSNumber]
                        NSMutableArray *tem = [NSMutableArray array];
                        for (NSString *groupIDString in modelIDModel.subscribe) {
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
            for (SigElementModel *element in self.elements) {
                for (SigModelIDModel *modelIDModel in element.models) {
                    if (modelIDModel.getIntModelID == modelID.intValue) {
                        //[NSString]->[NSNumber]
                        NSMutableArray *tem = [NSMutableArray array];
                        for (NSString *groupIDString in modelIDModel.subscribe) {
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
            for (SigElementModel *element in self.elements) {
                BOOL hasPublish = NO;
                for (SigModelIDModel *sigModelIDModel in element.models) {
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
                        sigModelIDModel.publish.address = [NSString stringWithFormat:@"%04X",kAllDo_SIGParameters];
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
            for (SigElementModel *element in self.elements) {
                BOOL hasPublish = NO;
                for (SigModelIDModel *sigModelIDModel in element.models) {
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
            for (SigElementModel *element in self.elements) {
                BOOL hasPublish = NO;
                for (SigModelIDModel *modelIDModel in element.models) {
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
            for (SigElementModel *element in self.elements) {
                BOOL hasPublish = NO;
                for (SigModelIDModel *modelIDModel in element.models) {
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

@end


@implementation SigFeatureModel

- (NSDictionary *)getDictionaryOfSigFeatureModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"proxy"] = @(_proxy);
    dict[@"friend"] = @(_friend);
    dict[@"relay"] = @(_relay);
    dict[@"lowPower"] = @(_lowPower);
    return dict;
}

- (void)setDictionaryToSigFeatureModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"proxy"]) {
        _proxy = [dictionary[@"proxy"] integerValue];
    }
    if ([allKeys containsObject:@"friend"]) {
        _friend = [dictionary[@"friend"] integerValue];
    }
    if ([allKeys containsObject:@"relay"]) {
        _relay = [dictionary[@"relay"] integerValue];
    }
    if ([allKeys containsObject:@"lowPower"]) {
        _lowPower = [dictionary[@"lowPower"] integerValue];
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
        for (SigModelIDModel *model in _models) {
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
            [models addObject:model];
        }
        _models = models;
    }
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
        for (NSNumber *num in _bind) {
            [array addObject:num];
        }
        dict[@"bind"] = array;
    }
    if (_subscribe) {
        NSMutableArray *array = [NSMutableArray array];
        for (NSString *str in _subscribe) {
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
