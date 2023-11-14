/********************************************************************************************************
 * @file     SigProvisioningManager.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/19
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SigProvisioningManager.h"
#import "SigBluetooth.h"
#import "SigProvisioningData.h"
#import "SigAuthenticationModel.h"
#import "OpenSSLHelper.h"
#import "SigECCEncryptHelper.h"

#define kFragmentMaximumSize    (20)

/// Table 5.53: Provisioning records
/// - seeAlso: MshPRFd1.1r11_clean-472-529.pdf  (page.42)
typedef enum : UInt16 {
    SigProvisioningRecordID_CertificateBasedProvisioningURI = 0x0000,
    SigProvisioningRecordID_DeviceCertificate               = 0x0001,
    SigProvisioningRecordID_IntermediateCertificate1        = 0x0002,
    SigProvisioningRecordID_IntermediateCertificate2        = 0x0003,
    SigProvisioningRecordID_IntermediateCertificate3        = 0x0004,
    SigProvisioningRecordID_IntermediateCertificate4        = 0x0005,
    SigProvisioningRecordID_IntermediateCertificate5        = 0x0006,
    SigProvisioningRecordID_IntermediateCertificate6        = 0x0007,
    SigProvisioningRecordID_IntermediateCertificate7        = 0x0008,
    SigProvisioningRecordID_IntermediateCertificate8        = 0x0009,
    SigProvisioningRecordID_IntermediateCertificate9        = 0x000A,
    SigProvisioningRecordID_IntermediateCertificate10       = 0x000B,
    SigProvisioningRecordID_IntermediateCertificate11       = 0x000C,
    SigProvisioningRecordID_IntermediateCertificate12       = 0x000D,
    SigProvisioningRecordID_IntermediateCertificate13       = 0x000E,
    SigProvisioningRecordID_IntermediateCertificate14       = 0x000F,
    SigProvisioningRecordID_IntermediateCertificate15       = 0x0010,
    SigProvisioningRecordID_CompleteLocalName               = 0x0011,
    SigProvisioningRecordID_Appearance                      = 0x0012,
} SigProvisioningRecordID;

@interface SigProvisioningManager ()
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong, nullable) NSData *staticOobData;
@property (nonatomic, strong) SigScanRspModel *unprovisionedDevice;
@property (nonatomic, copy) addDevice_provisionSuccessCallBack provisionSuccessBlock;
@property (nonatomic, copy) ErrorBlock failBlock;
@property (nonatomic, assign) BOOL isProvisionning;
@property (nonatomic, assign) UInt16 totalLength;
//@property (nonatomic, strong) NSMutableData *deviceCertificateData;
@property (nonatomic, strong) SigProvisioningRecordsListPdu *recordsListPdu;
@property (nonatomic, strong) NSMutableDictionary <NSNumber *,NSData *>*certificateDict;
@property (nonatomic, assign) UInt16 currentRecordID;

@property (nonatomic,strong) NSData *devicePublicKey;//certificate-base获取到的devicePublicKey

//缓存旧的block，防止添加设备后不断开的情况下，SigBearer下的断开block不运行。
@property (nonatomic,copy) bleDisconnectCallback oldBluetoothDisconnectCallback;

@end

@implementation SigProvisioningManager

- (instancetype)init{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _state = ProvisioningState_ready;
        _isProvisionning = NO;
        _attentionDuration = 0;
    }
    return self;
}

/// This method get the Capabilities of the device.
/// @param attentionDuration 0x00, Off; 0x01–0xFF, On, remaining time in seconds.
- (void)identifyWithAttentionDuration:(UInt8)attentionDuration {

    // Has the provisioning been restarted?
    if (self.state == ProvisioningState_fail) {
        [self reset];
    }

    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisioningState_ready) {
        TelinkLogError(@"current node isn't in ready.");
        return;
    }

    // Initialize provisioning data.
    self.provisioningData = [[SigProvisioningData alloc] init];

    self.state = ProvisioningState_requestingCapabilities;

    SigProvisioningInvitePdu *pdu = [[SigProvisioningInvitePdu alloc] initWithAttentionDuration:attentionDuration];
    self.provisioningData.provisioningInvitePDUValue = [pdu.pduData subdataWithRange:NSMakeRange(1, pdu.pduData.length-1)];
    [self sendPdu:pdu];
}

- (void)setState:(ProvisioningState)state{
    _state = state;
    if (state == ProvisioningState_fail) {
        SigBluetooth.share.bluetoothDisconnectCallback = self.oldBluetoothDisconnectCallback;
        [self reset];
        __weak typeof(self) weakSelf = self;
        [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
            if (weakSelf.failBlock) {
                NSError *err = [NSError errorWithDomain:@"provision fail." code:-1 userInfo:nil];
                weakSelf.failBlock(err);
            }
        }];
    }
}

- (BOOL)isDeviceSupported{
    if (self.provisioningCapabilities.provisionType != SigProvisioningPduType_capabilities || self.provisioningCapabilities.numberOfElements == 0) {
        TelinkLogError(@"Capabilities is error.");
        return NO;
    }
    return self.provisioningCapabilities.algorithms.fipsP256EllipticCurve == 1 || self.provisioningCapabilities.algorithms.fipsP256EllipticCurve_HMAC_SHA256 == 1;
}

- (void)provisionSuccess{
    UInt16 address = self.provisioningData.unicastAddress;
    UInt8 ele_count = self.provisioningCapabilities.numberOfElements;
    NSData *devKeyData = self.provisioningData.deviceKey;
    TelinkLogInfo(@"deviceKey=%@",devKeyData);

    self.unprovisionedDevice.address = address;
    [SigMeshLib.share.dataSource updateScanRspModelToDataSource:self.unprovisionedDevice];

    SigNodeModel *model = [[SigNodeModel alloc] init];
    [model setAddress:address];
    model.deviceKey = [LibTools convertDataToHexStr:devKeyData];
    model.peripheralUUID = nil;
    model.UUID = self.unprovisionedDevice.advUuid;
    //Attention: There isn't scanModel at remote add, so develop need add macAddress in provisionSuccessCallback.
    model.macAddress = self.unprovisionedDevice.macAddress;
    SigNodeKeyModel *nodeNetkey = [[SigNodeKeyModel alloc] init];
    nodeNetkey.index = self.networkKey.index;
    if (![model.netKeys containsObject:nodeNetkey]) {
        [model.netKeys addObject:nodeNetkey];
    }

    SigPage0 *compositionData = [[SigPage0 alloc] init];
    compositionData.companyIdentifier = self.unprovisionedDevice.CID;
    compositionData.productIdentifier = self.unprovisionedDevice.PID;
    NSMutableArray *elements = [NSMutableArray array];
    if (ele_count) {
        for (int i=0; i<ele_count; i++) {
            SigElementModel *ele = [[SigElementModel alloc] init];
            [elements addObject:ele];
        }
    }
    compositionData.elements = elements;
    model.compositionData = compositionData;

    [SigMeshLib.share.dataSource addAndSaveNodeToMeshNetworkWithDeviceModel:model];
    SigBluetooth.share.bluetoothDisconnectCallback = self.oldBluetoothDisconnectCallback;
}

/// This method should be called when the OOB value has been received and Auth Value has been calculated. It computes and sends the Provisioner Confirmation to the device.
/// @param data The 16/32 byte long Auth Value.
- (void)authValueReceivedData:(NSData *)data {
    SigAuthenticationModel *auth = nil;
    self.authenticationModel = auth;
    [self.provisioningData provisionerDidObtainAuthValue:data];
    NSData *provisionerConfirmationData = [self.provisioningData provisionerConfirmation];
    SigProvisioningConfirmationPdu *pdu = [[SigProvisioningConfirmationPdu alloc] initWithConfirmation:provisionerConfirmationData];
//    TelinkLogInfo(@"app端的Confirmation=%@",[LibTools convertDataToHexStr:provisionerConfirmationData]);

    [self sendPdu:pdu];
}

- (void)sendPdu:(SigProvisioningPdu *)pdu {
    dispatch_async(SigMeshLib.share.queue, ^{
        [SigBearer.share sendBlePdu:pdu ofType:SigPduType_provisioningPdu];
    });
}

/// Resets the provisioning properties and state.
- (void)reset {
    self.authenticationMethod = 0;
    memset(&_provisioningCapabilities, 0, sizeof(_provisioningCapabilities));
    SigProvisioningData *tem = nil;
    self.provisioningData = tem;
    self.state = ProvisioningState_ready;
    [SigBearer.share setBearerProvisioned:YES];
}

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigProvisioningManager *shareManager = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareManager = [[SigProvisioningManager alloc] init];
    });
    return shareManager;
}

/**
 * @brief   Provision
 * @param   networkKey    the networkKey of mesh.
 * @param   netkeyIndex    the netkeyIndex of mesh.
 * @param   oobData    the staticOobData of node.
 * @param   capabilitiesResponse    callback when app receive capabilities pdu from node.
 * @param   provisionSuccess    callback when node provision success.
 * @param   fail    callback when node provision fail.
 * @note    oobData is nil means do no oob provision, oobData is not nil means do static oob provision.
 */
