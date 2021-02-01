/********************************************************************************************************
 * @file     MeshOTAVC.m
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
//  MeshOTAVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/4/17.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "MeshOTAVC.h"
#import "MeshOTAItemCell.h"
#import "UIButton+extension.h"
#import "HomeViewController.h"
#import "OTAFileSource.h"
#import "UIViewController+Message.h"

/**
 Attention: more detail about mesh OTA can look document Mesh_Firmware_update_20180228_d05r05.pdf
 */
@interface MeshOTAVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*allItemArray;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*allItemAddressArray;
@property (nonatomic, strong) NSMutableArray *binStringArray;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*selectItemArray;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*selectItemAddressArray;
@property (nonatomic, assign) NSInteger binIndex;
@property (nonatomic, strong) NSMutableDictionary *allItemVIDDict;
@property (assign, nonatomic) BOOL needDelayReloadData;
@property (assign, nonatomic) BOOL isDelaying;
@property (nonatomic, strong) NSMutableArray <SigUpdatingNodeEntryModel *>*receiversList;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*receiversAddressList;
@end

@implementation MeshOTAVC

- (void)viewDidLoad {
    [super viewDidLoad];
     
    self.title = @"Mesh OTA";
    self.needDelayReloadData = NO;

    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];

    self.binIndex = -1;
    self.selectItemArray = [NSMutableArray array];
    self.selectItemAddressArray = [NSMutableArray array];
    self.allItemArray = [[NSMutableArray alloc] initWithArray:SigDataSource.share.curNodes];
    self.allItemAddressArray = [NSMutableArray array];
    for (SigNodeModel *node in self.allItemArray) {
        [self.allItemAddressArray addObject:@(node.address)];
    }
    self.binStringArray = [NSMutableArray arrayWithArray:OTAFileSource.share.getAllBinFile];
    self.allItemVIDDict = [NSMutableDictionary dictionary];
    
    #ifdef kExist
    if (kExistMeshOTA) {
        [MeshOTAManager.share saveIsMeshOTAing:NO];
    }
    #endif
}

- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid{
    [self.allItemVIDDict setObject:@(vid) forKey:@(address)];
    
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        if ([LibTools uint16From16String:node.vid] != vid) {
            node.vid = [NSString stringWithFormat:@"%04X",vid];
            [SigDataSource.share saveLocationData];
        }
    }
    [self performSelectorOnMainThread:@selector(delayReloadTableViewView) withObject:nil waitUntilDone:YES];
//    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

//刷新UI需要间隔0.1秒，防止100个设备时出现界面卡顿。
- (void)delayReloadTableViewView {
    if (!self.needDelayReloadData) {
        self.needDelayReloadData = YES;
        self.isDelaying = NO;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.tableView reloadData];
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayFinish) object:nil];
            [self performSelector:@selector(delayFinish) withObject:nil afterDelay:0.1];
        });
    } else {
        if (!self.isDelaying) {
            self.isDelaying = YES;
        }
    }
}

- (void)delayFinish {
    self.needDelayReloadData = NO;
    if (self.isDelaying) {
        [self delayReloadTableViewView];
    }
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    
    #ifdef kExist
    //界面可手动返回的情况，需要手动调用stopMeshOTA
    if (kExistMeshOTA) {
        if (MeshOTAManager.share.isMeshOTAing) {
            [MeshOTAManager.share stopMeshOTA];
        }
    }
    #endif

}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
}

- (void)setStartButtonEnable:(BOOL)enable{
    self.startButton.backgroundColor = enable ? kDefultColor : kDefultUnableColor;
}

