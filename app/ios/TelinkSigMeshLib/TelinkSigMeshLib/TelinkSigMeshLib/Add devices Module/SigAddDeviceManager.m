/********************************************************************************************************
 * @file     SigAddDeviceManager.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/4
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

#import "SigAddDeviceManager.h"
#import "SigKeyBindManager.h"


typedef enum : NSUInteger {
    SigAddStatusScanning,
    SigAddStatusConnectFirst,
    SigAddStatusProvisioning,
    SigAddStatusConnectSecond,
    SigAddStatusKeyBinding,
    SigAddStatusKeyFree,
} SigAddStatus;
/*
 needDisconnectBetweenProvisionToKeyBind：标记是否需要SigAddStatusConnectSecond
 retryCount：用于SigAddStatusConnectFirst和SigAddStatusConnectSecond的总共重连次数
 function1与function2的区别：function1存在扫描流程SigAddStatusScanning，function2跳过了扫描流程直接进入连接流程SigAddStatusConnectFirst。
 */

@interface SigAddDeviceManager ()
@property (nonatomic,assign) UInt16 unicastAddress;
@property (nonatomic,strong) NSData *networkKey;
@property (nonatomic,assign) UInt16 netkeyIndex;
@property (nonatomic,strong) SigAppkeyModel *appkeyModel;
@property (nonatomic,assign) KeyBindType keyBindType;
@property (nonatomic,copy) addDevice_startConnectCallBack startConnectBlock;
@property (nonatomic,copy) addDevice_startProvisionCallBack startProvisionBlock;
@property (nonatomic,copy) addDevice_capabilitiesWithReturnCallBack capabilitiesResponseBlock;
@property (nonatomic,copy) addDevice_provisionSuccessCallBack provisionSuccessBlock;
@property (nonatomic,copy) addDevice_keyBindSuccessCallBack keyBindSuccessBlock;
@property (nonatomic,copy) ErrorBlock provisionFailBlock;
@property (nonatomic,copy) ErrorBlock keyBindFailBlock;
@property (nonatomic,copy) AddDeviceFinishCallBack finishBlock;
@property (nonatomic,assign) SigAddStatus addStatus;
//it is need to call start scan after add one node successful. default is NO.
@property (nonatomic,assign) BOOL isAutoAddDevice;
//contains provsion fail list
@property (nonatomic,strong) NSMutableArray <NSString *>*tempProvisionFailList;
@property (nonatomic,strong) CBPeripheral *curPeripheral;
@property (nonatomic,strong) CBPeripheral *proxyPeripheralForKeyBind;

@property (nonatomic,assign) UInt16 fastKeybindProductID;
@property (nonatomic,strong) NSData *fastKeybindCpsData;
@property (nonatomic,strong) NSData *staticOOBData;

@property (nonatomic,assign) BOOL isCertificateBasedProvision;//default NO.

@end

@implementation SigAddDeviceManager

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigAddDeviceManager *shareManager = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareManager = [[SigAddDeviceManager alloc] init];
        shareManager.needDisconnectBetweenProvisionToKeyBind = YES;
        shareManager.retryCount = 3;
        shareManager.addStatus = SigAddStatusKeyFree;
        shareManager.isAutoAddDevice = NO;
        shareManager.tempProvisionFailList = [NSMutableArray array];
    });
    return shareManager;
}

