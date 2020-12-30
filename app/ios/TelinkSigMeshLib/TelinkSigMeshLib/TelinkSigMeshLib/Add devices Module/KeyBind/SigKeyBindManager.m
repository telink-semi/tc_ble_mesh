/********************************************************************************************************
 * @file     SigKeyBindManager.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  SigKeyBindManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/4.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigKeyBindManager.h"

@interface SigKeyBindManager ()
@property (nonatomic,strong) SigMessageHandle *messageHandle;
@property (nonatomic,copy) addDevice_keyBindSuccessCallBack keyBindSuccessBlock;
@property (nonatomic,copy) ErrorBlock failBlock;
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,strong) SigAppkeyModel *appkeyModel;
@property (nonatomic,assign) KeyBindTpye type;
@property (nonatomic,strong) SigCompositionDataPage *page;
@property (nonatomic,strong) SigNodeModel *node;
@property (nonatomic,assign) BOOL isKeybinding;

@property (nonatomic,assign) UInt16 fastKeybindProductID;
@property (nonatomic,strong) NSData *fastKeybindCpsData;

@end

@implementation SigKeyBindManager

+ (SigKeyBindManager *)share {
    static SigKeyBindManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigKeyBindManager alloc] init];
        shareManager.getCompositionTimeOut = 10;
        shareManager.appkeyAddTimeOut = 10;
        shareManager.bindModelTimeOut = 60;
    });
    return shareManager;
}

- (void)keyBind:(UInt16)address appkeyModel:(SigAppkeyModel *)appkeyModel keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail {
    self.keyBindSuccessBlock = keyBindSuccess;
    self.failBlock = fail;
    self.address = address;
    self.appkeyModel = appkeyModel;
    self.type = type;
    self.node = [SigDataSource.share getNodeWithAddress:address];
    self.fastKeybindProductID = productID;
    self.fastKeybindCpsData = cpsData;
    self.isKeybinding = YES;
    __weak typeof(self) weakSelf = self;
    TeLogInfo(@"start keybind.");
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isKeybinding) {
                TeLogInfo(@"disconnect in keybinding，keybind fail.");
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
     KeyBindTpye_Normal:
     (原来已经连接则不需要连接逻辑)1.扫描连接、读att列表、
     2.set filter、get composition、
     3.appkey add
     4.bind model to appkey
     KeyBindTpye_Quick:
     1.appkey add
     */
    if (self.type == KeyBindTpye_Normal) {
        [self getCompositionData];
    } else if (self.type == KeyBindTpye_Fast) {
        [self appkeyAdd];
    }else{
        TeLogError(@"KeyBindTpye is error");
    }
    
}

- (void)getCompositionData {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
        [self performSelector:@selector(getCompositionDataTimeOut) withObject:nil afterDelay:self.getCompositionTimeOut];
    });
    TeLogDebug(@"getCompositionData 0x%02x",self.address);
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand configCompositionDataGetWithDestination:self.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigCompositionDataStatus * _Nonnull responseMessage) {
        TeLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        weakSelf.page = ((SigConfigCompositionDataStatus *)responseMessage).page;
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (weakSelf.isKeybinding) {
            if (!isResponseAll || error) {
                [weakSelf showKeyBindEnd];
                weakSelf.isKeybinding = NO;
                if (weakSelf.failBlock) {
                    weakSelf.failBlock(error);
                }
            } else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
                });
                [weakSelf appkeyAdd];
            }
        }
    }];
    if (self.messageHandle == nil && self.isKeybinding) {
        [self showKeyBindEnd];
        self.isKeybinding = NO;
        if (self.failBlock) {
            NSError *error = [NSError errorWithDomain:@"KeyBind Fail:getCompositionData fail." code:-1 userInfo:nil];
            self.failBlock(error);
        }
    }
}

- (void)getCompositionDataTimeOut {
    if (self.isKeybinding) {
        [self showKeyBindEnd];
        [self.messageHandle cancel];
        self.isKeybinding = NO;
        if (self.failBlock) {
            NSError *error = [NSError errorWithDomain:@"KeyBind Fail:getCompositionData TimeOut." code:-1 userInfo:nil];
            self.failBlock(error);
        }
    }
}