- (IBAction)clickGetFwInfo:(UIButton *)sender {
    [self.allItemVIDDict removeAllObjects];
    __weak typeof(self) weakSelf = self;
    
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for GetFwInfo, but busy now.");
        [self showTips:@"busy now."];
        return;
    }
    
    //2.firmwareUpdateInformationGet
    UInt16 modelIdentifier = kSigModel_ObjectTransferServer_ID;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    NSInteger responseMax = 0;
    for (SigNodeModel *model in curNodes) {
            NSArray *addressArray = [model getAddressesWithModelID:@(modelIdentifier)];
        if (model.state != DeviceStateOutOfLine && addressArray && addressArray.count > 0) {
            responseMax ++;
        }
    }
    
    [SDKLibCommand firmwareUpdateInformationGetWithDestination:kMeshAddress_allNodes firstIndex:0 entriesLimit:1 retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMax successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
        if (responseMessage.firmwareInformationListCount > 0) {
            /*
             responseMessage.firmwareInformationList.firstObject.currentFirmwareID.length = 4: 2 bytes pid(设备类型) + 2 bytes vid(版本id).
             */
            if (responseMessage.firmwareInformationList.count > 0) {
                NSData *currentFirmwareID = responseMessage.firmwareInformationList.firstObject.currentFirmwareID;
                UInt16 pid = 0,vid = 0;
                Byte *pu = (Byte *)[currentFirmwareID bytes];
                if (currentFirmwareID.length >= 2) memcpy(&pid, pu, 2);
                if (currentFirmwareID.length >= 4) memcpy(&vid, pu + 2, 2);
                TeLogDebug(@"firmwareUpdateInformationGet=%@,pid=%d,vid=%d",[LibTools convertDataToHexStr:currentFirmwareID],pid,vid);
                [weakSelf updateNodeModelVidWithAddress:source vid:vid];
            }
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
    }];
}

- (IBAction)clickStartMeshOTA:(UIButton *)sender {
    #ifdef kExist
    if (kExistMeshOTA) {
        if (![MeshOTAManager share].isMeshOTAing) {
            [self startMeshOTA];
        }else{
            [self showTips:@"Device is meshOTAing, needn't call repeat."];
        }
    }
    #endif
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

#pragma mark - UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return section == 0 ? self.allItemArray.count+1 : self.binStringArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(MeshOTAItemCell.class) forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];
    
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = (MeshOTAItemCell *)cell;
    __weak typeof(self) weakSelf = self;
    
    if (indexPath.section == 0) {
        if (indexPath.row == 0) {
            itemCell.titleLabel.text = @"choose all";
            itemCell.selectButton.selected = self.selectItemArray.count == self.allItemArray.count;
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (!button.selected) {
                    NSArray *allItemArray = [NSArray arrayWithArray:weakSelf.allItemArray];
                    NSMutableArray *selectNodes = [NSMutableArray array];
                    NSMutableArray *selectAddresses = [NSMutableArray array];
                    for (SigNodeModel *model in allItemArray) {
                        if (model.state != DeviceStateOutOfLine) {
                            [selectNodes addObject:model];
                            [selectAddresses addObject:@(model.address)];
                        }
                    }
                    weakSelf.selectItemArray = selectNodes;
                    weakSelf.selectItemAddressArray = selectAddresses;
                } else {
                    [weakSelf.selectItemArray removeAllObjects];
                    [weakSelf.selectItemAddressArray removeAllObjects];
                }
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }];
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            NSNumber *address = self.allItemAddressArray[indexPath.row-1];
            int vid = 0;
            if ([self.allItemVIDDict.allKeys containsObject:address]) {
                vid = [self.allItemVIDDict[address] intValue];
            }
            UInt16 modelIdentifier = kSigModel_ObjectTransferServer_ID;
            NSArray *addressArray = [model getAddressesWithModelID:@(modelIdentifier)];
            if (addressArray && addressArray.count > 0) {
                NSString *str = [NSString stringWithFormat:@"adr:0x%X pid:0x%X vid:%c%c",model.address,[LibTools uint16From16String:model.pid],vid&0xff,(vid>>8)&0xff];//显示两个字节的ASCII
                if ([self.receiversAddressList containsObject:@(model.address)]) {
                    SigUpdatingNodeEntryModel *nodeEntryModel = self.receiversList[[self.receiversAddressList indexOfObject:@(model.address)]];
                    str = [NSString stringWithFormat:@"%@ %@",str,[self getDetailStringOfSigUpdatingNodeEntryModel:nodeEntryModel]];
                }
                itemCell.titleLabel.text = str;
            } else {
                vid = [LibTools uint16From16String:model.vid];
                itemCell.titleLabel.text = [NSString stringWithFormat:@"adr:0x%X pid:0x%X vid:%c%c Not support",model.address,[LibTools uint16From16String:model.pid],vid&0xff,(vid>>8)&0xff];//显示两个字节的ASCII
            }
            
            if (self.selectItemAddressArray.count > 0) {
                itemCell.selectButton.selected = [self.selectItemAddressArray containsObject:address];
            } else {
                itemCell.selectButton.selected = NO;
            }
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (model.state != DeviceStateOutOfLine) {
                    if ([weakSelf.selectItemAddressArray containsObject:address]) {
                        [weakSelf.selectItemArray removeObject:model];
                        [weakSelf.selectItemAddressArray removeObject:address];
                    }else{
                        [weakSelf.selectItemArray addObject:model];
                        [weakSelf.selectItemAddressArray addObject:address];
                    }
                    [weakSelf.tableView reloadData];
                } else {
                    [self showTips:@"This node is outline."];
                    return;
                }
            }];
        }
    } else {
        NSString *binString = self.binStringArray[indexPath.row];
        NSData *data = [OTAFileSource.share getDataWithBinName:binString];
        UInt16 vid = [OTAFileSource.share getVidWithOTAData:data];
        itemCell.titleLabel.text = [NSString stringWithFormat:@"%@ pid:0x%X vid:%c%c",binString,[OTAFileSource.share getPidWithOTAData:data],vid&0xff,(vid>>8)&0xff];//vid显示两个字节的ASCII
        itemCell.selectButton.selected = indexPath.row == _binIndex;
        [itemCell.selectButton addAction:^(UIButton *button) {
            weakSelf.binIndex = indexPath.row;
            [weakSelf checkPID];
            [weakSelf.tableView reloadData];
        }];
    }
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    if (section == 0) {
        return @"Device list";
    } else {
        return @"OTA file";
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.section == 0) {
        if (indexPath.row == 0) {
            __weak typeof(self) weakSelf = self;
            MeshOTAItemCell *itemCell = [tableView cellForRowAtIndexPath:indexPath];
            if (!itemCell.selectButton.selected) {
                NSArray *allItemArray = [NSArray arrayWithArray:weakSelf.allItemArray];
                NSMutableArray *selectNodes = [NSMutableArray array];
                NSMutableArray *selectAddresses = [NSMutableArray array];
                for (SigNodeModel *model in allItemArray) {
                    if (model.state != DeviceStateOutOfLine) {
                        [selectNodes addObject:model];
                        [selectAddresses addObject:@(model.address)];
                    }
                }
                weakSelf.selectItemArray = selectNodes;
                weakSelf.selectItemAddressArray = selectAddresses;
            } else {
                [weakSelf.selectItemArray removeAllObjects];
                [weakSelf.selectItemAddressArray removeAllObjects];
            }
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            NSNumber *address = self.allItemAddressArray[indexPath.row-1];
            if (model.state != DeviceStateOutOfLine) {
                if ([self.selectItemAddressArray containsObject:address]) {
                    [self.selectItemArray removeObject:model];
                    [self.selectItemAddressArray removeObject:address];
                }else{
                    [self.selectItemArray addObject:model];
                    [self.selectItemAddressArray addObject:address];
                }
            } else {
                [self showTips:@"This node is outline."];
                return;
            }
        }
        [self.tableView reloadData];
    } else {
        if (_binIndex != indexPath.row) {
            _binIndex = indexPath.row;
            [self checkPID];
            [self.tableView reloadData];
        }
    }
}

