/********************************************************************************************************
* @file     AppKeyAddVC.m
*
* @brief    Add or edit the AppKey of Mesh network.
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
//  AppKeyAddVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/9/17.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "AppKeyAddVC.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"

@interface AppKeyAddVC ()<UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UITextField *indexTF;
@property (weak, nonatomic) IBOutlet UITextField *boundNetKeyTF;
@property (weak, nonatomic) IBOutlet UITextField *keyTF;
@property (weak, nonatomic) IBOutlet UILabel *oldKeyLabel;

@end

@implementation AppKeyAddVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = self.isAdd ? @"Add AppKey" : @"Edit AppKey";
    self.indexTF.delegate = self;
    self.boundNetKeyTF.delegate = self;
    self.keyTF.delegate = self;

    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;

    [self updateUI];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self.indexTF resignFirstResponder];
    [self.boundNetKeyTF resignFirstResponder];
    [self.keyTF resignFirstResponder];
}

- (void)updateUI {
    self.nameLabel.text = [NSString stringWithFormat:@"Name:%@",self.appKeyModel.name];
    self.indexTF.text = [NSString stringWithFormat:@"%04lX",(long)self.appKeyModel.index];
    self.boundNetKeyTF.text = [NSString stringWithFormat:@"%04lX",(long)self.appKeyModel.boundNetKey];
    self.keyTF.text = self.appKeyModel.key;
    self.oldKeyLabel.text = [NSString stringWithFormat:@"OldKey:0x%@",self.appKeyModel.oldKey];
}

- (void)clickSave {
    BOOL hadBound = NO;
    NSArray *temNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in temNodes) {
        if (node.appKeys && node.appKeys.count > 0) {
            for (SigNodeKeyModel *nodeKey in node.appKeys) {
                if (nodeKey.index == self.appKeyModel.index) {
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
        [self showAlertSureWithTitle:@"Hits" message:@"Some nodes have already bound this appkey, you can`t edit it!" sure:nil];
        /*客户需要保证绑定该AppKey的设备都在线，并且给所有绑定该AppKey的设备发送SigConfigAppKeyUpdate指令且成功后，才可以修改mesh里面的AppKey的值。此处发送逻辑过于复杂且限制太多，当前只是禁止客户进行修改操作。客户使用demoAPP时，可以通过下面的步骤实现修改mesh网络的某个AppKey的功能：可以先从设备里面移除该AppKey，再在setting界面修改AppKey，再给设备重新绑定AppKey。*/
        return;
    }

    [self.indexTF resignFirstResponder];
    [self.boundNetKeyTF resignFirstResponder];
    [self.keyTF resignFirstResponder];
    if (![LibTools validateHex:self.indexTF.text.removeAllSapceAndNewlines] || self.indexTF.text.length > 4) {
        [self showTips:@"Please input `index` in range 0~0x0FFF."];
        return;
    }
    UInt16 index = [LibTools uint16From16String:self.indexTF.text];
    if (index > 0x0FFF) {
        [self showTips:@"Please input `index` in range 0~0x0FFF."];
        return;
    }
    
    if (![LibTools validateHex:self.boundNetKeyTF.text.removeAllSapceAndNewlines]) {
        [self showTips:@"Please input `boundNetKey` in range 0~0x0FFF."];
        return;
    }
    UInt16 boundNetKey = [LibTools uint16From16String:self.boundNetKeyTF.text];
    if (boundNetKey > 0x0FFF) {
        [self showTips:@"Please input `boundNetKey` in range 0~0x0FFF."];
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
    if (self.appKeyModel) {
        BOOL hadExist = NO;
        NSArray *temAppkeys = [NSArray arrayWithArray:SigDataSource.share.appKeys];
        for (SigAppkeyModel *tem in temAppkeys) {
            if (tem.index == index) {
                if (self.isAdd) {
                    hadExist = YES;
                    break;
                } else {
                    if (tem != self.appKeyModel) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [self showTips:[NSString stringWithFormat:@"appkey index:0x%04X already exists, please add other appkey index.",index]];
            return;
        }

        NSArray *temNetkeys = [NSArray arrayWithArray:SigDataSource.share.netKeys];
        for (SigNetkeyModel *tem in temNetkeys) {
            if (tem.index == boundNetKey) {
                hadExist = YES;
                break;
            }
        }
        if (!hadExist) {
            [self showTips:[NSString stringWithFormat:@"boundNetKey index:0x%04X is not exists, please add other boundNetKey index.",boundNetKey]];
            return;
        }

        hadExist = NO;
        for (SigAppkeyModel *tem in temAppkeys) {
            if ([tem.key isEqualToString:self.keyTF.text]) {
                //不允许不同的NetKey下的APPkey相同
//                if (self.isAdd) {
                //允许不同的NetKey下的APPkey相同
                if (self.isAdd && tem.boundNetKey == boundNetKey) {
                    hadExist = YES;
                    break;
                } else {
                    //不允许不同的NetKey下的APPkey相同
//                    if (tem != self.appKeyModel) {
                    //允许不同的NetKey下的APPkey相同
                        if (tem != self.appKeyModel && tem.boundNetKey == boundNetKey) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [self showTips:[NSString stringWithFormat:@"appkey:0x%@ already exists, please add other appkey.",self.keyTF.text]];
            return;
        }

        self.appKeyModel.index = index;
        self.appKeyModel.boundNetKey = boundNetKey;
        self.appKeyModel.key = self.keyTF.text;
        if (self.backAppKeyModel) {
            self.backAppKeyModel(self.appKeyModel);
        }
    }
    [self.navigationController popViewControllerAnimated:YES];
}

//显示:大写，去空格
- (void)refreshShowLabel {
    self.indexTF.text = [self.indexTF.text.uppercaseString formatToLength:4];
    self.boundNetKeyTF.text = [self.boundNetKeyTF.text.uppercaseString formatToLength:4];
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