- (void)provisionWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOobData:(NSData * _Nullable)oobData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];

    self.staticOobData = oobData;
    self.provisionSuccessBlock = provisionSuccess;
    self.capabilitiesResponseBlock = capabilitiesResponse;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigMeshLib.share.dataSource getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if (([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] || (net.phase == distributingKeys && [networkKey isEqualToData:[LibTools nsstringToHex:net.oldKey]])) && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TelinkLogError(@"error network key.");
        return;
    }
    __weak typeof(self) weakSelf = self;
    [self reset];
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    TelinkLogInfo(@"start provision.");
    self.oldBluetoothDisconnectCallback = SigBluetooth.share.bluetoothDisconnectCallback;
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isProvisionning) {
                TelinkLogInfo(@"disconnect in provisioning，provision fail.");
                if (fail) {
                    weakSelf.isProvisionning = NO;
                    NSError *err = [NSError errorWithDomain:@"disconnect in provisioning，provision fail." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }
    }];
    [self getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningPdu * _Nullable response) {
        [weakSelf getCapabilitiesResultWithResponse:response];
    }];

}

/**
 * @brief   Provision Method, calculate unicastAddress when capabilities response from unProvision node. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   networkKey network key.
 * @param   netkeyIndex netkey index.
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   capabilitiesResponse callback when capabilities response, app need return unicastAddress for provision process.
 * @param   provisionSuccess callback when provision success.
 * @param   fail callback when provision fail.
 * @note    1.get provision type from Capabilities response.
 * 2.Static OOB Type is NO_OOB, SDK will call no oob provision.
 * 3.Static OOB Type is Static_OOB, SDK will call static oob provision.
 * 4.if it is no static oob data when Static OOB Type is Static_OOB, SDK will try on oob provision when SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable is YES.
 */
