//
//  TestAddDeviceVC.m
//  TelinkBlueDemo
//
//  Created by Liangjiazhi on 2019/3/7.
//  Copyright © 2019年 Green. All rights reserved.
//

/* 1.demoAPP显示在setting界面需如下设置：SigMeshOCDemo->General->SigMeshOCDemo copy->Deploymant Info->Main interface->Main.storyboard
   2.demoAPP只显示测试界面需如下设置：SigMeshOCDemo->General->SigMeshOCDemo copy->Deploymant Info->Main interface->TestAddDeviceVC.storyboard */

#import "TestAddDeviceVC.h"
#import "TLog.h"

@interface TestAddDeviceVC ()
@property (nonatomic,strong) NSOperationQueue *operation;

@property (weak, nonatomic) IBOutlet UITextField *macTF;
@property (weak, nonatomic) IBOutlet UITextField *testCountTF;
@property (weak, nonatomic) IBOutlet UILabel *testResultLabel;
@property (weak, nonatomic) IBOutlet UIButton *controlButton;
@property (weak, nonatomic) IBOutlet UITextView *logTV;
@property (weak, nonatomic) IBOutlet UITextField *delayOfDeleteTF;

@property (nonatomic,strong) NSString *macString;
@property (nonatomic,assign) UInt32 delayOfDelete;//删除成功到下一次添加之间的延时。
@property (nonatomic,assign) UInt32 testCount;//总测试次数，0为一直测试
@property (nonatomic,assign) UInt32 currentCount;//当前已经测试次数
@property (nonatomic,assign) UInt32 currentSuccessCount;//当前成功次数
@property (nonatomic,assign) UInt32 currentFailCount;//当前失败次数
@property (nonatomic,assign) UInt32 currentKickoutFailCount;//当前删除失败次数

@property (nonatomic,assign) double averageScanTime;//当前平均扫描耗时
@property (nonatomic,assign) double averageConnectTime;//当前平均连接耗时
@property (nonatomic,assign) double averageProvisionTime;//当前平均provision耗时
@property (nonatomic,assign) double averageKeybindTime;//当前平均keybind耗时
@property (nonatomic,assign) double currentScanTime;//本次扫描耗时
@property (nonatomic,assign) double currentConnectTime;//本次连接耗时
@property (nonatomic,assign) double currentProvisionTime;//本次provision耗时
@property (nonatomic,assign) double currentKeybindTime;//本次keybind耗时

@property (nonatomic,assign) NSTimeInterval time;

@end

static NSUInteger provisionAddress;

@implementation TestAddDeviceVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(hiddenKB)];
    [self.view addGestureRecognizer:tap];
    
    //界面默认显示的MAC和次数
    NSString *mac = [self getCurrentMac];
    if (mac == nil) {
        mac = @"ABCD7B12C87A";
//        mac = @"ABCDBBCD2A4C";
        
        [self saveCurrentMac:mac];
    }
    self.macTF.text = mac;
    self.delayOfDeleteTF.text = @"3";
    self.testCountTF.text = @"100";
    self.title = @"TestAddDevcies";

    //默认只添加一个特定MAC的设备，且修改后的短地址都是2.
    provisionAddress = 2;
    [SigDataSource.share removeModelWithDeviceAddress:provisionAddress];

    [TLog.share initLogFile];
    __weak typeof(self) weakSelf = self;
    self.operation = [[NSOperationQueue alloc] init];
    [self.operation addOperationWithBlock:^{
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.logTV.text = TLog.share.allLogString;
            [weakSelf.logTV scrollRangeToVisible:NSMakeRange(weakSelf.logTV.text.length, 1)];
        });
    }];
}

