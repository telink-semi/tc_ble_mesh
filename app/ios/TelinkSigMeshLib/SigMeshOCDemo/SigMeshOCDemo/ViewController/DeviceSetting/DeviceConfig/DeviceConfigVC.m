/********************************************************************************************************
 * @file     DeviceConfigVC.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/9/15
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

#import "DeviceConfigVC.h"
#import "UIViewController+Message.h"
#import "DeviceConfigCell.h"
#import "UIButton+extension.h"
#import "CustomAlertView.h"

@interface ShowModel : NSObject
/** 是否展开 */
@property (nonatomic, assign) BOOL isExpand;
@property (strong, nonatomic) NSString *title;
@property (strong, nonatomic) NSString *detail;
@property (strong, nonatomic) NSString *value;
- (instancetype)initWithTitle:(NSString *)title detail:(NSString *)detail value:(NSString *)value;
@end
@implementation ShowModel
- (instancetype)initWithTitle:(NSString *)title detail:(NSString *)detail value:(NSString *)value {
    if (self = [super init]) {
        _title = title;
        _detail = detail;
        _value = value;
    }
    return self;
}
@end

@interface DeviceConfigVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ShowModel *>*dataArray;

@end

@implementation DeviceConfigVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"Device Config";
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([DeviceConfigCell class]) bundle:nil] forCellReuseIdentifier:@"DeviceConfigCell"];
    self.dataArray = [NSMutableArray array];
    [self refreshAllDeviceConfigValue];
}

