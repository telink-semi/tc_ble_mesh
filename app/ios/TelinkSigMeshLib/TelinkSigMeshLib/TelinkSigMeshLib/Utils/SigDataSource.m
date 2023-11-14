/********************************************************************************************************
 * @file     SigDataSource.m
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

#import "SigDataSource.h"
#import "OpenSSLHelper.h"
#import "SDKLibCommand+directForwarding.h"

@interface SigDataSource ()<SigDataSourceDelegate>
@property (nonatomic,assign) UInt32 sequenceNumberOnDelegate;//通过SigDataSourceDelegate回调的sequenceNumber值。
@end

@implementation SigDataSource

///nodes should show in HomeViewController
- (NSMutableArray<SigNodeModel *> *)curNodes {
    @synchronized(self) {
        if (_curNodes != nil) {
            return _curNodes;
        } else {
            _curNodes = [NSMutableArray array];
            NSArray *nodes = [NSArray arrayWithArray:_nodes];
            for (SigNodeModel *node in nodes) {
                if (node.excluded) {
                    continue;
                }
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

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigDataSource *shareDS = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareDS = [[SigDataSource alloc] init];
    });
    return shareDS;
}

- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self initData];
    }
    return self;
}

- (void)initData {
    _provisioners = [NSMutableArray array];
    _nodes = [NSMutableArray array];
    _groups = [NSMutableArray array];
    _scenes = [NSMutableArray array];
    _netKeys = [NSMutableArray array];
    _appKeys = [NSMutableArray array];
    _scanList = [NSMutableArray array];
    _networkExclusions = [NSMutableArray array];
    _nodeSequenceNumberCacheList = [NSMutableArray array];
    [self setIvIndexUInt32:kDefaultIvIndex];
    [self setSequenceNumberUInt32:0];
    _partial = false;
    _encryptedArray = [NSMutableArray array];
    _defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:@[@(kSigModel_GenericOnOffServer_ID),@(kSigModel_LightLightnessServer_ID),@(kSigModel_LightCTLServer_ID),@(kSigModel_LightCTLTemperatureServer_ID),@(kSigModel_LightHSLServer_ID)]];
    _defaultNodeInfos = [NSMutableArray array];
    DeviceTypeModel *model1 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_Panel compositionData:nil];
    DeviceTypeModel *model2 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_CT compositionData:nil];
    DeviceTypeModel *model3 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_HSL compositionData:nil];
    DeviceTypeModel *model4 = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:SigNodePID_LPN compositionData:nil];
    [_defaultNodeInfos addObject:model1];
    [_defaultNodeInfos addObject:model2];
    [_defaultNodeInfos addObject:model3];
    [_defaultNodeInfos addObject:model4];
    _meshUUID = [LibTools UUIDToMeshUUID:[LibTools convertDataToHexStr:[LibTools createRandomDataWithLength:16]]];
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
    _defaultNetKeyA.ivIndex = _defaultIvIndexA;
    _needPublishTimeModel = YES;
    _defaultUnsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
    _telinkExtendBearerMode = SigTelinkExtendBearerMode_noExtend;
    _aggregatorEnable = NO;

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
    _addStaticOOBDeviceByNoOOBEnable = YES;
    _defaultRetryCount = 2;
    _defaultAllocatedUnicastRangeHighAddress = kAllocatedUnicastRangeHighAddress;
    _defaultSequenceNumberIncrement = kSequenceNumberIncrement;
    SigPeriodModel *periodModel = [[SigPeriodModel alloc] init];
    periodModel.numberOfSteps = kPublishInterval;
    periodModel.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_seconds];
    _defaultPublishPeriodModel = periodModel;
    _security = SigMeshMessageSecurityLow;
    _defaultReliableIntervalOfNotLPN = kSDKLibCommandTimeout;
    _defaultReliableIntervalOfLPN = kSDKLibCommandTimeout * 2;
    //默认为写死的设备端的root.der根证书
    _defaultRootCertificateData = [LibTools nsstringToHex:@"308202873082022DA00302010202147FCD3C7C01BD4649E2295D3F04668931FD4E1FA7300A06082A8648CE3D04030230819E310B300906035504061302434E3111300F06035504080C085368616E6748616931143012060355040A0C0B54656C696E6B2D53656D69310F300D060355040B0C0654656C696E6B312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303126302406092A864886F70D0109011617737570706F72744074656C696E6B2D73656D692E636F6D301E170D3233313031313036303034385A170D3333313030383036303034385A30819E310B300906035504061302434E3111300F06035504080C085368616E6748616931143012060355040A0C0B54656C696E6B2D53656D69310F300D060355040B0C0654656C696E6B312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303126302406092A864886F70D0109011617737570706F72744074656C696E6B2D73656D692E636F6D3059301306072A8648CE3D020106082A8648CE3D0301070342000441DBF54A701EFAFC88D34A7233C383A6406F815AE7BA9D52BB4625A12303699595A3B139D35EEDC1E65E33C128A69AB0D037BFFF985AA23A62B3218BAEB17E3FA347304530090603551D2304023000301D0603551D0E041604143C62D196F66D337D40CC7826518C1AF22F425D3B300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D0403020348003045022100CACF7006A1C0ACC96350D00E716585AEB46B9553538487F43FB411FA56EEB86402201DA19E992E677A93FDC1609E7A8C4706A88777CE41AF626E4A95300918A7762B"];
    //PTS v8.5.1 build10 测试项的根证书
//    _defaultRootCertificateData = [LibTools nsstringToHex:@"308202883082022EA003020102020100300A06082A8648CE3D04030230819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3233303831373230353731305A170D3334313130333230353731305A30819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D3059301306072A8648CE3D020106082A8648CE3D030107034200046E0844268E1DA5556DA9B85D90A06DE152FB96A1521918B33B8E081F50B0274001E979A03E322BCD83E10CC447FD1124B65E71FDFEBE4BDD43712AFB5FA40D9EA35D305B301D0603551D0E04160414C9C527F8E3D36EC844538CA132C7282C459DDFFF301F0603551D23041830168014C9C527F8E3D36EC844538CA132C7282C459DDFFF300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D0403020348003045022100D034154A3ED26CA04402534B3F11CB6D5C9174C5A31274FD06F8F7395456B4FD02203F0B86574C5AAB707573A99DC9ABF815CB7AD8A11ACBE5C8D2D80C31F74E64D7"];
    _forwardingTableModelList = [NSMutableArray array];
    _filterModel = [[SigProxyFilterModel alloc] init];
}

- (SigAppkeyModel *)curAppkeyModel {
    if (_curAppkeyModel == nil) {
        //The default use first appkey temporarily
        if (_appKeys && _appKeys.count > 0) {
            _curAppkeyModel = _appKeys.firstObject;
        }
    }
    return _curAppkeyModel;
}

- (SigNetkeyModel *)curNetkeyModel {
    if (_curNetkeyModel == nil) {
        //The default use first netkey temporarily
        if (_netKeys && _netKeys.count > 0) {
            _curNetkeyModel = _netKeys.firstObject;
        }
    }
    return _curNetkeyModel;
}

- (NSData *)getLocationMeshData {
    return [NSUserDefaults.standardUserDefaults objectForKey:kSaveLocationDataKey];
}

- (void)saveLocationMeshData:(NSData *)data {
    [NSUserDefaults.standardUserDefaults setObject:data forKey:kSaveLocationDataKey];
    [NSUserDefaults.standardUserDefaults synchronize];
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
    netkey.name = @"Default NetKey";
    netkey.minSecurity = @"secure";
    _curNetkeyModel = nil;
    [_netKeys removeAllObjects];
    [_netKeys addObject:netkey];

    //2.appKeys
    SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
    appkey.oldKey = @"00000000000000000000000000000000";
    appkey.key = [LibTools convertDataToHexStr:[LibTools initAppKey]];
    appkey.name = @"Default AppKey";
    appkey.boundNetKey = 0;
    appkey.index = 0;
    _curAppkeyModel = nil;
    [_appKeys removeAllObjects];
    [_appKeys addObject:appkey];

    //3.provisioners
    SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerMaxHighAddressUnicast:0 andProvisionerUUID:[self getCurrentProvisionerUUID]];
    [_provisioners removeAllObjects];
    [_provisioners addObject:provisioner];

    //4.add new provisioner to nodes
    _curNodes = nil;
    [_nodes removeAllObjects];
    [self addLocationNodeWithProvisioner:provisioner];

    //5.add default group
    Groups *defaultGroup = [[Groups alloc] init];
    [_groups removeAllObjects];
    for (int i=0; i<defaultGroup.groupCount; i++) {
        SigGroupModel *group = [[SigGroupModel alloc] init];
        group.address = [NSString stringWithFormat:@"%04X",0xc000+i];
        group.parentAddress = [NSString stringWithFormat:@"%04X",0];
        group.name = defaultGroup.names[i];
        [_groups addObject: group];
    }

    [_scenes removeAllObjects];
    [_networkExclusions removeAllObjects];
    [_encryptedArray removeAllObjects];
    [_forwardingTableModelList removeAllObjects];

    _meshUUID = [LibTools UUIDToMeshUUID:[LibTools convertDataToHexStr:[LibTools createRandomDataWithLength:16]]];
    _schema = @"http://json-schema.org/draft-04/schema#";
    _jsonFormatID = @"http://www.bluetooth.com/specifications/assigned-numbers/mesh-profile/cdb-schema.json#";
    _meshName = @"Default Mesh";
    _version = @"1.0.0";
    _timestamp = timestamp;
    [self setIvIndexUInt32:kDefaultIvIndex];
    [self setSequenceNumberUInt32:0];
    [self saveCurrentIvIndex:kDefaultIvIndex sequenceNumber:0];
    _filterModel = [[SigProxyFilterModel alloc] init];
    [self addExtendGroupList];
}

/// every provisioner has at most one node that blacklisted is set to YES.
- (void)optimizationDataOfBlacklisted {
    NSArray *provisioners = [NSArray arrayWithArray:self.provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        NSMutableArray *nodes = [NSMutableArray arrayWithArray:[self getNodesOfProvisioner:provisioner]];
        if (nodes && nodes.count > 0) {
            [nodes removeLastObject];
            for (SigNodeModel *node in nodes) {
                if (node.excluded) {
                    [self.nodes removeObject:node];
                }
            }
        }
    }
    _curNodes = nil;
}

- (NSArray <SigNodeModel *>*)getNodesOfProvisioner:(SigProvisionerModel *)provisioner {
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *allNodes = [NSArray arrayWithArray:self.nodes];
    for (SigNodeModel *node in allNodes) {
        if ([self checkTheNode:node isBelongToProvisioner:provisioner]) {
            [mArray addObject:node];
        }
    }
    [mArray sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(SigNodeModel *)obj1 address] > [(SigNodeModel *)obj2 address];
    }];
    return mArray;
}

- (BOOL)checkTheNode:(SigNodeModel *)node isBelongToProvisioner:(SigProvisionerModel *)provisioner {
    BOOL tem = NO;
    NSMutableArray <SigRangeModel *>*mArray = [NSMutableArray arrayWithArray:provisioner.allocatedUnicastRange];
    for (SigRangeModel *range in mArray) {
        if (range.lowIntAddress <= node.address && range.heightIntAddress >= node.address) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (NSInteger)getProvisionerCount {
    NSInteger max = 0;
    NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        if (max < provisioner.allocatedUnicastRange.firstObject.heightIntAddress) {
            max = provisioner.allocatedUnicastRange.firstObject.heightIntAddress;
        }
    }
    NSInteger count = (max >> 8) + 1;
    return count;
}

///Special handling: store the uuid and MAC mapping relationship.
- (void)saveScanList {
    NSMutableArray *tem = [NSMutableArray array];
    NSArray *nodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *node in nodes) {
        SigScanRspModel *rsp = [self getScanRspModelWithUnicastAddress:node.address];
        if (rsp) {
            [tem addObject:rsp];
        }
    }
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:tem];
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:kScanList_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

///Special handling: load the uuid and MAC mapping relationship.
- (void)loadScanList {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *data = [defaults objectForKey:kScanList_key];
    if (data) {
        NSArray *array = [NSKeyedUnarchiver unarchiveObjectWithData:data];
        if (array && array.count) {
            [self.scanList addObjectsFromArray:array];
        }
    }
}

///Special handling: clean the uuid and MAC mapping relationship.
- (void)cleanScanList {
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kScanList_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (SigNodeModel *)getDeviceWithAddress:(UInt16)address {
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

/**
 * @brief   Get dictionary for save local.
 * @return  mesh dictionary
 * @note    Developer need use this API to get mesh data save locally.
 */