/**
 * @brief   Deprecated Add Device Method1 (auto add)
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   uuid uuid of remote device.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   isAuto 添加完成一个设备后，是否自动扫描添加下一个设备.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @param   finish callback when add device finish.
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish {
    self.unicastAddress = address;
    TeLogInfo(@"更新自动添加节点的地址，API传入为0x%X",self.unicastAddress);
    self.networkKey = networkKey;
    self.netkeyIndex = netkeyIndex;
    self.appkeyModel = appkeyModel;
    self.keyBindType = type;
    self.provisionSuccessBlock = provisionSuccess;
    self.provisionFailBlock = provisionFail;
    self.keyBindSuccessBlock = keyBindSuccess;
    self.keyBindFailBlock = keyBindFail;
    self.finishBlock = finish;
    self.isAutoAddDevice = isAuto;
    self.retryCount = 3;
    self.addStatus = SigAddStatusScanning;
    self.fastKeybindCpsData = cpsData;
    self.fastKeybindProductID = productID;
    self.staticOOBData = nil;
    self.isCertificateBasedProvision = NO;
    [self.tempProvisionFailList removeAllObjects];
    [self startScan];
}

/**
 * @brief   Deprecated Add Device Method2 (auto add), add new callback of startConnect and startProvision.
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   uuid uuid of remote device.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   isAuto 添加完成一个设备后，是否自动扫描添加下一个设备.
 * @param   startConnect callback when SDK start connect node.
 * @param   startProvision callback when SDK start provision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @param   finish callback when add device finish.
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish {
    self.startConnectBlock = startConnect;
    self.startProvisionBlock = startProvision;
    [self startAddDeviceWithNextAddress:address networkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel uuid:uuid keyBindType:type productID:productID cpsData:cpsData isAutoAddNextDevice:isAuto provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail finish:finish];
}

/**
 * @brief   Deprecated Add Device Method3 (single add)
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    self.unicastAddress = address;
    [self startAddDeviceWithNetworkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel peripheral:peripheral staticOOBData:staticOOBData keyBindType:type productID:productID cpsData:cpsData capabilitiesResponse:nil provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

/**
 * @brief   Deprecated Add Device Method4 (single add), calculate unicastAddress when capabilities response from unProvision node.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   capabilitiesResponse callback when capabilities response from unProvision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
- (void)startAddDeviceWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData capabilitiesResponse:(nullable addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    self.networkKey = networkKey;
    self.netkeyIndex = netkeyIndex;
    self.appkeyModel = appkeyModel;
    self.keyBindType = type;
    self.capabilitiesResponseBlock = capabilitiesResponse;
    self.provisionSuccessBlock = provisionSuccess;
    self.provisionFailBlock = provisionFail;
    self.keyBindSuccessBlock = keyBindSuccess;
    self.keyBindFailBlock = keyBindFail;
    self.isAutoAddDevice = NO;
    self.retryCount = 3;
    self.addStatus = SigAddStatusConnectFirst;
    self.fastKeybindCpsData = cpsData;
    self.fastKeybindProductID = productID;
    self.staticOOBData = staticOOBData;
    self.isCertificateBasedProvision = NO;
    [self.tempProvisionFailList removeAllObjects];
    self.curPeripheral = peripheral;
    [self startAddPeripheral:peripheral];
}

/**
 * @brief   Add Single Device (provision+keyBind), the flag of Certificate Based in unProvision adv data is 1, SDK will get and verify the Certificate of node.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   startConnect callback when SDK start connect node.
 * @param   startProvision callback when SDK start provision node.
 * @param   capabilitiesResponse callback when capabilities response from unProvision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
- (void)startCertificateBasedWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    self.isCertificateBasedProvision = YES;
    self.networkKey = networkKey;
    self.netkeyIndex = netkeyIndex;
    self.appkeyModel = appkeyModel;
    self.keyBindType = type;
    self.startConnectBlock = startConnect;
    self.startProvisionBlock = startProvision;
    self.capabilitiesResponseBlock = capabilitiesResponse;
    self.provisionSuccessBlock = provisionSuccess;
    self.provisionFailBlock = provisionFail;
    self.keyBindSuccessBlock = keyBindSuccess;
    self.keyBindFailBlock = keyBindFail;
    self.isAutoAddDevice = NO;
    self.retryCount = 3;
    self.addStatus = SigAddStatusConnectFirst;
    self.fastKeybindCpsData = cpsData;
    self.fastKeybindProductID = productID;
    self.staticOOBData = staticOOBData;
    [self.tempProvisionFailList removeAllObjects];
    self.curPeripheral = peripheral;
    [self startAddPeripheral:peripheral];
}

/**
 * @brief   Add Single Device (provision+keyBind)
 * @param   configModel all config message of add device.
 * @param   capabilitiesResponse callback when capabilities response.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @note    SDK will call this api automatically, when SDK set filter success.
 * @note    parameter of SigAddConfigModel:
 *  1.normal provision + normal keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+keyBindType:KeyBindType_Normal
 *  2.normal provision + fast keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+keyBindType:KeyBindType_Fast+productID+cpsData
 *  3.static oob provision(cloud oob) + normal keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+staticOOBData+keyBindType:KeyBindType_Normal
 *  4.static oob provision(cloud oob) + fast keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+staticOOBData+keyBindType:KeyBindType_Fast+productID+cpsData
 */
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    SigAppkeyModel *appkeyModel = [SigMeshLib.share.dataSource getAppkeyModelWithAppkeyIndex:configModel.appkeyIndex];
    if (!appkeyModel || ![appkeyModel.getDataKey isEqualToData:configModel.appKey]) {
        TeLogVerbose(@"appKey is error.");
        if (provisionFail) {
            NSError *err = [NSError errorWithDomain:@"appKey is error." code:-1 userInfo:nil];
            provisionFail(err);
        }
        return;
    }
    [self startAddDeviceWithNetworkKey:configModel.networkKey netkeyIndex:configModel.netkeyIndex appkeyModel:appkeyModel peripheral:configModel.peripheral staticOOBData:configModel.staticOOBData keyBindType:configModel.keyBindType productID:configModel.productID cpsData:configModel.cpsData capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

- (void)startScan {
    self.addStatus = SigAddStatusScanning;
    __weak typeof(self) weakSelf = self;
    [SigBluetooth.share scanUnprovisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
        if (unprovisioned && ![weakSelf.tempProvisionFailList containsObject:peripheral.identifier.UUIDString]) {
            //自动添加新增逻辑：判断本地是否存在该UUID的OOB数据，存在则缓存到self.staticOOBData中。
            SigScanRspModel *model = [SigMeshLib.share.dataSource getScanRspModelWithUUID:peripheral.identifier.UUIDString];
            weakSelf.addStatus = SigAddStatusConnectFirst;
            [SigBluetooth.share stopScan];
            SigOOBModel *oobModel = [SigMeshLib.share.dataSource getSigOOBModelWithUUID:model.advUuid];
            if (oobModel && oobModel.OOBString && oobModel.OOBString.length == 32) {
                weakSelf.staticOOBData = [LibTools nsstringToHex:oobModel.OOBString];
            }
            weakSelf.isCertificateBasedProvision = model.advOobInformation.supportForCertificateBasedProvisioning;
            weakSelf.curPeripheral = peripheral;
            [weakSelf startAddPeripheral:peripheral];
        }
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanTimeout) object:nil];
        [self performSelector:@selector(scanTimeout) withObject:nil afterDelay:kScanUnprovisionDeviceTimeout];
    });
}