- (void)saveCurrentMac:(NSString *)mac {
    [[NSUserDefaults standardUserDefaults] setObject:mac forKey:@"TestAddDeviceMac"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (NSString *)getCurrentMac {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *mac = [defaults objectForKey:@"TestAddDeviceMac"];
    return mac;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)hiddenKB{
    [self.view endEditing:YES];
}

- (IBAction)clearLogButton:(UIButton *)sender {
    self.logTV.text = @"";
    [TLog.share clearAllLog];
}

- (IBAction)clickControlButton:(UIButton *)sender {
    [self hiddenKB];
    if ([self.controlButton.titleLabel.text isEqualToString:@"Start"]) {
        //开始测试
        if (![self checkMacTF]) {
            return;
        }
        if (![self checkTestCountTF]) {
            return;
        }
        if (![self checkDelayOfDeleteTF]) {
            return;
        }
        self.currentCount = 0;
        self.currentSuccessCount = 0;
        self.currentFailCount = 0;
        self.currentKickoutFailCount = 0;
        self.averageScanTime = 0;
        self.averageConnectTime = 0;
        self.averageProvisionTime = 0;
        self.averageKeybindTime = 0;

        [self.controlButton setTitle:@"Stop" forState:UIControlStateNormal];
        [self startTest];
    } else {
        //结束测试
        [self.controlButton setTitle:@"Start" forState:UIControlStateNormal];
        [self stopTest];
    }
}

- (void)scanAndConnnectAndProvisionAndKeybind {
    [self showAndSaveLog:[NSString stringWithFormat:@"[Start] scan [%d]",_currentCount]];
    __weak typeof(self) weakSelf = self;
    __block BOOL scanSuccess = NO;
    [Bluetooth.share.commandHandle startScanWithProvisionAble:YES timeout:10.0 discoverDevice:^(CBPeripheral *peripheral, SigScanRspModel *model, BOOL provisionAble) {
        if (model && ((weakSelf.macString.length == 12 && [weakSelf.macString isEqualToString:model.macAddress]) || weakSelf.macString.length == 0)) {
            scanSuccess = YES;
            [Bluetooth.share.commandHandle stopScan];
            weakSelf.currentScanTime = [[NSDate date] timeIntervalSince1970] - weakSelf.time;
            [weakSelf showAndSaveLog:[NSString stringWithFormat:@"[End] scan callback, mac:%@ scanT:%0.2f",model.macAddress,weakSelf.currentScanTime]];
            weakSelf.time = [[NSDate date] timeIntervalSince1970];
            [weakSelf refreshShowLabel];
            [weakSelf showAndSaveLog:@"[Start] connecting"];
            [Bluetooth.share.commandHandle startProvisionWithPeripheral:peripheral unicastAddress:provisionAddress networkKey:[SigDataSource.share curNetKey] netkeyIndex:SigDataSource.share.curNetkeyModel.index provisionType:ProvisionTpye_NoOOB staticOOBData:nil startSendProvision:^(CBPeripheral *peripheral) {
                weakSelf.currentConnectTime = [[NSDate date] timeIntervalSince1970] - weakSelf.time;
                [weakSelf showAndSaveLog:[NSString stringWithFormat:@"[End] connect and read service success, connectT:%0.2f",weakSelf.currentConnectTime]];
                weakSelf.time = [[NSDate date] timeIntervalSince1970];
                [weakSelf refreshShowLabel];
                [weakSelf showAndSaveLog:@"[Start] provisioning"];
            } provisionSuccess:^(NSString *identify, UInt16 address) {
                weakSelf.currentProvisionTime = [[NSDate date] timeIntervalSince1970] - weakSelf.time;
                [weakSelf showAndSaveLog:[NSString stringWithFormat:@"[End] provision success, provisionT:%0.2f",weakSelf.currentProvisionTime]];
                weakSelf.time = [[NSDate date] timeIntervalSince1970];
                [weakSelf refreshShowLabel];
                [weakSelf showAndSaveLog:@"[Start] setFilter"];
                [Bluetooth.share.commandHandle setFilterWithLocationAddress:SigDataSource.share.curLocationNodeModel.address timeout:kSetFilterTimeout result:^(BOOL isSuccess) {
                    if (isSuccess) {
                        [weakSelf showAndSaveLog:@"[End] setFilter success"];
                        [weakSelf showAndSaveLog:@"[Start] keybinding"];
                        [Bluetooth.share.commandHandle startKeyBindWithPeripheral:peripheral unicastAddress:provisionAddress appKey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index netkeyIndex:SigDataSource.share.curNetkeyModel.index keyBindType:KeyBindTpye_Normal retryCount:0 productID:0 cpsData:nil keyBindSuccess:^(NSString *identify, UInt16 address) {
                            weakSelf.currentKeybindTime = [[NSDate date] timeIntervalSince1970] - weakSelf.time;
                            [weakSelf showAndSaveLog:[NSString stringWithFormat:@"[End] keybind success, keybindT:%0.2f",weakSelf.currentKeybindTime]];
                            weakSelf.time = [[NSDate date] timeIntervalSince1970];
                            [weakSelf refreshShowLabel];
                            //添加成功
                            [weakSelf showAndSaveLog:@"[End] add device success"];
                            if (weakSelf.macTF.text.length == 0) {
                                dispatch_async(dispatch_get_main_queue(), ^{
                                    weakSelf.macTF.text = model.macAddress;
                                });
                                weakSelf.macString = model.macAddress;
                                [weakSelf saveCurrentMac:model.macAddress];
                            }
                            [weakSelf refreshShowLabel];
                            [weakSelf performSelector:@selector(deleteDevice) withObject:nil afterDelay:1.0];
                        } fail:^(NSError *error) {
                            //添加keybind失败
                            [weakSelf showAndSaveLog:@"[End] keybind fail"];
                            [weakSelf failAction];
                        }];
                    } else {
                        //添加setFilter失败
                        [weakSelf showAndSaveLog:@"[End] setFilter fail"];
                        [weakSelf failAction];
                    }
                }];
            } fail:^(NSError *error) {
                //添加provision失败
                [weakSelf showAndSaveLog:@"[End] provision fail"];
                [weakSelf failAction];
            }];
        }
    } finish:^{
        if (!scanSuccess) {
//            TeLog(@"scan finish.");
            [weakSelf showAndSaveLog:@"[End] scan timeout 10s."];
            //扫描失败，添加失败
            [weakSelf failAction];
        }
    }];
}

- (void)failAction {
    _currentFailCount++;
    [self refreshShowLabel];
    [Bluetooth.share.commandHandle stopScan];
    [Bluetooth.share stopAutoConnect];
    __weak typeof(self) weakSelf = self;
    [Bluetooth.share cancelAllConnecttionWithComplete:^{
        [weakSelf enterInitMeshInfoModel];
    }];
}

- (BOOL)checkMacTF{
    if (self.macTF.text.length != 12 && self.macTF.text.length != 0) {
        return NO;
    }
    self.macString = self.macTF.text.uppercaseString;
    return YES;
}

- (BOOL)checkTestCountTF{
    if (self.testCountTF.text.length == 0) {
        return NO;
    }
    self.testCount = (UInt32)self.testCountTF.text.integerValue;
    return YES;
}

- (BOOL)checkDelayOfDeleteTF{
    if (self.delayOfDeleteTF.text.length == 0) {
        return NO;
    }
    self.delayOfDelete = 10 + (UInt32)self.delayOfDeleteTF.text.integerValue;
    return YES;
}

- (void)refreshShowLabel{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        weakSelf.testResultLabel.text = [weakSelf getResultString];
    });
}

