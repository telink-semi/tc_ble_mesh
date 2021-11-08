/********************************************************************************************************
 * @file     DeviceRemoteVC.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/8/10
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

#import "DeviceRemoteVC.h"
#import "UIViewController+Message.h"
#import "UIButton+extension.h"
#import "NSString+extension.h"
#import "RemoteElementCell.h"

@interface DeviceRemoteVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *connectTipButton;

@end

@implementation DeviceRemoteVC

- (IBAction)clickConnectDevice:(UIButton *)sender {
    [self tryConnectRemote];
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;//去掉下划线
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.allowsSelection = NO;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_RemoteElementCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_RemoteElementCellID];
    
    [self tryConnectRemote];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if([self isBeingDismissed] || [self isMovingFromParentViewController]) {
        // pop / dismiss
        // 如果直连了遥控器，需要在首页断开，非遥控器则无需断开，停止自动setNodeIdentity即可。
        if (SigDataSource.share.unicastAddressOfConnected == self.model.address) {
//            [ConnectTools.share stopConnectToolsWithComplete:nil];
        } else {
            TeLogInfo(@"===========endConnectTools");
            [ConnectTools.share endConnectTools];
        }
    } else {
        // push /present from here
    }
}
- (void)tryConnectRemote {
    //判断直连节点，重试直连遥控器
    if (SigBearer.share.isOpen && SigDataSource.share.unicastAddressOfConnected == self.model.address) {
        [self.connectTipButton setTitle:@"device connected." forState:UIControlStateNormal];
    } else {
        [self.connectTipButton setTitle:@"device connecting..." forState:UIControlStateNormal];
        __weak typeof(self) weakSelf = self;
        [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
            if (successful) {
                [ConnectTools.share startConnectToolsWithNodeList:@[weakSelf.model] timeout:20 Complete:^(BOOL successful) {
                    if (successful) {
                        [weakSelf.connectTipButton setTitle:@"device connected." forState:UIControlStateNormal];
                    } else {
                        [weakSelf.connectTipButton setTitle:@"device disconnected." forState:UIControlStateNormal];
                    }
                }];
            } else {
                [weakSelf.connectTipButton setTitle:@"device disconnected." forState:UIControlStateNormal];
            }
        }];
    }
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.model.elements.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    RemoteElementCell *cell = (RemoteElementCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_RemoteElementCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    SigElementModel *ele = self.model.elements[indexPath.row];
    UInt16 eleAdr = ele.unicastAddress;
    SigModelIDModel *model = [self.model getModelIDModelWithModelID:kSigModel_GenericOnOffClient_ID andElementAddress:eleAdr];
    if (model == nil && ele.models && ele.models.count > 0) {
        model = ele.models.firstObject;
    }
    UInt16 pubAdr = 0;
    if (model) {
        pubAdr = [LibTools uint16From16String:model.publish.address];
    }
    cell.eleAdrTF.text = [NSString stringWithFormat:@"%04X",eleAdr];
    cell.modelTF.text = model.modelId;
    cell.pubAdrTF.text = [NSString stringWithFormat:@"%04X",pubAdr];
    cell.bgView.layer.cornerRadius = 5.0;
    cell.bgView.clipsToBounds = YES;
    __weak typeof(self) weakSelf = self;
    [cell.sendButton addAction:^(UIButton *button) {
        [weakSelf.view endEditing:YES];
        //1.判断是否直连遥控器
        if (SigBearer.share.isOpen && SigDataSource.share.unicastAddressOfConnected == weakSelf.model.address) {
            //直连遥控器成功
        } else {
            //直连遥控器失败
            [weakSelf showTips:@"The remote is disconnected!"];
            return;
        }
        
        //2.判断model合法性
        UInt16 newModelID = model.getIntModelID;
        UInt16 newCompanyIdentifier = model.getIntCompanyIdentifier;
        if ([weakSelf validateString:cell.modelTF.text.removeAllSapceAndNewlines] && cell.modelTF.text.length <= 8) {
            UInt32 tem = [LibTools uint32From16String:cell.modelTF.text];
            SigModelIDModel *temModel =  [self.model getModelIDModelWithModelID:tem andElementAddress:eleAdr];
            if (temModel) {
                if (tem >= 0xFFFF) {
                    newModelID = (tem >> 16) & 0xFFFF;
                    newCompanyIdentifier = tem & 0xFFFF;
                } else {
                    newModelID = tem;
                }
            } else {
                [self showTips:@"Please enter the correct modelID!"];
                return;
            }
        } else {
            [self showTips:@"Please enter the correct modelID!"];
            return;
        }
        
        //3.判断pubAdr合法性
        UInt16 newPubAdr = pubAdr;
        if ([weakSelf validateString:cell.pubAdrTF.text.removeAllSapceAndNewlines] && cell.pubAdrTF.text.length <= 4) {
            newPubAdr = [LibTools uint16From16String:cell.pubAdrTF.text];
        } else {
            [self showTips:@"Please enter the correct pubAdr!"];
            return;
        }
        
        //4.发送pubSet指令
        [DemoCommand editPublishListWithPublishAddress:newPubAdr nodeAddress:weakSelf.model.address elementAddress:eleAdr modelIdentifier:newModelID companyIdentifier:newCompanyIdentifier periodSteps:SigDataSource.share.defaultPublishPeriodModel.numberOfSteps periodResolution:[LibTools getSigStepResolutionWithSigPeriodModel:SigDataSource.share.defaultPublishPeriodModel] retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
            TeLogDebug(@"editPublishList callback");
            if (responseMessage.status == SigConfigMessageStatus_success && responseMessage.elementAddress == eleAdr) {
                if (responseMessage.publish.publicationAddress.address == newPubAdr && newPubAdr != 0) {
                    model.publish = [[SigPublishModel alloc] init];
                    SigRetransmitModel *retransmit = [[SigRetransmitModel alloc] init];
                    retransmit.count = 5;
                    retransmit.interval = (2+1)*50;//4.2.2.7 Publish Retransmit Interval Steps
                    model.publish.index = SigDataSource.share.curAppkeyModel.index;
                    model.publish.credentials = 0;
                    model.publish.ttl = 0xff;
                    //json数据中，period为publish周期的毫秒数据，默认20秒
                    model.publish.period.numberOfSteps = SigMeshLib.share.dataSource.defaultPublishPeriodModel.numberOfSteps;
                    model.publish.period.resolution = SigMeshLib.share.dataSource.defaultPublishPeriodModel.resolution;
                    model.publish.retransmit = retransmit;
                    model.publish.address = [NSString stringWithFormat:@"%04lX",(long)newPubAdr];
                } else {
                    model.publish = nil;
                }
                [SigDataSource.share saveLocationData];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            if (isResponseAll && error == nil) {
                [weakSelf showTips:@"App set pubAdr succeeded."];
            } else {
                [weakSelf showTips:@"App set pubAdr failed."];
            }
        }];
    }];
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 176.0;
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

- (BOOL)validateString:(NSString *)str{
    NSString *strRegex = @"^[0-9a-fA-F]{0,}$";
    NSPredicate *strPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",strRegex];
    return [strPredicate evaluateWithObject:str];
}

@end