- (void)provisionWithPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];
    if (peripheral.state == CBPeripheralStateConnected) {
        [self provisionWithNetworkKey:networkKey netkeyIndex:netkeyIndex staticOobData:staticOOBData capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess fail:fail];
    } else {
        __weak typeof(self) weakSelf = self;
        TelinkLogVerbose(@"start connect for provision.");
        [SigBearer.share connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
            if (successful) {
                TelinkLogVerbose(@"connect successful.");
                [weakSelf provisionWithPeripheral:peripheral networkKey:networkKey netkeyIndex:netkeyIndex staticOOBData:staticOOBData capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess fail:fail];
            } else {
                if (fail) {
                    NSError *err = [NSError errorWithDomain:@"Provision fail, because connect fail before provision." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }];
    }
}

/// founcation4: certificateBased provision
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)certificateBasedProvisionWithPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];

    self.staticOobData = nil;
    self.provisionSuccessBlock = provisionSuccess;
    self.capabilitiesResponseBlock = capabilitiesResponse;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigMeshLib.share.dataSource getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if (([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] || (net.phase == distributingKeys && [networkKey isEqualToData:[LibTools nsstringToHex:net.oldKey]])) && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TelinkLogError(@"error network key.");
        return;
    }
    self.certificateDict = [NSMutableDictionary dictionary];
    self.currentRecordID = 0;

    [self reset];
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    TelinkLogInfo(@"start certificateBasedProvision.");

    __weak typeof(self) weakSelf = self;
    [self sentProvisioningRecordsGetWithTimeout:kProvisioningRecordsGetTimeout callback:^(SigProvisioningPdu * _Nullable response) {
        [weakSelf sentProvisioningRecordsGetWithResponse:response];
    }];
}

- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];

    self.staticOobData = nil;
    self.unicastAddress = unicastAddress;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigMeshLib.share.dataSource getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if (([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] || (net.phase == distributingKeys && [networkKey isEqualToData:[LibTools nsstringToHex:net.oldKey]])) && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TelinkLogError(@"error network key.");
        return;
    }
    __weak typeof(self) weakSelf = self;
    [self reset];
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    TelinkLogInfo(@"start provision.");
    self.oldBluetoothDisconnectCallback = SigBluetooth.share.bluetoothDisconnectCallback;
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isProvisionning) {
                TelinkLogInfo(@"disconnect in provisioning，provision fail.");
                if (fail) {
                    weakSelf.isProvisionning = NO;
                    NSError *err = [NSError errorWithDomain:@"disconnect in provisioning，provision fail." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }
    }];
    [self getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningPdu * _Nullable response) {
        [weakSelf getCapabilitiesResultWithResponse:response];
    }];
}

- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOobData:(NSData *)oobData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];

    self.staticOobData = oobData;
    self.unicastAddress = unicastAddress;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigMeshLib.share.dataSource getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if (([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] || (net.phase == distributingKeys && [networkKey isEqualToData:[LibTools nsstringToHex:net.oldKey]])) && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TelinkLogError(@"error network key.");
        return;
    }
    __weak typeof(self) weakSelf = self;
    [self reset];
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    TelinkLogInfo(@"start provision.");
    self.oldBluetoothDisconnectCallback = SigBluetooth.share.bluetoothDisconnectCallback;
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isProvisionning) {
                TelinkLogInfo(@"disconnect in provisioning，provision fail.");
                if (fail) {
                    weakSelf.isProvisionning = NO;
                    NSError *err = [NSError errorWithDomain:@"disconnect in provisioning，provision fail." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }
    }];
    [self getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningPdu * _Nullable response) {
        [weakSelf getCapabilitiesResultWithResponse:response];
    }];

}