- (void)checkPID {
    NSString *binString = self.binStringArray[_binIndex];
    NSData *data = [OTAFileSource.share getDataWithBinName:binString];
    UInt16 pid = [OTAFileSource.share getPidWithOTAData:data];

    //如果存在非当前PID的设备选中了要进行MeshOTA，则全部取消设备的选中，让客户重新选择。
    BOOL chooseDifferent = NO;
    for (SigNodeModel *node in self.selectItemArray) {
        if ([LibTools uint16From16String:node.pid] != pid) {
            chooseDifferent = YES;
            break;
        }
    }
    if (chooseDifferent) {
        [self showAlertSureWithTitle:@"Hits" message:@"The PID of node is different from the PID of Bin file! Please choose device again." sure:nil];
        [self.selectItemArray removeAllObjects];
    }
}

- (void)startMeshOTA{
    #ifdef kExist
    if (kExistMeshOTA) {
        if (self.selectItemArray.count == 0) {
            [self showTips:@"Please choose some devices for mesh OTA."];
            return;
        }
        if (self.binIndex < 0) {
            [self showTips:@"Please choose the bin file for mesh OTA."];
            return;
        }
        
        [self userAbled:NO];
        NSData *data = [OTAFileSource.share getDataWithBinName:self.binStringArray[self.binIndex]];
        if (data == nil || data.length == 0) {
            [self showTips:@"APP can't load this Bin file."];
            return;
        }
        NSMutableArray *tem = [NSMutableArray array];
        NSArray *selectItemArray = [NSArray arrayWithArray:self.selectItemArray];
        for (SigNodeModel *model in selectItemArray) {
            [tem addObject:@(model.address)];
        }
        
        NSData *incomingFirmwareMetadata = nil;
        if (data && data.length >= 6) {
            //incomingFirmwareMetadata默认为8个字节的0。需要bin文件里面从index为2开始取4个字节的数据，再补充4个字节的0。
            UInt32 tem32 = 0;
            NSData *temData = [NSData dataWithBytes:&tem32 length:4];
            NSMutableData *mData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(2, 4)]];
            //test meshOTA vid
//            NSMutableData *mData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(2, 2)]];
//            UInt16 vid = 0x3633;//对应vid为‘36’
//            NSData *vidData = [NSData dataWithBytes:&vid length:2];
//            [mData appendData:vidData];
            [mData appendData:temData];
            incomingFirmwareMetadata = mData;
        } else {
            [self showTips:@"This Bin file is invalid."];
            return;
        }
        self.receiversList = [NSMutableArray array];
        self.receiversAddressList = [NSMutableArray array];
        __weak typeof(self) weakSelf = self;
        [MeshOTAManager.share startMeshOTAWithLocationAddress:SigDataSource.share.curLocationNodeModel.address deviceAddresses:tem otaData:data incomingFirmwareMetadata:incomingFirmwareMetadata progressHandle:^(NSInteger progress) {
            dispatch_async(dispatch_get_main_queue(), ^{
                NSString *t = [NSString stringWithFormat:@"mesh ota... progress:%ld%%", (long)progress];
                [ShowTipsHandle.share show:t];
                if (progress == 100) {
                    [weakSelf userAbled:YES];
                    [ShowTipsHandle.share delayHidden:2.0];
                }
            });
        } finishHandle:^(NSArray<NSNumber *> *successAddresses, NSArray<NSNumber *> *failAddresses) {
            NSString *tip = [NSString stringWithFormat:@"Mesh ota finish, success:%ld,fail:%ld", (long)successAddresses.count, (long)failAddresses.count];
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf userAbled:YES];
                [ShowTipsHandle.share delayHidden:0];
                [weakSelf showTips:tip];
            });
        } errorHandle:^(NSError *error) {
            NSString *tip = [NSString stringWithFormat:@"Mesh ota fail, error = %@", error];
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf userAbled:YES];
                [ShowTipsHandle.share delayHidden:0];
                [weakSelf showTips:tip];
            });
            [SigBearer.share startMeshConnectWithComplete:nil];
        }];
