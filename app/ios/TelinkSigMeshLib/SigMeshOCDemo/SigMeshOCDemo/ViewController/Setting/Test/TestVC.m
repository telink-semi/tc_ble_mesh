/********************************************************************************************************
 * @file     TestVC.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/3/16
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

#import "TestVC.h"

@interface TestVC ()
@property (weak, nonatomic) IBOutlet UITextView *contentText;
@property (nonatomic,strong) NSString *logString;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*allList;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*lostList;
@end

@implementation TestVC

- (IBAction)clear:(UIButton *)sender {
    self.contentText.text = @"";
    self.logString = @"";
}

- (IBAction)allONOFF:(UIButton *)sender {
    [_lostList removeAllObjects];
    [_lostList addObjectsFromArray:_allList];
    NSInteger count = SigDataSource.share.curNodes.count;
    __weak typeof(self) weakSelf = self;
    NSDate *sendDate = [NSDate date];
    BOOL on = sender.tag == 102;
    [self showNewLogMessage:[NSString stringWithFormat:@"send all on, responseMaxCount=%ld",(long)count]];
    [DemoCommand switchOnOffWithIsOn:on address:kMeshAddress_allNodes responseMaxCount:(int)count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        [weakSelf.lostList removeObject:@(source)];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (isResponseAll) {
            NSDate *endDate = [NSDate date];
            NSTimeInterval interval = [endDate timeIntervalSinceDate:sendDate];
            [self showNewLogMessage:[NSString stringWithFormat:@"all %@ successful, time=%.3fS.",on ? @"on" : @"off",interval]];
        } else {
            NSInteger responseCount = weakSelf.allList.count - weakSelf.lostList.count;
            [self showNewLogMessage:[NSString stringWithFormat:@"all on timeout, responseCount=%ld, %.2f%%.",(long)responseCount,100 * responseCount/(float)weakSelf.allList.count]];
        }
    }];
}

- (void)showNewLogMessage:(NSString *)msg{
    TeLogInfo(@"%@",msg);
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat =@"HH:mm:ss.SSS";
    self.logString = [self.logString stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],msg]];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.contentText.text = self.logString;
        [self.contentText scrollRangeToVisible:NSMakeRange(self.contentText.text.length, 1)];
    });
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Test";
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
}

- (void)dealloc {
    TeLogInfo(@"");
}

@end