- (NSDictionary *)getDictionaryFromDataSource {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_meshUUID) {
        if (_meshUUID.length == 32) {
            dict[@"meshUUID"] = [[LibTools UUIDToMeshUUID:_meshUUID] uppercaseString];
        } else if (_meshUUID.length == 36) {
            dict[@"meshUUID"] = [_meshUUID uppercaseString];
        }
    }
    if (_meshName) {
        dict[@"meshName"] = _meshName;
    }
    if (_schema) {
        dict[@"$schema"] = _schema;
    }
    if (_jsonFormatID) {
        dict[@"id"] = _jsonFormatID;
    }
    if (_version) {
        dict[@"version"] = _version;
    }
    if (_timestamp) {
        dict[@"timestamp"] = _timestamp;
    }
    dict[@"partial"] = [NSNumber numberWithBool:_partial];
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
    if (_networkExclusions) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *networkExclusions = [NSArray arrayWithArray:_networkExclusions];
        for (SigExclusionListObjectModel *model in networkExclusions) {
            NSDictionary *networkExclusionDict = [model getDictionaryOfSigExclusionListObjectModel];
            [array addObject:networkExclusionDict];
        }
        dict[@"networkExclusions"] = array;
    }
    if (_forwardingTableModelList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *forwardingTableModelList = [NSArray arrayWithArray:_forwardingTableModelList];
        for (SigForwardingTableModel *model in forwardingTableModelList) {
            NSDictionary *forwardingTableModelDict = [model getDictionaryOfSigForwardingTableModel];
            [array addObject:forwardingTableModelDict];
        }
        dict[@"forwardingTableModelList"] = array;
    }
    return dict;
}

/**
 * @brief   Set mesh dictionary to SDK DataSource.
 * @param   dictionary    mesh dictionary.
 */
- (void)setDictionaryToDataSource:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    _curNodes = nil;
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"meshUUID"]) {
        NSString *str = [dictionary[@"meshUUID"] uppercaseString];
        if (str.length == 32) {
            _meshUUID = [LibTools UUIDToMeshUUID:str];
        } else if (str.length == 36) {
            _meshUUID = str;
        }
    }
    if ([allKeys containsObject:@"meshName"]) {
        _meshName = dictionary[@"meshName"];
    }
    if ([allKeys containsObject:@"$schema"]) {
        _schema = dictionary[@"$schema"];
    }
    if ([allKeys containsObject:@"id"]) {
        _jsonFormatID = dictionary[@"id"];
    }
    if ([allKeys containsObject:@"version"]) {
        _version = dictionary[@"version"];
    }
    if ([allKeys containsObject:@"timestamp"]) {
        _timestamp = dictionary[@"timestamp"];
    }
    if ([allKeys containsObject:@"partial"]) {
        _partial = [dictionary[@"partial"] boolValue];
    }
    if ([allKeys containsObject:@"netKeys"]) {
        NSMutableArray *netKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"netKeys"];
        for (NSDictionary *netkeyDict in array) {
            SigNetkeyModel *model = [[SigNetkeyModel alloc] init];
            [model setDictionaryToSigNetkeyModel:netkeyDict];
            if ([self getIvIndexUInt32] != 0) {
                model.ivIndex = [[SigIvIndex alloc] initWithIndex:[self getIvIndexUInt32] updateActive:NO];
            }
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
    if ([allKeys containsObject:@"networkExclusions"]) {
        NSMutableArray *networkExclusions = [NSMutableArray array];
        NSArray *array = dictionary[@"networkExclusions"];
        for (NSDictionary *networkExclusionDict in array) {
            SigExclusionListObjectModel *model = [[SigExclusionListObjectModel alloc] init];
            [model setDictionaryToSigExclusionListObjectModel:networkExclusionDict];
            [networkExclusions addObject:model];
        }
        _networkExclusions = networkExclusions;
    }
    if ([allKeys containsObject:@"forwardingTableModelList"]) {
        NSMutableArray *forwardingTableModelList = [NSMutableArray array];
        NSArray *array = dictionary[@"forwardingTableModelList"];
        for (NSDictionary *forwardingTableModelDict in array) {
            SigForwardingTableModel *model = [[SigForwardingTableModel alloc] init];
            [model setDictionaryToSigForwardingTableModel:forwardingTableModelDict];
            [forwardingTableModelList addObject:model];
        }
        _forwardingTableModelList = forwardingTableModelList;
    }
    _curNetkeyModel = nil;
    _curAppkeyModel = nil;
    [self addExtendGroupList];
    [self optimizationDataOfBlacklisted];
}

/**
 * @brief   Get formatDictionary for Export mesh.
 * @return  mesh formatDictionary
 * @note    Developer need use this API to export mesh data.
 */
- (NSDictionary *)getFormatDictionaryFromDataSource {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_meshUUID) {
        if (_meshUUID.length == 32) {
            dict[@"meshUUID"] = [[LibTools UUIDToMeshUUID:_meshUUID] uppercaseString];
        } else if (_meshUUID.length == 36) {
            dict[@"meshUUID"] = [_meshUUID uppercaseString];
        }
    }
    if (_meshName) {
        dict[@"meshName"] = _meshName;
    }
    if (_schema) {
        dict[@"$schema"] = _schema;
    }
    if (_jsonFormatID) {
        dict[@"id"] = _jsonFormatID;
    }
    if (_version) {
        dict[@"version"] = _version;
    }
    if (_timestamp) {
        dict[@"timestamp"] = _timestamp;
    }
    dict[@"partial"] = [NSNumber numberWithBool:_partial];
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
    if (_networkExclusions) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *networkExclusions = [NSArray arrayWithArray:_networkExclusions];
        for (SigExclusionListObjectModel *model in networkExclusions) {
            NSDictionary *networkExclusionDict = [model getDictionaryOfSigExclusionListObjectModel];
            [array addObject:networkExclusionDict];
        }
        dict[@"networkExclusions"] = array;
    }
    dict[@"id"] = @"http://www.bluetooth.com/specifications/assigned-numbers/mesh-profile/cdb-schema.json#";
    return dict;
}

/**
 * @brief   Get the unicastAddress for provision in add device process.
 * @return  unicastAddress for provision
 * @note    The address of the last node may be out of range..
 */
- (UInt16)provisionAddress {
    if (!self.curProvisionerModel) {
        TelinkLogInfo(@"warning: Abnormal situation, there is not provisioner.");
        return kLocationAddress;
    } else {
        //2023年07月25日15:31:42
        //the address of add device is get from the node of this provisioner, max address of node add one.
        //if allocatedUnicastRange of provisioner is depleted, need add a new SigRangeModel to allocatedUnicastRange.
        NSMutableArray <SigRangeModel *>*allocatedUnicastRange = [NSMutableArray arrayWithArray:self.curProvisionerModel.allocatedUnicastRange];
        [allocatedUnicastRange sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigRangeModel *)obj1 lowIntAddress] > [(SigRangeModel *)obj2 lowIntAddress];
        }];
        SigRangeModel *range = allocatedUnicastRange.lastObject;
        UInt16 maxAddr = range.lowIntAddress;
        NSMutableArray <SigNodeModel *>*nodeList = [NSMutableArray arrayWithArray:[self getNodesOfProvisioner:self.curProvisionerModel]];
        if (nodeList && nodeList.count > 0) {
            SigNodeModel *node = nodeList.lastObject;
            if (node.address + node.elements.count <= range.heightIntAddress) {
                maxAddr = node.address + node.elements.count;
            } else {
                // allocatedUnicastRange of provisioner is depleted
                BOOL result = [self addNewUnicastRangeToCurrentProvisioner];
                if (result) {
                    return self.curProvisionerModel.allocatedUnicastRange.lastObject.lowIntAddress;
                } else {
                    return 0;
                }
            }
        }
        return maxAddr;
    }
}

/**
 * @brief   Get the unicastAddress for provision in add device process.
 * @param   elementCount    the element count of node.
 * @return  unicastAddress for provision
 * @note    If the allocatedUnicastRange of provision has been exhausted, SDK will add a new allocatedUnicastRange. 0 means all allocatedUnicastRange is exhausted.
 */
- (UInt16)getProvisionAddressWithElementCount:(UInt8)elementCount {
    SigRangeModel *rangeModel = self.curProvisionerModel.allocatedUnicastRange.lastObject;
    UInt16 maxUnicastAddress = rangeModel.lowIntAddress;
    NSArray *array = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *node in array) {
        if (node.address >= rangeModel.lowIntAddress && node.lastUnicastAddress <= rangeModel.heightIntAddress) {
            if ((node.lastUnicastAddress + elementCount) <= rangeModel.heightIntAddress) {
                maxUnicastAddress = node.lastUnicastAddress + 1;
            } else {
                // The allocatedUnicastRange of provision has been exhausted, need add a new allocatedUnicastRange.
                // if can not allocated a new UnicastRange, return 0.
                BOOL result = [self addNewUnicastRangeToCurrentProvisioner];
                if (result) {
                    return [self getProvisionAddressWithElementCount:elementCount];
                } else {
                    return 0;
                }
            }
        }
    }
    return maxUnicastAddress;
}

/**
 * @brief   Get current netKey hex data of current mesh.
 * @return  netKey hex data
 */
- (NSData *)curNetKey {
    if (self.curNetkeyModel) {
        return [LibTools nsstringToHex:self.curNetkeyModel.key];
    }
    return nil;
}

/**
 * @brief   Get current appKey hex data of current mesh.
 * @return  appKey hex data
 */