- (NSString *)getResultString {
    double scanT = (_averageScanTime*_currentSuccessCount+_currentScanTime)/(_currentSuccessCount+(_currentScanTime==0?0:1));
    double connectT = (_averageConnectTime*_currentSuccessCount+_currentConnectTime)/(_currentSuccessCount+(_currentConnectTime==0?0:1));
    double provisionT = (_averageProvisionTime*_currentSuccessCount+_currentProvisionTime)/(_currentSuccessCount+(_currentProvisionTime==0?0:1));
    double keybindT = (_averageKeybindTime*_currentSuccessCount+_currentKeybindTime)/(_currentSuccessCount+(_currentKeybindTime==0?0:1));
    NSString *str = [NSString stringWithFormat:@"success:%d fail:%d precent:%.f%% kickout fail:%d\nscanT:%0.2fs connectT:%0.2fs provisionT:%0.2fs keybindT:%0.2fs allT:%0.2fs",_currentSuccessCount,_currentFailCount,100*(((double)_currentSuccessCount)/(_currentSuccessCount+_currentFailCount)),_currentKickoutFailCount,scanT,connectT,provisionT,keybindT,_averageScanTime+_averageConnectTime+_averageProvisionTime+_averageKeybindTime];
    if (_currentScanTime != 0 && _currentConnectTime != 0 && _currentProvisionTime != 0 && _currentKeybindTime != 0) {
        _averageScanTime = scanT;
        _averageConnectTime = connectT;
        _averageProvisionTime = provisionT;
        _averageKeybindTime = keybindT;
        _currentSuccessCount ++;
        _currentScanTime = 0;
        _currentConnectTime = 0;
        _currentProvisionTime = 0;
        _currentKeybindTime = 0;
    }
    return str;
}

