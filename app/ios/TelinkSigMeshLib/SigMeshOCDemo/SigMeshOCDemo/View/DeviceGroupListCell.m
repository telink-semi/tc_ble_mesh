/********************************************************************************************************
 * @file     DeviceGroupListCell.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "DeviceGroupListCell.h"

@interface DeviceGroupListCell()
@property (weak, nonatomic) IBOutlet UILabel *groupName;
@property (weak, nonatomic) IBOutlet UIButton *subToGroup;
@property (assign, nonatomic) UInt16 groupAddress;
@property (strong, nonatomic) SigNodeModel *model;
@property (strong, nonatomic) NSArray <NSNumber *>*options;
@property (strong, nonatomic) NSMutableArray <NSNumber *>*temOptions;
@property (assign, nonatomic) BOOL isEditing;
@property (strong, nonatomic) NSError *editSubscribeListError;
@property (nonatomic,strong) SigMessageHandle *messageHandle;

@end

@implementation DeviceGroupListCell

- (void)awakeFromNib {
    [super awakeFromNib];

    self.options = SigDataSource.share.defaultGroupSubscriptionModels;
    self.temOptions = [[NSMutableArray alloc] init];

}

//归属group
- (IBAction)ChangeSubStatus:(UIButton *)sender {
    sender.selected = !sender.isSelected;
    BOOL isAdd = sender.isSelected;
    self.isEditing = YES;
    [ShowTipsHandle.share show:Tip_EditGroup];
    self.editSubscribeListError = [NSError errorWithDomain:Tip_EditGroupFail code:-1 userInfo:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(editGroupFail:) object:sender];
        [self performSelector:@selector(editGroupFail:) withObject:sender afterDelay:10.0];
    });
    __weak typeof(self) weakSelf = self;
    //子线程执行Subscribe
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        for (NSNumber *modelIdNumber in weakSelf.options) {
            UInt16 modelIdentifier = 0,companyIdentifier = 0;
            if (modelIdNumber.intValue >= 0x1000 && modelIdNumber.intValue <= 0x13ff) {
                //sig model
                modelIdentifier = modelIdNumber.intValue;
            } else {
                //vendor model
                modelIdentifier = (modelIdNumber.intValue >> 16) & 0xFFFF;
                companyIdentifier = modelIdNumber.intValue & 0xFFFF;
            }
            UInt16 destination = weakSelf.model.address;
            UInt16 retryCount = SigDataSource.share.defaultRetryCount;
            UInt16 responseMaxCount = 1;
            NSArray *elementAddresses = [weakSelf.model getAddressesWithModelID:modelIdNumber];
            __block int i = 0;
            for ( ; i<elementAddresses.count; i++) {
                __block BOOL isFail = NO;
                __block BOOL isSuccess = NO;
                NSNumber *elementNumber = elementAddresses[i];
                UInt16 groupAddress = weakSelf.groupAddress;
                UInt16 elementAddress = self.model.address;
                elementAddress = elementNumber.intValue;
                if (modelIdNumber.intValue == kSigModel_GenericLevelServer_ID) {
                    SigElementModel *elementModel = weakSelf.model.elements[elementAddress-destination];
                    if ([elementModel hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightCTLServer_ID]] || [elementModel hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightLightnessServer_ID]]) {
                        groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress];
                    } else if ([elementModel hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightCTLTemperatureServer_ID]]) {
                        groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 1;
                    } else if ([elementModel hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightHSLHueServer_ID]]) {
                        groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 2;
                    } else if ([elementModel hasModelIdString:[SigHelper.share getUint16String:kSigModel_LightHSLSaturationServer_ID]]) {
                        groupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:groupAddress] + 3;
                    }
                }
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                if (isAdd) {
                    self.messageHandle = [SDKLibCommand configModelSubscriptionAddWithDestination:destination toGroupAddress:groupAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:retryCount responseMaxCount:responseMaxCount successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
                        if (weakSelf.isEditing && responseMessage.elementAddress == elementAddress && responseMessage.address == groupAddress) {
                            if (responseMessage.modelIdentifier == modelIdentifier && responseMessage.companyIdentifier == companyIdentifier) {
                                if (responseMessage.status == SigConfigMessageStatus_success) {
                                    isSuccess = YES;
                                } else {
                                    isFail = YES;
                                    TelinkLogError(@"订阅组号失败：error code=%d",responseMessage.status);
                                    if (responseMessage.status == SigConfigMessageStatus_insufficientResources) {
                                        //资源不足，设备的组号已经添加满了。
                                        weakSelf.editSubscribeListError = [NSError errorWithDomain:@"Insufficient Resources!" code:-1 userInfo:nil];
                                    } else {
                                        weakSelf.editSubscribeListError = [NSError errorWithDomain:Tip_EditGroupFail code:-1 userInfo:nil];
                                    }
                                }
                            }
                        }
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        if (error) {
                            weakSelf.editSubscribeListError = error;
                        } else if (!isSuccess && !isFail) {
                            i--;
                        }
                        dispatch_semaphore_signal(semaphore);
                    }];
                } else {
                    self.messageHandle = [SDKLibCommand configModelSubscriptionDeleteWithDestination:destination groupAddress:groupAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:retryCount responseMaxCount:responseMaxCount successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
                        if (weakSelf.isEditing && responseMessage.elementAddress == elementAddress && responseMessage.address == groupAddress) {
                            if (responseMessage.modelIdentifier == modelIdentifier && responseMessage.companyIdentifier == companyIdentifier) {
                                if (responseMessage.status == SigConfigMessageStatus_success) {
                                    isSuccess = YES;
                                } else {
                                    isFail = YES;
                                    TelinkLogError(@"订阅组号失败：error code=%d",responseMessage.status);
                                    if (responseMessage.status == SigConfigMessageStatus_insufficientResources) {
                                        //资源不足，设备的组号已经添加满了。
                                        weakSelf.editSubscribeListError = [NSError errorWithDomain:@"Insufficient Resources!" code:-1 userInfo:nil];
                                    } else {
                                        weakSelf.editSubscribeListError = [NSError errorWithDomain:Tip_EditGroupFail code:-1 userInfo:nil];
                                    }
                                }
                            }
                        }
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        if (error) {
                            weakSelf.editSubscribeListError = error;
                        } else if (!isSuccess && !isFail) {
                            i--;
                        }
                        dispatch_semaphore_signal(semaphore);
                    }];
                }
                if (weakSelf.messageHandle == nil) {
                    isFail = YES;
                    [weakSelf editGroupFail:weakSelf.subToGroup];
                    return;
                } else {
                    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
                }
            }
        }
        [weakSelf editSuccessful];
    }];
}

- (void)contentWithGroupAddress:(NSNumber *)groupAddress groupName:(NSString *)groupName model:(SigNodeModel *)model{
    if (model) {
        self.model = model;
    }
    self.groupName.text = groupName;
    self.groupAddress = groupAddress.intValue;
    self.subToGroup.selected = [self.model.getGroupIDs containsObject:groupAddress];

    // 根据defaultGroupSubscriptionModels过滤出当前设备存在的modelID，只绑定存在的modelID。
    NSMutableArray *temArray = [NSMutableArray array];
    NSArray *defaultGroupSubscriptionModels = [NSArray arrayWithArray:SigDataSource.share.defaultGroupSubscriptionModels];
    for (NSNumber *modelID in defaultGroupSubscriptionModels) {
        NSArray *addArray = [self.model getAddressesWithModelID:modelID];
        if (addArray && addArray.count > 0) {
            [temArray addObject:modelID];
        }
    }
    self.options = temArray;
}

- (void)editGroupFail:(UIButton *)sender{
    if (!self.isEditing) {
        return;
    }
    self.isEditing = NO;
    if (self.messageHandle) {
        [self.messageHandle cancel];
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        sender.selected = !sender.isSelected;
        [ShowTipsHandle.share show:weakSelf.editSubscribeListError.domain];
        [ShowTipsHandle.share delayHidden:2.0];
    });
}

- (void)editSuccessful{
    if (!self.isEditing) {
        return;
    }
    self.isEditing = NO;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(editGroupFail:) object:self.subToGroup];
        [ShowTipsHandle.share hidden];
        [SigDataSource.share editGroupIDsOfDevice:self.subToGroup.isSelected unicastAddress:@(self.model.address) groupAddress:@(self.groupAddress)];
    });
}

@end