- (NSData *)curAppKey {
    if (self.curAppkeyModel) {
        return [LibTools nsstringToHex:self.curAppkeyModel.key];
    }
    return nil;
}

/**
 * @brief   Get local provisioner of current mesh.
 * @return  local provisioner
 */
- (SigProvisionerModel *)curProvisionerModel {
    NSString *curUUID = [self getCurrentProvisionerUUID];
    NSArray *provisioners = [NSArray arrayWithArray: _provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        if ([provisioner.UUID isEqualToString:curUUID]) {
            return provisioner;
        }
    }
    return nil;
}

/**
 * @brief   Get local provisioner node of current mesh.
 * @return  local provisioner node
 */
- (SigNodeModel *)curLocationNodeModel {
    if (self.curProvisionerModel) {
        NSArray *nodes = [NSArray arrayWithArray: self.nodes];
        for (SigNodeModel *model in nodes) {
            if ([model.UUID isEqualToString:self.curProvisionerModel.UUID]) {
                return model;
            }
        }
    }
    return nil;
}

/**
 * @brief   Get online node count of current mesh.
 * @return  online node count
 */
- (NSInteger)getOnlineDevicesNumber {
    NSInteger count = 0;
    NSArray *curNodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *model in curNodes) {
        if (model.state != DeviceStateOutOfLine) {
            count ++;
        }
    }
    return count;
}

/**
 * @brief   Determine whether there is a node containing the time modelID.
 * @return  `YES` means exist, `NO` means not exist.
 */
- (BOOL)hasNodeExistTimeModelID {
    BOOL tem = NO;
    NSArray *curNodes = [NSArray arrayWithArray:self.curNodes];
    for (SigNodeModel *node in curNodes) {
        UInt32 option = kSigModel_TimeServer_ID;
        NSArray *elementAddresses = [node getAddressesWithModelID:@(option)];
        if (elementAddresses.count > 0) {
            tem = YES;
            break;
        }
    }
    return tem;
}

/**
 * @brief   Get the ivIndex of current mesh.
 * @return  ivIndex
 */
- (UInt32)getIvIndexUInt32 {
    return [LibTools uint32From16String:_ivIndex];
}

/**
 * @brief   Set the ivIndex to current mesh.
 * @param   ivIndexUInt32    the ivIndex of mesh.
 * @note    Developer can set ivIndex by this API if ivIndex is save in other place. Range is 0~0xFFFFFFFF, SDK will trigger ivUpdate process when sequenceNumber is greater than 0xC00000.
 */
- (void)setIvIndexUInt32:(UInt32)ivIndexUInt32 {
    _ivIndex = [NSString stringWithFormat:@"%08X", ivIndexUInt32];
}

/**
 * @brief   Get the SequenceNumber of current mesh.
 * @return  SequenceNumber
 */
- (UInt32)getSequenceNumberUInt32 {
    return [LibTools uint32From16String:_sequenceNumber];
}

/**
 * @brief   Set the sequenceNumber to current mesh.
 * @param   sequenceNumberUInt32    the sequenceNumber of mesh.
 * @note    Developer can set sequenceNumber by this API if sequenceNumber is save in other place. Range is 0~0xFFFFFF, SDK will trigger ivUpdate process when sequenceNumber is greater than 0xC00000.
 */
- (void)setSequenceNumberUInt32:(UInt32)sequenceNumberUInt32 {
    _sequenceNumber = [NSString stringWithFormat:@"%06X", sequenceNumberUInt32];
}

/**
 * @brief   Get the SigNodeSequenceNumberCacheModel from the match SequenceNumberCache through the unicastAddress of node..
 * @param   unicastAddress    the unicastAddress of node.
 * @return  A SigNodeSequenceNumberCacheModel that save NodeSequenceNumber information. nil means there are no SigNodeSequenceNumberCacheModel match this unicastAddress had been receive.
 * @note    SDK will clean all SigNodeSequenceNumberCacheModel when connect mesh success, then SDK add SigNodeSequenceNumberCacheModel by all mesh message notify on current mesh.
 */
- (SigNodeSequenceNumberCacheModel *)getSigNodeSequenceNumberCacheModelWithUnicastAddress:(UInt16)unicastAddress {
    SigNodeSequenceNumberCacheModel *tem = nil;
    NSArray *array = [NSArray arrayWithArray:_nodeSequenceNumberCacheList];
    for (SigNodeSequenceNumberCacheModel *model in array) {
        if (model.unicastAddress == unicastAddress) {
            tem = model;
            break;
        }
    }
    return tem;
}

/**
 * @brief   Update ivIndex in local after SDK receivce a new valid beacon.
 * @param   ivIndexUInt32    the new ivIndexUInt32.
 * @note    Callback this API when SDK receivce a new valid beacon.
 */
- (void)updateIvIndexUInt32FromBeacon:(UInt32)ivIndexUInt32 {
    if ([self getIvIndexUInt32] != ivIndexUInt32) {
        [self setIvIndexUInt32:ivIndexUInt32];
        UInt32 newSequenceNumber = 0;
        _sequenceNumberOnDelegate = newSequenceNumber;
        [self setSequenceNumberUInt32:newSequenceNumber];
        // since v3.3.3, Because after the ivIndex is updated, the sequenceNumber of all nodes will be reset to zero.
        [self.nodeSequenceNumberCacheList removeAllObjects];
        [self saveCurrentIvIndex:ivIndexUInt32 sequenceNumber:newSequenceNumber];
        __block NSString *blockIv = _ivIndex;
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([weakSelf.delegate respondsToSelector:@selector(onSequenceNumberUpdate:ivIndexUpdate:)]) {
                [weakSelf.delegate onSequenceNumberUpdate:weakSelf.sequenceNumberOnDelegate ivIndexUpdate:[LibTools uint32From16String:blockIv]];
            }
        });
    }
}

/**
 * @brief   Update sequenceNumber after SDK send a mesh message.
 * @param   sequenceNumberUInt32    the sequenceNumberUInt32 for next networkPDU.
 * @note    Callback this API when SDK send a networkPDU in SigNetworkLayer.
 */
- (void)updateSequenceNumberUInt32WhenSendMessage:(UInt32)sequenceNumberUInt32 {
    _sequenceNumber = [NSString stringWithFormat:@"%06X", sequenceNumberUInt32];
    if ((sequenceNumberUInt32 - _sequenceNumberOnDelegate >= self.defaultSequenceNumberIncrement) || (sequenceNumberUInt32 < _sequenceNumberOnDelegate)) {
        self.sequenceNumberOnDelegate = sequenceNumberUInt32;
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([weakSelf.delegate respondsToSelector:@selector(onSequenceNumberUpdate:ivIndexUpdate:)]) {
                [weakSelf.delegate onSequenceNumberUpdate:weakSelf.sequenceNumberOnDelegate ivIndexUpdate:self.getIvIndexUInt32];
            }
        });
    }
    [self saveCurrentIvIndex:[self getIvIndexUInt32] sequenceNumber:sequenceNumberUInt32];
}

/**
 * @brief   Loading local mesh data
 * @note    1.create provisionerUUID if need. 2.create mesh if need. 3.check provisioner. 4.load SigScanRspModel cache list.
 */
- (void)configData {
    // Initialize the unique identifier UUID of the current phone, and it will only be regenerated after uninstalling and reinstalling.
    NSString *provisionerUUID = [self getCurrentProvisionerUUID];
    if (provisionerUUID == nil) {
        [self saveCurrentProvisionerUUID:[LibTools convertDataToHexStr:[LibTools initMeshUUID]]];
    }

    NSData *locationData = [self getLocationMeshData];
    BOOL exist = locationData.length > 0;
    if (!exist) {
        //don't exist mesh.json, create and init mesh
        [self initMeshData];
        TelinkLogInfo(@"creat mesh_sample.json success");
        [self saveLocationData];
    }else{
        //exist mesh.json, load json
        NSData *data = [self getLocationMeshData];
        NSDictionary *meshDict = [LibTools getDictionaryWithJSONData:data];
        [self setDictionaryToDataSource:meshDict];
    }
    //check provisioner
    [self checkExistLocationProvisioner];
    //init SigScanRspModel list
    [self loadScanList];
}

/**
 * @brief   Check SigDataSource.provisioners.
 * @note    This api will auto create a provisioner when SigDataSource.provisioners hasn't provisioner corresponding to app's UUID.
 */
- (void)checkExistLocationProvisioner {
    //1.clean the match advertisementDataServiceData cache.
    if (_encryptedArray) {
        [_encryptedArray removeAllObjects];
    }

    //2.Check if _curProvisionerModel exists?
    if (self.curProvisionerModel) {
        TelinkLogInfo(@"exist local provisioner, needn't create");
    }else{
        //don't exist local provisioner, create and add to SIGDataSource.provisioners, then save location.
        //Attention: the max local address is 0x7fff, so max provisioner's allocatedUnicastRange highAddress can not bigger than 0x7fff.
        if (self.provisioners.count <= 0x7f) {
            SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerMaxHighAddressUnicast:[self getMaxHighAllocatedUnicastAddress] andProvisionerUUID:[self getCurrentProvisionerUUID]];
            [_provisioners addObject:provisioner];
            [self addLocationNodeWithProvisioner:provisioner];
            _timestamp = [LibTools getNowTimeStringOfJson];
            [self saveLocationData];
        }else{
            TelinkLogInfo(@"warning: count of provisioners is bigger than 0x7f, app allocates node address will be error.");
            return;
        }
    }

    // 3.config _filterModel.addressList
    // addresses is unicastAddress of curLocationNodeModel.
    SigNodeModel *node = self.curLocationNodeModel;
    NSMutableArray *addresses = [NSMutableArray array];
    if (node.elements && node.elements.count > 0) {
        NSArray *elements = [NSArray arrayWithArray:node.elements];
        for (SigElementModel *element in elements) {
            element.parentNodeAddress = node.address;
            // Add Unicast Addresses of all Elements of the Provisioner's Node.
            [addresses addObject:@(element.unicastAddress)];
            // Add all addresses that the Node's Models are subscribed to.
            NSArray *models = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *model in models) {
                if (model.subscribe && model.subscribe.count > 0) {
                    NSArray *subscribe = [NSArray arrayWithArray:model.subscribe];
                    for (NSString *addr in subscribe) {
                        UInt16 indAddr = [LibTools uint16From16String:addr];
                        [addresses addObject:@(indAddr)];
                    }
                }
            }
        }
    } else {
        [addresses addObject:@(node.address)];
    }
    // If _filterModel.filterType is a SigProxyFilerType_whitelist, the local address must exist and cannot be deleted, if it does not exist, add it here. If _filterModel.filterType is a SigProxyFilerType_blacklist, the local address cannot exist, and if it exists, it will be deleted here.
    for (NSNumber *localAddressNumber in addresses) {
        if (_filterModel.filterType == SigProxyFilerType_whitelist) {
            if (![_filterModel.addressList containsObject:localAddressNumber]) {
                [_filterModel.addressList addObject:localAddressNumber];
            }
        } else {
            if ([_filterModel.addressList containsObject:localAddressNumber]) {
                [_filterModel.addressList removeObject:localAddressNumber];
            }
        }
    }

    if (SigDataSource.share.existLocationIvIndexAndLocationSequenceNumber) {
        //SequenceNumber add defaultSequenceNumberIncrement.
        SigDataSource.share.ivIndex = SigDataSource.share.getLocalIvIndexString;
        //fix the ivIndex
        NSArray *netkeys = [NSArray arrayWithArray:_netKeys];
        for (SigNetkeyModel *key in netkeys) {
            key.ivIndex = [[SigIvIndex alloc] initWithIndex:[LibTools uint32From16String:SigMeshLib.share.dataSource.ivIndex] updateActive:NO];
        }
        [SigDataSource.share setSequenceNumberUInt32:SigDataSource.share.getLocalSequenceNumberUInt32 + SigDataSource.share.defaultSequenceNumberIncrement];
        [SigDataSource.share saveCurrentIvIndex:SigDataSource.share.getIvIndexUInt32 sequenceNumber:SigDataSource.share.getSequenceNumberUInt32];
    }
}