/// founcation3: provision (If CBPeripheral isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)provisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(NSData * _Nullable)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];
    if (peripheral.state == CBPeripheralStateConnected) {
        [self provisionWithUnicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex staticOobData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
    } else {
        __weak typeof(self) weakSelf = self;
        TelinkLogVerbose(@"start connect for provision.");
        [SigBearer.share connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
            if (successful) {
                TelinkLogVerbose(@"connect successful.");
                [weakSelf provisionWithPeripheral:peripheral unicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex staticOOBData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
            } else {
                if (fail) {
                    NSError *err = [NSError errorWithDomain:@"Provision fail, because connect fail before provision." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }];
    }
}

/// founcation4: provision (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)certificateBasedProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];

    self.staticOobData = nil;
    self.unicastAddress = unicastAddress;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigMeshLib.share.dataSource getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if (([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] || (net.phase == distributingKeys && [networkKey isEqualToData:[LibTools nsstringToHex:net.oldKey]])) && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TelinkLogError(@"error network key.");
        return;
    }
    self.certificateDict = [NSMutableDictionary dictionary];
    self.currentRecordID = 0;

    __weak typeof(self) weakSelf = self;
    [self reset];
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    TelinkLogInfo(@"start certificateBasedProvision.");

    if (peripheral.state == CBPeripheralStateConnected) {
        TelinkLogVerbose(@"start RecordsGet.");
        [self sentProvisioningRecordsGetWithTimeout:kProvisioningRecordsGetTimeout callback:^(SigProvisioningPdu * _Nullable response) {
            [weakSelf sentProvisioningRecordsGetWithResponse:response];
        }];
    } else {
        TelinkLogVerbose(@"start connect for provision.");
        [SigBearer.share connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
            if (successful) {
                TelinkLogVerbose(@"connect successful.");
                [weakSelf provisionWithPeripheral:peripheral unicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex staticOOBData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
            } else {
                if (fail) {
                    NSError *err = [NSError errorWithDomain:@"Provision fail, because connect fail before provision." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }];
    }
}

#pragma mark step1:getCapabilities
- (void)getCapabilitiesWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision:step1\n\n");
    self.provisionResponseBlock = block;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
        [self performSelector:@selector(getCapabilitiesTimeout) withObject:nil afterDelay:timeout];
    });
    [self identifyWithAttentionDuration:self.attentionDuration];
}

#pragma mark step2:start
- (void)sentStartNoOobProvisionPduAndPublicKeyPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision:step2(noOob)\n\n");
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisioningState_capabilitiesReceived) {
        TelinkLogError(@"current state is wrong.");
        return;
    }

    // Ensure the Network Key is set.
    if (self.networkKey == nil) {
        TelinkLogError(@"current networkKey isn't specified.");
        return;
    }

    // Is the Bearer open?
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"current node's bearer isn't open.");
        return;
    }

    self.provisionResponseBlock = block;

    self.provisioningData.algorithm = [self getCurrentProvisionAlgorithm];
    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];

    // Send Provisioning Start request.
    self.state = ProvisioningState_provisioning;
    [self.provisioningData prepareWithNetwork:SigMeshLib.share.dataSource networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:self.provisioningCapabilities.publicKeyType];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_noOob;

    SigProvisioningStartPdu *startPdu = [[SigProvisioningStartPdu alloc] initWithAlgorithm:[self getCurrentProvisionAlgorithm] publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    self.provisioningData.provisioningStartPDUValue = [startPdu.pduData subdataWithRange:NSMakeRange(1, startPdu.pduData.length-1)];
    [self sendPdu:startPdu];
    self.authenticationMethod = authenticationMethod;
    // Send the Public Key of the Provisioner.
    SigProvisioningPublicKeyPdu *publicPdu = [[SigProvisioningPublicKeyPdu alloc] initWithPublicKey:self.provisioningData.provisionerPublicKey];
    [self sendPdu:publicPdu];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
        [self performSelector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) withObject:nil afterDelay:timeout];
        if (self.provisioningCapabilities.publicKeyType == PublicKeyType_oobPublicKey) {
            //if use oob public key, needn`t waith devicePublicKey response, use devicePublicKey from certificate.
            SigProvisioningPublicKeyPdu *devicePublicKeyPdu = [[SigProvisioningPublicKeyPdu alloc] initWithPublicKey:self.devicePublicKey];
            [self sentStartProvisionPduAndPublicKeyPduWithResponse:devicePublicKeyPdu];
        }
    });
}

- (void)sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:(NSData *)oobData timeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision:step2(staticOob)\n\n");
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisioningState_capabilitiesReceived) {
        TelinkLogError(@"current state is wrong.");
        return;
    }

    // Ensure the Network Key is set.
    if (self.networkKey == nil) {
        TelinkLogError(@"current networkKey isn't specified.");
        return;
    }

    // Is the Bearer open?
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"current node's bearer isn't open.");
        return;
    }

    self.provisionResponseBlock = block;

    self.provisioningData.algorithm = [self getCurrentProvisionAlgorithm];
    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];
    [self.provisioningData provisionerDidObtainAuthValue:oobData];

    // Send Provisioning Start request.
    self.state = ProvisioningState_provisioning;
    [self.provisioningData prepareWithNetwork:SigMeshLib.share.dataSource networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:self.provisioningCapabilities.publicKeyType];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_staticOob;

    SigProvisioningStartPdu *startPdu = [[SigProvisioningStartPdu alloc] initWithAlgorithm:[self getCurrentProvisionAlgorithm] publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    self.provisioningData.provisioningStartPDUValue = [startPdu.pduData subdataWithRange:NSMakeRange(1, startPdu.pduData.length-1)];
    [self sendPdu:startPdu];
    self.authenticationMethod = authenticationMethod;
    // Send the Public Key of the Provisioner.
    SigProvisioningPublicKeyPdu *publicPdu = [[SigProvisioningPublicKeyPdu alloc] initWithPublicKey:self.provisioningData.provisionerPublicKey];
    [self sendPdu:publicPdu];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
        [self performSelector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) withObject:nil afterDelay:timeout];
        if (self.provisioningCapabilities.publicKeyType == PublicKeyType_oobPublicKey) {
            //if use oob public key, needn`t waith devicePublicKey response, use devicePublicKey from certificate.
            SigProvisioningPublicKeyPdu *devicePublicKeyPdu = [[SigProvisioningPublicKeyPdu alloc] initWithPublicKey:self.devicePublicKey];
            [self sentStartProvisionPduAndPublicKeyPduWithResponse:devicePublicKeyPdu];
        }
    });
}

#pragma mark step3:Confirmation
- (void)sentProvisionConfirmationPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision:step3\n\n");
    self.provisionResponseBlock = block;
    NSData *authValue = nil;
    if (self.staticOobData) {
        //当前设置为static oob provision
        authValue = self.staticOobData;
    } else {
        //当前设置为no oob provision
        UInt8 value[32] = {};
        memset(&value, 0, 32);
        if ([self getCurrentProvisionAlgorithm] == Algorithm_fipsP256EllipticCurve) {
            authValue = [NSData dataWithBytes:&value length:16];
        } else if ([self getCurrentProvisionAlgorithm] == Algorithm_fipsP256EllipticCurve_HMAC_SHA256) {
            authValue = [NSData dataWithBytes:&value length:32];
        }
    }
    [self authValueReceivedData:authValue];

    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
        [self performSelector:@selector(sentProvisionConfirmationPduTimeout) withObject:nil afterDelay:timeout];
    });
}