- (void)keybind{
    TeLogVerbose(@"");
    self.addStatus = SigAddStatusKeyBinding;
    __weak typeof(self) weakSelf = self;
    CBPeripheral *peripheral = SigBearer.share.getCurrentPeripheral;
    [SDKLibCommand setFilterForProvisioner:SigMeshLib.share.dataSource.curProvisionerModel successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
    } finishCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (error) {
            if (weakSelf.retryCount > 0) {
                TeLogDebug(@"retry setFilter peripheral=%@,retry count=%d",peripheral,weakSelf.retryCount);
                weakSelf.retryCount --;
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(checkToTryAgain:) object:weakSelf.curPeripheral];
                    [weakSelf performSelector:@selector(checkToTryAgain:) withObject:weakSelf.curPeripheral afterDelay:0.1];
                });
            } else {
                TeLogDebug(@"setFilter fail, so keybind fail.");
                if (weakSelf.keyBindFailBlock) {
                    NSError *err = [NSError errorWithDomain:@"setFilter fail, so keybind fail." code:0 userInfo:nil];
                    weakSelf.keyBindFailBlock(err);
                }
                [weakSelf refreshNextUnicastAddress];
                [weakSelf addPeripheralFail:peripheral];
            }
        } else {
            TeLogInfo(@"start keyBind.");
            weakSelf.addStatus = SigAddStatusKeyBinding;
            KeyBindType currentKeyBindType = KeyBindType_Normal;
            if (weakSelf.keyBindType == KeyBindType_Fast) {
                if (weakSelf.fastKeybindCpsData && weakSelf.fastKeybindCpsData.length > 0) {
                    currentKeyBindType = KeyBindType_Fast;
                }else{
                    SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:weakSelf.unicastAddress];
                    DeviceTypeModel *deviceType = [SigMeshLib.share.dataSource getNodeInfoWithCID:[LibTools uint16From16String:node.cid] PID:[LibTools uint16From16String:node.pid]];
                    if (deviceType != nil) {
                        currentKeyBindType = KeyBindType_Fast;
                    } else {
                        TeLogInfo(@"fast bind no support, bind Normal!!!");
                    }
                }
            }
            [SigKeyBindManager.share keyBind:weakSelf.unicastAddress appkeyModel:weakSelf.appkeyModel keyBindType:currentKeyBindType productID:weakSelf.fastKeybindProductID cpsData:weakSelf.fastKeybindCpsData keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                if (weakSelf.keyBindSuccessBlock) {
                    weakSelf.keyBindSuccessBlock(weakSelf.curPeripheral.identifier.UUIDString, address);
                }
                [weakSelf refreshNextUnicastAddress];
                [weakSelf addPeripheralSuccess:weakSelf.curPeripheral];
            } fail:^(NSError * _Nonnull error) {
                if (weakSelf.addStatus == SigAddStatusConnectSecond || weakSelf.addStatus == SigAddStatusKeyBinding) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(checkToTryAgain:) object:peripheral];
                        [weakSelf performSelector:@selector(checkToTryAgain:) withObject:peripheral afterDelay:0.1];
                    });
                }
            }];
        }
    }];
}

