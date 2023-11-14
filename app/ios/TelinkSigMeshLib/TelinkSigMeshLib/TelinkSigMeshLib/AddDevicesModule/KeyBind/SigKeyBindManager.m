/********************************************************************************************************
 * @file     SigKeyBindManager.m
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

#import "SigKeyBindManager.h"
#import "SDKLibCommand+opcodesAggregatorSequence.h"

@interface SigKeyBindManager ()
@property (nonatomic,strong) SigMessageHandle *messageHandle;
@property (nonatomic,copy) addDevice_keyBindSuccessCallBack keyBindSuccessBlock;
@property (nonatomic,copy) ErrorBlock failBlock;
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,strong) SigAppkeyModel *appkeyModel;
@property (nonatomic,assign) KeyBindType type;
@property (nonatomic,strong) SigCompositionDataPage *page;
@property (nonatomic,strong) SigNodeModel *node;
@property (nonatomic,assign) BOOL isKeybinding;

@property (nonatomic,assign) UInt16 fastKeybindProductID;
@property (nonatomic,strong) NSData *fastKeybindCpsData;

@property (nonatomic,assign) NSInteger retryCount;

//缓存旧的block，防止添加设备后不断开的情况下，SigBearer下的断开block不运行。
@property (nonatomic,copy) bleDisconnectCallback oldBluetoothDisconnectCallback;

@end

@implementation SigKeyBindManager

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigKeyBindManager *shareManager = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareManager = [[SigKeyBindManager alloc] init];
        shareManager.getCompositionTimeOut = 20;
        shareManager.appkeyAddTimeOut = 20;
        shareManager.bindModelTimeOut = 60*2;
        shareManager.retryCount = SigDataSource.share.defaultRetryCount;
    });
    return shareManager;
}

- (void)keyBind:(UInt16)address appkeyModel:(SigAppkeyModel *)appkeyModel keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail {
    self.keyBindSuccessBlock = keyBindSuccess;
    self.failBlock = fail;
    self.address = address;
    self.appkeyModel = appkeyModel;
    self.type = type;
    self.node = [SigMeshLib.share.dataSource getNodeWithAddress:address];
    self.fastKeybindProductID = productID;
    self.fastKeybindCpsData = cpsData;
    self.isKeybinding = YES;
    if (address == SigDataSource.share.unicastAddressOfConnected) {
        self.retryCount = SigDataSource.share.defaultRetryCount;
    } else {
        self.retryCount = SigDataSource.share.defaultRetryCount * 4;
    }
    __weak typeof(self) weakSelf = self;
    TelinkLogInfo(@"start keybind.");
    self.oldBluetoothDisconnectCallback = SigBluetooth.share.bluetoothDisconnectCallback;
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isKeybinding) {
                TelinkLogInfo(@"disconnect in keybinding，keybind fail.");
                [weakSelf showKeyBindEnd];
                if (fail) {
                    weakSelf.isKeybinding = NO;
                    NSError *err = [NSError errorWithDomain:@"disconnect in keybinding，keybind fail." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }
    }];

    /*
     KeyBindType_Normal:
     (原来已经连接则不需要连接逻辑)1.扫描连接、读att列表、
     2.set filter、get composition、
     3.appkey add
     4.bind model to appkey
     KeyBindType_Quick:
     1.appkey add
     */
    if (self.type == KeyBindType_Normal) {
        [self getCompositionData];
    } else if (self.type == KeyBindType_Fast) {
        [self appkeyAdd];
    }else{
        TelinkLogError(@"KeyBindType is error");
    }

}