//        [MeshOTAManager.share startFirmwareUpdateWithDeviceAddresses:tem otaData:data incomingFirmwareMetadata:incomingFirmwareMetadata gattDistributionProgressHandle:^(NSInteger progress) {
//            dispatch_async(dispatch_get_main_queue(), ^{
//                NSString *t = [NSString stringWithFormat:@"mesh ota... gatt progress:%ld%%", (long)progress];
//                [ShowTipsHandle.share show:t];
//                if (progress == 100) {
//                    [weakSelf userAbled:YES];
//                    [ShowTipsHandle.share delayHidden:2.0];
//                }
//            });
//        } advDistributionProgressHandle:^(SigFirmwareDistributionReceiversList *responseMessage) {
//            TeLogInfo(@"adv responseMessage=%@",responseMessage);
//            if (responseMessage && responseMessage.receiversList && responseMessage.receiversList.count > 0) {
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [weakSelf updateReceiversList:responseMessage.receiversList];
//                });
//            }
//        } finishHandle:^(NSArray<NSNumber *> *successAddresses, NSArray<NSNumber *> *failAddresses) {
//            NSString *tip = [NSString stringWithFormat:@"Mesh ota finish, success:%ld,fail:%ld", (long)successAddresses.count, (long)failAddresses.count];
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [weakSelf userAbled:YES];
//                [ShowTipsHandle.share delayHidden:0];
//                [weakSelf showTips:tip];
//            });
//        } errorHandle:^(NSError * _Nullable error) {
//            NSString *tip = [NSString stringWithFormat:@"Mesh ota fail, error = %@", error];
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [weakSelf userAbled:YES];
//                [ShowTipsHandle.share delayHidden:0];
//                [weakSelf showTips:tip];
//            });
//            [SigBearer.share startMeshConnectWithComplete:nil];
//        }];
    }
    #endif
}

