/********************************************************************************************************
 * @file     ResponseTestVC.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/7/6
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
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

#import "ResponseTestVC.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"

@interface CommandModel : NSObject
@property (nonatomic,strong) NSString *name;
@property (nonatomic,assign) BOOL isOn;
@end
@implementation CommandModel
@end

@interface ResponseTestVC ()
@property (weak, nonatomic) IBOutlet UITextView *contentText;
@property (weak, nonatomic) IBOutlet UIButton *autoScrollButtom;
@property (weak, nonatomic) IBOutlet UIButton *commandButtom;
@property (weak, nonatomic) IBOutlet UITextField *intervalTimeTF;
@property (weak, nonatomic) IBOutlet UITextField *totalCountTF;
@property (weak, nonatomic) IBOutlet UIButton *clearLogButtom;
@property (weak, nonatomic) IBOutlet UIButton *startButtom;
@property (nonatomic,strong) NSString *logString;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*allList;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*lostList;
@property (nonatomic,strong) NSMutableArray <CommandModel *>*commands;
@property (nonatomic,assign) NSInteger responseCount;
@property (nonatomic,assign) NSInteger commandIndex;
@property (nonatomic,assign) NSInteger intervalTime;
@property (nonatomic,assign) NSInteger totalCount;
@property (nonatomic,assign) BOOL testing;

@end

@implementation ResponseTestVC

- (void)normalSetting{
    [super normalSetting];
    _commands = [NSMutableArray array];
    if (_isResponseTest) {
        self.title = @"Response Test";
        _intervalTimeTF.text = @"3000";
        _totalCountTF.text = @"300";
        CommandModel *m1 = [[CommandModel alloc] init];
        m1.name = @"ALL ON";
        m1.isOn = YES;
        [_commands addObject:m1];
        CommandModel *m2 = [[CommandModel alloc] init];
        m2.name = @"ALL OFF";
        m2.isOn = NO;
        [_commands addObject:m2];
    } else {
        self.title = @"Interval Test";
        _intervalTimeTF.text = @"500";
        _totalCountTF.text = @"300";
        CommandModel *m1 = [[CommandModel alloc] init];
        m1.name = @"ALL ON NO-ACK";
        m1.isOn = YES;
        [_commands addObject:m1];
        CommandModel *m2 = [[CommandModel alloc] init];
        m2.name = @"ALL OFF NO-ACK";
        m2.isOn = NO;
        [_commands addObject:m2];
    }
    self.testing = NO;
    self.commandIndex = 0;
    [self refreshFirstCommandUI];
    self.autoScrollButtom.selected = YES;
    self.commandButtom.layer.borderWidth = 1;
    self.commandButtom.layer.borderColor = kDefultColor.CGColor;
    self.logString = @"";
    _allList = [NSMutableArray array];
    _lostList = [NSMutableArray array];
    for (SigNodeModel *node in SigDataSource.share.curNodes) {
        [_allList addObject:@(node.address)];
    }
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    _testing = NO;
}

- (IBAction)clickAutoScrollButtom:(UIButton *)sender {
    sender.selected = !sender.selected;
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self.autoScrollButtom.selected) {
            [self.contentText scrollRangeToVisible:NSMakeRange(self.contentText.text.length, 1)];
        }
    });
}

- (IBAction)clickCommandButtom:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Select First Command" message:nil preferredStyle:UIAlertControllerStyleAlert];
    for (CommandModel *model in self.commands) {
        UIAlertAction *alertT = [UIAlertAction actionWithTitle:model.name style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            weakSelf.commandIndex = [weakSelf.commands indexOfObject:model];
            TeLogInfo(@"index=%d,name=%@",weakSelf.commandIndex,model.name);
            [weakSelf refreshFirstCommandUI];
        }];
        [actionSheet addAction:alertT];
    }
    
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"Cancel");
    }];
    [actionSheet addAction:alertF];
    [self presentViewController:actionSheet animated:YES completion:nil];
}

- (IBAction)clickClearLogButtom:(UIButton *)sender {
    self.contentText.text = @"";
    self.logString = @"";
}

- (IBAction)clickStartButtom:(UIButton *)sender {
    [self.intervalTimeTF resignFirstResponder];
    [self.totalCountTF resignFirstResponder];
    if (self.intervalTimeTF.text.length == 0) {
        [self showTips:@"Please input interval time!"];
        return;
    }
    if (self.totalCountTF.text.length == 0) {
        [self showTips:@"Please input total count!"];
        return;
    }
    if (![self validateString:self.intervalTimeTF.text.removeAllSapceAndNewlines]) {
        [self showTips:@"Please input correct interval time!"];
        return;
    }
    if (![self validateString:self.totalCountTF.text.removeAllSapceAndNewlines]) {
        [self showTips:@"Please input correct total count!"];
        return;
    }
    self.intervalTime = [self.intervalTimeTF.text integerValue];
    if (self.intervalTime == 0) {
        [self showTips:@"Interval time cann′t be zero!"];
        return;
    }
    self.totalCount = [self.totalCountTF.text integerValue];
    if (self.totalCount == 0) {
        [self showTips:@"Total count cann′t be zero!"];
        return;
    }
    [self setUseEnable:NO];

    __weak typeof(self) weakSelf = self;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        for (NSInteger i = weakSelf.commandIndex; i < weakSelf.commandIndex + weakSelf.totalCount; i ++) {
            [weakSelf showNewLogMessage:[NSString stringWithFormat:@"round start(%ld)",(long)((weakSelf.commandIndex==1)?(i):(i+1))]];
            NSInteger sendIndex = i % weakSelf.commands.count;
            NSInteger responseMaxCount = SigDataSource.share.getOnlineDevicesNumber;
            weakSelf.responseCount = 0;
            NSTimeInterval beforeTime = [[NSDate date] timeIntervalSince1970];
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [weakSelf showNewLogMessage:[NSString stringWithFormat:@"send on/off - %@",sendIndex == 0?@"ON(1)":@"OFF(0)"]];
            [DemoCommand switchOnOffWithIsOn:sendIndex == 0 address:kMeshAddress_allNodes responseMaxCount:weakSelf.isResponseTest?(int)responseMaxCount:0 ack:weakSelf.isResponseTest successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
                weakSelf.responseCount = weakSelf.responseCount + 1;
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                if (weakSelf.isResponseTest) {
                    dispatch_semaphore_signal(semaphore);
                }
            }];
            if (weakSelf.isResponseTest) {
                //Most provide 10.0 seconds
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
                [weakSelf showNewLogMessage:[NSString stringWithFormat:@"on/off msg complete: rspCut:%ld rspMax:%d success:%@",(long)weakSelf.responseCount,(int)responseMaxCount,weakSelf.responseCount >= responseMaxCount?@"true":@"false"]];
                NSTimeInterval curTime = [[NSDate date] timeIntervalSince1970];
                [weakSelf showNewLogMessage:[NSString stringWithFormat:@"time spent(ms):%d",(int)((curTime-beforeTime)*1000)]];
            }
            [weakSelf showNewLogMessage:[NSString stringWithFormat:@"round complete(%ld)\n",(long)((weakSelf.commandIndex==1)?(i):(i+1))]];

            if (weakSelf.testing) {
                [NSThread sleepForTimeInterval:weakSelf.intervalTime / 1000.0];
            } else {
                break;
            }
        }
        [weakSelf showNewLogMessage:@"=====test complete=====\n"];
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf setUseEnable:YES];
        });
    }];
}

- (void)setUseEnable:(BOOL)enable {
    _testing = !enable;
    _commandButtom.enabled = enable;
    _intervalTimeTF.enabled = enable;
    _totalCountTF.enabled = enable;
    _startButtom.enabled = enable;
    [_startButtom setBackgroundColor:enable?kDefultColor:[UIColor lightGrayColor]];
}

- (void)refreshFirstCommandUI {
    CommandModel *model = self.commands[self.commandIndex];
    [_commandButtom setTitle:model.name forState:UIControlStateNormal];
}

- (void)showNewLogMessage:(NSString *)msg{
    TeLogInfo(@"%@",msg);
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat =@"HH:mm:ss.SSS";
    self.logString = [self.logString stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],msg]];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.contentText.text = self.logString;
        if (self.autoScrollButtom.selected) {
            [self.contentText scrollRangeToVisible:NSMakeRange(self.contentText.text.length, 1)];
        }
    });
}

- (BOOL)validateString:(NSString *)str{
    NSString *strRegex = @"^[0-9]{0,}$";
    NSPredicate *strPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",strRegex];
    return [strPredicate evaluateWithObject:str];
}

- (void)showTips:(NSString *)message{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

- (void)dealloc {
    TeLogInfo(@"");
}

@end