/**
 * @brief   Add node of provisioner to mesh network.
 * @param   provisioner    the SigProvisionerModel object.
 * @note    Callback this API when add a new provisioner to the mesh network.
 */
- (void)addLocationNodeWithProvisioner:(SigProvisionerModel *)provisioner {
    SigNodeModel *node = [[SigNodeModel alloc] init];

    // init defoult data
    node.UUID = provisioner.UUID;
    node.secureNetworkBeacon = YES;
    node.defaultTTL = TTL_DEFAULT;
    node.features.proxyFeature = SigNodeFeaturesState_notSupported;
    node.features.friendFeature = SigNodeFeaturesState_notEnabled;
    node.features.relayFeature = SigNodeFeaturesState_notSupported;
    node.relayRetransmit.relayRetransmitCount = 5;
    node.relayRetransmit.relayRetransmitIntervalSteps = 2;
    node.unicastAddress = [NSString stringWithFormat:@"%04X",(UInt16)provisioner.allocatedUnicastRange.firstObject.lowIntAddress];
    node.name = @"Telink iOS provisioner node";
    node.cid = @"0211";
    node.pid = @"0100";
    node.vid = @"0100";
    node.crpl = @"0100";

    // add elements
    NSMutableArray *elements = [NSMutableArray array];
    SigElementModel *element = [[SigElementModel alloc] init];
    element.name = @"Primary Element";
    element.location = @"0000";
    element.index = 0;
    NSMutableArray *models = [NSMutableArray array];
//    NSArray *defaultModelIDs = @[@"0000",@"0001",@"0002",@"0003",@"0005",@"FE00",@"FE01",@"FE02",@"FE03",@"FF00",@"FF01",@"1202",@"1001",@"1003",@"1005",@"1008",@"1205",@"1208",@"1302",@"1305",@"1309",@"1311",@"1015",@"00010211"];
    NSArray *defaultModelIDs = @[@"0000",@"0001"];
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

    NSData *devicekeyData = [LibTools createRandomDataWithLength:16];
    node.deviceKey = [LibTools convertDataToHexStr:devicekeyData];
    SigAppkeyModel *appkey = [self curAppkeyModel];
    SigNodeKeyModel *nodeAppkey = [[SigNodeKeyModel alloc] init];
    nodeAppkey.index = appkey.index;
    if (![node.appKeys containsObject:nodeAppkey]) {
        [node.appKeys addObject:nodeAppkey];
    }
    SigNodeKeyModel *nodeNetkey = [[SigNodeKeyModel alloc] init];
    nodeNetkey.index = self.curNetkeyModel.index;
    if (![node.netKeys containsObject:nodeNetkey]) {
        [node.netKeys addObject:nodeNetkey];
    }

    [_nodes addObject:node];
}

/**
 * @brief   Add node to mesh network.
 * @param   model    the SigNodeModel object.
 * @note    Callback this API when provision success.
 */
- (void)addAndSaveNodeToMeshNetworkWithDeviceModel:(SigNodeModel *)model {
    @synchronized(self) {
        if ([_nodes containsObject:model]) {
            NSInteger index = [_nodes indexOfObject:model];
            _nodes[index] = model;
        } else {
            [_nodes addObject:model];
        }
        _curNodes = nil;
        [self optimizationDataOfBlacklisted];
        [self saveLocationData];
    }
}

/**
 * @brief   Delete node from mesh network.
 * @param   deviceAddress    the unicastAddress of node.
 * @note    Callback this API when APP delete node complete. 1.SDK will set this node to blacklisted, then optimization Data Of Blacklisted.  2.Delete action with this unicasetAddress in scene. 3.Delete ScanRspModel of this unicastAddress. 4.Delete SigEncryptedModel of this unicastAddress.
 */
- (void)deleteNodeFromMeshNetworkWithDeviceAddress:(UInt16)deviceAddress {
    @synchronized(self) {
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (int i=0; i<nodes.count; i++) {
            SigNodeModel *model = nodes[i];
            if (model.address == deviceAddress) {
//                [_nodes removeObjectAtIndex:i];
                // use the `excluded` to flag the node of delete, need not remove node.
                model.excluded = YES;
                break;
            }
        }
        NSArray *scenes = [NSArray arrayWithArray:_scenes];
        for (SigSceneModel *scene in scenes) {
            NSArray *actionList = [NSArray arrayWithArray:scene.actionList];
            for (ActionModel *action in actionList) {
                if (action.address == deviceAddress) {
                    [scene.actionList removeObject:action];
                    break;
                }
            }
        }
        [self optimizationDataOfBlacklisted];
        [self saveLocationData];
        [self deleteScanRspModelWithUnicastAddress:deviceAddress];
        [self deleteSigEncryptedModelWithUnicastAddress:deviceAddress];
    }
}

/**
 * @brief   Get max unicast address of all provisioner.allocatedUnicastRange.
 * @return  new scene address, min address is 1.
 * @note    new scene address = exist scene address + 1, min address is 1.
 */
- (UInt16)getMaxHighAllocatedUnicastAddress {
    UInt16 max = 0;
    NSArray *provisioners = [NSArray arrayWithArray:_provisioners];
    for (SigProvisionerModel *provisioner in provisioners) {
        for (SigRangeModel *unicastRange in provisioner.allocatedUnicastRange) {
            if (max < unicastRange.heightIntAddress) {
                max = unicastRange.heightIntAddress;
            }
        }
    }
    return max;
}

/**
 * @brief   add or delete the groupID of node.
 * @param   add    `YES` means add groupID to node, `NO` means delete groupID from node.
 * @param   unicastAddress    the unicastAddress of node.
 * @param   groupAddress    the groupAddress of group.
 */
- (void)editGroupIDsOfDevice:(BOOL)add unicastAddress:(NSNumber *)unicastAddress groupAddress:(NSNumber *)groupAddress {
    @synchronized(self) {
        SigNodeModel *model = [self getDeviceWithAddress:[unicastAddress intValue]];
        if (model) {
            if (add) {
                if (![model.getGroupIDs containsObject:groupAddress]) {
                    [model addGroupID:groupAddress];
                    [self saveLocationData];
                } else {
                    TelinkLogInfo(@"groupID is exist, add group model fail.");
                }
            } else {
                if (![model.getGroupIDs containsObject:groupAddress]) {
                    TelinkLogInfo(@"groupID is not exist, delete group model fail.");
                } else {
                    [model deleteGroupID:groupAddress];
                    [self saveLocationData];
                }
            }
        } else {
            TelinkLogInfo(@"node no found, edit group model fail.");
        }
    }
}

/**
 * @brief   Update the state of all nodes to DeviceStateOutOfLine.
 * @note    1.set _curNodes to nil. 2.set node of _nodes to DeviceStateOutOfLine.
 */
- (void)setAllDevicesOutline {
    @synchronized(self) {
        _curNodes = nil;
        NSArray *nodes = [NSArray arrayWithArray:_nodes];
        for (SigNodeModel *model in nodes) {
            model.state = DeviceStateOutOfLine;
        }
    }
}

/// Manager mesh network, switch new mesh.
/// @param dict new Mesh Dictionary
- (void)switchToNewMeshDictionary:(NSDictionary *)dict {
    //v3.3.3.6及以后新逻辑：
    //1.当前手机的provisioner只在当前手机使用，且本地地址不变。
    //2.使用setDictionaryToDataSource接收分享后调用checkExistLocationProvisioner判断provisioner.
    //3.判断手机本地是否存在ivIndex+sequenceNumber，存在则赋值到SigDataSource且sequenceNumber+128.且立刻缓存一次本地。
    //4.不存在则需要连接获取到beacon的ivIndex。sequenceNumber从0开始。
    //5.重新计算下一次添加设备使用的unicastAddress
    [SigDataSource.share resetMesh];
    [SigDataSource.share setDictionaryToDataSource:dict];
    [SigDataSource.share checkExistLocationProvisioner];
    [SigDataSource.share saveLocationData];

    //（可选）注意：调用[SigDataSource.share resetMesh]后，filter的配置将恢复默认的白名单和本地地址+0xFFFF的配置。如果客户需要想要自定义配置，则需要再这下面进行filter相关配置。
//    NSData *filterData = [[NSUserDefaults standardUserDefaults] valueForKey:kFilter];
//    NSDictionary *filterDict = [LibTools getDictionaryWithJSONData:filterData];
//    SigProxyFilterModel *filter = [[SigProxyFilterModel alloc] init];
//    [filter setDictionaryToSigProxyFilterModel:filterDict];
}

/**
 * @brief   Save Current SigDataSource to local by NSUserDefaults.
 * @note    1.sort _nodes, sort _groups, sort _scene. 2.change SigDataSource to data, and save by NSUserDefaults. 3. save encrypt json data to file `TelinkSDKMeshJsonData`.
 */
- (void)saveLocationData {
    @synchronized(self) {
        [self.nodes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigNodeModel *)obj1 address] > [(SigNodeModel *)obj2 address];
        }];
        [self.groups sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(SigGroupModel *)obj1 intAddress] > [(SigGroupModel *)obj2 intAddress];
        }];
        [self.scenes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [LibTools uint16From16String:[(SigSceneModel *)obj1 number]] > [LibTools uint16From16String:[(SigSceneModel *)obj2 number]];
        }];

        NSDictionary *meshDict = [self getDictionaryFromDataSource];
        NSData *tempData = [LibTools getJSONDataWithDictionary:meshDict];
        [self saveLocationMeshData:tempData];
        saveMeshJsonData([LibTools getReadableJSONStringWithDictionary:meshDict]);
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([self.delegate respondsToSelector:@selector(onMeshNetworkUpdated:)]) {
                [self.delegate onMeshNetworkUpdated:self];
            }
        });
    }
}

/**
 * @brief   update the state of node.
 * @param   responseMessage    the state responseMessage of node.
 * @param   source    the unicastAddress of responseMessage.
 */
- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source {
    SigNodeModel *node = [self getDeviceWithAddress:source];
    if (responseMessage && node) {
        [node updateNodeStatusWithBaseMeshMessage:responseMessage source:source];
    }
}