- (void)startTest{
    [self showAndSaveLog:[NSString stringWithFormat:@"[Start test] mac:%@,count=%u\n",_macString,(unsigned int)_testCount]];
    [self enterInitMeshInfoModel];
}

- (void)stopTest{
    [Bluetooth.share.commandHandle stopScan];
    [Bluetooth.share stopAutoConnect];
    NSString *str = [NSString stringWithFormat:@"[Stop test] %@\n\n",[self getResultString]];
    [self showAndSaveLog:str];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(deleteDeviceTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(enterInitMeshInfoModel) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(deleteDevice) object:nil];
    });
}

- (void)showAndSaveLog:(NSString *)log{
    [self showLog:log];
    saveTestLogData(log);
}

- (void)showLog:(NSString *)log{
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";
    dformatter.locale = [[NSLocale alloc] initWithLocaleIdentifier:@"zh_CN"];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.logTV.text = [self.logTV.text stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],log]];
        [self.logTV scrollRangeToVisible:NSMakeRange(self.logTV.text.length, 1)];
    });
}

- (void)enterInitMeshInfoModel{
    if (_testCount != 0 && _testCount <= _currentCount) {
        //测试完成
        [self.controlButton setTitle:@"Start" forState:UIControlStateNormal];
        [self stopTest];
        [self showAndSaveLog:@"\nEnd\n"];
    }else{
        //测试未完成，继续
        self.currentCount ++;
        self.currentScanTime = 0;
        self.currentConnectTime = 0;
        self.currentProvisionTime = 0;
        self.currentKeybindTime = 0;
        self.time = [[NSDate date] timeIntervalSince1970];

        [self refreshShowLabel];

        [self scanAndConnnectAndProvisionAndKeybind];
    }
}

- (void)deleteDevice{
    [self showAndSaveLog:@"[Start] delete device"];
    __weak typeof(self) weakSelf = self;
    [Bluetooth.share setNormalState];
    [Bluetooth.share.commandHandle kickoutDevice:provisionAddress complete:^{
        [weakSelf showAndSaveLog:@"[End] delete device success\n"];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(deleteDeviceTimeout) object:nil];
            [SigDataSource.share removeModelWithDeviceAddress:provisionAddress];
            //删除成功，延时后进入下一轮
            [weakSelf performSelector:@selector(enterInitMeshInfoModel) withObject:nil afterDelay:weakSelf.delayOfDelete];
        });
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(deleteDeviceTimeout) object:nil];
        [self performSelector:@selector(deleteDeviceTimeout) withObject:nil afterDelay:5.0];
    });
}

- (void)deleteDeviceTimeout{
    self.currentFailCount ++;
    [SigDataSource.share removeModelWithDeviceAddress:provisionAddress];
    [self showAndSaveLog:@"[End] delete device timeout\n"];
    [Bluetooth.share cancelAllConnecttionWithComplete:^{
        
    }];
    //删除模式时，表示删除超时。10+n秒后开始添加操作。
    dispatch_async(dispatch_get_main_queue(), ^{
        [self refreshShowLabel];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(deleteDevice) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(enterInitMeshInfoModel) object:nil];
        [self performSelector:@selector(enterInitMeshInfoModel) withObject:nil afterDelay:self.delayOfDelete];
    });
}

-(void)dealloc{
    NSLog(@"%s",__func__);
}

@end