- (void)getCompositionData {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
        [self performSelector:@selector(getCompositionDataTimeOut) withObject:nil afterDelay:self.getCompositionTimeOut];
    });
    TelinkLogDebug(@"getCompositionData 0x%02x",self.address);
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand configCompositionDataGetWithDestination:self.address retryCount:self.retryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigCompositionDataStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        weakSelf.page = ((SigConfigCompositionDataStatus *)responseMessage).page;
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (weakSelf.isKeybinding) {
            if (!isResponseAll || error) {
                [weakSelf keyBindFailActionWithErrorString:error.domain];
            } else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
                });
                BOOL hasOpCodes = NO;
                SigPage0 *page0 = (SigPage0 *)weakSelf.page;
                NSArray *elements = [NSArray arrayWithArray:page0.elements];
                for (SigElementModel *element in elements) {
                    element.parentNodeAddress = weakSelf.node.address;
                    NSArray *models = [NSArray arrayWithArray:element.models];
                    for (SigModelIDModel *modelID in models) {
                        if (modelID.getIntModelID == kSigModel_OpcodesAggregatorServer_ID) {
                            hasOpCodes = YES;
                            break;
                        }
                    }
                    if (hasOpCodes) {
                        break;
                    }
                }
                if (hasOpCodes && SigDataSource.share.aggregatorEnable) {
                    [weakSelf sendAppkeyAddAndBindModelByUsingOpcodesAggregatorSequence];
                } else {
                    [weakSelf appkeyAdd];
                }
            }
        }
    }];
    if (self.messageHandle == nil && self.isKeybinding) {
        [self keyBindFailActionWithErrorString:@"KeyBind Fail:getCompositionData fail."];
    }
}

- (void)getCompositionDataTimeOut {
    if (self.isKeybinding) {
        [self keyBindFailActionWithErrorString:@"KeyBind Fail:getCompositionData TimeOut."];
    }
}

- (void)appkeyAdd {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addAppkeyTimeOut) object:nil];
        [self performSelector:@selector(addAppkeyTimeOut) withObject:nil afterDelay:self.appkeyAddTimeOut];
    });
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand configAppKeyAddWithDestination:self.address appkeyModel:self.appkeyModel sendBySegmentPdu:YES unsegmentedMessageLowerTransportPDUMaxLength:kUnsegmentedMessageLowerTransportPDUMaxLength retryCount:self.retryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigAppKeyStatus * _Nonnull responseMessage) {
//        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        if (weakSelf.isKeybinding) {
            if (((SigConfigAppKeyStatus *)responseMessage).status == SigConfigMessageStatus_success) {
                if (weakSelf.type == KeyBindType_Normal) {
                    [weakSelf bindModel];
                } else if (weakSelf.type == KeyBindType_Fast) {
                    DeviceTypeModel *deviceType = nil;
                    if (weakSelf.fastKeybindCpsData != nil) {
                        TelinkLogVerbose(@"init cpsData from config.cpsdata.");
                        deviceType = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:weakSelf.fastKeybindProductID compositionData:weakSelf.fastKeybindCpsData];
                    } else {
                        deviceType = [SigMeshLib.share.dataSource getNodeInfoWithCID:[LibTools uint16From16String:weakSelf.node.cid] PID:[LibTools uint16From16String:weakSelf.node.pid]];
                    }
                    if (deviceType == nil) {
                        TelinkLogError(@"this node not support fast bind!!!");
                        deviceType = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:weakSelf.fastKeybindProductID compositionData:nil];
                    }
                    if (deviceType.defaultCompositionData.elements == nil || deviceType.defaultCompositionData.elements.count == 0) {
                        TelinkLogError(@"defaultCompositionData had set to CT");
                        deviceType = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:1 compositionData:nil];
                    }
                    weakSelf.page = deviceType.defaultCompositionData;
                    [weakSelf keyBindSuccessAction];
                }else{
                    TelinkLogError(@"KeyBindType is error");
                }
            } else {
                [weakSelf keyBindFailActionWithErrorString:@"KeyBind Fail:add appKey status is not success."];
            }
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (weakSelf.isKeybinding) {
            if (!isResponseAll || error) {
                [weakSelf keyBindFailActionWithErrorString:error.domain];
            }
        }
    }];
    if (self.messageHandle == nil && self.isKeybinding) {
        [self keyBindFailActionWithErrorString:@"KeyBind Fail:model bind fail."];
    }
}

