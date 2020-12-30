/********************************************************************************************************
 * @file     DeviceGroupListCell.m 
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
//  DeviceGroupListCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
    //标准做法，配置多项。
    self.temOptions = [[NSMutableArray alloc] init];
    NSArray *options = [NSArray arrayWithArray:self.options];
    for (NSNumber *option in options) {
        NSArray *addresses = [self.model getAddressesWithModelID:option];
        if (addresses && addresses.count > 0) {
            [self.temOptions addObject:option];
        }
    }
    if (self.temOptions.count > 0) {
        self.isEditing = YES;
        [ShowTipsHandle.share show:Tip_EditGroup];
        self.editSubscribeListError = [NSError errorWithDomain:Tip_EditGroupFail code:-1 userInfo:nil];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(editGroupFail:) object:sender];
            [self performSelector:@selector(editGroupFail:) withObject:sender afterDelay:10.0];
        });
        [self editionNextOption:sender];
    }
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
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        sender.selected = !sender.isSelected;
        [ShowTipsHandle.share show:weakSelf.editSubscribeListError.domain];
        [ShowTipsHandle.share delayHidden:2.0];
    });
}

- (void)editingOption:(UInt32)option button:(UIButton *)button{
    if (!self.isEditing) {
        return;
    }
    if (self.model && self.groupAddress) {
        __weak typeof(self) weakSelf = self;
        //注意：多个element的情况，色温在第二个element。
        UInt16 eleAddress = self.model.address;
        
        NSMutableArray *temAddress = [self.model getAddressesWithModelID:@(option)];
        if (temAddress.count > 0) {
            eleAddress = [temAddress.firstObject intValue];
        } else {
            [self editOneOptionSuccessWithAddress:self.model.address];
            return;
        }
        __block BOOL editSubSuccess = NO;
        UInt16 modelIdentifier = 0,companyIdentifier = 0;
        if (option >= 0x1000 && option <= 0x13ff) {
            //sig model
            modelIdentifier = option;
        } else {
            //vendor model
            modelIdentifier = (option >> 16) & 0xFFFF;
            companyIdentifier = option & 0xFFFF;
        }
        [DemoCommand editSubscribeListWithWithDestination:weakSelf.model.address isAdd:button.isSelected groupAddress:weakSelf.groupAddress elementAddress:eleAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
            if (weakSelf.isEditing && responseMessage.elementAddress == eleAddress && responseMessage.address == weakSelf.groupAddress) {
                if (responseMessage.modelIdentifier == modelIdentifier && responseMessage.companyIdentifier == companyIdentifier) {
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        editSubSuccess = YES;
                    } else {
                        editSubSuccess = NO;
                        TeLogError(@"订阅组号失败：error code=%d",responseMessage.status);
                        if (responseMessage.status == SigConfigMessageStatus_insufficientResources) {
                            //资源不足，设备的组号已经添加满了。
                            weakSelf.editSubscribeListError = [NSError errorWithDomain:@"Insufficient Resources!" code:-1 userInfo:nil];
                        } else {
                            weakSelf.editSubscribeListError = [NSError errorWithDomain:Tip_EditGroupFail code:-1 userInfo:nil];
                        }
                    }
                }
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            TeLogDebug(@"edit SubscribeList finish.")
            __strong __typeof(weakSelf)strongSelf = weakSelf;
            if (editSubSuccess && error == nil) {
                [strongSelf editOneOptionSuccessWithAddress:strongSelf.model.address];
            } else {
                [strongSelf editGroupFail:button];
            }
        }];
    }
}

- (void)editOneOptionSuccessWithAddress:(int)address{
    if (self.temOptions.count > 0 && address == self.model.address) {
        [self.temOptions removeObjectAtIndex:0];
    }
    if (self.temOptions.count > 0) {
        
        //配置多项，每项之间延时200ms
//        [self performSelector:@selector(editionNextOption:) withObject:self.subToGroup afterDelay:0.2];
        
        //无需延时
        [self editionNextOption:self.subToGroup];
        
    }else{
        //所有配置已经完成
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(editGroupFail:) object:self.subToGroup];
            [self performSelector:@selector(editSuccessful) withObject:nil afterDelay:1.0];
        });
    }
}

- (void)editSuccessful{
    if (!self.isEditing) {
        return;
    }
    self.isEditing = NO;
    [ShowTipsHandle.share hidden];
    [SigDataSource.share editGroupIDsOfDevice:self.subToGroup.isSelected unicastAddress:@(self.model.address) groupAddress:@(self.groupAddress)];
    self.subToGroup.selected = self.subToGroup.isSelected;
}

- (void)editionNextOption:(UIButton *)sender{
    UInt32 option = [self.temOptions.firstObject intValue];
    [self editingOption:option button:sender];
}

@end