- (void)startAddPeripheral:(CBPeripheral *)peripheral {
    TeLogVerbose(@"");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(keybind) object:nil];
    });
    if (self.startConnectBlock) {
        self.startConnectBlock(self.curPeripheral.identifier.UUIDString);
    }
    __weak typeof(self) weakSelf = self;
    [[SigBearer share] connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
        if (weakSelf.addStatus != SigAddStatusKeyFree) {
            if (successful) {
                if (weakSelf.addStatus == SigAddStatusConnectFirst || weakSelf.addStatus == SigAddStatusProvisioning) {
                    //it need delay 500ms before sent provision invite.
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                        if (weakSelf.startProvisionBlock) {
                            weakSelf.startProvisionBlock(peripheral.identifier.UUIDString, weakSelf.unicastAddress);
                        }
                        weakSelf.addStatus = SigAddStatusProvisioning;
                        if (weakSelf.isCertificateBasedProvision == NO) {
                            if (weakSelf.capabilitiesResponseBlock) {
                                [SigProvisioningManager.share provisionWithNetworkKey:weakSelf.networkKey netkeyIndex:weakSelf.netkeyIndex staticOobData:weakSelf.staticOOBData capabilitiesResponse:weakSelf.capabilitiesResponseBlock provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                                    weakSelf.unicastAddress = address;
                                    if (weakSelf.provisionSuccessBlock) {
                                        weakSelf.provisionSuccessBlock(identify, address);
                                    }
                                    if (weakSelf.needDisconnectBetweenProvisionToKeyBind || [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] == nil) {
                                        weakSelf.addStatus = SigAddStatusConnectSecond;
                                        [weakSelf performSelector:@selector(scanCurrentPeripheralForKeyBind) withObject:nil];
                                    } else {
                                        [weakSelf keybind];
                                    }
                                } fail:^(NSError * _Nullable error) {
                                    if (weakSelf.addStatus == SigAddStatusConnectFirst || weakSelf.addStatus == SigAddStatusProvisioning) {
                                        if (weakSelf.provisionFailBlock) {
                                            weakSelf.provisionFailBlock(error);
                                        }
                                        [weakSelf addPeripheralFail:peripheral];
                                    }
                                }];
                            } else {
                                [SigProvisioningManager.share provisionWithUnicastAddress:weakSelf.unicastAddress networkKey:weakSelf.networkKey netkeyIndex:weakSelf.netkeyIndex staticOobData:weakSelf.staticOOBData provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                                    weakSelf.unicastAddress = address;
                                    if (weakSelf.provisionSuccessBlock) {
                                        weakSelf.provisionSuccessBlock(identify, address);
                                    }
                                    if (weakSelf.needDisconnectBetweenProvisionToKeyBind || [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] == nil) {
                                        weakSelf.addStatus = SigAddStatusConnectSecond;
                                        [weakSelf performSelector:@selector(scanCurrentPeripheralForKeyBind) withObject:nil];
                                    } else {
                                        [weakSelf keybind];
                                    }
                                } fail:^(NSError * _Nullable error) {
                                    if (weakSelf.addStatus == SigAddStatusConnectFirst || weakSelf.addStatus == SigAddStatusProvisioning) {
                                        if (weakSelf.provisionFailBlock) {
                                            weakSelf.provisionFailBlock(error);
                                        }
                                        [weakSelf addPeripheralFail:peripheral];
                                    }
                                }];
                            }
                        } else {
                            if (weakSelf.capabilitiesResponseBlock) {
                                [SigProvisioningManager.share certificateBasedProvisionWithPeripheral:peripheral networkKey:weakSelf.networkKey netkeyIndex:weakSelf.netkeyIndex staticOOBData:weakSelf.staticOOBData capabilitiesResponse:weakSelf.capabilitiesResponseBlock provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                                    weakSelf.unicastAddress = address;
                                    if (weakSelf.provisionSuccessBlock) {
                                        weakSelf.provisionSuccessBlock(identify, address);
                                    }
                                    if (weakSelf.needDisconnectBetweenProvisionToKeyBind || [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] == nil) {
                                        weakSelf.addStatus = SigAddStatusConnectSecond;
                                        [weakSelf performSelector:@selector(scanCurrentPeripheralForKeyBind) withObject:nil];
                                    } else {
                                        [weakSelf keybind];
                                    }
                                } fail:^(NSError * _Nullable error) {
                                    if (weakSelf.addStatus == SigAddStatusConnectFirst || weakSelf.addStatus == SigAddStatusProvisioning) {
                                        if (weakSelf.provisionFailBlock) {
                                            weakSelf.provisionFailBlock(error);
                                        }
                                        [weakSelf addPeripheralFail:peripheral];
                                    }
                                }];
                            } else {
                                [SigProvisioningManager.share certificateBasedProvisionWithPeripheral:peripheral unicastAddress:weakSelf.unicastAddress networkKey:weakSelf.networkKey netkeyIndex:weakSelf.netkeyIndex staticOOBData:weakSelf.staticOOBData provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                                    weakSelf.unicastAddress = address;
                                    if (weakSelf.provisionSuccessBlock) {
                                        weakSelf.provisionSuccessBlock(identify, address);
                                    }
                                    if (weakSelf.needDisconnectBetweenProvisionToKeyBind || [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] == nil) {
                                        weakSelf.addStatus = SigAddStatusConnectSecond;
                                        [weakSelf performSelector:@selector(scanCurrentPeripheralForKeyBind) withObject:nil];
                                    } else {
                                        [weakSelf keybind];
                                    }
                                } fail:^(NSError * _Nullable error) {
                                    if (weakSelf.addStatus == SigAddStatusConnectFirst || weakSelf.addStatus == SigAddStatusProvisioning) {
                                        if (weakSelf.provisionFailBlock) {
                                            weakSelf.provisionFailBlock(error);
                                        }
                                        [weakSelf addPeripheralFail:peripheral];
                                    }
                                }];
                            }
                        }
                    });
                } else if (weakSelf.addStatus == SigAddStatusConnectSecond || weakSelf.addStatus == SigAddStatusKeyBinding) {
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                        [weakSelf keybind];
                    });
                }else{
                    TeLogError(@"error addStatus=%d!!!!!!!!!",weakSelf.addStatus);
                }
            } else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(checkToTryAgain:) object:peripheral];
                    [weakSelf performSelector:@selector(checkToTryAgain:) withObject:peripheral afterDelay:0.1];
                });
            }
        }
    }];
}