- (void)addAppkeyTimeOut {
    [self keyBindFailActionWithErrorString:@"KeyBind Fail:add appkey timeout."];
}

- (void)bindModel {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addAppkeyTimeOut) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(bindModelToAppkeyTimeOut) object:nil];
        [self performSelector:@selector(bindModelToAppkeyTimeOut) withObject:nil afterDelay:self.bindModelTimeOut];
    });
    __weak typeof(self) weakSelf = self;
    //子线程执行bindModel
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        __block BOOL isFail = NO;
        SigPage0 *page0 = (SigPage0 *)weakSelf.page;
        NSArray *elements = [NSArray arrayWithArray:page0.elements];
        for (SigElementModel *element in elements) {
            element.parentNodeAddress = weakSelf.node.address;
            NSArray *models = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *modelID in models) {
                if (!modelID.isVendorModelID && modelID.isDeviceKeyModelID) {
                    TelinkLogVerbose(@"app needn't Bind modelID=%@",modelID.modelId);
                    continue;
                }
                TelinkLogVerbose(@"appBind modelID=%@",modelID.modelId);
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);

                // 写法1：不判断modelID
//                self.messageHandle = [SDKLibCommand configModelAppBindWithSigAppkeyModel:weakSelf.appkeyModel toModelIDModel:modelID toNode:weakSelf.node successCallback:^(UInt16 source, UInt16 destination, SigConfigModelAppStatus * _Nonnull responseMessage) {
//                    TelinkLogVerbose(@"SigConfigModelAppStatus.parameters=%@",responseMessage.parameters);
//                    dispatch_semaphore_signal(semaphore);
//                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                    if (!isResponseAll || error) {
//                        isFail = YES;
//                    }
//                    dispatch_semaphore_signal(semaphore);
//                }];

                // 写法2：判断modelID
                self.messageHandle = [SDKLibCommand configModelAppBindWithDestination:weakSelf.address applicationKeyIndex:weakSelf.appkeyModel.index elementAddress:element.unicastAddress modelIdentifier:modelID.getIntModelIdentifier companyIdentifier:modelID.getIntCompanyIdentifier isVendorModelID:modelID.isVendorModelID retryCount:weakSelf.retryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelAppStatus * _Nonnull responseMessage) {
                    TelinkLogInfo(@"SigConfigModelAppStatus.parameters=%@",responseMessage.parameters);
                    if (responseMessage.modelIdentifier == modelID.getIntModelIdentifier && responseMessage.companyIdentifier == modelID.getIntCompanyIdentifier && responseMessage.elementAddress == element.unicastAddress) {
//                        if (responseMessage.status == SigConfigMessageStatus_success || modelID.isVendorModelID) {//sig model判断状态，vendor model不判断状态
                        if (responseMessage.status == SigConfigMessageStatus_success) {//v3.3.3.6及之后的版本 sig model判断状态，vendor model也判断状态
                            isFail = NO;
                        } else {
                            isFail = YES;
                        }
                        dispatch_semaphore_signal(semaphore);
                    }
                    //如果判断status失败，应该设置isFail = YES;才会回调keyBind失败。
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    if (!isFail) {
                        if (!isResponseAll || error) {
                            isFail = YES;
                        }
                        dispatch_semaphore_signal(semaphore);
                    }
                }];
                if (self.messageHandle == nil) {
                    isFail = YES;
                } else {
                    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 20.0));
                }
                if (isFail) {
                    break;
                }
            }
            if (isFail) {
                break;
            }
        }
        if (!isFail) {
            [weakSelf keyBindSuccessAction];
        } else {
            [weakSelf keyBindFailActionWithErrorString:@"KeyBind Fail:model bind fail."];
        }
    }];
}