/**
 * @brief   Get the new scene address for add a new scene object.
 * @return  new scene address, min address is 1.
 * @note    new scene address = exist scene address + 1, min address is 1.
 */
- (UInt16)getNewSceneAddress {
    UInt16 address = 1;
    if (_scenes.count > 0) {
        [_scenes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [LibTools uint16From16String:[(SigSceneModel *)obj1 number]] > [LibTools uint16From16String:[(SigSceneModel *)obj2 number]];
        }];
        address = [LibTools uint16From16String:_scenes.lastObject.number] + 1;
    }
    return address;
}

/**
 * @brief   Add or Update the information of  SigSceneModel to _scenes..
 * @param   model    the SigSceneModel object.
 */
- (void)saveSceneModelWithModel:(SigSceneModel *)model {
    @synchronized(self) {
        SigSceneModel *scene = [[SigSceneModel alloc] init];
        scene.number = model.number;
        scene.name = model.name;
        scene.actionList = [[NSMutableArray alloc] initWithArray:model.actionList];
        [scene.actionList sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
            return [(ActionModel *)obj1 address] > [(ActionModel *)obj2 address];
        }];

        if ([self.scenes containsObject:scene]) {
            NSInteger index = [self.scenes indexOfObject:scene];
            self.scenes[index] = scene;
        } else {
            [self.scenes addObject:scene];
        }
        [self saveLocationData];
    }
}

/**
 * @brief   Delete SigSceneModel from _scenes..
 * @param   model    the SigSceneModel object.
 */
- (void)deleteSceneModelWithModel:(SigSceneModel *)model {
    @synchronized(self) {
        if ([self.scenes containsObject:model]) {
            [self.scenes removeObject:model];
            [self saveLocationData];
        }
    }
}

/**
 * @brief   Get the SigEncryptedModel from the match advertisementDataServiceData cache through the unicastAddress of node..
 * @param   address    the unicastAddress of node.
 * @return  A SigEncryptedModel that save advertisementDataServiceData information. nil means there are no SigEncryptedModel match this unicastAddress had been scanned.
 * @note    SDK will cache the SigEncryptedModel when SDK scanned the nodeIdentity broadcast data of provisioned node or when SDK scanned the nodeIdentity broadcast data of provisioned node.
 */
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

/**
 * @brief   Delete the SigEncryptedModel from the match advertisementDataServiceData cache.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    SDK need to delete the SigEncryptedModel of the node when kickout the node or scan the newer nodeIdentity broadcast data of this unicastAddress.
 */
- (void)deleteSigEncryptedModelWithUnicastAddress:(UInt16)unicastAddress {
    @synchronized(self) {
        NSArray *encryptedArray = [NSArray arrayWithArray:_encryptedArray];
        for (SigEncryptedModel *model in encryptedArray) {
            if (model.address == unicastAddress) {
                [_encryptedArray removeObject:model];
                break;
            }
        }
    }
}

/**
 * @brief   Determine whether the SigScanRspModel is in current mesh network.
 * @param   model    the ScanRspModel object.
 * @return  `YES` means model belong to the network, `NO` means model does not belong to the network.
 * @note    The SigScanRspModel has all information of peripheral in Apple System.
 */
- (BOOL)existScanRspModelOfCurrentMeshNetwork:(SigScanRspModel *)model {
    if (model && model.advertisementDataServiceData && model.advertisementDataServiceData.length) {
        SigIdentificationType advType = [model getIdentificationType];
        switch (advType) {
            case SigIdentificationType_networkID:
            {
                if (model.advertisementDataServiceData.length >= 9) {
                    NSData *networkID = [model.advertisementDataServiceData subdataWithRange:NSMakeRange(1, 8)];
                    return [self matchWithNetworkID:networkID];
                }
            }
                break;
            case SigIdentificationType_nodeIdentity:
            {
                if (model.advertisementDataServiceData.length >= 17) {
                    if ([self existEncryptedWithAdvertisementDataServiceData:model.advertisementDataServiceData]) {
                        return YES;
                    } else {
                        return [self matchNodeIdentityWithAdvertisementDataServiceData:model.advertisementDataServiceData peripheralUUIDString:model.uuid nodes:self.curNodes networkKey:self.curNetkeyModel];
                    }
                }
            }
                break;
            case SigIdentificationType_privateNetworkIdentity:
            {
//                TelinkLogDebug(@"receive SigIdentificationType_privateNetworkIdentity");
                if (model.advertisementDataServiceData.length >= 17) {
                    if ([self existEncryptedWithAdvertisementDataServiceData:model.advertisementDataServiceData]) {
                        return YES;
                    } else {
                        return [self matchPrivateNetworkIdentityWithAdvertisementDataServiceData:model.advertisementDataServiceData peripheralUUIDString:model.uuid networkKey:self.curNetkeyModel];
                    }
                }
            }
                break;
            case SigIdentificationType_privateNodeIdentity:
            {
//                TelinkLogDebug(@"receive SigIdentificationType_privateNodeIdentity");
                if (model.advertisementDataServiceData.length >= 17) {
                    if ([self existEncryptedWithAdvertisementDataServiceData:model.advertisementDataServiceData]) {
                        return YES;
                    } else {
                        return [self matchPrivateNodeIdentityWithAdvertisementDataServiceData:model.advertisementDataServiceData peripheralUUIDString:model.uuid nodes:self.curNodes networkKey:self.curNetkeyModel];
                    }
                }
            }
                break;

            default:
                break;
        }
    }
    return NO;
}

/**
 * @brief   Determine whether the peripheralUUIDString is in current mesh network.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @return  `YES` means peripheralUUIDString belong to the network, `NO` means peripheralUUIDString does not belong to the network.
 * @note    The peripheralUUIDString is the peripheral identify in Apple System.
 */
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString {
    SigNodeModel *node = [self getNodeWithPeripheralUUIDString:peripheralUUIDString];
    return node != nil;
}

/**
 * @brief   Determine whether the advertisementDataServiceData is exist in the match advertisementDataServiceData cache of current mesh network. (the match advertisementDataServiceData cache can avoid repeated calculations of advertisementDataServiceData.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)existEncryptedWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData {
    SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
    tem.advertisementDataServiceData = advertisementDataServiceData;
    return [_encryptedArray containsObject:tem];
}

/**
 * @brief   Determine whether the networkID is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is networkID.)
 * @param   networkID    the networkID in ScanRspModel object.
 * @return  `YES` means networkID belong to the network, `NO` means networkID does not belong to the network.
 * @note    The networkID is the broadcast data of provisioned node.
 */
- (BOOL)matchWithNetworkID:(NSData *)networkID {
    if (self.curNetkeyModel.phase == distributingKeys) {
        if (self.curNetkeyModel.oldNetworkId && self.curNetkeyModel.oldNetworkId.length > 0) {
            return [self.curNetkeyModel.oldNetworkId isEqualToData:networkID];
        }
    } else {
        if (self.curNetkeyModel.networkId && self.curNetkeyModel.networkId.length > 0) {
            return [self.curNetkeyModel.networkId isEqualToData:networkID];
        }
    }
    return NO;
}

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is nodeIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   nodes    the node list of mesh network.
 * @param   networkKey    the network key of mesh network.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchNodeIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString nodes:(NSArray <SigNodeModel *>*)nodes networkKey:(SigNetkeyModel *)networkKey {
    // Data are: 48 bits of Padding (0s), 64 bit Random and Unicast Address.
    Byte byte[6];
    memset(byte, 0, 6);
    NSData *paddingData = [NSData dataWithBytes:byte length:6];
    return [self matchNodeIdentityWithAdvertisementDataServiceData:advertisementDataServiceData peripheralUUIDString:peripheralUUIDString nodes:nodes networkKey:networkKey paddingData:paddingData];
}

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is privateNetworkIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   networkKey    the network key of mesh network.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchPrivateNetworkIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString networkKey:(SigNetkeyModel *)networkKey {
    NSData *networkID = networkKey.networkId;
    NSData *netKey = [LibTools nsstringToHex:networkKey.key];
    NSData *identityKey = networkKey.keys.identityKey;
    if (networkKey.phase == distributingKeys) {
        if (networkKey.oldNetworkId && networkKey.oldNetworkId.length > 0) {
            networkID = networkKey.oldNetworkId;
            netKey = [LibTools nsstringToHex:networkKey.oldKey];
            identityKey = networkKey.oldKeys.identityKey;
        }
    } else {
        if (networkKey.networkId && networkKey.networkId.length > 0) {
            networkID = networkKey.networkId;
            netKey = [LibTools nsstringToHex:networkKey.key];
            identityKey = networkKey.keys.identityKey;
        }
    }
    if (networkID == nil || netKey == nil || identityKey == nil) {
        return NO;
    }
    NSData *hash = [advertisementDataServiceData subdataWithRange:NSMakeRange(1, 8)];
    NSData *random = [advertisementDataServiceData subdataWithRange:NSMakeRange(9, 8)];
    NSMutableData *mData = [NSMutableData dataWithData:networkID];
    [mData appendData:random];
    NSData *encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:identityKey];
    BOOL isExist = NO;
    if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
        isExist = YES;
    }
    // If the Key refresh procedure is in place, the identity might have been generated with the old key.
    if (!isExist && networkKey.oldKey && networkKey.oldKey.length > 0 && ![networkKey.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
        networkID = networkKey.oldNetworkId;
        netKey = [LibTools nsstringToHex:networkKey.oldKey];
        identityKey = networkKey.oldKeys.identityKey;
        if (networkID == nil || netKey == nil || identityKey == nil) {
            return NO;
        }
        mData = [NSMutableData dataWithData:networkID];
        [mData appendData:random];
        encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:identityKey];
        if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
            isExist = YES;
        }
    }
    if (isExist) {
        SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
        tem.advertisementDataServiceData = advertisementDataServiceData;
        tem.hashData = hash;
        tem.randomData = random;
        tem.peripheralUUID = peripheralUUIDString;
        tem.encryptedData = encryptedData;
        [self.encryptedArray addObject:tem];
        return YES;
    }
    return NO;
}

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is privateNodeIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   nodes    the node list of mesh network.
 * @param   networkKey    the network key of mesh network.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchPrivateNodeIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString nodes:(NSArray <SigNodeModel *>*)nodes networkKey:(SigNetkeyModel *)networkKey {
    // Data are: 40 bits of Padding (0s), 8bits is 0x03, 64 bits Random and 16 bits Unicast Address.
    Byte byte[6];
    memset(byte, 0, 6);
    byte[5] = SigIdentificationType_privateNodeIdentity;
    NSData *paddingData = [NSData dataWithBytes:byte length:6];
    return [self matchNodeIdentityWithAdvertisementDataServiceData:advertisementDataServiceData peripheralUUIDString:peripheralUUIDString nodes:nodes networkKey:networkKey paddingData:paddingData];
}

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is privateNodeIdentity of nodeIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   nodes    the node list of mesh network.
 * @param   networkKey    the network key of mesh network.
 * @param   paddingData    the length of paddingData is 8 bytes, paddingData of nodeIdentity is 0x000000000000, paddingData of privateNodeIdentity is 0x000000000003.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchNodeIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString nodes:(NSArray <SigNodeModel *>*)nodes networkKey:(SigNetkeyModel *)networkKey paddingData:(NSData *)paddingData {
    NSData *hash = [advertisementDataServiceData subdataWithRange:NSMakeRange(1, 8)];
    NSData *random = [advertisementDataServiceData subdataWithRange:NSMakeRange(9, 8)];
    NSArray *curNodes = [NSArray arrayWithArray:nodes];
    for (SigNodeModel *node in curNodes) {
        NSMutableData *mData = [NSMutableData dataWithData:paddingData];
        [mData appendData:random];
        // Convert the digital Number in big-endian mode to the local data storage mode
        UInt16 address = CFSwapInt16BigToHost(node.address);;
        NSData *data = [NSData dataWithBytes:&address length:2];
        [mData appendData:data];
        NSData *encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:networkKey.keys.identityKey];
        BOOL isExist = NO;
        if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
            isExist = YES;
        }
        // If the Key refresh procedure is in place, the identity might have been generated with the old key.
        if (!isExist && networkKey.oldKey && networkKey.oldKey.length > 0 && ![networkKey.oldKey isEqualToString:@"00000000000000000000000000000000"]) {
            encryptedData = [OpenSSLHelper.share calculateEvalueWithData:mData andKey:networkKey.oldKeys.identityKey];
            if ([[encryptedData subdataWithRange:NSMakeRange(8, encryptedData.length-8)] isEqualToData:hash]) {
                isExist = YES;
            }
        }
        if (isExist) {
            NSMutableData *mData = [NSMutableData dataWithData:hash];
            [mData appendData:random];
            SigEncryptedModel *tem = [[SigEncryptedModel alloc] init];
            tem.advertisementDataServiceData = advertisementDataServiceData;
            tem.hashData = hash;
            tem.randomData = random;
            tem.peripheralUUID = peripheralUUIDString;
            tem.encryptedData = encryptedData;
            tem.address = node.address;
            [self deleteSigEncryptedModelWithUnicastAddress:node.address];
            [self.encryptedArray addObject:tem];
            return YES;
        }
    }
    return NO;
}