- (void)appkeyAdd {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addAppkeyTimeOut) object:nil];
        [self performSelector:@selector(addAppkeyTimeOut) withObject:nil afterDelay:self.appkeyAddTimeOut];
    });
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand configAppKeyAddWithDestination:self.address appkeyModel:self.appkeyModel retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigAppKeyStatus * _Nonnull responseMessage) {
        TeLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        if (weakSelf.isKeybinding) {
            if (((SigConfigAppKeyStatus *)responseMessage).status == SigConfigMessageStatus_success) {
                if (self.type == KeyBindTpye_Normal) {
                    [weakSelf bindModel];
                } else if (self.type == KeyBindTpye_Fast) {
                    DeviceTypeModel *deviceType = nil;
                    if (weakSelf.fastKeybindCpsData != nil) {
                        TeLogVerbose(@"init cpsData from config.cpsdata.");
                        deviceType = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:weakSelf.fastKeybindProductID compositionData:weakSelf.fastKeybindCpsData];
                    } else {
                        deviceType = [SigDataSource.share getNodeInfoWithCID:[LibTools uint16From16String:weakSelf.node.cid] PID:[LibTools uint16From16String:weakSelf.node.pid]];
                    }
                    if (deviceType == nil) {
                        TeLogError(@"this node not support fast bind!!!");
                        deviceType = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:weakSelf.fastKeybindProductID];
                    }
                    if (deviceType.defaultCompositionData.elements == nil || deviceType.defaultCompositionData.elements.count == 0) {
                        TeLogError(@"defaultCompositionData had setted to CT");
                        deviceType = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:1];
                    }
                    weakSelf.page = deviceType.defaultCompositionData;
                    [weakSelf keyBindSuccessAction];
                }else{
                    TeLogError(@"KeyBindTpye is error");
                }
            } else {
                [weakSelf showKeyBindEnd];
                weakSelf.isKeybinding = NO;
                if (weakSelf.failBlock) {
                    NSError *error = [NSError errorWithDomain:@"KeyBind Fail:add appKey status is not success." code:-1 userInfo:nil];
                    weakSelf.failBlock(error);
                }
            }
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (weakSelf.isKeybinding) {
            if (!isResponseAll || error) {
                [weakSelf showKeyBindEnd];
                weakSelf.isKeybinding = NO;
                if (weakSelf.failBlock) {
                    weakSelf.failBlock(error);
                }
            }
        }
    }];
    if (self.messageHandle == nil && self.isKeybinding) {
        [self showKeyBindEnd];
        self.isKeybinding = NO;
        if (self.failBlock) {
            NSError *error = [NSError errorWithDomain:@"KeyBind Fail:model bind fail." code:-1 userInfo:nil];
            self.failBlock(error);
        }
    }
}

- (void)addAppkeyTimeOut {
    if (self.isKeybinding) {
        [self showKeyBindEnd];
        [self.messageHandle cancel];
        self.isKeybinding = NO;
        if (self.failBlock) {
            NSError *error = [NSError errorWithDomain:@"KeyBind Fail:appkeyAdd fail." code:-1 userInfo:nil];
            self.failBlock(error);
        }
    }
}

- (void)bindModel {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addAppkeyTimeOut) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(bindModelToAppkeyTimeOut) object:nil];
        [self performSelector:@selector(bindModelToAppkeyTimeOut) withObject:nil afterDelay:self.bindModelTimeOut];
    });
    __weak typeof(self) weakSelf = self;
    //子线程执行bindModel
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        __block BOOL isFail = NO;
        SigPage0 *page0 = (SigPage0 *)weakSelf.page;
        NSArray *elements = [NSArray arrayWithArray:page0.elements];
        for (SigElementModel *element in elements) {
            element.parentNodeAddress = weakSelf.node.address;
            NSArray *models = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *modelID in models) {
                if (modelID.isConfigurationServer) {
                    TeLogVerbose(@"app needn't Bind modelID=%@",modelID.modelId);
                    continue;
                }
                TeLogVerbose(@"appBind modelID=%@",modelID.modelId);
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                
                // 写法1：不判断modelID
//                self.messageHandle = [SDKLibCommand configModelAppBindWithSigAppkeyModel:weakSelf.appkeyModel toModelIDModel:modelID toNode:weakSelf.node successCallback:^(UInt16 source, UInt16 destination, SigConfigModelAppStatus * _Nonnull responseMessage) {
//                    TeLogVerbose(@"SigConfigModelAppStatus.parameters=%@",responseMessage.parameters);
//                    dispatch_semaphore_signal(semaphore);
//                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                    if (!isResponseAll || error) {
//                        isFail = YES;
//                    }
//                    dispatch_semaphore_signal(semaphore);
//                }];
                
                // 写法2：判断modelID
                self.messageHandle = [SDKLibCommand configModelAppBindWithDestination:weakSelf.address applicationKeyIndex:weakSelf.appkeyModel.index elementAddress:element.unicastAddress modelIdentifier:modelID.getIntModelIdentifier companyIdentifier:modelID.getIntCompanyIdentifier retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelAppStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"SigConfigModelAppStatus.parameters=%@",responseMessage.parameters);
                    if (responseMessage.status == SigConfigMessageStatus_success && responseMessage.modelIdentifier == modelID.getIntModelIdentifier && responseMessage.companyIdentifier == modelID.getIntCompanyIdentifier && responseMessage.elementAddress == element.unicastAddress) {
                        isFail = NO;
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
                    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 3.0));
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
            if (weakSelf.isKeybinding) {
                [weakSelf showKeyBindEnd];
                TeLogInfo(@"keyBind fail.");
                [weakSelf.messageHandle cancel];
                weakSelf.isKeybinding = NO;
                if (weakSelf.failBlock) {
                    NSError *error = [NSError errorWithDomain:@"KeyBind Fail:model bind fail." code:-1 userInfo:nil];
                    weakSelf.failBlock(error);
                }
            }
        }
    }];
}