- (void)bindModelToAppkeyTimeOut {
    [self keyBindFailActionWithErrorString:@"KeyBind Fail:bind model timeout."];
}

- (void)sendAppkeyAddAndBindModelByUsingOpcodesAggregatorSequence {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sendAppkeyAddAndBindModelByUsingOpcodesAggregatorSequenceTimeout) object:nil];
        [self performSelector:@selector(sendAppkeyAddAndBindModelByUsingOpcodesAggregatorSequenceTimeout) withObject:nil afterDelay:self.appkeyAddTimeOut+self.bindModelTimeOut];
    });

    NSMutableArray *AggregatorArray = [NSMutableArray array];
    NSMutableArray *mArray = [NSMutableArray array];
    NSInteger maxLength = SigDataSource.share.security == SigMeshMessageSecurityLow ? 380 : 376;
    NSInteger sendLength = 2+2;
    NSInteger responseLength = 2+1+2;
    SigOpcodesAggregatorItemModel *model1 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigAppKeyAdd alloc] initWithApplicationKey:self.appkeyModel]];
    SigOpcodesAggregatorItemModel *model1Response = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigAppKeyStatus alloc] init]];
    sendLength = sendLength + model1.parameters.length;
    responseLength = responseLength + model1Response.parameters.length;
    [mArray addObject:model1];
    BOOL hasTimeServerModel = NO;
    UInt16 timeServerModelElementAddress = 0;
    SigPage0 *page0 = (SigPage0 *)self.page;
    NSArray *elements = [NSArray arrayWithArray:page0.elements];
    for (SigElementModel *element in elements) {
        element.parentNodeAddress = self.node.address;
        NSArray *models = [NSArray arrayWithArray:element.models];
        for (SigModelIDModel *modelID in models) {
            if (!modelID.isVendorModelID && modelID.isDeviceKeyModelID) {
                TelinkLogVerbose(@"app needn't Bind modelID=%@",modelID.modelId);
                continue;
            }
            TelinkLogVerbose(@"appBind modelID=%@",modelID.modelId);

            SigConfigModelAppBind *bindModel = [[SigConfigModelAppBind alloc] initWithApplicationKey:self.appkeyModel toModel:modelID elementAddress:element.unicastAddress];
            SigOpcodesAggregatorItemModel *model = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:bindModel];
            SigOpcodesAggregatorItemModel *modelResponse = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigModelAppStatus alloc] initWithStatus:SigConfigMessageStatus_success applicationKey:self.appkeyModel toModel:modelID elementAddress:element.unicastAddress]];
            sendLength = sendLength + model.parameters.length;
            responseLength = responseLength + modelResponse.parameters.length;
            if (sendLength > maxLength || responseLength > maxLength) {
                [AggregatorArray addObject:mArray];
                mArray = [NSMutableArray array];
                [mArray addObject:model];
                sendLength = 2+2;
                responseLength = 2+1+2;
                sendLength = sendLength + model.parameters.length;
                responseLength = responseLength + modelResponse.parameters.length;
            } else {
                [mArray addObject:model];
            }
            if (modelID.getIntModelID == kSigModel_TimeServer_ID) {
                hasTimeServerModel = YES;
                timeServerModelElementAddress = element.unicastAddress;
            }
        }
    }
    //publish time model
    if (hasTimeServerModel == YES && timeServerModelElementAddress > 0 && SigMeshLib.share.dataSource.needPublishTimeModel) {
        TelinkLogInfo(@"SDK need publish time");
        //周期，20秒上报一次。ttl:0xff（表示采用节点默认参数），0表示不relay。
        SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:0 intervalSteps:0];
        SigPublish *publish = [[SigPublish alloc] initWithDestination:kMeshAddress_allNodes withKeyIndex:SigMeshLib.share.dataSource.curAppkeyModel.index friendshipCredentialsFlag:0 ttl:0 periodSteps:kTimePublishInterval periodResolution:1 retransmit:retransmit];
        SigConfigModelPublicationSet *timePublication = [[SigConfigModelPublicationSet alloc] initWithPublish:publish toElementAddress:timeServerModelElementAddress modelIdentifier:kSigModel_TimeServer_ID companyIdentifier:0];
        SigOpcodesAggregatorItemModel *model = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:timePublication];
        SigOpcodesAggregatorItemModel *modelResponse = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigModelPublicationStatus alloc] initResponseToSigConfigAnyModelMessage:timePublication withPublish:publish]];
        sendLength = sendLength + model.parameters.length;
        responseLength = responseLength + modelResponse.parameters.length;
        if (sendLength > maxLength || responseLength > maxLength) {
            [AggregatorArray addObject:mArray];
            mArray = [NSMutableArray array];
            [mArray addObject:model];
//            sendLength = 2+2;
//            responseLength = 2+1+2;
//            sendLength = sendLength + model.parameters.length;
//            responseLength = responseLength + modelResponse.parameters.length;
        } else {
            [mArray addObject:model];
        }
    }

    if (mArray.count > 0) {
        [AggregatorArray addObject:mArray];
    }

    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        __block NSString *errorStr = @"";
        for (NSArray *items in AggregatorArray) {
            SigOpcodesAggregatorSequence *message = [[SigOpcodesAggregatorSequence alloc] initWithElementAddress:weakSelf.address items:items];
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            weakSelf.messageHandle = [SDKLibCommand sendSigOpcodesAggregatorSequenceMessage:message retryCount:weakSelf.retryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigOpcodesAggregatorStatus * _Nonnull responseMessage) {
                TelinkLogInfo(@"SigOpcodesAggregatorStatus=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (responseMessage.status != SigOpcodesAggregatorMessagesStatus_success) {
                    errorStr = [NSString stringWithFormat:@"KeyBind Fail:send AppkeyAdd And BindModel By Using OpcodesAggregatorSequence fail, status=0x%X.",responseMessage.status];
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    errorStr = error.domain;
                }
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 60.0));
            if (errorStr.length > 0) {
                break;
            }
        }
        if (errorStr.length > 0) {
            [weakSelf keyBindFailActionWithErrorString:errorStr];
        } else {
            if (weakSelf.isKeybinding) {
                [weakSelf showKeyBindEnd];
                TelinkLogInfo(@"keyBind successful.");
                weakSelf.isKeybinding = NO;
            }
            //后台发送25字节长度的ACK，需要一定的时间。
            NSInteger mtu = [SigBearer.share.getCurrentPeripheral maximumWriteValueLengthForType:CBCharacteristicWriteWithoutResponse];
            if (mtu < 25) {
                [weakSelf performSelector:@selector(finishTimePublicationAction) withObject:nil afterDelay:0.1];
            } else {
                [weakSelf finishTimePublicationAction];
            }
        }
    }];
}