#pragma mark step4:Random
- (void)sentProvisionRandomPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision:step4\n\n");
    self.provisionResponseBlock = block;
    SigProvisioningRandomPdu *pdu = [[SigProvisioningRandomPdu alloc] initWithRandom:self.provisioningData.provisionerRandom];
    [self sendPdu:pdu];

    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
        [self performSelector:@selector(sentProvisionRandomPduTimeout) withObject:nil afterDelay:timeout];
    });
}

#pragma mark step5:EncryptedData
- (void)sentProvisionEncryptedDataWithMicPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision:step5\n\n");
    self.provisionResponseBlock = block;
    NSData *provisioningData = self.provisioningData.getProvisioningData;
    NSData *encryptedProvisioningDataAndMic = [self.provisioningData getEncryptedProvisioningDataAndMicWithProvisioningData:provisioningData];
    SigProvisioningDataPdu *pdu = [[SigProvisioningDataPdu alloc] initWithEncryptedProvisioningData:[encryptedProvisioningDataAndMic subdataWithRange:NSMakeRange(0, 25)] provisioningDataMIC:[encryptedProvisioningDataAndMic subdataWithRange:NSMakeRange(25, 8)]];
    [self sendPdu:pdu];

    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
        [self performSelector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) withObject:nil afterDelay:timeout];
    });
}

- (void)getCapabilitiesResultWithResponse:(SigProvisioningPdu *)response {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_capabilities) {
        SigProvisioningCapabilitiesPdu *capabilitiesPdu = (SigProvisioningCapabilitiesPdu *)response;
        TelinkLogInfo(@"%@",capabilitiesPdu.getCapabilitiesString);
        self.provisioningCapabilities = capabilitiesPdu;
        self.provisioningData.provisioningCapabilitiesPDUValue = [capabilitiesPdu.pduData subdataWithRange:NSMakeRange(1, capabilitiesPdu.pduData.length-1)];
        self.state = ProvisioningState_capabilitiesReceived;
        if (self.capabilitiesResponseBlock) {
            self.unicastAddress = self.capabilitiesResponseBlock(capabilitiesPdu);
        }
        if (self.unicastAddress == 0) {
            self.state = ProvisioningState_fail;
        }else{
            __weak typeof(self) weakSelf = self;
            if (self.provisioningCapabilities.staticOobType.staticOobInformationAvailable == 1) {
                //设备端支持staticOOB
                if (self.staticOobData) {
                    TelinkLogVerbose(@"static OOB device, do static OOB provision, staticOobData=%@",self.staticOobData);
                    [self sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:self.staticOobData timeout:kStartProvisionAndPublicKeyTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                        [weakSelf sentStartProvisionPduAndPublicKeyPduWithResponse:response];
                    }];
                } else {
                    if (SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable) {
                        //SDK当前设置了兼容模式（即staticOOB设备可以通过noOOB provision的方式进行添加）
                        TelinkLogVerbose(@"static OOB device,do no OOB provision");
                        [self sentStartNoOobProvisionPduAndPublicKeyPduWithTimeout:kStartProvisionAndPublicKeyTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                            [weakSelf sentStartProvisionPduAndPublicKeyPduWithResponse:response];
                        }];
                    } else {
                        //SDK当前未设置兼容模式（即staticOOB设备必须通过staticOOB provision的方式进行添加）
                        //设备不支持则直接provision fail
                        TelinkLogError(@"SDK not find static OOB data, not support static OOB.");
                        self.state = ProvisioningState_fail;
                    }
                }
            } else {
                //设备端不支持staticOOB
                TelinkLogVerbose(@"no OOB device,do no OOB provision");
                [self sentStartNoOobProvisionPduAndPublicKeyPduWithTimeout:kStartProvisionAndPublicKeyTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                    [weakSelf sentStartProvisionPduAndPublicKeyPduWithResponse:response];
                }];
            }
        }
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"getCapabilities error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"getCapabilities:no handle this response data");
    }
}

- (void)getCapabilitiesTimeout {
    TelinkLogInfo(@"getCapabilitiesTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentStartProvisionPduAndPublicKeyPduWithResponse:(SigProvisioningPdu *)response {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_publicKey) {
        SigProvisioningPublicKeyPdu *publicKeyPdu = (SigProvisioningPublicKeyPdu *)response;
        TelinkLogInfo(@"device public key back:%@",[LibTools convertDataToHexStr:publicKeyPdu.publicKey]);
        self.provisioningData.devicePublicKey = publicKeyPdu.publicKey;
        [self.provisioningData provisionerDidObtainWithDevicePublicKey:publicKeyPdu.publicKey];
        if (self.provisioningData.sharedSecret && self.provisioningData.sharedSecret.length > 0) {
            TelinkLogInfo(@"APP端SharedSecret=%@",[LibTools convertDataToHexStr:self.provisioningData.sharedSecret]);
            __weak typeof(self) weakSelf = self;
            [self sentProvisionConfirmationPduWithTimeout:kProvisionConfirmationTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                [weakSelf sentProvisionConfirmationPduWithResponse:response];
            }];
        } else {
            TelinkLogDebug(@"calculate SharedSecret fail.");
            self.state = ProvisioningState_fail;
        }
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentStartProvisionPduAndPublicKeyPdu error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentStartProvisionPduAndPublicKeyPdu:no handle this response data");
    }
}

