/********************************************************************************************************
* @file     EditOOBVC.m
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
//  EditOOBVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/5/12.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "EditOOBVC.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"

@interface EditOOBVC ()<UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UITextField *uuidTF;
@property (weak, nonatomic) IBOutlet UITextField *oobTF;
@property (weak, nonatomic) IBOutlet UILabel *showUUIDLabel;
@property (weak, nonatomic) IBOutlet UILabel *showOOBLabel;

@end

@implementation EditOOBVC
//添加要保证UUID不重复
- (void)viewDidLoad {
    [super viewDidLoad];

    if (self.isAdd) {
        self.title = @"Add OOB";
    } else {
        self.title = @"Edit OOB";
        self.uuidTF.text = self.oobModel.UUIDString;
        self.oobTF.text = self.oobModel.OOBString;
        [self refreshShowLabel];
    }
    self.uuidTF.delegate = self;
    self.oobTF.delegate = self;
    
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickSave {
    [self.uuidTF resignFirstResponder];
    [self.oobTF resignFirstResponder];
    if (![LibTools validateHex:self.uuidTF.text.removeAllSapceAndNewlines]) {
        return;
    }
    if (![LibTools validateHex:self.oobTF.text.removeAllSapceAndNewlines]) {
        return;
    }
    if (self.uuidTF.text.length != 32) {
        [self showTips:@"Please enter 16 bytes uuid."];
        return;
    }
    if (self.oobTF.text.length != 32) {
        [self showTips:@"Please enter 16 bytes oob."];
        return;
    }
    TeLogInfo(@"oob input success!");
    if (self.backOobModel) {
        if (self.isAdd) {
            SigOOBModel *model = [[SigOOBModel alloc] initWithSourceType:OOBSourceTpyeManualInput UUIDString:self.uuidTF.text OOBString:self.oobTF.text];
            if ([SigDataSource.share.OOBList containsObject:model]) {
                [self showTips:[NSString stringWithFormat:@"uuid:%@ already exists, please add other uuid.",self.uuidTF.text]];
                return;
            }
            self.backOobModel(model);
        } else {
            [self.oobModel updateWithUUIDString:self.uuidTF.text OOBString:self.oobTF.text];
            self.backOobModel(self.oobModel);
        }
    }
    [self.navigationController popViewControllerAnimated:YES];
}

//显示:大写，去空格，加空格
- (void)refreshShowLabel {
    NSString *temString = [self.uuidTF.text.uppercaseString removeAllSapceAndNewlines];
    self.uuidTF.text = temString;
    temString = [temString insertSpaceNum:1 charNum:2];
    self.showUUIDLabel.text = temString;
    
    temString = [self.oobTF.text.uppercaseString removeAllSapceAndNewlines];
    self.oobTF.text = temString;
    temString = [temString insertSpaceNum:1 charNum:2];
    self.showOOBLabel.text = temString;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self.uuidTF resignFirstResponder];
    [self.oobTF resignFirstResponder];
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