- (void)bindModelToAppkeyTimeOut {
    if (self.isKeybinding) {
        [self showKeyBindEnd];
        TeLogInfo(@"keyBind timeout.");
        [self.messageHandle cancel];
        self.isKeybinding = NO;
        if (self.failBlock) {
            NSError *error = [NSError errorWithDomain:@"KeyBind Fail:bind model TimeOut." code:-1 userInfo:nil];
            self.failBlock(error);
        }
    }
}

- (void)keyBindSuccessAction {
    if (self.isKeybinding) {
        [self showKeyBindEnd];
        TeLogInfo(@"keyBind successful.");
        self.isKeybinding = NO;
        //publish time model
        UInt32 option = SIG_MD_TIME_S;
        
        SigNodeModel *node = [[SigNodeModel alloc] init];
        [node setAddress:self.node.address];
        [node setAddSigAppkeyModelSuccess:self.appkeyModel];
        [node setCompositionData:(SigPage0 *)self.page];
        NSArray *elementAddresses = [node getAddressesWithModelID:@(option)];
        if (elementAddresses.count > 0 && SigDataSource.share.needPublishTimeModel) {
            TeLogInfo(@"SDK need publish time");
            __weak typeof(self) weakSelf = self;
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                UInt16 eleAdr = [elementAddresses.firstObject intValue];
                //周期，20秒上报一次。ttl:0xff（表示采用节点默认参数），0表示不relay。
                SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:0 intervalSteps:2];
                SigPublish *publish = [[SigPublish alloc] initWithDestination:kMeshAddress_allNodes withKeyIndex:SigDataSource.share.curAppkeyModel.index friendshipCredentialsFlag:0 ttl:0 periodSteps:kTimePublishInterval periodResolution:1 retransmit:retransmit];
                SigModelIDModel *modelID = [node getModelIDModelWithModelID:option andElementAddress:eleAdr];
                [SDKLibCommand configModelPublicationSetWithDestination:self.address publish:publish elementAddress:eleAdr modelIdentifier:modelID.getIntModelIdentifier companyIdentifier:modelID.getIntCompanyIdentifier retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"publish time callback");
                    if (responseMessage.elementAddress == eleAdr) {
                        if (responseMessage.status == SigConfigMessageStatus_success && [LibTools uint16From16String:responseMessage.publish.address] == kMeshAddress_allNodes) {
                            TeLogInfo(@"publish time success");
                        } else {
                            TeLogInfo(@"publish time status=%d,pubModel.publishAddress=%@",responseMessage.status,responseMessage.publish.address);
                        }
                        [weakSelf saveKeyBindSuccessToLocationData];
                        [SigMeshLib.share cleanAllCommandsAndRetry];
                        //callback
                        if (weakSelf.keyBindSuccessBlock) {
                            weakSelf.keyBindSuccessBlock(weakSelf.node.peripheralUUID, weakSelf.address);
                        }
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"publish time finish.");
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(publicationSetTimeout) object:nil];
                    });
                    if (error) {
                        if (weakSelf.failBlock) {
                            weakSelf.failBlock(error);
                        }
                    }
                }];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(publicationSetTimeout) object:nil];
                    [weakSelf performSelector:@selector(publicationSetTimeout) withObject:nil afterDelay:2.0];
                });
            });
        }else{
            [self saveKeyBindSuccessToLocationData];
            [SigMeshLib.share cleanAllCommandsAndRetry];
            TeLogInfo(@"SDK needn't publish time");
            //callback
            if (self.keyBindSuccessBlock) {
                self.keyBindSuccessBlock(self.node.peripheralUUID, self.address);
            }
        }
    }
}

- (void)publicationSetTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(publicationSetTimeout) object:nil];
    });
    TeLogInfo(@"publish time timeout.");
    NSError *error = [NSError errorWithDomain:@"KeyBind Fail:publicationSet time model TimeOut." code:-1 userInfo:nil];
    if (error) {
        if (self.failBlock) {
            self.failBlock(error);
        }
    }
}

- (void)saveKeyBindSuccessToLocationData {
    //appkeys
    [self.node setAddSigAppkeyModelSuccess:self.appkeyModel];
    //composition data
    [self.node setCompositionData:(SigPage0 *)self.page];
    //save
    [SigDataSource.share saveLocationData];
}

- (void)showKeyBindEnd {
    TeLogInfo(@"\n\n==========keyBind end.\n\n");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

@end