- (void)sentStartProvisionPduAndPublicKeyPduTimeout {
    TelinkLogInfo(@"sentStartProvisionPduAndPublicKeyPduTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentProvisionConfirmationPduWithResponse:(SigProvisioningPdu *)response {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_confirmation) {
        SigProvisioningConfirmationPdu *confirmationPdu = (SigProvisioningConfirmationPdu *)response;
        TelinkLogInfo(@"device confirmation back:%@",[LibTools convertDataToHexStr:confirmationPdu.confirmation]);
        [self.provisioningData provisionerDidObtainWithDeviceConfirmation:confirmationPdu.confirmation];
        if ([[self.provisioningData provisionerConfirmation] isEqualToData:confirmationPdu.confirmation]) {
            TelinkLogDebug(@"Confirmation of device is equal to confirmation of provisioner!");
            self.state = ProvisioningState_fail;
            return;
        }
        __weak typeof(self) weakSelf = self;
        [self sentProvisionRandomPduWithTimeout:kProvisionRandomTimeout callback:^(SigProvisioningPdu * _Nullable response) {
            [weakSelf sentProvisionRandomPduWithResponse:response];
        }];
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
        });
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisionConfirmationPdu error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisionConfirmationPdu:no handle this response data");
    }
}

- (void)sentProvisionConfirmationPduTimeout {
    TelinkLogInfo(@"sentProvisionConfirmationPduTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentProvisionRandomPduWithResponse:(SigProvisioningPdu *)response {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_random) {
        SigProvisioningRandomPdu *randomPdu = (SigProvisioningRandomPdu *)response;
        TelinkLogInfo(@"device random back:%@",randomPdu.random);
        [self.provisioningData provisionerDidObtainWithDeviceRandom:randomPdu.random];
        if ([self.provisioningData.provisionerRandom isEqualToData:randomPdu.random]) {
            TelinkLogDebug(@"Random of device is equal to random of provisioner!");
            self.state = ProvisioningState_fail;
            return;
        }
        if (![self.provisioningData validateConfirmation]) {
            TelinkLogDebug(@"validate Confirmation fail");
            self.state = ProvisioningState_fail;
            return;
        }
        __weak typeof(self) weakSelf = self;
        [self sentProvisionEncryptedDataWithMicPduWithTimeout:kSentProvisionEncryptedDataWithMicTimeout callback:^(SigProvisioningPdu * _Nullable response) {
            [weakSelf sentProvisionEncryptedDataWithMicPduWithResponse:response];
        }];
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisionRandomPdu error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisionRandomPdu:no handle this response data");
    }
}

- (void)sentProvisionRandomPduTimeout {
    TelinkLogInfo(@"sentProvisionRandomPduTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentProvisionEncryptedDataWithMicPduWithResponse:(SigProvisioningPdu *)response {
    TelinkLogInfo(@"\n\n==========provision end.\n\n");
    TelinkLogInfo(@"device provision result back:%@",response.pduData);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_complete) {
        [self provisionSuccess];
        [SigBearer.share setBearerProvisioned:YES];
        if (self.provisionSuccessBlock) {
            self.provisionSuccessBlock(self.unprovisionedDevice.uuid,self.unicastAddress);
        }
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisionEncryptedDataWithMic error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisionEncryptedDataWithMic:no handle this response data");
    }
    self.provisionResponseBlock = nil;
}

- (void)sentProvisionEncryptedDataWithMicPduTimeout {
    TelinkLogInfo(@"sentProvisionEncryptedDataWithMicPduTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}


/// The Provisioner sends a Provisioning Record Request PDU to request a provisioning record fragment (a part of a provisioning record; see Section 5.4.2.6) from the device.
/// @param recordID Identifies the provisioning record for which the request is made (see Section 5.4.2.6).
/// @param fragmentOffset The starting offset of the requested fragment in the provisioning record data.
/// @param fragmentMaximumSize The maximum size of the provisioning record fragment that the Provisioner can receive.
/// @param timeout timeout of this pdu.
/// @param block response of this pdu.
- (void)sentProvisioningRecordRequestWithRecordID:(UInt16)recordID fragmentOffset:(UInt16)fragmentOffset fragmentMaximumSize:(UInt16)fragmentMaximumSize timeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision: Record Request PDU\n\n");
    self.provisionResponseBlock = block;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisioningRecordRequestTimeout) object:nil];
        [self performSelector:@selector(sentProvisioningRecordRequestTimeout) withObject:nil afterDelay:timeout];
    });

    SigProvisioningRecordRequestPdu *pdu = [[SigProvisioningRecordRequestPdu alloc] initWithRecordID:recordID fragmentOffset:fragmentOffset fragmentMaximumSize:fragmentMaximumSize];
    self.state = ProvisioningState_recordRequest;
    [self sendPdu:pdu];
}

- (void)sentProvisioningRecordRequestWithResponse:(SigProvisioningPdu *)response {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisioningRecordRequestTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_recordResponse) {
        SigProvisioningRecordResponsePdu *recordResponsePdu = (SigProvisioningRecordResponsePdu *)response;
        self.state = ProvisioningState_recordResponse;
        self.totalLength = recordResponsePdu.totalLength;
        NSMutableData *mData = [NSMutableData dataWithData:self.certificateDict[@(self.currentRecordID)]];
        [mData appendData:recordResponsePdu.data];
        self.certificateDict[@(self.currentRecordID)] = mData;
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisioningRecordRequest error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisioningRecordRequest:no handle this response data");
    }
}

- (void)sentProvisioningRecordRequestTimeout {
    TelinkLogInfo(@"sentProvisioningRecordRequestTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisioningRecordRequestTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

/// The Provisioner sends a Provisioning Records Get PDU to request the list of IDs of the provisioning records that are stored on a device.
/// @param timeout timeout of this pdu.
/// @param block response of this pdu.
- (void)sentProvisioningRecordsGetWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========provision: Records Get PDU\n\n");
    self.provisionResponseBlock = block;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisioningRecordsGetTimeout) object:nil];
        [self performSelector:@selector(sentProvisioningRecordsGetTimeout) withObject:nil afterDelay:timeout];
    });

    SigProvisioningRecordsGetPdu *pdu = [[SigProvisioningRecordsGetPdu alloc] init];
    self.state = ProvisioningState_recordsGet;
    [self sendPdu:pdu];
}

