/********************************************************************************************************
 * @file     PTSViewController.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/5/12
 *
 * @par     Copyright (c) [2022], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "PTSViewController.h"
#import "UIViewController+Message.h"

@interface PTSViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;

@end

@implementation PTSViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"PTS Test";
    self.source = [NSMutableArray arrayWithArray:@[@"largeCompositionDataGet", @"modelsMetadataGet", @"SARTransmitterGet", @"SARTransmitterSet", @"SARReceiverGet", @"SARReceiverSet", @"solicitationPduRplItemsClear", @"solicitationPduRplItemsClearUnacknowledged", @"onDemandPrivateProxyGet", @"onDemandPrivateProxySet", @"aggOnOffGet100"]];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell=[[UITableViewCell alloc]initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:@"UITableViewCell"];
    cell.textLabel.text = self.source[indexPath.row];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *str = self.source[indexPath.row];
    if ([str isEqualToString:@"largeCompositionDataGet"]) {
        [self largeCompositionDataGet];
    } else if ([str isEqualToString:@"modelsMetadataGet"]) {
        [self modelsMetadataGet];
    } else if ([str isEqualToString:@"SARTransmitterGet"]) {
        [self SARTransmitterGet];
    } else if ([str isEqualToString:@"SARTransmitterSet"]) {
        [self SARTransmitterSet];
    } else if ([str isEqualToString:@"SARReceiverGet"]) {
        [self SARReceiverGet];
    } else if ([str isEqualToString:@"SARReceiverSet"]) {
        [self SARReceiverSet];
    } else if ([str isEqualToString:@"solicitationPduRplItemsClear"]) {
        [self solicitationPduRplItemsClear];
    } else if ([str isEqualToString:@"solicitationPduRplItemsClearUnacknowledged"]) {
        [self solicitationPduRplItemsClearUnacknowledged];
    } else if ([str isEqualToString:@"onDemandPrivateProxyGet"]) {
        [self onDemandPrivateProxyGet];
    } else if ([str isEqualToString:@"onDemandPrivateProxySet"]) {
        [self onDemandPrivateProxySet];
    } else if ([str isEqualToString:@"aggOnOffGet100"]) {
        [self aggOnOffGet100];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 51.0;
}

- (void)largeCompositionDataGet {
    [ShowTipsHandle.share show:Tip_GetComposition];
    __weak typeof(self) weakSelf = self;
    //largeCompositionDataGet
    [SDKLibCommand largeCompositionDataGetWithPage:0xFF offset:0 destination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLargeCompositionDataStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        if (responseMessage.totalSize == responseMessage.data.length) {
            UInt8 tem8 = responseMessage.page;
            NSMutableData *mData = [NSMutableData dataWithBytes:&tem8 length:1];
            [mData appendData:responseMessage.data];
            weakSelf.model.compositionData = [[SigPage0 alloc] initWithParameters:mData];
            [SigDataSource.share saveLocationData];
        } else {
            TelinkLogDebug(@"单次获取未完整，待完善！！！");
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_GetCompositionFail];
            } else {
                [weakSelf showTips:Tip_GetCompositionSuccess];
            }
        });
    }];
}

- (void)modelsMetadataGet {
    [ShowTipsHandle.share show:Tip_GetModelsMetadata];
    __weak typeof(self) weakSelf = self;
    //modelsMetadataGet
    [SDKLibCommand modelsMetadataGetWithMetadataPage:0xFF offset:0 destination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigModelsMetadataStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
        if (responseMessage.totalSize == responseMessage.data.length) {
            if (responseMessage.metadataPage == 0) {
                SigModelsMetadataPage0Model *page0 = [[SigModelsMetadataPage0Model alloc] initWithParameters:responseMessage.data];
                TelinkLogInfo(@"page0=%@",page0.getDescription);
            }
        } else {
            TelinkLogDebug(@"单次获取未完整，待完善！！！");
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_GetModelsMetadataFail];
            } else {
                [weakSelf showTips:Tip_GetModelsMetadataSuccess];
            }
        });
    }];
}

- (void)SARTransmitterGet {
    [ShowTipsHandle.share show:Tip_GetSARTransmitter];
    __weak typeof(self) weakSelf = self;
    //SARTransmitterGet
    [SDKLibCommand SARTransmitterGetWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSARTransmitterStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_GetSARTransmitterFail];
            } else {
                [weakSelf showTips:Tip_GetSARTransmitterSuccess];
            }
        });
    }];
}

- (void)SARTransmitterSet {
    [ShowTipsHandle.share show:Tip_SetSARTransmitter];
    __weak typeof(self) weakSelf = self;
    //SARTransmitterSet
    struct SARTransmitterStructure transmitter = {};
    transmitter.value = 0;
    [SDKLibCommand SARTransmitterSetWithSARTransmitter:transmitter destination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSARTransmitterStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_SetSARTransmitterFail];
            } else {
                [weakSelf showTips:Tip_SetSARTransmitterSuccess];
            }
        });
    }];
}

- (void)SARReceiverGet {
    [ShowTipsHandle.share show:Tip_GetSARReceiver];
    __weak typeof(self) weakSelf = self;
    //SARReceiverGet
    [SDKLibCommand SARReceiverGetWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSARReceiverStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_GetSARReceiverFail];
            } else {
                [weakSelf showTips:Tip_GetSARReceiverSuccess];
            }
        });
    }];
}

- (void)SARReceiverSet {
    [ShowTipsHandle.share show:Tip_SetSARReceiver];
    __weak typeof(self) weakSelf = self;
    //SARReceiverSet
    struct SARReceiverStructure receiver = {};
    receiver.value = 0;
    [SDKLibCommand SARReceiverSetWithSARReceiver:receiver destination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSARReceiverStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_SetSARReceiverFail];
            } else {
                [weakSelf showTips:Tip_SetSARReceiverSuccess];
            }
        });
    }];
}

- (void)solicitationPduRplItemsClear {
    [ShowTipsHandle.share show:Tip_ClearSolicitationPduRplItems];
    __weak typeof(self) weakSelf = self;
    //solicitationPduRplItemsClear
//    SigUnicastAddressRangeFormatModel *m = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:self.model.elements.count > 1 rangeStart:self.model.address rangeLength:self.model.elements.count];
    SigUnicastAddressRangeFormatModel *m = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:NO rangeStart:self.model.address rangeLength:0];
    [SDKLibCommand solicitationPduRplItemsClearWithAddressRange:m destination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSolicitationPduRplItemsStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_ClearSolicitationPduRplItemsFail];
            } else {
                [weakSelf showTips:Tip_ClearSolicitationPduRplItemsSuccess];
            }
        });
    }];
}

- (void)solicitationPduRplItemsClearUnacknowledged {
    [ShowTipsHandle.share show:Tip_ClearSolicitationPduRplItemsUnacknowledged];
    __weak typeof(self) weakSelf = self;
    //solicitationPduRplItemsClearUnacknowledged
//    SigUnicastAddressRangeFormatModel *m = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:self.model.elements.count > 1 rangeStart:self.model.address rangeLength:self.model.elements.count];
    SigUnicastAddressRangeFormatModel *m = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:NO rangeStart:self.model.address rangeLength:0];
    [SDKLibCommand solicitationPduRplItemsClearUnacknowledgedWithAddressRange:m destination:self.model.address retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_ClearSolicitationPduRplItemsUnacknowledgedFail];
            } else {
                [weakSelf showTips:Tip_ClearSolicitationPduRplItemsUnacknowledgedSuccess];
            }
        });
    }];
}

- (void)onDemandPrivateProxyGet {
    [ShowTipsHandle.share show:Tip_GetOnDemandPrivateProxy];
    __weak typeof(self) weakSelf = self;
    //onDemandPrivateProxyGet
    [SDKLibCommand onDemandPrivateProxyGetWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigOnDemandPrivateProxyStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_GetOnDemandPrivateProxyFail];
            } else {
                [weakSelf showTips:Tip_GetOnDemandPrivateProxySuccess];
            }
        });
    }];
}

- (void)onDemandPrivateProxySet {
    [ShowTipsHandle.share show:Tip_SetOnDemandPrivateProxy];
    __weak typeof(self) weakSelf = self;
    //onDemandPrivateProxySet
    [SDKLibCommand onDemandPrivateProxySetWithOnDemandPrivateGATTProxy:0 destination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigOnDemandPrivateProxyStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            if (!isResponseAll || error) {
                [weakSelf showTips:Tip_SetOnDemandPrivateProxyFail];
            } else {
                [weakSelf showTips:Tip_SetOnDemandPrivateProxySuccess];
            }
        });
    }];
}

- (void)aggOnOffGet100 {
    NSMutableArray *items = [NSMutableArray array];
    for (int i=0; i<100; i++) {
        SigOpcodesAggregatorItemModel *modelResponse = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigGenericOnOffGet alloc] init]];
        [items addObject:modelResponse];
    }
    SigOpcodesAggregatorSequence *message = [[SigOpcodesAggregatorSequence alloc] initWithElementAddress:self.model.address items:items];
    [SDKLibCommand sendSigOpcodesAggregatorSequenceMessage:message retryCount:0 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigOpcodesAggregatorStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"SigOpcodesAggregatorStatus=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
    }];
}

@end
