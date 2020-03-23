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
//  Created by Liangjiazhi on 2018/7/31.
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
    self.temOptions = [[NSMutableArray alloc] initWithArray:self.options];
    if (self.temOptions.count > 0) {
        self.isEditing = YES;
        [ShowTipsHandle.share show:Tip_EditGroup];
        [self performSelector:@selector(editGroupFail:) withObject:sender afterDelay:10.0];
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
    for (NSNumber *modelID in SigDataSource.share.defaultGroupSubscriptionModels) {
        NSArray *addArray = [self.model getAddressesWithModelID:modelID];
        if (addArray.count > 0) {
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
    sender.selected = !sender.isSelected;
    [ShowTipsHandle.share show:Tip_EditGroupFail];
    [ShowTipsHandle.share delayHidden:2.0];
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
        
        [Bluetooth.share commondAfterNoBusy:^{
            [DemoCommand editSubscribeList:YES groAdd:weakSelf.groupAddress nodeAddress:weakSelf.model.address eleAddress:eleAddress  add:button.isSelected option:option complete:^(ResponseModel *m) {
                UInt16 gAddress = 0,optionBack = 0;
                UInt8 status = 0;//0 is success, other is fail, see Mesh_v1.0.pdf page 194.
                Byte *pu = (Byte *)[m.rspData bytes];
                memcpy(&gAddress, pu + 11+1, 2);
                memcpy(&optionBack, pu + 13+1, 2);
                memcpy(&status, pu + 8+1, 1);
                
                if (weakSelf.isEditing && m.address == weakSelf.model.address && gAddress == weakSelf.groupAddress && optionBack == option) {
                    if (status != 0) {
                        TeLog(@"modelID:0x%04x,subscribe to group fail.error code=%d",option,status);
                    }
                    [weakSelf editOneOptionSuccessWithAddress:m.address];
                }
            }];
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
        });
        [self performSelector:@selector(editSuccessful) withObject:nil afterDelay:1.0];
    }
}

- (void)editSuccessful{
    if (!self.isEditing) {
        return;
    }
    self.isEditing = NO;
    [ShowTipsHandle.share hidden];
    [SigDataSource.share editGroupIDsOfDevice:self.subToGroup.isSelected device_address:@(self.model.address) group_address:@(self.groupAddress)];
    self.subToGroup.selected = self.subToGroup.isSelected;
}

- (void)editionNextOption:(UIButton *)sender{
    UInt32 option = [self.temOptions.firstObject intValue];
    [self editingOption:option button:sender];
}

@end