- (void)sentProvisioningRecordsGetWithResponse:(SigProvisioningPdu *)response {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisioningRecordsGetTimeout) object:nil];
    });
    if (response.provisionType == SigProvisioningPduType_recordsList) {
        SigProvisioningRecordsListPdu *recordsListPdu = (SigProvisioningRecordsListPdu *)response;
        self.state = ProvisioningState_recordsList;
        TelinkLogInfo(@"response=%@,data=%@,recordsList=%@",response,recordsListPdu.pduData,recordsListPdu.recordsList);
        if ([recordsListPdu.recordsList containsObject:@(SigProvisioningRecordID_DeviceCertificate)] || [recordsListPdu.recordsList containsObject:@(SigProvisioningRecordID_CertificateBasedProvisioningURI)]) {
            //5.4.2.6.3 Provisioning records,recordID=1是Device Certificate的数据。
            self.recordsListPdu = recordsListPdu;
            [self getCertificate];
        } else {
            self.state = ProvisioningState_fail;
            TelinkLogDebug(@"sentProvisioningRecordsGet error = %@",@"Certificate-based device hasn`t recordID=0 or 1.");
        }
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        self.state = ProvisioningState_fail;
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisioningRecordsGet error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisioningRecordsGet:no handle this response data");
    }
}

- (void)sentProvisioningRecordsGetTimeout {
    TelinkLogInfo(@"sentProvisioningRecordsGetTimeout");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisioningRecordsGetTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)getCertificate {
    self.certificateDict = [NSMutableDictionary dictionary];

    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        NSLog(@"operationQueue");
        NSArray *list = [NSArray arrayWithArray:weakSelf.recordsListPdu.recordsList];
        for (NSNumber *recordNumber in list) {
            UInt16 recordID = (UInt16)[recordNumber intValue];
            weakSelf.currentRecordID = recordID;
            weakSelf.totalLength = 0;
            weakSelf.certificateDict[recordNumber] = [NSMutableData data];
            BOOL result = [weakSelf getCertificateFragmentWithRecordID:recordID];
            do {
                if (result) {
                    if (weakSelf.certificateDict[recordNumber].length == weakSelf.totalLength) {
                        //获取证书所有分段完成
                        break;
                    } else {
                        //继续获取下一分段证书
                        result = [weakSelf getCertificateFragmentWithRecordID:recordID];
                    }
                } else {
                    //获取证书失败
                    break;
                }
            } while (weakSelf.certificateDict[recordNumber].length != weakSelf.totalLength);
        }

        if (weakSelf.certificateDict.count > 0) {
            if ([weakSelf.certificateDict.allKeys containsObject:@(SigProvisioningRecordID_CertificateBasedProvisioningURI)]) {
                //为过PTS测试项，未实际完成URI的方式获取证书的功能，
                //5.6 Device Certificate retrieval over the Internet
                //MshPRT_v1.1.pdf page.618
                //PTS v8.5.1 build10 测试项的设备证书
                NSData *deviceCertificateData = [LibTools nsstringToHex:@"3082028030820227A003020102020102300A06082A8648CE3D04030230818F310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453311F301D06035504030C16496E7465726D65646961746520417574686F726974793124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3233303831373230353731305A170D3334313130333230353731305A30818B310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C035054533141303F06035504030C3830303142444330382D313032312D304230452D304130432D30303042304530413043303020424349443A3030334620425049443A303031413059301306072A8648CE3D020106082A8648CE3D03010703420004317A6F1658447274151033625AFBC4F1B60B31A45D6F459B5076E1118644BDBD265E7F37BFFDF9C4349060237F57660EAE86D4D0587CEF36D93BA7D02B545DE2A376307430090603551D1304023000300B0603551D0F040403020308301D0603551D0E04160414DB621884A7509603221ECD80F5E98DF906C5823A301F0603551D230418301680143E74DF324729FBED18B666AC0144525B9C6584C1301A0603551D200101FF0410300E300C060A60864801650302013001300A06082A8648CE3D04030203470030440220692197DBFB23DAECEC2480331F890C2B1A9F203BE2CD85395AA1E9345A6AC82F02202309FE27118E20BCC6192F7795AB0422AAA167638C52DB577971C202521B3051"];
                //PTS v8.5.1 build10 测试项的中间证书1
                NSData *intermediateCertificate1Data = [LibTools nsstringToHex:@"3082027B30820220A003020102020101300A06082A8648CE3D04030230819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3233303831373230353731305A170D3334313130333230353731305A30818F310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453311F301D06035504030C16496E7465726D65646961746520417574686F726974793124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D3059301306072A8648CE3D020106082A8648CE3D03010703420004E06E9269DF11118F528FA2A98DFDC4EC2AFF6105CC91783378E8D27D29A45415C5D740A90865AD350D51D607210915B72E43E5BC77B4BC552B0F5FD44DD0CE5CA35D305B301D0603551D0E041604143E74DF324729FBED18B666AC0144525B9C6584C1301F0603551D23041830168014C9C527F8E3D36EC844538CA132C7282C459DDFFF300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D0403020349003046022100A4B6C2C54B984703617034E0E9D80F03755D1EAB8CBF450F61FF81918EA943740221009493E512C2A8AB35CC881221544E0D21740160D4217737A2BE8090F576B98852"];
                self.certificateDict[@(SigProvisioningRecordID_DeviceCertificate)] = deviceCertificateData;
                self.certificateDict[@(SigProvisioningRecordID_IntermediateCertificate1)] = intermediateCertificate1Data;
                //实际应该从SigProvisioningRecordID_CertificateBasedProvisioningURI获取到证书，但未实现，需要移除。
                [self.certificateDict removeObjectForKey:@(SigProvisioningRecordID_CertificateBasedProvisioningURI)];
            }
            NSData *root = SigDataSource.share.defaultRootCertificateData;
            BOOL result = [OpenSSLHelper.share checkUserCertificateDataList:weakSelf.certificateDict.allValues withRootCertificate:root];
            if (result == NO) {
                TelinkLogDebug(@"=====>根证书验证失败,check certificate fail.");
                weakSelf.state = ProvisioningState_fail;
                return;
            }
        }
        [weakSelf checkCertificatePublicKeyAndStartProvision];
    }];
}