- (void)refreshAllDeviceConfigValue {
    ShowModel *m1 = [[ShowModel alloc] initWithTitle:@"Default TTL" detail:@"The Default TTL state determines the TTL value used when sending messages." value:[NSString stringWithFormat:@"value:0x%X",self.model.defaultTTL]];
    ShowModel *m2 = [[ShowModel alloc] initWithTitle:@"Relay & RelayRetransmit" detail:@"The Relay state indicates support for the Relay feature; The Relay Retransmit state is a composite state that controls parameters of retransmission of the Network PDU relayed by the node." value:[NSString stringWithFormat:@"value:%@\nretransmit count: %ld\nretransmit interval steps: %ld",[SigHelper.share getDetailOfSigNodeFeaturesState:self.model.features.relayFeature],(long)self.model.relayRetransmit.relayRetransmitCount,(long)self.model.relayRetransmit.relayRetransmitIntervalSteps]];
    ShowModel *m3 = [[ShowModel alloc] initWithTitle:@"Secure Network Beacon" detail:@"The Secure Network Beacon state determines if a node is periodically broadcasting Secure Network beacon messages." value:[NSString stringWithFormat:@"value:%@",self.model.secureNetworkBeacon?@"opened":@"closed"]];
    ShowModel *m4 = [[ShowModel alloc] initWithTitle:@"GATT Proxy" detail:@"The GATT Proxy state indicates if the Mesh Proxy Service is supported, and if supported, it indicates and controls the status of the Mesh Proxy Service." value:[NSString stringWithFormat:@"value:%@",[SigHelper.share getDetailOfSigNodeFeaturesState:self.model.features.proxyFeature]]];
    ShowModel *m5 = [[ShowModel alloc] initWithTitle:@"Node Identity" detail:@"The Node Identity state determines if a node that supports the Mesh Proxy Service is advertising on a subnet using Node Identity messages." value:@"value:stopped"];
    ShowModel *m6 = [[ShowModel alloc] initWithTitle:@"Friend" detail:@"The Friend state indicates support for the Friend feature. If Friend feature is supported, then this also indicates and controls whether Friend feature is enabled or disabled." value:[NSString stringWithFormat:@"value:%@",[SigHelper.share getDetailOfSigNodeFeaturesState:self.model.features.friendFeature]]];
    ShowModel *m7 = [[ShowModel alloc] initWithTitle:@"Key Refresh Phase" detail:@"The Key Refresh Phase state indicates and controls the Key Refresh procedure for each NetKey in the NetKey List." value:[NSString stringWithFormat:@"value: phase: %@",[SigHelper.share getDetailOfKeyRefreshPhase:3]]];
    ShowModel *m8 = [[ShowModel alloc] initWithTitle:@"Network Transmit" detail:@"The Network Transmit state is a composite state that controls the number and timing of the transmissions of Network PDU originating from a node." value:[NSString stringWithFormat:@"value:%@\nretransmit count: %ld\nretransmit interval steps: %ld",@"",(long)self.model.networkTransmit.networkTransmitCount,(long)self.model.networkTransmit.networkTransmitIntervalSteps]];

    [self.dataArray addObject:m1];
    [self.dataArray addObject:m2];
    [self.dataArray addObject:m3];
    [self.dataArray addObject:m4];
    [self.dataArray addObject:m5];
    [self.dataArray addObject:m6];
    [self.dataArray addObject:m7];
    [self.dataArray addObject:m8];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return self.dataArray.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (self.dataArray[section].isExpand) {
        return 1;
    } else {
        return 0;
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    if (section==0) {
        return 60;
    }
    return 50.0f;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    UIView *headView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, SCREENWIDTH, 50)];
    UIControl *backView = [[UIControl alloc] initWithFrame:CGRectMake(15, section==0?10:0, SCREENWIDTH - 30, 50)];
    UIImageView *turnImageView = [[UIImageView alloc] initWithFrame:CGRectMake(SCREENWIDTH - 50, 21, 12, 7)];
    turnImageView.image = [[UIImage imageNamed:@"fb_bottom"] imageWithRenderingMode:1];
    [backView addSubview:turnImageView];
    
    backView.tag = 1000 + section;
    headView.backgroundColor = [UIColor clearColor];
    backView.backgroundColor = [UIColor whiteColor];
    
    UILabel *titlelabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 0, SCREENWIDTH - 60, 50)];
    [backView addSubview:titlelabel];
    titlelabel.font = kFont(15);
    titlelabel.text = [NSString stringWithFormat:@"%ld、%@",(long)section + 1,self.dataArray[section].title];
    titlelabel.numberOfLines = 0;
    if (self.dataArray[section].isExpand) {
        turnImageView.image = [[UIImage imageNamed:@"fb_top"] imageWithRenderingMode:1];
        UIBezierPath *maskPath = [UIBezierPath bezierPathWithRoundedRect:backView.bounds byRoundingCorners:UIRectCornerTopLeft|UIRectCornerTopRight cornerRadii:CGSizeMake(6, 6)];
        CAShapeLayer *maskLayer = [[CAShapeLayer alloc] init];
        maskLayer.frame = backView.bounds;
        maskLayer.path = maskPath.CGPath;
        backView.layer.mask = maskLayer;
    }else{
        turnImageView.image = [[UIImage imageNamed:@"fb_bottom"] imageWithRenderingMode:1];
        UIBezierPath *maskPath = [UIBezierPath bezierPathWithRoundedRect:backView.bounds byRoundingCorners:UIRectCornerAllCorners cornerRadii:CGSizeMake(6, 6)];
        CAShapeLayer *maskLayer = [[CAShapeLayer alloc] init];
        maskLayer.frame = backView.bounds;
        maskLayer.path = maskPath.CGPath;
        backView.layer.mask = maskLayer;
    }
    [headView addSubview:backView];
    [backView addTarget:self action:@selector(didClickedSection:) forControlEvents:(UIControlEventTouchUpInside)];
    return headView;
}

- (void)didClickedSection:(UIControl *)view {
    NSInteger i = view.tag - 1000;
    self.dataArray[i].isExpand = !self.dataArray[i].isExpand;
    NSIndexSet *index = [NSIndexSet indexSetWithIndex:i];
    [_tableView reloadSections:index withRowAnimation:(UITableViewRowAnimationAutomatic)];
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return 10;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    DeviceConfigCell *cell = [tableView dequeueReusableCellWithIdentifier:@"DeviceConfigCell"];
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    ShowModel *m = self.dataArray[indexPath.section];
    cell.titleLabel.text = m.detail;
    cell.valueLabel.text = m.value;
    cell.setButton.hidden = indexPath.section == 6;
    __weak typeof(self) weakSelf = self;
    [cell.getButton addAction:^(UIButton *button) {
        [weakSelf clickGetWithIndexPath:indexPath];
    }];
    [cell.setButton addAction:^(UIButton *button) {
        [weakSelf clickSetWithIndexPath:indexPath];
    }];
    return cell;
}

