/********************************************************************************************************
 * @file     DeviceCompositionDataVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/6/24
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

#import "DeviceCompositionDataVC.h"
#import "UIViewController+Message.h"

@interface DeviceCompositionDataVC ()
@property (weak, nonatomic) IBOutlet UITextView *compositionDataTV;
@property (nonatomic,strong) SigMessageHandle *messageHandle;

@end

@implementation DeviceCompositionDataVC

- (void)normalSetting {
    [super normalSetting];
    self.title = @"Composition Data";
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshCompositionData)];
    self.navigationItem.rightBarButtonItem = rightItem;
    self.compositionDataTV.font = [UIFont systemFontOfSize:12.0];
    [self showCompositionDataUI];
}

- (void)clickRefreshCompositionData {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCompositionDataTimeOut) object:nil];
        [self performSelector:@selector(getCompositionDataTimeOut) withObject:nil afterDelay:10.0];
        [ShowTipsHandle.share show:Tip_GetComposition];
    });
    TelinkLogDebug(@"getCompositionData 0x%02x",self.model.address);
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand configCompositionDataGetWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigCompositionDataStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
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

- (void)showCompositionDataUI {
    NSString *str = [NSString stringWithFormat:@"Composition Data:\ncid: 0x%04X\npid: 0x%04X\nvid: 0x%04X\ncrpl: 0x%04X\nfeatures: 0x%04X\n\trelay support: %@\n\tproxy support: %@\n\tfriend support: %@\n\tlow power support: %@\nelement count:%d\n%@",self.model.compositionData.companyIdentifier,self.model.compositionData.productIdentifier,CFSwapInt16HostToBig(self.model.compositionData.versionIdentifier),self.model.compositionData.minimumNumberOfReplayProtectionList,self.model.compositionData.features.rawValue,self.model.compositionData.features.relayFeature == SigNodeFeaturesState_notSupported ? @"false" : @"true",self.model.compositionData.features.proxyFeature == SigNodeFeaturesState_notSupported ? @"false" : @"true",self.model.compositionData.features.friendFeature == SigNodeFeaturesState_notSupported ? @"false" : @"true",self.model.compositionData.features.lowPowerFeature == SigNodeFeaturesState_notSupported ? @"false" : @"true",(int)self.model.compositionData.elements.count,[self getElementsString]];
    self.compositionDataTV.text = str;
}

- (NSString *)getElementsString {
    NSString *tem = [NSString stringWithFormat:@""];
    for (int i=0; i < self.model.compositionData.elements.count; i++) {
        SigElementModel *element = self.model.compositionData.elements[i];
        tem = [tem stringByAppendingFormat:@"  element adr:0x%04X\n%@",i+self.model.address,[self getStringOfElement:element]];
    }
    return tem;
}

- (NSString *)getStringOfElement:(SigElementModel *)element {
    NSString *tem = [NSString stringWithFormat:@""];
    for (SigModelIDModel *modelID in element.models) {
        if (modelID.isBluetoothSIGAssigned) {
            ModelIDModel *m = [SigDataSource.share getModelIDModel:@(modelID.getIntModelID)];
            tem = [tem stringByAppendingFormat:@"\tSig model - 0x%04X - %@\n",modelID.getIntModelIdentifier,m.modelName];
        } else {
//            tem = [tem stringByAppendingFormat:@"\tVendor Model ID:0x%04X CID:0x%04X\n",modelID.getIntModelIdentifier,modelID.getIntCompanyIdentifier];
            tem = [tem stringByAppendingFormat:@"\tVendor model - 0x%08X\n",modelID.getIntModelID];
        }
    }
    return tem;
}

@end