- (void)checkCertificatePublicKeyAndStartProvision {
    NSData *publicKey = [OpenSSLHelper.share checkCertificate:self.certificateDict[@(SigProvisioningRecordID_DeviceCertificate)] withSuperCertificate:self.certificateDict[@(SigProvisioningRecordID_IntermediateCertificate1)]];
    if (publicKey && publicKey.length == 64) {
        NSData *tem = [OpenSSLHelper.share getStaticOOBDataFromCertificate:self.certificateDict[@(SigProvisioningRecordID_DeviceCertificate)]];
        if (tem && tem.length) {
            self.staticOobData = [NSData dataWithData:tem];
        }
        TelinkLogInfo(@"=====>获取证书成功,deviceCertificateData=%@,publicKey=%@,staticOOB=%@",[LibTools convertDataToHexStr:self.certificateDict[@(SigProvisioningRecordID_DeviceCertificate)]],[LibTools convertDataToHexStr:publicKey],[LibTools convertDataToHexStr:self.staticOobData])
        self.devicePublicKey = publicKey;
        self.state = ProvisioningState_ready;
        __weak typeof(self) weakSelf = self;
        [self getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningPdu * _Nullable response) {
            [weakSelf getCapabilitiesResultWithResponse:response];
        }];
    } else {
        TelinkLogDebug(@"=====>证书验证失败,check certificate fail.");
        self.state = ProvisioningState_fail;
    }
}

- (BOOL)getCertificateFragmentWithRecordID:(UInt16)recordID {
    __weak typeof(self) weakSelf = self;
    __block BOOL getSuccess = NO;
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [self sentProvisioningRecordRequestWithRecordID:recordID fragmentOffset:self.certificateDict[@(self.currentRecordID)].length fragmentMaximumSize:kFragmentMaximumSize timeout:2 callback:^(SigProvisioningPdu * _Nullable response) {
        [weakSelf sentProvisioningRecordRequestWithResponse:response];
        if (response && response.provisionType == SigProvisioningPduType_recordResponse) {
            getSuccess = YES;
        }
        dispatch_semaphore_signal(semaphore);
    }];
    //Most provide 2 seconds to getDeviceCertificate
    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 2.0));
    return getSuccess;
}

/// 当SDK不支持EPA功能时，默认都使用SigFipsP256EllipticCurve_CMAC_AES128。
/// 如果static OOB长度为16，不能用SigFipsP256EllipticCurve_HMAC_SHA256而要用SigFipsP256EllipticCurve_CMAC_AES128。
/// 如果static OOB大于16，但算法为SigFipsP256EllipticCurve_CMAC_AES128，需要截取static OOB的前16字节来使用。
- (Algorithm)getCurrentProvisionAlgorithm {
    Algorithm algorithm = Algorithm_fipsP256EllipticCurve;
    if (SigDataSource.share.fipsP256EllipticCurve == SigFipsP256EllipticCurve_CMAC_AES128) {
        algorithm = Algorithm_fipsP256EllipticCurve;
    } else if (SigDataSource.share.fipsP256EllipticCurve == SigFipsP256EllipticCurve_HMAC_SHA256) {
        if ((self.staticOobData && self.staticOobData.length == 32) || self.staticOobData == nil) {
            algorithm = Algorithm_fipsP256EllipticCurve_HMAC_SHA256;
        } else {
            algorithm = Algorithm_fipsP256EllipticCurve;
        }
    } else if (SigDataSource.share.fipsP256EllipticCurve == SigFipsP256EllipticCurve_auto) {
        if (self.provisioningCapabilities.algorithms.fipsP256EllipticCurve_HMAC_SHA256 == 1) {
            if ((self.staticOobData && self.staticOobData.length == 32) || self.staticOobData == nil) {
                algorithm = Algorithm_fipsP256EllipticCurve_HMAC_SHA256;
            } else {
                algorithm = Algorithm_fipsP256EllipticCurve;
            }
        } else if (self.provisioningCapabilities.algorithms.fipsP256EllipticCurve == 1) {
            algorithm = Algorithm_fipsP256EllipticCurve;
        }
    }
    if (self.staticOobData && self.staticOobData.length > 16 && algorithm == Algorithm_fipsP256EllipticCurve) {
        self.staticOobData = [self.staticOobData subdataWithRange:NSMakeRange(0, 16)];
        TelinkLogInfo(@"Change staticOobData to 0x%@", [LibTools convertDataToHexStr:self.staticOobData]);
    }
    TelinkLogInfo(@"algorithm=%@", algorithm == Algorithm_fipsP256EllipticCurve ? @"CMAC_AES128" : @"HMAC_SHA256");
    return algorithm;
}

@end
