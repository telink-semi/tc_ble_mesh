/********************************************************************************************************
 * @file     SchedulerDetailViewController.m 
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
//  SchedulerDetailViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/9/26.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SchedulerDetailViewController.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"

@interface SchedulerDetailViewController ()
@property (weak, nonatomic) IBOutlet UIButton *yearAnyButton;
@property (weak, nonatomic) IBOutlet UIButton *yearCustomButton;
@property (weak, nonatomic) IBOutlet UITextField *yearTextField;

@property (weak, nonatomic) IBOutlet UIButton *monthAllButton;
@property (weak, nonatomic) IBOutlet UIButton *month1Button;
@property (weak, nonatomic) IBOutlet UIButton *month2Button;
@property (weak, nonatomic) IBOutlet UIButton *month3Button;
@property (weak, nonatomic) IBOutlet UIButton *month4Button;
@property (weak, nonatomic) IBOutlet UIButton *month5Button;
@property (weak, nonatomic) IBOutlet UIButton *month6Button;
@property (weak, nonatomic) IBOutlet UIButton *month7Button;
@property (weak, nonatomic) IBOutlet UIButton *month8Button;
@property (weak, nonatomic) IBOutlet UIButton *month9Button;
@property (weak, nonatomic) IBOutlet UIButton *month10Button;
@property (weak, nonatomic) IBOutlet UIButton *month11Button;
@property (weak, nonatomic) IBOutlet UIButton *month12Button;

@property (weak, nonatomic) IBOutlet UIButton *dayAnyButton;
@property (weak, nonatomic) IBOutlet UIButton *dayCustomButton;
@property (weak, nonatomic) IBOutlet UITextField *dayTextField;

@property (weak, nonatomic) IBOutlet UIButton *hourAnyButton;
@property (weak, nonatomic) IBOutlet UIButton *hourOnceButton;
@property (weak, nonatomic) IBOutlet UIButton *hourCustomButton;
@property (weak, nonatomic) IBOutlet UITextField *hourTextField;

@property (weak, nonatomic) IBOutlet UIButton *minuteAnyButton;
@property (weak, nonatomic) IBOutlet UIButton *minute15Button;
@property (weak, nonatomic) IBOutlet UIButton *minute20Button;
@property (weak, nonatomic) IBOutlet UIButton *minuteOnceButton;
@property (weak, nonatomic) IBOutlet UIButton *minuteCustomButton;
@property (weak, nonatomic) IBOutlet UITextField *minuteTextField;

@property (weak, nonatomic) IBOutlet UIButton *secondAnyButton;
@property (weak, nonatomic) IBOutlet UIButton *second15Button;
@property (weak, nonatomic) IBOutlet UIButton *second20Button;
@property (weak, nonatomic) IBOutlet UIButton *secondOnceButton;
@property (weak, nonatomic) IBOutlet UIButton *secondCustomButton;
@property (weak, nonatomic) IBOutlet UITextField *secondTextField;

@property (weak, nonatomic) IBOutlet UIButton *weekAllButton;
@property (weak, nonatomic) IBOutlet UIButton *week1Button;
@property (weak, nonatomic) IBOutlet UIButton *week2Button;
@property (weak, nonatomic) IBOutlet UIButton *week3Button;
@property (weak, nonatomic) IBOutlet UIButton *week4Button;
@property (weak, nonatomic) IBOutlet UIButton *week5Button;
@property (weak, nonatomic) IBOutlet UIButton *week6Button;
@property (weak, nonatomic) IBOutlet UIButton *week7Button;

@property (weak, nonatomic) IBOutlet UIButton *actionOffButton;
@property (weak, nonatomic) IBOutlet UIButton *actionOnButton;
@property (weak, nonatomic) IBOutlet UIButton *actionNoButton;
@property (weak, nonatomic) IBOutlet UIButton *actionSceneButton;
@property (weak, nonatomic) IBOutlet UITextField *actionSceneIdField;

@property (nonatomic, strong) NSArray *monthButtons;
@property (nonatomic, strong) NSArray *weekButtons;
@property (nonatomic, strong) NSArray *checkTextFields;

@end

@implementation SchedulerDetailViewController

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = [NSString stringWithFormat:@"Index:0x%llX",self.model.schedulerID];
    
    UIButton *button = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
    [button setTitle:@"setTime" forState:UIControlStateNormal];
    [button addTarget:self action:@selector(clickSetTime) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithCustomView:button];
    UIBarButtonItem *rightItem2 = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItems = @[rightItem2,rightItem1];
    
    self.monthButtons = @[self.month1Button,self.month2Button,self.month3Button,self.month4Button,self.month5Button,self.month6Button,self.month7Button,self.month8Button,self.month9Button,self.month10Button,self.month11Button,self.month12Button,self.monthAllButton];
    self.weekButtons = @[self.week1Button,self.week2Button,self.week3Button,self.week4Button,self.week5Button,self.week6Button,self.week7Button,self.weekAllButton];
    self.checkTextFields = @[self.yearTextField,self.dayTextField,self.hourTextField,self.minuteTextField,self.secondTextField,self.actionSceneIdField];
    
    [self reloadView];
}

- (void)clickSetTime {
    [DemoCommand setNowTimeWithAddress:self.device.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigTimeStatus * _Nonnull responseMessage) {
        TeLogVerbose(@"setNowTimeWithAddress finish.");
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogVerbose(@"isResponseAll=%d,error=%@",isResponseAll,error);
    }];
}

- (void)clickSave{
    [self.view endEditing:YES];
    
    if ([self hasInputError]) {
        return;
    }
    
    NSMutableArray *allArray = [[NSMutableArray alloc] initWithObjects:self.device, nil];
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        while (allArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            SigNodeModel *curDevice = allArray.firstObject;
            [DemoCommand setSchedulerActionWithAddress:curDevice.address schedulerModel:weakSelf.model responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSchedulerActionStatus * _Nonnull responseMessage) {
                TeLogVerbose(@"responseMessage=%@,parameters=%@",responseMessage,responseMessage.parameters);
                UInt16 sceneID = weakSelf.model.sceneId;
                UInt64 scheduler = weakSelf.model.schedulerData;
                NSMutableData *mData = [NSMutableData dataWithData:[NSData dataWithBytes:&scheduler length:8]];
                [mData appendData:[NSData dataWithBytes:&sceneID length:2]];
                if ([responseMessage.parameters isEqualToData:mData]) {
                    [allArray removeObject:curDevice];
                    dispatch_semaphore_signal(semaphore);
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                TeLogDebug(@"");
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
        }
        TeLogDebug(@"save success");
        [weakSelf.device saveSchedulerModelWithModel:weakSelf.model];
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf.navigationController popViewControllerAnimated:YES];
        });
    }];
}

- (BOOL)hasInputError{
    NSArray *checkButtons = @[self.yearCustomButton,self.dayCustomButton,self.hourCustomButton,self.minuteCustomButton,self.secondCustomButton,self.actionSceneButton];
    NSArray *checkRangeMin = @[@(2000),@(1),@(0),@(0),@(0),@(0)];
    NSArray *checkRangeMax = @[@(2099),@(31),@(23),@(59),@(59),@(kMeshAddress_allNodes)];
    NSArray *checkTips = @[@"year",@"day",@"hour",@"minute",@"second",@"sceneId"];
    
    for (int i=0; i<checkButtons.count; i++) {
        UIButton *button = checkButtons[i];
        if (button.isSelected) {
            NSString *temStr = [(UITextField *)self.checkTextFields[i] text];
            if ([self validateString:temStr]) {
                int tem = [temStr intValue];
                int min = [checkRangeMin[i] intValue];
                int max = [checkRangeMax[i] intValue];
                if (tem < min || tem > max) {
                    [self showTips:[NSString stringWithFormat:@"%@ is not in range",checkTips[i]]];
                    return YES;
                }
            } else {
                [self showTips:[NSString stringWithFormat:@"%@ is error",checkTips[i]]];
                return YES;
            }
        }
    }
    
    return NO;
}

- (IBAction)clickButton:(UIButton *)sender {
    for (UITextField *tf in self.checkTextFields) {
        if (tf.isFirstResponder) {
            [self textFieldEditingEnd:tf];
        }
    }
    
    //year
    if (sender == self.yearAnyButton) {
        self.model.year = 0x64;
        [self.view endEditing:YES];
    }else if (sender == self.yearCustomButton) {
        if (self.yearTextField.text.length == 0) {
            self.yearTextField.text = @"2000";
        }
        self.model.year = ([self.yearTextField.text intValue] - 2000) & 0x7F;
    }
    
    //month
    else if ([self.monthButtons containsObject:sender]) {
        sender.selected = !sender.isSelected;
        if (sender == self.monthAllButton) {
            if (sender.isSelected) {
                self.model.month = 0xFFF;
            } else {
                self.model.month = 0;
            }
        } else {
            self.model.month = self.model.month ^ (1 << [self.monthButtons indexOfObject:sender]);
        }
    }
    
    //day
    if (sender == self.dayAnyButton) {
        self.model.day = 0;
        [self.view endEditing:YES];
    }else if (sender == self.dayCustomButton) {
        if (self.dayTextField.text.length == 0) {
            self.dayTextField.text = @"1";
        }
        self.model.day = [self.dayTextField.text intValue] & 0x1F;
    }
    
    //hour
    if (sender == self.hourAnyButton || sender == self.hourOnceButton) {
        self.model.hour = sender == self.hourAnyButton ? 0x18 : 0x19;
        [self.view endEditing:YES];
    }else if (sender == self.hourCustomButton) {
        if (self.hourTextField.text.length == 0) {
            self.hourTextField.text = @"0";
        }
        self.model.hour = [self.hourTextField.text intValue] & 0x1F;
    }
    
    //minute
    if (sender == self.minuteAnyButton || sender == self.minute15Button || sender == self.minute20Button || sender == self.minuteOnceButton) {
        if (sender == self.minuteAnyButton) {
            self.model.minute = 0x3C;
        }else if (sender == self.minute15Button) {
            self.model.minute = 0x3D;
        }else if (sender == self.minute20Button) {
            self.model.minute = 0x3E;
        }else if (sender == self.minuteOnceButton) {
            self.model.minute = 0x3F;
        }
        [self.view endEditing:YES];
    }else if (sender == self.minuteCustomButton) {
        if (self.minuteTextField.text.length == 0) {
            self.minuteTextField.text = @"0";
        }
        self.model.minute = [self.minuteTextField.text intValue] & 0x3F;
    }
    
    //second
    if (sender == self.secondAnyButton || sender == self.second15Button || sender == self.second20Button || sender == self.secondOnceButton) {
        if (sender == self.secondAnyButton) {
            self.model.second = 0x3C;
        }else if (sender == self.second15Button) {
            self.model.second = 0x3D;
        }else if (sender == self.second20Button) {
            self.model.second = 0x3E;
        }else if (sender == self.secondOnceButton) {
            self.model.second = 0x3F;
        }
        [self.view endEditing:YES];
    }else if (sender == self.secondCustomButton) {
        if (self.secondTextField.text.length == 0) {
            self.secondTextField.text = @"0";
        }
        self.model.second = [self.secondTextField.text intValue] & 0x3F;
    }
    
    //week
    else if ([self.weekButtons containsObject:sender]) {
        sender.selected = !sender.isSelected;
        if (sender == self.weekAllButton) {
            if (sender.isSelected) {
                self.model.week = 0x7F;
            } else {
                self.model.week = 0;
            }
        } else {
            self.model.week = self.model.week ^ (1 << [self.weekButtons indexOfObject:sender]);
        }
    }
    
    //action
    if (sender == self.actionOffButton || sender == self.actionOnButton || sender == self.actionNoButton) {
        if (sender == self.actionOffButton) {
            self.model.action = 0x0;
        }else if (sender == self.actionOnButton) {
            self.model.action = 0x1;
        }else if (sender == self.actionNoButton) {
            self.model.action = 0xF;
        }
        [self.view endEditing:YES];
        self.model.sceneId = 0;
    }else if (sender == self.actionSceneButton) {
        if (self.actionSceneIdField.text.length == 0) {
            self.actionSceneIdField.text = @"0";
        }
        self.model.sceneId = [self.actionSceneIdField.text intValue] & 0xFFFF;
        self.model.action = 0x2;
    }
    
    [self reloadView];
}

- (IBAction)textFieldEditingEnd:(UITextField *)sender {
    //1.remove all sapce
    sender.text = [sender.text removeAllSapceAndNewlines];
    //2.正则判断合法性
    if ([self validateString:sender.text]) {
        //3.change to int
        int tem = [sender.text intValue];
        if (sender == self.yearTextField) {
            if (tem < 2000) {
                self.model.year = 0;
            }else if (tem > 2099) {
                self.model.year = 99;
            }else{
                self.model.year = (tem - 2000) & 0x7F;
            }
        }else if (sender == self.dayTextField) {
            self.model.day = tem & 0x1F;
        }else if (sender == self.hourTextField) {
            self.model.hour = tem & 0x1F;
        }else if (sender == self.minuteTextField) {
            self.model.minute = tem & 0x3F;
        }else if (sender == self.secondTextField) {
            self.model.second = tem & 0x3F;
        }else if (sender == self.actionSceneIdField) {
            self.model.sceneId = tem & 0xFFFF;
        }
    }
}

- (void)reloadView{
    //year
    if (self.model.year <= 0x63) {
        self.yearAnyButton.selected = NO;
        self.yearCustomButton.selected = YES;
        self.yearTextField.enabled = YES;
        self.yearTextField.text = [NSString stringWithFormat:@"%d",(int)(2000+self.model.year)];
    } else if (self.model.year == 0x64) {
        self.yearAnyButton.selected = YES;
        self.yearCustomButton.selected = NO;
        self.yearTextField.enabled = NO;
    }
    
    //month
    self.monthAllButton.selected = self.model.month == 0xFFF;
    for (int i=0; i<12; i++) {
        UIButton *but = self.monthButtons[i];
        but.selected = ((self.model.month >> i) & 1) == 1;
    }
    
    //day
    if (self.model.day <= 0x1F && self.model.day > 0) {
        self.dayAnyButton.selected = NO;
        self.dayCustomButton.selected = YES;
        self.dayTextField.text = [NSString stringWithFormat:@"%d",(int)self.model.day];
        self.dayTextField.enabled = YES;
    } else if (self.model.day == 0) {
        self.dayAnyButton.selected = YES;
        self.dayCustomButton.selected = NO;
        self.dayTextField.enabled = NO;
    }
    
    //hour
    if (self.model.hour <= 0x17) {
        self.hourAnyButton.selected = NO;
        self.hourOnceButton.selected = NO;
        self.hourCustomButton.selected = YES;
        self.hourTextField.text = [NSString stringWithFormat:@"%d",(int)self.model.hour];
        self.hourTextField.enabled = YES;
    } else if (self.model.hour == 0x18 || self.model.hour == 0x19) {
        self.hourAnyButton.selected = self.model.hour == 0x18;
        self.hourOnceButton.selected = self.model.hour == 0x19;
        self.hourCustomButton.selected = NO;
        self.hourTextField.enabled = NO;
    }
    
    //minute
    self.minuteAnyButton.selected = NO;
    self.minute15Button.selected = NO;
    self.minute20Button.selected = NO;
    self.minuteOnceButton.selected = NO;
    if (self.model.minute <= 0x3B) {
        self.minuteCustomButton.selected = YES;
        self.minuteTextField.enabled = YES;
        self.minuteTextField.text = [NSString stringWithFormat:@"%d",(int)self.model.minute];
    } else if (self.model.minute >= 0x3C && self.model.minute <= 0x3F) {
        self.minuteCustomButton.selected = NO;
        self.minuteTextField.enabled = NO;
        if (self.model.minute == 0x3C) {
            self.minuteAnyButton.selected = YES;
        }else if (self.model.minute == 0x3D) {
            self.minute15Button.selected = YES;
        }else if (self.model.minute == 0x3E) {
            self.minute20Button.selected = YES;
        }else if (self.model.minute == 0x3F) {
            self.minuteOnceButton.selected = YES;
        }
    }
    
    //second
    self.secondAnyButton.selected = NO;
    self.second15Button.selected = NO;
    self.second20Button.selected = NO;
    self.secondOnceButton.selected = NO;
    if (self.model.second <= 0x3B) {
        self.secondCustomButton.selected = YES;
        self.secondTextField.enabled = YES;
        self.secondTextField.text = [NSString stringWithFormat:@"%d",(int)self.model.second];
    } else if (self.model.second >= 0x3C && self.model.second <= 0x3F) {
        self.secondCustomButton.selected = NO;
        self.secondTextField.enabled = NO;
        if (self.model.second == 0x3C) {
            self.secondAnyButton.selected = YES;
        }else if (self.model.second == 0x3D) {
            self.second15Button.selected = YES;
        }else if (self.model.second == 0x3E) {
            self.second20Button.selected = YES;
        }else if (self.model.second == 0x3F) {
            self.secondOnceButton.selected = YES;
        }
    }
    
    //week
    self.weekAllButton.selected = self.model.week == 0x7F;
    for (int i=0; i<7; i++) {
        UIButton *but = self.weekButtons[i];
        but.selected = ((self.model.week >> i) & 1) == 1;
    }
    
    //action
    self.actionOffButton.selected = NO;
    self.actionOnButton.selected = NO;
    self.actionNoButton.selected = NO;
    if (self.model.action == 0x2) {
        self.actionSceneButton.selected = YES;
        self.actionSceneIdField.enabled = YES;
        self.actionSceneIdField.text = [NSString stringWithFormat:@"%d",(int)self.model.sceneId];
    } else if (self.model.action == 0x0 || self.model.action == 0x1 || self.model.action == 0xF) {
        self.actionSceneButton.selected = NO;
        self.actionSceneIdField.enabled = NO;
        if (self.model.action == 0x0) {
            self.actionOffButton.selected = YES;
        }else if (self.model.action == 0x1) {
            self.actionOnButton.selected = YES;
        }else if (self.model.action == 0xF) {
            self.actionNoButton.selected = YES;
        }
    }
}

- (BOOL)validateString:(NSString *)str{
    NSString *strRegex = @"^[0-9]{0,}$";
    NSPredicate *strPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",strRegex];
    return [strPredicate evaluateWithObject:str];
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

@end
