/********************************************************************************************************
 * @file     PrivateBeaconVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/27
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "PrivateBeaconVC.h"
#import "PrivateBeaconCell.h"

@interface PrivateBeaconVC ()
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (strong, nonatomic) NSArray <NSString *>*dataArray;
@property (nonatomic, assign) BOOL nodeIdentityEnable;
@property (nonatomic, assign) BOOL privateNodeIdentityEnable;

@end

@implementation PrivateBeaconVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"Private Beacon";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass(PrivateBeaconCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(PrivateBeaconCell.class)];
    _nodeIdentityEnable = NO;
    _privateNodeIdentityEnable = NO;
    [self refreshSourceAndUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)refreshSourceAndUI {
    self.dataArray = @[@"Config GATT Proxy", @"Private GATT Proxy", @"Config Node Identity", @"Private Node Identity", @"Config Beacon", @"Private Beacon"];
    [self.tableView reloadData];
}

- (void)startTimerForNodeIdentity {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(nodeIdentityTimerAction) object:nil];
        [self performSelector:@selector(nodeIdentityTimerAction) withObject:nil afterDelay:60];
    });
}

- (void)startTimerForPrivateNodeIdentity {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(privateNodeIdentityTimerAction) object:nil];
        [self performSelector:@selector(privateNodeIdentityTimerAction) withObject:nil afterDelay:60];
    });
}

- (void)nodeIdentityTimerAction {
    _nodeIdentityEnable = NO;
    [self refreshSourceAndUI];
}

- (void)privateNodeIdentityTimerAction {
    _privateNodeIdentityEnable = NO;
    [self refreshSourceAndUI];
}

#pragma mark - UITableViewDataSource,UITableViewDelegate

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataArray.count / 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    PrivateBeaconCell *cell = (PrivateBeaconCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(PrivateBeaconCell.class) forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.firstTitleLabel.text = self.dataArray[indexPath.row * 2];
    cell.secondTitleLabel.text = self.dataArray[indexPath.row * 2 + 1];
    cell.firstSwitch.on = NO;
    cell.secondSwitch.on = NO;
    [cell.firstSwitch removeTarget:nil action:nil forControlEvents:UIControlEventValueChanged];
    [cell.secondSwitch removeTarget:nil action:nil forControlEvents:UIControlEventValueChanged];
    if (indexPath.row == 0) {
        cell.firstSwitch.on = [SigDataSource.share getLocalConfigGattProxyStateOfUnicastAddress:self.model.address];
        cell.secondSwitch.on = [SigDataSource.share getLocalPrivateGattProxyStateOfUnicastAddress:self.model.address];
        [cell.firstSwitch addTarget:self action:@selector(clickConfigGattProxySwitch:) forControlEvents:UIControlEventValueChanged];
        [cell.secondSwitch addTarget:self action:@selector(clickPrivateGattProxySwitch:) forControlEvents:UIControlEventValueChanged];
    } else if (indexPath.row == 1) {
        cell.firstSwitch.on = _nodeIdentityEnable;
        cell.secondSwitch.on = _privateNodeIdentityEnable;
        [cell.firstSwitch addTarget:self action:@selector(clickConfigNodeIdentitySwitch:) forControlEvents:UIControlEventValueChanged];
        [cell.secondSwitch addTarget:self action:@selector(clickPrivateNodeIdentitySwitch:) forControlEvents:UIControlEventValueChanged];
    } else if (indexPath.row == 2) {
        cell.firstSwitch.on = [SigDataSource.share getLocalConfigBeaconStateOfUnicastAddress:self.model.address];
        cell.secondSwitch.on = [SigDataSource.share getLocalPrivateBeaconStateOfUnicastAddress:self.model.address];
        [cell.firstSwitch addTarget:self action:@selector(clickConfigBeaconSwitch:) forControlEvents:UIControlEventValueChanged];
        [cell.secondSwitch addTarget:self action:@selector(clickPrivateBeaconSwitch:) forControlEvents:UIControlEventValueChanged];
    }
    return cell;
}

- (void)clickPrivateGattProxySwitch:(UISwitch *)switchButton {
    // Mesh is disconnected,can not send Private GATT Proxy.
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        switchButton.on = !switchButton.on;
        return;
    }
    //1. Private GATT Proxy
    UInt16 des = self.model.address;
    __block BOOL result = NO;
    __weak typeof(self) weakSelf = self;
    SigPrivateGattProxyState privateGattProxy = switchButton.isOn ? SigPrivateGattProxyState_enable : SigPrivateGattProxyState_disable;
    [SDKLibCommand privateGattProxySetWithPrivateGattProxy:privateGattProxy destination:des retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigPrivateGattProxyStatus * _Nonnull responseMessage) {
        if (source == des && responseMessage.privateGattProxy == privateGattProxy) {
            result = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (result) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Private GATT Proxy Success!", switchButton.isOn ? @"Enable" : @"Disable"]];
                [SigDataSource.share setLocalPrivateGattProxyState:switchButton.isOn unicastAddress:des];
            } else {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Private GATT Proxy Fail!", switchButton.isOn ? @"Enable" : @"Disable"]];
                switchButton.on = !switchButton.isOn;
            }
        });
    }];
}

- (void)clickConfigGattProxySwitch:(UISwitch *)switchButton {
    // Mesh is disconnected,can not send Config GATT Proxy.
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        switchButton.on = !switchButton.on;
        return;
    }
    //2. Config GATT Proxy
    UInt16 des = self.model.address;
    __block BOOL result = NO;
    __weak typeof(self) weakSelf = self;
    SigNodeGATTProxyState nodeGATTProxyState = switchButton.isOn ? SigNodeGATTProxyState_enabled : SigNodeGATTProxyState_notEnabled;
    [SDKLibCommand configGATTProxySetWithDestination:des nodeGATTProxyState:nodeGATTProxyState retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigGATTProxyStatus * _Nonnull responseMessage) {
        if (source == des && responseMessage.state == nodeGATTProxyState) {
            result = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (result) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Config GATT Proxy Success!", switchButton.isOn ? @"Enable" : @"Disable"]];
                [SigDataSource.share setLocalConfigGattProxyState:switchButton.isOn unicastAddress:des];
            } else {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Config GATT Proxy Fail!", switchButton.isOn ? @"Enable" : @"Disable"]];
                switchButton.on = !switchButton.isOn;
            }
        });
    }];
}

- (void)clickPrivateNodeIdentitySwitch:(UISwitch *)switchButton {
    // Mesh is disconnected,can not send Private Node Identity.
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        switchButton.on = !switchButton.on;
        return;
    }
    //3. Private Node Identity
    UInt16 des = self.model.address;
    __block BOOL result = NO;
    __weak typeof(self) weakSelf = self;
    if (switchButton.on) {
        [self startTimerForPrivateNodeIdentity];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(privateNodeIdentityTimerAction) object:nil];
        });
    }
    SigPrivateNodeIdentityState privateIdentity = switchButton.isOn ? SigPrivateNodeIdentityState_enabled : SigPrivateNodeIdentityState_notEnabled;
    //1. Private Node Identity
    [SDKLibCommand privateNodeIdentitySetWithNetKeyIndex:SigDataSource.share.curNetkeyModel.index privateIdentity:privateIdentity destination:des retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigPrivateNodeIdentityStatus * _Nonnull responseMessage) {
        if (source == des && responseMessage.status == SigConfigMessageStatus_success && responseMessage.netKeyIndex == SigDataSource.share.curNetkeyModel.index && responseMessage.privateIdentity == privateIdentity) {
            result = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (result) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Private Node Identity Success!", switchButton.isOn ? @"Enable" : @"Disable"]];
            } else {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Private Node Identity Fail!", switchButton.isOn ? @"Enable" : @"Disable"]];
                switchButton.on = !switchButton.isOn;
            }
        });
    }];
}

- (void)clickConfigNodeIdentitySwitch:(UISwitch *)switchButton {
    // Mesh is disconnected,can not send Config Node Identity.
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        switchButton.on = !switchButton.on;
        return;
    }
    //4. Config Node Identity
    UInt16 des = self.model.address;
    __block BOOL result = NO;
    __weak typeof(self) weakSelf = self;
    if (switchButton.on) {
        [self startTimerForNodeIdentity];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(nodeIdentityTimerAction) object:nil];
        });
    }
    SigNodeIdentityState identity = switchButton.isOn ? SigNodeIdentityState_enabled : SigNodeIdentityState_notEnabled;
    [SDKLibCommand configNodeIdentitySetWithDestination:des netKeyIndex:SigDataSource.share.curNetkeyModel.index identity:identity retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeIdentityStatus * _Nonnull responseMessage) {
        if (source == des && responseMessage.status == SigConfigMessageStatus_success && responseMessage.netKeyIndex == SigDataSource.share.curNetkeyModel.index && responseMessage.identity == identity) {
            result = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (result) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Config Node Identity Success!", switchButton.isOn ? @"Enable" : @"Disable"]];
            } else {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Config Node Identity Fail!", switchButton.isOn ? @"Enable" : @"Disable"]];
                switchButton.on = !switchButton.isOn;
            }
        });
    }];
}

- (void)clickPrivateBeaconSwitch:(UISwitch *)switchButton {
    // Mesh is disconnected,can not send Private Beacon.
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        switchButton.on = !switchButton.on;
        return;
    }
    //5. Private Beacon
    UInt16 des = self.model.address;
    __block BOOL result = NO;
    __weak typeof(self) weakSelf = self;
    SigPrivateBeaconState privateBeacon = switchButton.isOn ? SigPrivateBeaconState_enable : SigPrivateBeaconState_disable;
    [SDKLibCommand privateBeaconSetWithPrivateBeacon:privateBeacon destination:des retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigPrivateBeaconStatus * _Nonnull responseMessage) {
        if (source == des && responseMessage.privateBeacon == privateBeacon) {
            result = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (result) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Private Beacon Success!", switchButton.isOn ? @"Enable" : @"Disable"]];
                [SigDataSource.share setLocalPrivateBeaconState:switchButton.isOn unicastAddress:des];
            } else {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Private Beacon Fail!", switchButton.isOn ? @"Enable" : @"Disable"]];
                switchButton.on = !switchButton.isOn;
            }
        });
    }];
}

- (void)clickConfigBeaconSwitch:(UISwitch *)switchButton {
    // Mesh is disconnected,can not send Config Beacon.
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        switchButton.on = !switchButton.on;
        return;
    }
    //2. Config Beacon
    UInt16 des = self.model.address;
    __block BOOL result = NO;
    __weak typeof(self) weakSelf = self;
    SigSecureNetworkBeaconState secureNetworkBeaconState = switchButton.isOn ? SigSecureNetworkBeaconState_open : SigSecureNetworkBeaconState_close;
    [SDKLibCommand configBeaconSetWithDestination:des secureNetworkBeaconState:secureNetworkBeaconState retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigBeaconStatus * _Nonnull responseMessage) {
        if (source == des && responseMessage.isEnabled == secureNetworkBeaconState) {
            result = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (result) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Config Beacon Success!", switchButton.isOn ? @"Enable" : @"Disable"]];
                [SigDataSource.share setLocalConfigBeaconState:switchButton.isOn unicastAddress:des];
            } else {
                [weakSelf showTips:[NSString stringWithFormat:@"%@ Config Beacon Fail!", switchButton.isOn ? @"Enable" : @"Disable"]];
                switchButton.on = !switchButton.isOn;
            }
        });
    }];
}

@end
