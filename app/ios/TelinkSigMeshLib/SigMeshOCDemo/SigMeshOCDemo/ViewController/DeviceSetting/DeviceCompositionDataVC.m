/********************************************************************************************************
* @file     DeviceCompositionDataVC.m
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
//  DeviceCompositionDataVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/6/24.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "DeviceCompositionDataVC.h"
#import "UIViewController+Message.h"

@interface DeviceCompositionDataVC ()
@property (weak, nonatomic) IBOutlet UITextView *compositionDataTV;
@property (nonatomic,strong) SigMessageHandle *messageHandle;

@end

@implementation DeviceCompositionDataVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"Composition Data";
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshCompositionData)];
    self.navigationItem.rightBarButtonItem = rightItem;
    [self showCompositionDataUI];
}

- (void)clickRefreshCompositionData {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
        [self performSelector:@selector(getCompositionDataTimeOut) withObject:nil afterDelay:10.0];
        [ShowTipsHandle.share show:Tip_GetComposition];
    });
    TeLogDebug(@"getCompositionData 0x%02x",self.model.address);
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand configCompositionDataGetWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigCompositionDataStatus * _Nonnull responseMessage) {
        TeLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        weakSelf.model.compositionData = [[SigPage0 alloc] initWithParameters:responseMessage.parameters];
        [SigDataSource.share saveLocationData];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_GetCompositionFail];
            } else {
                [weakSelf showCompositionDataUI];
                [weakSelf showTips:Tip_GetCompositionSuccess];
            }
        });
    }];
}

- (void)getCompositionDataTimeOut {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
        [ShowTipsHandle.share hidden];
        [self showTips:Tip_GetCompositionFail];
    });
    [self.messageHandle cancel];
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

- (void)showCompositionDataUI {
    NSString *str = [NSString stringWithFormat:@"Composition Data:\ncid: 0x%04X\npid: 0x%04X\nvid: 0x%04X\ncrpl: 0x%04X\nfeatures: 0x%04X\n\trelay support: %@\n\tproxy support: %@\n\tfriend support: %@\n\tlow power support: %@\nelement count:%d\n%@",self.model.compositionData.companyIdentifier,self.model.compositionData.productIdentifier,self.model.compositionData.versionIdentifier,self.model.compositionData.minimumNumberOfReplayProtectionList,self.model.compositionData.features.rawValue,self.model.compositionData.features.relayFeature == SigNodeFeaturesState_notSupported ? @"false" : @"ture",self.model.compositionData.features.proxyFeature == SigNodeFeaturesState_notSupported ? @"false" : @"ture",self.model.compositionData.features.friendFeature == SigNodeFeaturesState_notSupported ? @"false" : @"ture",self.model.compositionData.features.lowPowerFeature == SigNodeFeaturesState_notSupported ? @"false" : @"ture",(int)self.model.compositionData.elements.count,[self getElementsString]];
    self.compositionDataTV.text = str;
}

- (NSString *)getElementsString {
    NSString *tem = [NSString stringWithFormat:@""];
    for (int i=0; i < self.model.compositionData.elements.count; i++) {
        SigElementModel *element = self.model.compositionData.elements[i];
        tem = [tem stringByAppendingFormat:@"\tElement%d:\n%@",i+1,[self getStringOfElement:element]];
    }
    return tem;
}

- (NSString *)getStringOfElement:(SigElementModel *)element {
    NSString *tem = [NSString stringWithFormat:@""];
    for (SigModelIDModel *modelID in element.models) {
        if (modelID.isBluetoothSIGAssigned) {
            tem = [tem stringByAppendingFormat:@"\tSig Model ID:0x%04X\n",modelID.getIntModelIdentifier];
        } else {
            tem = [tem stringByAppendingFormat:@"\tVendor Model ID:0x%04X CID:0x%04X\n",modelID.getIntModelIdentifier,modelID.getIntCompanyIdentifier];
        }
    }
    return tem;
}

@end