- (void)clickGetWithIndexPath:(NSIndexPath *)indexPath {
    if (!SigBearer.share.isOpen || SigDataSource.share.unicastAddressOfConnected == 0) {
        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Mesh is outline!"]];
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:1.0];
        });
        return;
    }
    ShowModel *m = self.dataArray[indexPath.section];
    __weak typeof(self) weakSelf = self;
    if (indexPath.section == 0) {
        [ShowTipsHandle.share show:@"Get Default TTL..."];
        //Get Default TTL
        [SDKLibCommand configDefaultTtlGetWithDestination:weakSelf.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigDefaultTtlStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configDefaultTtlGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigDefaultTtlStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Default TTL failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Default TTL successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 1) {
        [ShowTipsHandle.share show:@"Get Relay & RelayRetransmit..."];
        //Get Relay & RelayRetransmit
        [SDKLibCommand configRelayGetWithDestination:weakSelf.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigRelayStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configRelayGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigRelayStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Relay & RelayRetransmit failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Relay & RelayRetransmit successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 2) {
        [ShowTipsHandle.share show:@"Get Secure Network Beacon..."];
        //Get Secure Network Beacon
        [SDKLibCommand configBeaconGetWithDestination:weakSelf.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigBeaconStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configBeaconGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigBeaconStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Secure Network Beacon failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Secure Network Beacon successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 3) {
        [ShowTipsHandle.share show:@"Get GATT Proxy..."];
        //Get GATT Proxy
        [SDKLibCommand configGATTProxyGetWithDestination:weakSelf.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigGATTProxyStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configGATTProxyGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigGATTProxyStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get GATT Proxy failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get GATT Proxy successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 4) {
        [ShowTipsHandle.share show:@"Get Node Identity..."];
        //Get Node Identity
        [SDKLibCommand configNodeIdentityGetWithDestination:weakSelf.model.address netKeyIndex:SigDataSource.share.curNetkeyModel.index retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeIdentityStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configNodeIdentityGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigNodeIdentityStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Node Identity failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Node Identity successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 5) {
        [ShowTipsHandle.share show:@"Get Friend..."];
        //Get Friend
        [SDKLibCommand configFriendGetWithDestination:weakSelf.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigFriendStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configFriendGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigFriendStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Friend failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Friend successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 6) {
        [ShowTipsHandle.share show:@"Get Key Refresh Phase..."];
       //Get Key Refresh Phase
        [SDKLibCommand configKeyRefreshPhaseGetWithDestination:weakSelf.model.address netKeyIndex:SigDataSource.share.curNetkeyModel.index retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigKeyRefreshPhaseStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configKeyRefreshPhaseGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                m.value = [NSString stringWithFormat:@"value: phase: %@",[SigHelper.share getDetailOfKeyRefreshPhase:responseMessage.phase]];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
                });
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Key Refresh Phase failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Key Refresh Phase successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    } else if (indexPath.section == 7) {
        [ShowTipsHandle.share show:@"Get Network Transmit..."];
        //Get Network Transmit
        [SDKLibCommand configNetworkTransmitGetWithDestination:weakSelf.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNetworkTransmitStatus * _Nonnull responseMessage) {
            TeLogInfo(@"configNetworkTransmitGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (weakSelf.model.address == source) {
                [weakSelf refreshUIWithSigConfigNetworkTransmitStatus:responseMessage andIndexPath:indexPath];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"Get Network Transmit failed! error=%@",error]];
            } else {
                [ShowTipsHandle.share show:@"Get Network Transmit successed!"];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:1.0];
            });
        }];
    }
}