- (void)checkToTryAgain:(CBPeripheral *)peripheral {
    __weak typeof(self) weakSelf = self;
    [SigBearer.share closeWithResult:^(BOOL successful) {
        if (weakSelf.retryCount > 0) {
            TeLogDebug(@"retry connect peripheral=%@,retry count=%d",peripheral,weakSelf.retryCount);
            weakSelf.retryCount --;
            if (weakSelf.addStatus == SigAddStatusConnectFirst || weakSelf.addStatus == SigAddStatusProvisioning) {
                //重试连接时如果断开连接超时会导致closeWithResult重复赋值。所以添加该延时。
                [weakSelf performSelector:@selector(startAddPeripheral:) withObject:peripheral afterDelay:0.1];
            } else {
                [weakSelf performSelector:@selector(scanCurrentPeripheralForKeyBind) withObject:nil];
            }
        } else {
            if (weakSelf.addStatus == SigAddStatusConnectFirst) {
                if (weakSelf.provisionFailBlock) {
                    NSError *err = [NSError errorWithDomain:@"connect or read ATT fail. provision fail." code:-1 userInfo:nil];
                    weakSelf.provisionFailBlock(err);
                }
            } else if (weakSelf.addStatus == SigAddStatusConnectSecond || weakSelf.addStatus == SigAddStatusKeyBinding) {
                [weakSelf refreshNextUnicastAddress];
                if (weakSelf.keyBindFailBlock) {
                    NSError *err = [NSError errorWithDomain:@"connect or read ATT fail. keybind fail." code:-1 userInfo:nil];
                    weakSelf.keyBindFailBlock(err);
                }
            }
            [weakSelf addPeripheralFail:peripheral];
        }
    }];
}