- (void)updateReceiversList:(NSArray <SigUpdatingNodeEntryModel *>*)receiversList {
    NSMutableArray *oldAddresses = [NSMutableArray arrayWithArray:self.receiversAddressList];
    NSMutableArray *newAddresses = [NSMutableArray array];
    for (SigUpdatingNodeEntryModel *model in receiversList) {
        [newAddresses addObject:@(model.address)];
    }
    for (NSNumber *newAddress in newAddresses) {
        if ([oldAddresses containsObject:newAddress]) {
            [self.receiversList replaceObjectAtIndex:[oldAddresses indexOfObject:newAddress] withObject:receiversList[[newAddresses indexOfObject:newAddress]]];
        } else {
            [self.receiversList addObject:receiversList[[newAddresses indexOfObject:newAddress]]];
            [self.receiversAddressList addObject:newAddress];
        }
    }
    NSString *advString = @"mesh ota... adv progress:";
    for (SigUpdatingNodeEntryModel *model in self.receiversList) {
        advString = [NSString stringWithFormat:@"%@\nadr:0x%X %@",advString,model.address,[self getDetailStringOfSigUpdatingNodeEntryModel:model]];
    }
    [ShowTipsHandle.share show:advString];
    [self.tableView reloadData];
}

- (NSString *)getDetailStringOfSigUpdatingNodeEntryModel:(SigUpdatingNodeEntryModel *)model {
    NSString *phaseString = [SigHelper.share getDetailOfSigFirmwareUpdatePhaseType:model.retrievedUpdatePhase];
    if (model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applyingUpdate) {
        phaseString = @"success";
    }
    NSString *tem = [NSString stringWithFormat:@"prog:%d%%-%@",model.transferProgress,phaseString];
    return tem;
}

- (void)userAbled:(BOOL)able{
    self.startButton.enabled = able;
    self.tableView.userInteractionEnabled = able;
    [self setStartButtonEnable:able];
}

typedef void(^getFirmwareImageCompleteHandle)(NSError *error, SigFirmwareInformationEntryModel *model, NSData *firmwareData);
- (void)getFirmwareImageWithNodeAddress:(UInt16)nodeAddress firstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit completeHandle:(getFirmwareImageCompleteHandle)completeHandle {
    [SDKLibCommand firmwareUpdateInformationGetWithDestination:nodeAddress firstIndex:firstIndex entriesLimit:entriesLimit retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
        TeLogDebug(@"firmwareUpdateInformationGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (responseMessage.firmwareInformationList && responseMessage.firmwareInformationList.count > 0) {
            SigFirmwareInformationEntryModel *model = responseMessage.firmwareInformationList.firstObject;
            [TelinkHttpManager.share firmwareCheckRequestWithFirewareIDString:model.getFirmwareIDString updateURI:model.getUpdateURIString didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
                if (!err) {
                    //check firmware response
                    UInt16 connectedNodeAddress = SigDataSource.share.unicastAddressOfConnected;
                    [SDKLibCommand firmwareDistributionUploadOOBStartWithDestination:connectedNodeAddress uploadURILength:model.updateURILength uploadURI:model.updateURL uploadFirmwareID:model.currentFirmwareID retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionUploadStatus * _Nonnull responseMessage) {
                        TeLogDebug(@"firmwareDistributionUploadOOBStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_success) {
                            [TelinkHttpManager.share firmwareGetRequestWithFirewareIDString:model.getFirmwareIDString updateURI:model.getUpdateURIString didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
                                if (!err) {
                                    //get firmware response
                                    //待完善：从result中取出实际的firmwareData
                                    NSData *fData = [NSData data];
                                    if (completeHandle) {
                                        completeHandle(nil,model,fData);
                                    }
                                } else {
                                    //get firmware fail
                                    if (completeHandle) {
                                        completeHandle(err,model,nil);
                                    }
                                }
                            }];
                        } else {
                            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"firmwareDistributionUploadOOBStart,status=0x%X",responseMessage.status] code:-responseMessage.status userInfo:nil];
                            if (completeHandle) {
                                completeHandle(error,model,nil);
                            }
                        }
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                        if (error) {
                            if (completeHandle) {
                                completeHandle(error,model,nil);
                            }
                        }
                    }];
                } else {
                    //check firmware fail
                    if (completeHandle) {
                        completeHandle(err,model,nil);
                    }
                }
            }];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            if (completeHandle) {
                completeHandle(error,nil,nil);
            }
        }
    }];
}

- (void)startMeshOTADistribution {
    
}

- (void)showMeshOTAError:(NSError *)error {
    TeLogDebug(@"error=%@",error);
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf userAbled:YES];
        [ShowTipsHandle.share delayHidden:0];
        [weakSelf showTips:error.domain];
    });
}

@end