/**
 * @brief   update the ScanRspModel object information to dataSource.
 * @param   model    the ScanRspModel object.
 * @note    The SigScanRspModel is the node bluetooth information of in Apple System.
 */
- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model {
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
                if (oldModel.advUuid && oldModel.advUuid.length > 0 && model.advUuid == nil) {
                    model.advUuid = oldModel.advUuid;
                }
                if (![oldModel.macAddress isEqualToString:model.macAddress] || oldModel.address != model.address || ![oldModel.advertisementDataServiceData isEqualToData:model.advertisementDataServiceData]) {
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

/**
 * @brief   Get the ScanRspModel object through the peripheral UUIDString of node.
 * @param   peripheralUUIDString    the peripheral UUIDString of node.
 * @note    The SigScanRspModel is the node bluetooth information of in Apple System.
 */
- (SigScanRspModel *)getScanRspModelWithUUID:(NSString *)peripheralUUIDString {
    NSArray *scanList = [NSArray arrayWithArray:_scanList];
    for (SigScanRspModel *model in scanList) {
        if ([model.uuid isEqualToString:peripheralUUIDString]) {
            return model;
        }
    }
    return nil;
}

/**
 * @brief   Get the ScanRspModel object through the macAddress of node.
 * @param   macAddress    the macAddress of node.
 * @note    The SigScanRspModel is the node bluetooth information of in Apple System.
 */
- (SigScanRspModel *)getScanRspModelWithMacAddress:(NSString *)macAddress {
    NSArray *scanList = [NSArray arrayWithArray:_scanList];
    for (SigScanRspModel *model in scanList) {
        if ([model.macAddress isEqualToString:macAddress]) {
            return model;
        }
    }
    return nil;
}

/**
 * @brief   Get the ScanRspModel object through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    The SigScanRspModel is the node bluetooth information of in Apple System.
 */
- (SigScanRspModel *)getScanRspModelWithUnicastAddress:(UInt16)unicastAddress {
    NSArray *scanList = [NSArray arrayWithArray:_scanList];
    for (SigScanRspModel *model in scanList) {
        if (model.provisioned == YES && model.address == unicastAddress) {
            return model;
        }
    }
    return nil;
}

/**
 * @brief   Delete the ScanRspModel object through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    The SigScanRspModel is the node bluetooth information of in Apple System.
 */
- (void)deleteScanRspModelWithUnicastAddress:(UInt16)unicastAddress {
    @synchronized(self) {
        NSArray *scanList = [NSArray arrayWithArray:_scanList];
        for (SigScanRspModel *model in scanList) {
            if (model.address == unicastAddress) {
                [_scanList removeObject:model];
                break;
            }
        }
        [self saveScanList];
    }
}

/**
 * @brief   Get the netKey object through the networkId of netKey.
 * @param   networkId    the networkId of netKey.
 * @return  A SigNetkeyModel that save netKey information. nil means there are no netKey with this networkId in mesh network.
 */
- (SigNetkeyModel * _Nullable)getNetkeyModelWithNetworkId:(NSData *)networkId {
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

/**
 * @brief   Get the netKey object through the netKeyIndex of netKey.
 * @param   netKeyIndex    the netKeyIndex of netKey.
 * @return  A SigNetkeyModel that save netKey information. nil means there are no netKey with this netKeyIndex in mesh network.
 */
- (SigNetkeyModel * _Nullable)getNetkeyModelWithNetkeyIndex:(NSInteger)netKeyIndex {
    SigNetkeyModel *tem = nil;
    NSArray *netKeys = [NSArray arrayWithArray:_netKeys];
    for (SigNetkeyModel *model in netKeys) {
        if (model.index == netKeyIndex) {
            tem = model;
            break;
        }
    }
    return tem;
}

/**
 * @brief   Get the appKey object through the appKeyIndex of appKey.
 * @param   appkeyIndex    the appKeyIndex of appKey.
 * @return  A SigAppkeyModel that save appKey information. nil means there are no appKey with this appKeyIndex in mesh network.
 */
- (SigAppkeyModel * _Nullable)getAppkeyModelWithAppkeyIndex:(NSInteger)appkeyIndex {
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

/**
 * @brief   Get the node object through the bluetooth PeripheralUUID of node.
 * @param   peripheralUUIDString    the bluetooth PeripheralUUID of node.
 * @return  A SigNodeModel that save node information. nil means there are no node with this unicastAddress in mesh network.
 * @note    The unicastAddress is the unique identifier of the node in the mesh network, app config the unicastAddress of node in provision progress when app add node to the mesh network. The bluetooth PeripheralUUID is the unique identifier of Apple System.
 */
- (SigNodeModel * _Nullable)getNodeWithPeripheralUUIDString:(NSString *)peripheralUUIDString {
    NSArray *nodes = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *model in nodes) {
        if ([model.peripheralUUID isEqualToString:peripheralUUIDString]) {
            return model;
        }
    }
    return nil;
}

/**
 * @brief   Get the node object through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  A SigNodeModel that save node information. nil means there are no node with this unicastAddress in mesh network.
 * @note    The unicastAddress is the unique identifier of the node in the mesh network, app config the unicastAddress of node in provision progress when app add node to the mesh network.
 */
- (SigNodeModel * _Nullable)getNodeWithAddress:(UInt16)unicastAddress {
    NSArray *nodes = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *model in nodes) {
        if (model.elements.count > 1) {
            if (model.address <= unicastAddress && model.address + model.elements.count - 1 >= unicastAddress) {
                return model;
            }
        } else {
            if (model.address == unicastAddress) {
                return model;
            }
        }
    }
    return nil;
}

/**
 * @brief   Get current connected node object.
 * @return  A SigNodeModel that save node information. nil means there are no connected node in the mesh network.
 */
- (SigNodeModel * _Nullable)getCurrentConnectedNode {
    SigNodeModel *node = [self getNodeWithAddress:self.unicastAddressOfConnected];
    return node;
}

/**
 * @brief   Get the provisioner object through the provisioner node address.
 * @param   address    the unicastAddress of provisioner node.
 * @return  A SigProvisionerModel that save povisioner information. nil means no povisioner information of this unicastAddress.
 */
- (SigProvisionerModel * _Nullable)getProvisionerModelWithAddress:(UInt16)address {
    SigProvisionerModel *tem = nil;
    NSArray *array = [NSArray arrayWithArray:_provisioners];
    for (SigProvisionerModel *model in array) {
        if (model.node && model.node.address == address) {
            tem = model;
            break;
        }
    }
    return tem;
}

/**
 * @brief   Get the provisioner object through the provisionerUUID.
 * @param   provisionerUUIDString    The UUID String of provisioner.
 * @return  A SigProvisionerModel that save povisioner information. nil means no povisioner information of this UUID.
 */
- (SigProvisionerModel * _Nullable)getProvisionerModelWithProvisionerUUIDString:(NSString *)provisionerUUIDString {
    SigProvisionerModel *tem = nil;
    NSArray *array = [NSArray arrayWithArray:_provisioners];
    for (SigProvisionerModel *model in array) {
        if ([model.UUID isEqualToString:provisionerUUIDString]) {
            tem = model;
            break;
        }
    }
    return tem;
}

/**
 * @brief   Get the modelID object through the modelID
 * @param   modelID    The id of model.
 * @return  A ModelIDModel that save model information. nil means no model information of this modelID.
 */
- (ModelIDModel * _Nullable)getModelIDModel:(NSNumber *)modelID{
    ModelIDs *modelIDs = [[ModelIDs alloc] init];
    NSArray *all = [NSArray arrayWithArray:modelIDs.modelIDs];
    for (ModelIDModel *model in all) {
        if (model.sigModelID == [modelID intValue]) {
            return model;
        }
    }
    return nil;
}

/**
 * @brief   Get the group object through the group address
 * @param   groupAddress    The address of group.
 * @return  A SigGroupModel that save group information. nil means no group information of this groupAddress.
 */
- (SigGroupModel * _Nullable)getGroupModelWithGroupAddress:(UInt16)groupAddress {
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

/**
 * @brief   Obtain the DeviceTypeModel through the PID and CID of the node.
 * @param   CID    The company id of node.
 * @param   PID    The product id of node.
 * @return  A SigOOBModel that save oob  information. nil means no oob information of this UUID.
 * @note    This API is using to get the DeviceTypeModel through the PID and CID of the node, DeviceTypeModel has default composition data of node.
 */
- (DeviceTypeModel * _Nullable)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID {
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

/**
 * @brief   Add a oobModel cached by the SDK.
 * @param   oobModel    an oob object.
 * @note    This API is using to add oob information of node, oob information is use for OOB provision. If there is an old OOB object that has same UUID, the SDK will replace it.
 */
- (void)addAndUpdateSigOOBModel:(SigOOBModel *)oobModel {
    if ([self.OOBList containsObject:oobModel]) {
        NSInteger index = [self.OOBList indexOfObject:oobModel];
        [self.OOBList replaceObjectAtIndex:index withObject:oobModel];
    } else {
        [self.OOBList addObject:oobModel];
    }
    [self saveCurrentOobList];
}

/**
 * @brief   Add a list of oobModel cached by the SDK.
 * @param   oobModelList    a list of oob object.
 * @note    This API is using to add oob information of node, oob information is use for OOB provision. If there is an old OOB object that has same UUID, the SDK will replace it.
 */
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

/**
 * @brief   Delete one oobModel cached by the SDK.
 * @param   oobModel    The oob object.
 * @note    This API is using to delete oob information of node, oob information is use for OOB provision.
 */
- (void)deleteSigOOBModel:(SigOOBModel *)oobModel {
    if ([self.OOBList containsObject:oobModel]) {
        [self.OOBList removeObject:oobModel];
        [self saveCurrentOobList];
    }
}

/**
 * @brief   Remove all OOB data cached by the SDK.
 * @note    This API is using to delete oob information of all nodes, oob information is use for OOB provision.
 */
- (void)deleteAllSigOOBModel {
    [self.OOBList removeAllObjects];
    [self saveCurrentOobList];
}

/**
 * @brief   Obtaining OOB information of node based on UUID of unprovision node.
 * @param   UUIDString    The UUID of unprovision node, app get UUID from scan unprovision node.
 * @return  A SigOOBModel that save oob  information. nil means no oob information of this UUID.
 * @note    This API is using to get oob information of node, oob information is use for OOB provision.
 */
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

/**
 * @brief   Save current OOB List to NSUserDefaults.
 */
- (void)saveCurrentOobList {
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:self.OOBList];
    [[NSUserDefaults standardUserDefaults] setObject:data forKey:kOOBStoreKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

#pragma mark - new api since v3.3.3

/**
 * @brief   Assign a new address range to the current provisioner. The SDK will automatically call this method internally, without the need for users to actively call it.
 * @return  `YES` means add success, `NO` means add fail.
 * @note    This API is using to add a new UnicastRange to  current provisioner.
 */
- (BOOL)addNewUnicastRangeToCurrentProvisioner {
    UInt16 maxAddress = [self getMaxHighAllocatedUnicastAddress];
    if (maxAddress >= 0x7FFF) {
        //All addresses have been assigned.
        TelinkLogError(@"All addresses have been assigned.");
        return NO;
    } else {
        SigRangeModel *rangeModel = [[SigRangeModel alloc] initWithMaxHighAddressUnicast:maxAddress];
        if (self.curProvisionerModel.allocatedUnicastRange) {
            TelinkLogInfo(@"Provisioner.UUID:%@ add Range:%@~%@",self.curProvisionerModel.UUID,rangeModel.lowAddress,rangeModel.highAddress);
            [self.curProvisionerModel.allocatedUnicastRange addObject:rangeModel];
            //if add some thing to json, how does app update json to cloud.
            if ([self.delegate respondsToSelector:@selector(onUpdateAllocatedUnicastRange:ofProvisioner:)]) {
                [self.delegate onUpdateAllocatedUnicastRange:rangeModel ofProvisioner:self.curProvisionerModel];
            }
        }
        return YES;
    }
}

/**
 * @brief   Initialize data for a mesh network. By default, all parameters are randomly generated. The data in SigDataSource.share (including scanList, sequenceNumber, and sequenceNumberOnDelegate) will not be cleared.
 * @note    This API is using to create a new mesh network.
 */
- (instancetype)initDefaultMesh {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self initData];
        [self initMeshData];
    }
    return self;
}

/**
 * @brief   Clear all parameters in SigDataSource.share (including scanList, sequenceNumber, sequenceNumberOnDelegate) and randomly generate new default parameters.
 * @note    This API is using to clear all parameter of SigDataSource. The sequenceNumber of current provisioner of current mesh will save in NSUserDefaults for use next time.
 */
- (void)resetMesh {
    [self initMeshData];
    [self cleanScanList];
    [self.scanList removeAllObjects];
    _sequenceNumberOnDelegate = 0;
    [self setIvIndexUInt32:0];
    [self setSequenceNumberUInt32:0];
    [self saveLocationData];
}

/**
 * @brief   Update and save the vid of node to SigDataSource.
 * @param   address    the unicastAddress of node.
 * @param   vid    the versionID of node.
 * @note    This API is using to update and save the vid of node when meshOTA success.
 */
- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid {
    SigNodeModel *node = [self getNodeWithAddress:address];
    if (node) {
        if ([LibTools uint16From16String:node.vid] != vid) {
            node.vid = [SigHelper.share getUint16String:vid];
            [self saveLocationData];
        }
    }
}

#pragma mark - new api since v4.1.0.0

/**
 * @brief   Method for add extend group information.
 * @note    This API is using to fix the bug about level control of group.(When customers export Mesh, if they do not want to export extension group information, they can call this method to remove the extension group information and then export Mesh.)
 */
-(void)addExtendGroupList {
    NSArray *baseGroupList = [self getAllBaseGroupList];
    NSMutableArray *createExtendGroupList = [NSMutableArray array];
    for (SigGroupModel *group in baseGroupList) {
        [createExtendGroupList addObjectsFromArray:[self createExtendGroupListOfGroupModel:group]];
    }
    NSArray *invalidGroupList = [self getAllInvalidGroupList];
    NSMutableArray *groups = [NSMutableArray array];
    [groups addObjectsFromArray:baseGroupList];
    [groups addObjectsFromArray:createExtendGroupList];
    [groups addObjectsFromArray:invalidGroupList];
    _groups = groups;
}

/**
 * @brief   Method for deleting extend group information.
 * @note    This API is using to fix the bug about level control of group.(When customers export Mesh, if they do not want to export extension group information, they can call this method to remove the extension group information and then export Mesh.)
 */
-(void)removeExtendGroupList {
    NSArray *baseGroupList = [self getAllBaseGroupList];
    NSArray *invalidGroupList = [self getAllInvalidGroupList];
    NSMutableArray *groups = [NSMutableArray array];
    [groups addObjectsFromArray:baseGroupList];
    [groups addObjectsFromArray:invalidGroupList];
    _groups = groups;
}

/**
 * @brief   Create an array of all extend groups for a single base group.
 * @param   groupModel    the base SigGroupModel.
 * @return  A list of SigGroupModel that is base group.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)createExtendGroupListOfGroupModel:(SigGroupModel *)groupModel {
    //(((基础 GroupAddress&0xFF)<<8)|0xD000 ~ ((基础GroupAddress&0xFF)<<8)| 0xD000+0xF)
    NSMutableArray *mArray = [NSMutableArray array];
    if (groupModel.intAddress >= 0xC000 && groupModel.intAddress <= 0xC0FF) {
        NSArray *subgroupNames = @[@"lightness", @"temperature", @"Hue", @"Saturation"];
        UInt16 extendAddress = [self getExtendGroupAddressWithBaseGroupAddress:groupModel.intAddress];
        for (int i=0; i<subgroupNames.count; i++) {
            SigGroupModel *group = [[SigGroupModel alloc] init];
            group.address = [NSString stringWithFormat:@"%04X", extendAddress + i];
            group.parentAddress = @"0000";
            group.name = [NSString stringWithFormat:@"%@ subgroup level %@", groupModel.name, subgroupNames[i]];
            [mArray addObject:group];
        }
    }
    return mArray;
}

/**
 * @brief   Get all base group arrays for the current Mesh, the base group addresses ranging from 0xC000 to 0xC0FF.
 * @return  A list of SigGroupModel that is base group.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllBaseGroupList {
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *groups = [NSArray arrayWithArray:_groups];
    for (SigGroupModel *model in groups) {
        if (model.intAddress >= 0xC000 && model.intAddress <= 0xC0FF) {
            [mArray addObject:model];
        }
    }
    return mArray;
}

/**
 * @brief   Get all extend group arrays for the current Mesh, the extend group addresses ranging from 0xD000 to 0xDFFF.
 * @return  A list of SigGroupModel that is extend group.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllExtendGroupList {
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *groups = [NSArray arrayWithArray:_groups];
    for (SigGroupModel *model in groups) {
//        if (model.intAddress >= 0xD000 && model.intAddress <= 0xDFFF) {
//            [mArray addObject:model];
//        }
        if ([model.name rangeOfString:@"subgroup"].location != NSNotFound) {
            [mArray addObject:model];
        }
    }
    return mArray;
}

/**
 * @brief   Get all invalid group arrays for the current Mesh, the invalid group addresses ranging from 0xE000 to 0xFEFF.
 * @return  A list of SigGroupModel that is invalid.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllInvalidGroupList {
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *groups = [NSArray arrayWithArray:_groups];
    for (SigGroupModel *model in groups) {
        if (model.intAddress >= 0xE000 && model.intAddress <= 0xFEFF) {
            [mArray addObject:model];
        }
    }
    return mArray;
}

/**
 * @brief   Get a list of all UI displayed groups for the current Mesh, including a basic group list and an invalid group list.
 * @return  A list of SigGroupModel that show on APP.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllShowGroupList {
    NSMutableArray *mArray = [NSMutableArray array];
    [mArray addObjectsFromArray:self.getAllBaseGroupList];
    [mArray addObjectsFromArray:self.getAllInvalidGroupList];
    return mArray;
}

/**
 * @brief   Calculate the extrnd group address based on the base group address.
 * @param   baseGroupAddress    the base group address.
 * @return  Extend group address is UInt16.
 * @note    This API is using to fix the bug about level control of group.
 */
- (UInt16)getExtendGroupAddressWithBaseGroupAddress:(UInt16)baseGroupAddress {
    return ((baseGroupAddress & 0xFF) << 4) | 0xD000;
}

#pragma mark - Special handling: store the PrivateGattProxy+ConfigGattProxy+PrivateBeacon+ConfigBeacon of node in current mesh

/**
 * @brief   Get loca PrivateGattProxy state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  PrivateGattProxy state.
 * @note    This API is used to get the value of PrivateGattProxy state stored locally.
 */
- (BOOL)getLocalPrivateGattProxyStateOfUnicastAddress:(UInt16)unicastAddress {
    return [self getLocalStateWithUnicastAddress:unicastAddress key:kLocalPrivateGattProxy_key];
}

/**
 * @brief   Get loca ConfigGattProxy state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  ConfigGattProxy state.
 * @note    This API is used to get the value of ConfigGattProxy state stored locally.
 */
- (BOOL)getLocalConfigGattProxyStateOfUnicastAddress:(UInt16)unicastAddress {
    return [self getLocalStateWithUnicastAddress:unicastAddress key:kLocalConfigGattProxy_key];
}

/**
 * @brief   Get loca PrivateBeacon state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  PrivateBeacon state.
 * @note    This API is used to get the value of PrivateBeacon state stored locally.
 */
- (BOOL)getLocalPrivateBeaconStateOfUnicastAddress:(UInt16)unicastAddress {
    return [self getLocalStateWithUnicastAddress:unicastAddress key:kLocalPrivateBeacon_key];
}

/**
 * @brief   Get loca ConfigBeacon state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  ConfigBeacon state.
 * @note    This API is used to get the value of ConfigBeacon state stored locally.
 */
- (BOOL)getLocalConfigBeaconStateOfUnicastAddress:(UInt16)unicastAddress {
    return [self getLocalStateWithUnicastAddress:unicastAddress key:kLocalConfigBeacon_key];
}

/**
 * @brief   Get loca  state.
 * @param   unicastAddress    the unicastAddress of node.
 * @param   key    the key of state.
 * @note    This API is used to get the value of state stored locally.
 */
- (BOOL)getLocalStateWithUnicastAddress:(UInt16)unicastAddress key:(NSString *)key {
    NSDictionary *dict = @{};
    NSData *data = [[NSUserDefaults standardUserDefaults] objectForKey:kLocalPrivateBeaconDictionary_key];
    if (data != nil) {
        dict = [LibTools getDictionaryWithJSONData:data];
    }
    NSMutableDictionary *mDict = [NSMutableDictionary dictionaryWithDictionary:dict];
    if ([mDict.allKeys containsObject:[SigHelper.share getUint16String:unicastAddress]]) {
        NSMutableDictionary *nodeDict = [NSMutableDictionary dictionaryWithDictionary:mDict[[SigHelper.share getUint16String:unicastAddress]]];
        return [nodeDict[key] boolValue];
    } else {
        NSDictionary *nodeDict = @{kLocalPrivateGattProxy_key:@(NO), kLocalConfigGattProxy_key:@(YES), kLocalPrivateBeacon_key:@(NO), kLocalConfigBeacon_key:@(YES)};
        mDict[[SigHelper.share getUint16String:unicastAddress]] = nodeDict;
        [[NSUserDefaults standardUserDefaults] setObject:[LibTools getJSONDataWithDictionary:mDict] forKey:kLocalPrivateBeaconDictionary_key];
        [[NSUserDefaults standardUserDefaults] synchronize];
        return [nodeDict[key] boolValue];
    }
}

/**
 * @brief   Set loca PrivateGattProxy state.
 * @param   state    the PrivateGattProxy state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of PrivateGattProxy state stored locally.
 */
- (void)setLocalPrivateGattProxyState:(BOOL)state unicastAddress:(UInt16)unicastAddress {
    [self setLocalWithState:state unicastAddress:unicastAddress key:kLocalPrivateGattProxy_key];
}

/**
 * @brief   Set loca ConfigGattProxy state.
 * @param   state    the ConfigGattProxy state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of ConfigGattProxy state stored locally.
 */
- (void)setLocalConfigGattProxyState:(BOOL)state unicastAddress:(UInt16)unicastAddress {
    [self setLocalWithState:state unicastAddress:unicastAddress key:kLocalConfigGattProxy_key];
}

/**
 * @brief   Set loca PrivateBeacon state.
 * @param   state    the PrivateBeacon state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of PrivateBeacon state stored locally.
 */
- (void)setLocalPrivateBeaconState:(BOOL)state unicastAddress:(UInt16)unicastAddress {
    [self setLocalWithState:state unicastAddress:unicastAddress key:kLocalPrivateBeacon_key];
}

/**
 * @brief   Set loca ConfigBeacon state.
 * @param   state    the ConfigBeacon state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of ConfigBeacon state stored locally.
 */
- (void)setLocalConfigBeaconState:(BOOL)state unicastAddress:(UInt16)unicastAddress {
    [self setLocalWithState:state unicastAddress:unicastAddress key:kLocalConfigBeacon_key];
}

/**
 * @brief   Set loca  state.
 * @param   state    the  state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @param   key    the key of state.
 * @note    This API is used to get the value of state stored locally.
 */
- (void)setLocalWithState:(BOOL)state unicastAddress:(UInt16)unicastAddress key:(NSString *)key {
    NSDictionary *dict = @{};
    NSData *data = [[NSUserDefaults standardUserDefaults] objectForKey:kLocalPrivateBeaconDictionary_key];
    if (data != nil) {
        dict = [LibTools getDictionaryWithJSONData:data];
    }
    NSMutableDictionary *mDict = [NSMutableDictionary dictionaryWithDictionary:dict];
    if ([mDict.allKeys containsObject:[SigHelper.share getUint16String:unicastAddress]]) {
        NSMutableDictionary *nodeDict = [NSMutableDictionary dictionaryWithDictionary:mDict[[SigHelper.share getUint16String:unicastAddress]]];
        nodeDict[key] = @(state);
        mDict[[SigHelper.share getUint16String:unicastAddress]] = nodeDict;
    } else {
        mDict[[SigHelper.share getUint16String:unicastAddress]] = @{kLocalPrivateGattProxy_key:@(NO), kLocalConfigGattProxy_key:@(YES), kLocalPrivateBeacon_key:@(NO), kLocalConfigBeacon_key:@(YES)};
    }
    [[NSUserDefaults standardUserDefaults] setObject:[LibTools getJSONDataWithDictionary:mDict] forKey:kLocalPrivateBeaconDictionary_key];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

#pragma mark - Special handling: store the ivIndex+sequenceNumber of current mesh

/**
 * @brief   Use the key meshUUID+provisionerUUID+unicastAddress to store ivIndex+sequenceNumber locally.
 * @param   ivIndex    the ivIndex of mesh.
 * @param   sequenceNumber    the sequenceNumber of mesh.
 */
- (void)saveCurrentIvIndex:(UInt32)ivIndex sequenceNumber:(UInt32)sequenceNumber {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSDictionary *dict = [defaults objectForKey:kLocationIvIndexAndSequenceNumberDictionary_key];
    if (dict == nil) {
        dict = @{};
    }
    NSMutableDictionary *mDict = [NSMutableDictionary dictionaryWithDictionary:dict];
    NSString *key = [NSString stringWithFormat:@"%@+%@+%@", self.meshUUID, self.curProvisionerModel.UUID, self.curLocationNodeModel.unicastAddress];
    NSString *value = [NSString stringWithFormat:@"%@+%@", self.ivIndex, self.sequenceNumber];
    [mDict setValue:value forKey:key];
    [defaults setObject:mDict forKey:kLocationIvIndexAndSequenceNumberDictionary_key];
    [defaults synchronize];
}

/**
 * @brief   Get local ivIndex.
 * @return  the ivIndex of mesh.
 * @note    This API is used to get the value of ivIndex stored locally.
 */
- (NSString *)getLocalIvIndexString {
    NSString *tem = 0;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSDictionary *dict = [defaults objectForKey:kLocationIvIndexAndSequenceNumberDictionary_key];
    if (dict != nil) {
        NSString *key = [NSString stringWithFormat:@"%@+%@+%@", self.meshUUID, self.curProvisionerModel.UUID, self.curLocationNodeModel.unicastAddress];
        NSString *value = [dict valueForKey:key];
        if (value) {
            NSArray *array = [value componentsSeparatedByString:@"+"];
            if (array && array.count > 0) {
                tem = array.firstObject;
            }
        }
    }
    return tem;
}

/**
 * @brief   Get local sequenceNumber.
 * @return  the sequenceNumber of mesh.
 * @note    This API is used to get the value of sequenceNumber stored locally.
 */
- (NSString *)getLocalSequenceNumberString {
    NSString *tem = 0;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSDictionary *dict = [defaults objectForKey:kLocationIvIndexAndSequenceNumberDictionary_key];
    if (dict != nil) {
        NSString *key = [NSString stringWithFormat:@"%@+%@+%@", self.meshUUID, self.curProvisionerModel.UUID, self.curLocationNodeModel.unicastAddress];
        NSString *value = [dict valueForKey:key];
        if (value) {
            NSArray *array = [value componentsSeparatedByString:@"+"];
            if (array && array.count > 1) {
                tem = array.lastObject;
            }
        }
    }
    return tem;
}

/**
 * @brief   Get local sequenceNumber.
 * @return  the sequenceNumber of mesh.
 * @note    This API is used to get the value of sequenceNumber stored locally.
 */
- (UInt32)getLocalSequenceNumberUInt32 {
    NSString *sequenceNumberStr = [self getLocalSequenceNumberString];
    return [LibTools uint32From16String:sequenceNumberStr];
}

/**
 * @brief   Get whether local had store IvIndexAndLocalSequenceNumber.
 * @return  `YES` means store, `NO` means no store.
 */
- (BOOL)existLocationIvIndexAndLocationSequenceNumber {
    BOOL tem = 0;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSDictionary *dict = [defaults objectForKey:kLocationIvIndexAndSequenceNumberDictionary_key];
    if (dict != nil) {
        NSString *key = [NSString stringWithFormat:@"%@+%@+%@", self.meshUUID, self.curProvisionerModel.UUID, self.curLocationNodeModel.unicastAddress];
        NSString *value = [dict valueForKey:key];
        if (value && value.length > 0) {
            tem = YES;
        }
    }
    return tem;
}

#pragma mark - provisioner UUID API

/**
 * @brief   Save the unique identifier UUID of the current phone, it also the unique identifier UUID of current provisioner.
 * @param   uuid    The unique identifier UUID of the current phone.
 * @note    The unique identifier UUID of the current phone, and it will only be regenerated after uninstalling and reinstalling.
 */
- (void)saveCurrentProvisionerUUID:(NSString *)uuid {
    if (uuid.length == 32) {
        uuid = [LibTools UUIDToMeshUUID:uuid];
    }
    [[NSUserDefaults standardUserDefaults] setObject:uuid forKey:self.getCurrentProvisionerUuidkey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

/**
 * @brief   Get the unique identifier UUID of the current phone, it also the unique identifier UUID of current provisioner.
 * @return  The unique identifier UUID of the current phone.
 * @note    The unique identifier UUID of the current phone, and it will only be regenerated after uninstalling and reinstalling.
 */
- (NSString *)getCurrentProvisionerUUID{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *uuid = [defaults objectForKey:self.getCurrentProvisionerUuidkey];
    if (uuid.length == 32) {
        uuid = [LibTools UUIDToMeshUUID:uuid];
    }
    return uuid;
}

/// Compatible with older versions of keys
- (NSString *)getCurrentProvisionerUuidkey {
    return [kCurrentProvisionerUUID_key stringByReplacingCharactersInRange:NSMakeRange(6, 1) withString:@""];
}

#pragma mark - deprecated API

/**
 * @brief   Get the node object through the bluetooth macAddress of node.
 * @param   macAddress    the bluetooth macAddress of node.
 * @return  A SigNodeModel that save node information. nil means there are no node with this macAddress in mesh network.
 * @note    The unprovision beacon UUID is the unique identifier of the node, macAddress information is no longer stored in the JSON data.
 */
- (SigNodeModel * _Nullable)getDeviceWithMacAddress:(NSString *)macAddress {
    NSArray *nodes = [NSArray arrayWithArray:_nodes];
    for (SigNodeModel *model in nodes) {
        //peripheralUUID || local node's uuid
        if (macAddress && model.macAddress && [model.macAddress.uppercaseString isEqualToString:macAddress.uppercaseString]) {
            return model;
        }
    }
    return nil;
}

@end