- (void)scanCurrentPeripheralForKeyBind {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(keyBindByProxy) object:nil];
        [self performSelector:@selector(keyBindByProxy) withObject:nil afterDelay:kScanNodeIdentityBeforeKeyBindTimeout];
    });
    __weak typeof(self) weakSelf = self;
//    [SigBluetooth.share setBluetoothDisconnectCallback:nil];
    [SigBearer.share closeWithResult:^(BOOL successful) {
        if (weakSelf.addStatus == SigAddStatusConnectSecond || weakSelf.addStatus == SigAddStatusKeyBinding) {
            __block int maxRssi = 0;
            [SigBluetooth.share scanProvisionedDevicesWithResult:^(CBPeripheral *peripheral, NSDictionary<NSString *,id> *advertisementData, NSNumber *RSSI, BOOL unprovisioned) {
                if (!unprovisioned) {
                    if (maxRssi == 0) {
                        maxRssi = RSSI.intValue;
                        weakSelf.proxyPeripheralForKeyBind = peripheral;
                    } else {
                        if (maxRssi > RSSI.intValue) {
                            weakSelf.proxyPeripheralForKeyBind = peripheral;
                        }
                    }
                    SigScanRspModel *rspModel = [SigMeshLib.share.dataSource getScanRspModelWithUUID:peripheral.identifier.UUIDString];
                    if (rspModel.getIdentificationType == SigIdentificationType_nodeIdentity || rspModel.getIdentificationType == SigIdentificationType_privateNodeIdentity) {
                        SigEncryptedModel *encryptedModel = [SigMeshLib.share.dataSource getSigEncryptedModelWithAddress:weakSelf.unicastAddress];
                        if (encryptedModel && encryptedModel.advertisementDataServiceData && encryptedModel.advertisementDataServiceData.length == 17 && [encryptedModel.advertisementDataServiceData isEqualToData:rspModel.advertisementDataServiceData]) {
                            TeLogInfo(@"keyBind start connect macAddress:%@,uuid=%@",rspModel.macAddress,rspModel.uuid);
                            [SigBluetooth.share stopScan];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(keyBindByProxy) object:nil];
                            });
                            [weakSelf performSelector:@selector(startAddPeripheral:) withObject:peripheral];
                        }
                    }
                }
            }];
        }
    }];
}

- (void)keyBindByProxy {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(keyBindByProxy) object:nil];
    });
    [self performSelector:@selector(startAddPeripheral:) withObject:self.proxyPeripheralForKeyBind];
}

- (void)addPeripheralFail:(CBPeripheral *)peripheral {
    TeLogInfo(@"addPeripheralFail,uuid=%@",peripheral.identifier.UUIDString);
    NSString *uuid = peripheral.identifier.UUIDString;
    if (![self.tempProvisionFailList containsObject:uuid]) {
        [self.tempProvisionFailList addObject:uuid];
    }
    if (self.isAutoAddDevice) {
        self.addStatus = SigAddStatusScanning;
        [self startScan];
    }else{
        self.addStatus = SigAddStatusKeyFree;
//        if (self.finishBlock) {
//            self.finishBlock();
//        }
    }
}

- (void)addPeripheralSuccess:(CBPeripheral *)peripheral {
    if (self.isAutoAddDevice) {
        self.addStatus = SigAddStatusScanning;
        [self startScan];
    }else{
        self.addStatus = SigAddStatusKeyFree;
//        if (self.finishBlock) {
//            self.finishBlock();
//        }
    }
}

- (void)scanTimeout {
    self.addStatus = SigAddStatusScanning;
    [SigBluetooth.share stopScan];
    self.addStatus = SigAddStatusKeyFree;
    if (self.finishBlock) {
        self.finishBlock();
    }
    self.startConnectBlock = nil;
    self.startProvisionBlock = nil;
    self.provisionSuccessBlock = nil;
    self.provisionFailBlock = nil;
    self.keyBindSuccessBlock = nil;
    self.keyBindFailBlock = nil;
    self.finishBlock = nil;
}

- (void)refreshNextUnicastAddress {
    TeLogInfo(@"更新自动添加节点的地址，更新前为0x%X",self.unicastAddress);
    self.unicastAddress += SigProvisioningManager.share.provisioningCapabilities.numberOfElements;
    TeLogInfo(@"更新自动添加节点的地址，更新后为0x%X",self.unicastAddress);
}

@end
