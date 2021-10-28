/********************************************************************************************************
* @file     NetKeyAddVC.h
*
* @brief    Add or edit the NetKey of Mesh network.
*
* @author       Telink, 梁家誌
* @date         2020
*
* @par      Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
*           All rights reserved.
*
*           The information contained herein is confidential property of Telink
*           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
*           of Commercial License Agreement between Telink Semiconductor (Shanghai)
*           Co., Ltd. and the licensee or the terms described here-in. This heading
*           MUST NOT be removed from this file.
*
*           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
*           alter) any information contained herein in whole or in part except as expressly authorized
*           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
*           for any claim to the extent arising out of or relating to such deletion(s), modification(s)
*           or alteration(s).
*
*           Licensees are granted free, non-transferable use of the information in this
*           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
*
*******************************************************************************************************/
//
//  NetKeyAddVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/9/17.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "NetKeyAddVC.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"

@interface NetKeyAddVC ()<UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UITextField *indexTF;
@property (weak, nonatomic) IBOutlet UITextField *keyTF;
@property (weak, nonatomic) IBOutlet UILabel *oldKeyLabel;
@property (weak, nonatomic) IBOutlet UILabel *phaseLabel;
@property (weak, nonatomic) IBOutlet UILabel *minSecurityLabel;
@property (weak, nonatomic) IBOutlet UILabel *timestampLabel;

@end

@implementation NetKeyAddVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = self.isAdd ? @"Add NetKey" : @"Edit NetKey";
    self.indexTF.delegate = self;
    self.keyTF.delegate = self;

    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;

    [self updateUI];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self.indexTF resignFirstResponder];
    [self.keyTF resignFirstResponder];
}

- (void)updateUI {
    self.nameLabel.text = [NSString stringWithFormat:@"Name:%@",self.netKeyModel.name];
    self.indexTF.text = [NSString stringWithFormat:@"%04lX",(long)self.netKeyModel.index];
    self.keyTF.text = self.netKeyModel.key;
    self.oldKeyLabel.text = [NSString stringWithFormat:@"OldKey:0x%@",self.netKeyModel.oldKey];
    self.phaseLabel.text = [NSString stringWithFormat:@"Phase:0x%x",self.netKeyModel.phase];
    self.minSecurityLabel.text = [NSString stringWithFormat:@"MinSecurity:%@",self.netKeyModel.minSecurity];
    self.timestampLabel.text = [NSString stringWithFormat:@"Timestamp:%@",self.netKeyModel.timestamp];
}

- (void)clickSave {
    [self.indexTF resignFirstResponder];
    [self.keyTF resignFirstResponder];
    if (!self.isAdd) {
        BOOL hadBound = NO;
        NSArray *temNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
        for (SigNodeModel *node in temNodes) {
            if (node.netKeys && node.netKeys.count > 0) {
                for (SigNodeKeyModel *nodeKey in node.netKeys) {
                    if (nodeKey.index == self.netKeyModel.index) {
                        hadBound = YES;
                        break;
                    }
                }
                if (hadBound) {
                    break;
                }
            }
        }
        if (hadBound) {
            [self showAlertSureWithTitle:@"Hits" message:@"Some nodes have already bound this netkey, you can`t edit it!" sure:nil];
            /*客户需要保证添加了该NetKey的设备都在线，并且给所有添加了该NetKey的设备发送SigConfigNetKeyUpdate指令且成功后，才可以修改mesh里面的NetKey的值。此处发送逻辑过于复杂且限制太多，当前只是禁止客户进行修改操作。客户使用demoAPP时，可以通过下面的步骤实现修改mesh网络的某个NetKey的功能：可以先从设备里面移除该NetKey，再在setting界面修改NetKey，再给设备重新添加NetKey。*/
            return;
        }
    }
    
    if (![LibTools validateHex:self.indexTF.text.removeAllSapceAndNewlines] || self.indexTF.text.length > 4) {
        [self showTips:@"Please input `index` in range 0~0x0FFF."];
        return;
    }
    UInt16 index = [LibTools uint16From16String:self.indexTF.text];
    if (index > 0x0FFF) {
        [self showTips:@"Please input `index` in range 0~0x0FFF."];
        return;
    }
    
    if (![LibTools validateHex:self.keyTF.text.removeAllSapceAndNewlines]) {
        [self showTips:@"Please enter 16 bytes hex key data."];
        return;
    }
    if (self.keyTF.text.length != 32) {
        [self showTips:@"Please enter 16 bytes hex key data."];
        return;
    }

    TeLogInfo(@"appkey input success!");
    if (self.netKeyModel) {
        BOOL hadExist = NO;
        NSArray *temNetkeys = [NSArray arrayWithArray:SigDataSource.share.netKeys];
        for (SigNetkeyModel *tem in temNetkeys) {
            if (tem.index == index) {
                if (self.isAdd) {
                    hadExist = YES;
                    break;
                } else {
                    if (tem != self.netKeyModel) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [self showTips:[NSString stringWithFormat:@"netkey index:0x%04X already exists, please add other netkey index.",index]];
            return;
        }

        for (SigNetkeyModel *tem in temNetkeys) {
            if ([tem.key isEqualToString:self.keyTF.text]) {
                if (self.isAdd) {
                    hadExist = YES;
                    break;
                } else {
                    if (tem != self.netKeyModel) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [self showTips:[NSString stringWithFormat:@"netkey:0x%@ already exists, please add other netkey.",self.keyTF.text]];
            return;
        }

        self.netKeyModel.index = index;
        self.netKeyModel.key = self.keyTF.text;
        if (self.backNetKeyModel) {
            self.backNetKeyModel(self.netKeyModel);
        }
    }
    [self.navigationController popViewControllerAnimated:YES];
}

//显示:大写，去空格
- (void)refreshShowLabel {
    self.indexTF.text = [self.indexTF.text.uppercaseString formatToLength:4];
    self.keyTF.text = [self.keyTF.text.uppercaseString removeAllSapceAndNewlines];
}

#pragma mark - UITextFieldDelegate

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField{
    if ([LibTools validateHex:textField.text.removeAllSapceAndNewlines]) {
        [self refreshShowLabel];
        return YES;
    }
    [self showTips:@"Please enter a valid hexadecimal data."];
    return YES;
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

@end
