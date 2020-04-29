/********************************************************************************************************
 * @file     LogViewController.m 
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
//  LogViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "LogViewController.h"

@interface LogViewController()
@property (weak, nonatomic) IBOutlet UITextView *contentText;
@property (nonatomic,strong) NSOperationQueue *operation;
@end

@implementation LogViewController

- (IBAction)clear:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    [self.operation addOperationWithBlock:^{
        NSFileHandle *handle = [NSFileHandle fileHandleForWritingAtPath:XHelp.share.logFilePath];
        [handle truncateFileAtOffset:0];
        [handle closeFile];
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.contentText.text = nil;
        });
        TeLog(@"click clear");
    }];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Log";
    self.operation = [[NSOperationQueue alloc] init];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self updateContent];
}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
}

- (void)updateContent{
    __weak typeof(self) weakSelf = self;
    [self.operation addOperationWithBlock:^{
        NSFileHandle *handle = [NSFileHandle fileHandleForReadingAtPath:XHelp.share.logFilePath];
        NSData *data = [handle readDataToEndOfFile];
        NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.contentText.text = str;
        });
    }];
}

-(void)dealloc{
    TeLog(@"");
}

@end