- (void)clickSetWithIndexPath:(NSIndexPath *)indexPath {
    if (!SigBearer.share.isOpen || SigDataSource.share.unicastAddressOfConnected == 0) {
        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Mesh is outline!"]];
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:1.0];
        });
        return;
    }
    __weak typeof(self) weakSelf = self;
    if (indexPath.section == 0) {
        //Set Default TTL
        AlertItemModel *item = [[AlertItemModel alloc] init];
        item.itemType = ItemType_Input;
        item.headerString = @"TTL:";
        item.defaultString = [NSString stringWithFormat:@"%d",weakSelf.model.defaultTTL];
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Default TTL" detail:@"input new value" itemArray:@[item] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set Default TTL..."];
                NSString *ttlString = [alertView getTextFieldOfRow:0].text;
                [SDKLibCommand configDefaultTtlSetWithDestination:weakSelf.model.address ttl:ttlString.intValue retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigDefaultTtlStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configDefaultTtlSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigDefaultTtlStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Default TTL failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set Default TTL successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    } else if (indexPath.section == 1) {
        //Set Relay & RelayRetransmit
        AlertItemModel *item1 = [[AlertItemModel alloc] init];
        item1.itemType = ItemType_Choose;
        item1.headerString = @"Relay:";
        item1.defaultString = [SigHelper.share getDetailOfSigNodeFeaturesState:weakSelf.model.features.relayFeature];
        NSMutableArray *mArray = [NSMutableArray array];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeFeaturesState:SigNodeFeaturesState_notEnabled]];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeFeaturesState:SigNodeFeaturesState_enabled]];
        item1.chooseItemsArray = mArray;
        AlertItemModel *item2 = [[AlertItemModel alloc] init];
        item2.itemType = ItemType_Input;
        item2.headerString = @"RelayRetransmit Count(3 bits):";
        item2.defaultString = [NSString stringWithFormat:@"%ld",(long)weakSelf.model.relayRetransmit.relayRetransmitCount];
        AlertItemModel *item3 = [[AlertItemModel alloc] init];
        item3.itemType = ItemType_Input;
        item3.headerString = @"RelayRetransmit Interval Steps(5 bits):";
        item3.defaultString = [NSString stringWithFormat:@"%ld",(long)weakSelf.model.relayRetransmit.relayRetransmitIntervalSteps];
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Relay & RelayRetransmit" detail:@"input new value" itemArray:@[item1,item2,item3] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set Relay & RelayRetransmit..."];
                SigNodeRelayState relayState = [alertView getSelectLeftOfRow:0]?SigNodeRelayState_notEnabled:SigNodeRelayState_enabled;
                NSString *networkTransmitCountString = [alertView getTextFieldOfRow:1].text;
                NSString *networkTransmitIntervalStepsString = [alertView getTextFieldOfRow:2].text;
                [SDKLibCommand configRelaySetWithDestination:weakSelf.model.address relay:relayState networkTransmitCount:networkTransmitCountString.intValue networkTransmitIntervalSteps:networkTransmitIntervalStepsString.intValue retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigRelayStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configRelaySet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigRelayStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Relay & RelayRetransmit failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set Relay & RelayRetransmit successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    } else if (indexPath.section == 2) {
        //Set Secure Network Beacon
        AlertItemModel *item1 = [[AlertItemModel alloc] init];
        item1.itemType = ItemType_Choose;
        item1.headerString = @"Proxy:";
        item1.defaultString = self.model.secureNetworkBeacon?@"opened":@"closed";
        NSMutableArray *mArray = [NSMutableArray array];
        [mArray addObject:@"opened"];
        [mArray addObject:@"closed"];
        item1.chooseItemsArray = mArray;
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Secure Network Beacon" detail:@"input new value" itemArray:@[item1] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set Secure Network Beacon..."];
                SigSecureNetworkBeaconState secureNetworkBeaconState = [alertView getSelectLeftOfRow:0]?SigSecureNetworkBeaconState_open:SigSecureNetworkBeaconState_close;
                [SDKLibCommand configBeaconSetWithDestination:weakSelf.model.address secureNetworkBeaconState:secureNetworkBeaconState retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigBeaconStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configBeaconSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigBeaconStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Secure Network Beacon failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set Secure Network Beacon successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    } else if (indexPath.section == 3) {
        //Set GATT Proxy
        AlertItemModel *item1 = [[AlertItemModel alloc] init];
        item1.itemType = ItemType_Choose;
        item1.headerString = @"Proxy:";
        item1.defaultString = [SigHelper.share getDetailOfSigNodeFeaturesState:weakSelf.model.features.proxyFeature];
        NSMutableArray *mArray = [NSMutableArray array];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeFeaturesState:SigNodeFeaturesState_notEnabled]];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeFeaturesState:SigNodeFeaturesState_enabled]];
        item1.chooseItemsArray = mArray;
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set GATT Proxy" detail:@"input new value" itemArray:@[item1] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set GATT Proxy..."];
                SigNodeGATTProxyState nodeGATTProxyState = [alertView getSelectLeftOfRow:0]?SigNodeGATTProxyState_notEnabled:SigNodeGATTProxyState_enabled;
                [SDKLibCommand configGATTProxySetWithDestination:weakSelf.model.address nodeGATTProxyState:nodeGATTProxyState retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigGATTProxyStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configGATTProxySet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigGATTProxyStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set GATT Proxy failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set GATT Proxy successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    } else if (indexPath.section == 4) {
        //Set Node Identity
        AlertItemModel *item1 = [[AlertItemModel alloc] init];
        item1.itemType = ItemType_Choose;
        item1.headerString = @"Proxy:";
        item1.defaultString = [SigHelper.share getDetailOfSigNodeIdentityState:SigNodeIdentityState_notEnabled];
        NSMutableArray *mArray = [NSMutableArray array];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeIdentityState:SigNodeIdentityState_notEnabled]];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeIdentityState:SigNodeIdentityState_enabled]];
        item1.chooseItemsArray = mArray;
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Node Identity" detail:@"input new value" itemArray:@[item1] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set Node Identity..."];
                SigNodeIdentityState identity = [alertView getSelectLeftOfRow:0]?SigNodeIdentityState_notEnabled:SigNodeIdentityState_enabled;
                [SDKLibCommand configNodeIdentitySetWithDestination:weakSelf.model.address netKeyIndex:SigDataSource.share.curNetkeyModel.index identity:identity retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeIdentityStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configNodeIdentitySet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigNodeIdentityStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Node Identity failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set Node Identity successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    } else if (indexPath.section == 5) {
        //Set Friend
        AlertItemModel *item1 = [[AlertItemModel alloc] init];
        item1.itemType = ItemType_Choose;
        item1.headerString = @"Friend:";
        item1.defaultString = [SigHelper.share getDetailOfSigNodeFeaturesState:weakSelf.model.features.friendFeature];
        NSMutableArray *mArray = [NSMutableArray array];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeFeaturesState:SigNodeFeaturesState_notEnabled]];
        [mArray addObject:[SigHelper.share getDetailOfSigNodeFeaturesState:SigNodeFeaturesState_enabled]];
        item1.chooseItemsArray = mArray;
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Friend" detail:@"input new value" itemArray:@[item1] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set Friend..."];
                SigNodeFeaturesState nodeFeaturesState = [alertView getSelectLeftOfRow:0]?SigNodeFeaturesState_notEnabled:SigNodeFeaturesState_enabled;
                [SDKLibCommand configFriendSetWithDestination:weakSelf.model.address nodeFeaturesState:nodeFeaturesState retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigFriendStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configFriendSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigFriendStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Friend failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set Friend successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];                
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    } else if (indexPath.section == 7) {
        //Set Network Transmit
        AlertItemModel *item1 = [[AlertItemModel alloc] init];
        item1.itemType = ItemType_Input;
        item1.headerString = @"Transmit Count(3 bits):";
        item1.defaultString = [NSString stringWithFormat:@"%ld",(long)weakSelf.model.networkTransmit.networkTransmitCount];
        AlertItemModel *item2 = [[AlertItemModel alloc] init];
        item2.itemType = ItemType_Input;
        item2.headerString = @"Transmit Interval Steps(5 bits):";
        item2.defaultString = [NSString stringWithFormat:@"%ld",(long)weakSelf.model.networkTransmit.networkTransmitIntervalSteps];
        CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Network Transmit" detail:@"input new value" itemArray:@[item1,item2] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
            if (isConfirm) {
                //CONFIRM
                [ShowTipsHandle.share show:@"Set Network Transmit..."];
                NSString *networkTransmitCountString = [alertView getTextFieldOfRow:0].text;
                NSString *networkTransmitIntervalStepsString = [alertView getTextFieldOfRow:1].text;
                [SDKLibCommand configNetworkTransmitSetWithDestination:weakSelf.model.address networkTransmitCount:networkTransmitCountString.intValue networkTransmitIntervalSteps:networkTransmitIntervalStepsString.intValue retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNetworkTransmitStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configDefaultTtlSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (weakSelf.model.address == source) {
                        [weakSelf refreshUIWithSigConfigNetworkTransmitStatus:responseMessage andIndexPath:indexPath];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"Set Relay & RelayRetransmit failed! error=%@",error]];
                    } else {
                        [ShowTipsHandle.share show:@"Set Relay & RelayRetransmit successed!"];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share delayHidden:1.0];
                    });
                }];
            } else {
                //cancel
            }
        }];
        [customAlertView showCustomAlertView];
    }
}