- (void)sendAppkeyAddAndBindModelByUsingOpcodesAggregatorSequenceTimeout {
    [self keyBindFailActionWithErrorString:@"KeyBind Fail:send AppkeyAdd And BindModel By Using OpcodesAggregatorSequence timeout."];
}

- (void)keyBindFailActionWithErrorString:(NSString *)errorString {
    if (self.isKeybinding) {
        [self showKeyBindEnd];
        TelinkLogInfo(@"%@",errorString);
        [self.messageHandle cancel];
        self.isKeybinding = NO;
        if (self.failBlock) {
            NSError *error = [NSError errorWithDomain:errorString code:-1 userInfo:nil];
            self.failBlock(error);
        }
    }
}

- (void)keyBindSuccessAction {
    if (self.isKeybinding) {
        [self showKeyBindEnd];
        TelinkLogInfo(@"keyBind successful.");
        self.isKeybinding = NO;
        //publish time model
        UInt32 option = kSigModel_TimeServer_ID;

        SigNodeModel *node = [[SigNodeModel alloc] init];
        [node setAddress:self.node.address];
        [node setAddSigAppkeyModelSuccess:self.appkeyModel];
        [node setCompositionData:(SigPage0 *)self.page];
        NSArray *elementAddresses = [node getAddressesWithModelID:@(option)];
        if (elementAddresses.count > 0 && SigMeshLib.share.dataSource.needPublishTimeModel) {
            TelinkLogInfo(@"SDK need publish time");
            __weak typeof(self) weakSelf = self;
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                UInt16 eleAdr = [elementAddresses.firstObject intValue];
                //周期，30秒上报一次。ttl:0xff（表示采用节点默认参数），0表示不relay。
                SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:0 intervalSteps:0];
                SigPublish *publish = [[SigPublish alloc] initWithDestination:kMeshAddress_allNodes withKeyIndex:SigMeshLib.share.dataSource.curAppkeyModel.index friendshipCredentialsFlag:0 ttl:0 periodSteps:kTimePublishInterval periodResolution:1 retransmit:retransmit];
                SigModelIDModel *modelID = [node getModelIDModelWithModelID:option andElementAddress:eleAdr];
                [SDKLibCommand configModelPublicationSetWithDestination:self.address publish:publish elementAddress:eleAdr modelIdentifier:modelID.getIntModelIdentifier companyIdentifier:modelID.getIntCompanyIdentifier retryCount:self.retryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
                    TelinkLogInfo(@"publish time callback");
                    if (responseMessage.elementAddress == eleAdr) {
                        if (responseMessage.status == SigConfigMessageStatus_success && [LibTools uint16From16String:responseMessage.publish.address] == kMeshAddress_allNodes) {
                            TelinkLogInfo(@"publish time success");
                        } else {
                            TelinkLogInfo(@"publish time status=%d,pubModel.publishAddress=%@",responseMessage.status,responseMessage.publish.address);
                        }
                        //后台发送25字节长度的ACK，需要一定的时间。
                        NSInteger mtu = [SigBearer.share.getCurrentPeripheral maximumWriteValueLengthForType:CBCharacteristicWriteWithoutResponse];
                        if (mtu < 25) {
                            [weakSelf performSelector:@selector(finishTimePublicationAction) withObject:nil afterDelay:0.1];
                        } else {
                            [weakSelf finishTimePublicationAction];
                        }
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"publish time finish.");
//                    dispatch_async(dispatch_get_main_queue(), ^{
//                        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(publicationSetTimeout) object:nil];
//                    });
                    if (error) {
                        if (weakSelf.failBlock) {
                            weakSelf.failBlock(error);
                        }
                    }
                }];
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(publicationSetTimeout) object:nil];
//                    [weakSelf performSelector:@selector(publicationSetTimeout) withObject:nil afterDelay:2.0];
//                });
            });
        }else{
            TelinkLogInfo(@"SDK needn't publish time");
            [self finishTimePublicationAction];
        }
    }
}

- (void)finishTimePublicationAction {
    [self saveKeyBindSuccessToLocationData];
    [SigMeshLib.share cleanAllCommandsAndRetry];
    SigBluetooth.share.bluetoothDisconnectCallback = self.oldBluetoothDisconnectCallback;
    //callback
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self.keyBindSuccessBlock) {
            self.keyBindSuccessBlock(self.node.peripheralUUID, self.address);
        }
    });
}

- (void)saveKeyBindSuccessToLocationData {
    //appkeys
    [self.node setAddSigAppkeyModelSuccess:self.appkeyModel];
    //composition data
    [self.node setCompositionData:(SigPage0 *)self.page];
    //save
    [SigMeshLib.share.dataSource saveLocationData];
}

- (void)showKeyBindEnd {
    TelinkLogInfo(@"\n\n==========keyBind end.\n\n");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

@end
