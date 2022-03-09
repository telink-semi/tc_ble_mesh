/********************************************************************************************************
 * @file     TestVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/3/16
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