- (void)refreshUIWithSigConfigDefaultTtlStatus:(SigConfigDefaultTtlStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    self.model.defaultTTL = responseMessage.ttl;
    [SigDataSource.share saveLocationData];
    m.value = [NSString stringWithFormat:@"value:0x%X",self.model.defaultTTL];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

- (void)refreshUIWithSigConfigRelayStatus:(SigConfigRelayStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    self.model.features.relayFeature = responseMessage.state;
    self.model.relayRetransmit.relayRetransmitCount = responseMessage.count;
    self.model.relayRetransmit.relayRetransmitIntervalSteps = responseMessage.steps;
    [SigDataSource.share saveLocationData];
    m.value = [NSString stringWithFormat:@"value:%@\nretransmit count: %ld\nretransmit interval steps: %ld",[SigHelper.share getDetailOfSigNodeFeaturesState:self.model.features.relayFeature],(long)self.model.relayRetransmit.relayRetransmitCount,(long)self.model.relayRetransmit.relayRetransmitIntervalSteps];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

- (void)refreshUIWithSigConfigBeaconStatus:(SigConfigBeaconStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    self.model.secureNetworkBeacon = responseMessage.isEnabled;
    [SigDataSource.share saveLocationData];
    m.value = [NSString stringWithFormat:@"value:%@",self.model.secureNetworkBeacon?@"opened":@"closed"];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

- (void)refreshUIWithSigConfigGATTProxyStatus:(SigConfigGATTProxyStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    self.model.features.proxyFeature = responseMessage.state;
    [SigDataSource.share saveLocationData];
    m.value = [NSString stringWithFormat:@"value:%@",[SigHelper.share getDetailOfSigNodeFeaturesState:self.model.features.proxyFeature]];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

- (void)refreshUIWithSigConfigNodeIdentityStatus:(SigConfigNodeIdentityStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    m.value = [NSString stringWithFormat:@"value:%@",[SigHelper.share getDetailOfSigNodeIdentityState:responseMessage.identity]];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

- (void)refreshUIWithSigConfigFriendStatus:(SigConfigFriendStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    self.model.features.friendFeature = responseMessage.state;
    [SigDataSource.share saveLocationData];
    m.value = [NSString stringWithFormat:@"value:%@",[SigHelper.share getDetailOfSigNodeFeaturesState:self.model.features.friendFeature]];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

- (void)refreshUIWithSigConfigNetworkTransmitStatus:(SigConfigNetworkTransmitStatus *)responseMessage andIndexPath:(NSIndexPath *)indexPath {
    ShowModel *m = self.dataArray[indexPath.section];
    self.model.networkTransmit.networkTransmitCount = responseMessage.count;
    self.model.networkTransmit.networkTransmitIntervalSteps = responseMessage.steps;
    [SigDataSource.share saveLocationData];
    m.value = [NSString stringWithFormat:@"value:%@\nretransmit count: %ld\nretransmit interval steps: %ld",@"",(long)self.model.networkTransmit.networkTransmitCount,(long)self.model.networkTransmit.networkTransmitIntervalSteps];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationAutomatic];
    });
}

/** 设置分区圆角 */
- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    //    if ([cell isKindOfClass:[LZRowTableViewCell class]]) {
    
    // 圆角弧度半径
    CGFloat cornerRadius = 6.f;
    // 设置cell的背景色为透明，如果不设置这个的话，则原来的背景色不会被覆盖
    cell.backgroundColor = UIColor.clearColor;
    
    // 创建一个shapeLayer
    CAShapeLayer *layer = [[CAShapeLayer alloc] init];
    CAShapeLayer *backgroundLayer = [[CAShapeLayer alloc] init]; //显示选中
    // 创建一个可变的图像Path句柄，该路径用于保存绘图信息
    CGMutablePathRef pathRef = CGPathCreateMutable();
    // 获取cell的size
    // 第一个参数,是整个 cell 的 bounds, 第二个参数是距左右两端的距离,第三个参数是距上下两端的距离
    CGRect bounds = CGRectInset(cell.bounds, 15, 0);
    
    // CGRectGetMinY：返回对象顶点坐标
    // CGRectGetMaxY：返回对象底点坐标
    // CGRectGetMinX：返回对象左边缘坐标
    // CGRectGetMaxX：返回对象右边缘坐标
    // CGRectGetMidX: 返回对象中心点的X坐标
    // CGRectGetMidY: 返回对象中心点的Y坐标
    
    // 这里要判断分组列表中的第一行，每组section的第一行，每组section的中间行
    
    // CGPathAddRoundedRect(pathRef, nil, bounds, cornerRadius, cornerRadius);
    //        if ([tableView numberOfRowsInSection:indexPath.section] == 1) {
    //            CGPathMoveToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds));
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMinY(bounds), CGRectGetMidX(bounds), CGRectGetMinY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMinY(bounds), CGRectGetMaxX(bounds), CGRectGetMidY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMaxY(bounds), CGRectGetMidX(bounds), CGRectGetMaxY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds), CGRectGetMinX(bounds), CGRectGetMidY(bounds), cornerRadius);
    //            CGPathAddLineToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds));
    //        }
    //        else if (indexPath.row == 0) {
    //            // 初始起点为cell的左下角坐标
    //            CGPathMoveToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds));
    //            // 起始坐标为左下角，设为p，（CGRectGetMinX(bounds), CGRectGetMinY(bounds)）为左上角的点，设为p1(x1,y1)，(CGRectGetMidX(bounds), CGRectGetMinY(bounds))为顶部中点的点，设为p2(x2,y2)。然后连接p1和p2为一条直线l1，连接初始点p到p1成一条直线l，则在两条直线相交处绘制弧度为r的圆角。
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMinY(bounds), CGRectGetMidX(bounds), CGRectGetMinY(bounds), cornerRadius);
    //            CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMinY(bounds), CGRectGetMaxX(bounds), CGRectGetMidY(bounds), cornerRadius);
    //            // 终点坐标为右下角坐标点，把绘图信息都放到路径中去,根据这些路径就构成了一块区域了
    //            CGPathAddLineToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMaxY(bounds));
    //
    //        }
    
    if (indexPath.row == [tableView numberOfRowsInSection:indexPath.section]-1) {
        // 初始起点为cell的左上角坐标
        CGPathMoveToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMinY(bounds));
        CGPathAddArcToPoint(pathRef, nil, CGRectGetMinX(bounds), CGRectGetMaxY(bounds), CGRectGetMidX(bounds), CGRectGetMaxY(bounds), cornerRadius);
        CGPathAddArcToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMaxY(bounds), CGRectGetMaxX(bounds), CGRectGetMidY(bounds), cornerRadius);
        // 添加一条直线，终点坐标为右下角坐标点并放到路径中去
        CGPathAddLineToPoint(pathRef, nil, CGRectGetMaxX(bounds), CGRectGetMinY(bounds));
    } else {
        // 添加cell的rectangle信息到path中（不包括圆角）
        CGPathAddRect(pathRef, nil, bounds);
    }
    // 把已经绘制好的可变图像路径赋值给图层，然后图层根据这图像path进行图像渲染render
    layer.path = pathRef;
    backgroundLayer.path = pathRef;
    // 注意：但凡通过Quartz2D中带有creat/copy/retain方法创建出来的值都必须要释放
    CFRelease(pathRef);
    // 按照shape layer的path填充颜色，类似于渲染render
    // layer.fillColor = [UIColor colorWithWhite:1.f alpha:0.8f].CGColor;
    layer.fillColor = [UIColor whiteColor].CGColor;
    
    // view大小与cell一致
    UIView *roundView = [[UIView alloc] initWithFrame:bounds];
    // 添加自定义圆角后的图层到roundView中
    [roundView.layer insertSublayer:layer atIndex:0];
    roundView.backgroundColor = UIColor.clearColor;
    // cell的背景view
    cell.backgroundView = roundView;
    
    // 以上方法存在缺陷当点击cell时还是出现cell方形效果，因此还需要添加以下方法
    // 如果你 cell 已经取消选中状态的话,那以下方法是不需要的.
    //    UIView *selectedBackgroundView = [[UIView alloc] initWithFrame:bounds];
    //    backgroundLayer.fillColor = [UIColor colorWithRed:0.90 green:0.90 blue:0.90 alpha:1].CGColor;
    //    [selectedBackgroundView.layer insertSublayer:backgroundLayer atIndex:0];
    //    selectedBackgroundView.backgroundColor = UIColor.clearColor;
    //    cell.selectedBackgroundView = selectedBackgroundView;
    //    }
}

@end
