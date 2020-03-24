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
//  TelinkBlueDemo
//
//  Created by Arvin on 2018/4/17.
//  Copyright © 2018年 Green. All rights reserved.
//

#import "MeshOTAVC.h"
#import "MeshOTAItemCell.h"
#import "UIButton+extension.h"
#import "HomeViewController.h"
#import "OTAFileSource.h"

/**
 Attention: more detail about mesh OTA can look document Mesh_Firmware_update_20180228_d05r05.pdf
 */
@interface MeshOTAVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*allItemArray;
@property (nonatomic, strong) NSMutableArray *binStringArray;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*selectItemArray;
@property (nonatomic, assign) NSInteger binIndex;
@property (nonatomic, strong) NSMutableDictionary *allItemVIDDict;

@property (nonatomic, strong) NSThread *meshOTAThread;

@end

@implementation MeshOTAVC

- (void)viewDidLoad {
    [super viewDidLoad];
     
    self.title = @"Mesh OTA";
    
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];

    self.binIndex = -1;
    self.selectItemArray = [NSMutableArray array];
    self.allItemArray = [[NSMutableArray alloc] initWithArray:SigDataSource.share.curNodes];
    self.binStringArray = [NSMutableArray arrayWithArray:OTAFileSource.share.getAllBinFile];
    self.allItemVIDDict = [NSMutableDictionary dictionary];
    
    [MeshOTAManager.share saveIsMeshOTAing:NO];
    
    _meshOTAThread = [[NSThread alloc] initWithTarget:self selector:@selector(startThread) object:nil];
    _meshOTAThread.name = @"meshOTAThread";
    [_meshOTAThread start];
}

#pragma mark - Private
- (void)startThread{
    [NSTimer scheduledTimerWithTimeInterval:[[NSDate distantFuture] timeIntervalSinceNow] target:self selector:@selector(nullFunc) userInfo:nil repeats:NO];
    while (1) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
}

- (void)nullFunc{}


- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid{
    [self.allItemVIDDict setObject:@(vid) forKey:@(address)];
    
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        if ([LibTools uint16From16String:node.vid] != vid) {
            node.vid = [NSString stringWithFormat:@"%04X",vid];
            [SigDataSource.share saveLocationData];
        }
    }
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    
    //界面可手动返回的情况，需要手动调用stopMeshOTA
    if (MeshOTAManager.share.isMeshOTAing) {
        [MeshOTAManager.share stopMeshOTA];
    }
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
    
    //2.firmwareInformationGet
//    [SDKLibCommand firmwareInformationGetWithDestination:kMeshAddress_allNodes resMax:SigMeshLib.share.dataSource.getOnlineDevicesNumber retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareInformationStatus * _Nonnull responseMessage) {
//        /*
//         responseMessage.parameters.length = 6: 2 bytes cid(vendor id) + 2 bytes pid(设备类型) + 2 bytes vid(版本id).
//         */
//        UInt16 cid = 0,pid = 0,vid = 0;
//        Byte *pu = (Byte *)[responseMessage.parameters bytes];
//        if (responseMessage.parameters.length >= 2) memcpy(&cid, pu, 2);
//        if (responseMessage.parameters.length >= 4) memcpy(&pid, pu + 2, 2);
//        if (responseMessage.parameters.length >= 6) memcpy(&vid, pu + 4, 2);
//        TeLogDebug(@"firmwareInformationGet=%@,cid=%d,pid=%d,vid=%d",[LibTools convertDataToHexStr:responseMessage.parameters],cid,pid,vid);
//        [weakSelf updateNodeModelVidWithAddress:source vid:vid];
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//    }];
    //=================test==================//
    
    //1.configModelSubscriptionAdd
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            UInt16 modelID = 0xFF00;
//            UInt16 groupAddress = 0xc000;
//            NSArray *addressArray = [node getAddressesWithModelID:@(modelID)];
//            if (addressArray && addressArray.count > 0) {
//                UInt16 eleAddress = [addressArray.firstObject intValue];
//                [DemoCommand editSubscribeListWithGroupAddress:groupAddress nodeAddress:node.address elementAddress:eleAddress isAdd:YES modelID:modelID successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
//                    if (responseMessage.elementAddress == eleAddress && responseMessage.address == groupAddress) {
//                        UInt32 modelId = 0;
//                        if (responseMessage.companyIdentifier == 0) {
//                            modelId = responseMessage.modelIdentifier;
//                        } else {
//                            modelId = responseMessage.modelIdentifier | (responseMessage.companyIdentifier << 16);
//                        }
//                        if (modelId == modelID) {
//                            if (responseMessage.status == SigConfigMessageStatus_success) {
//                        TeLogDebug(@"firmwareInformationGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//                            } else {
//                                TeLogError(@"订阅组号失败：error code=%d",responseMessage.status);
//                            }
//                        }
//                    }
//                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//                }];
//            }
//        }
//    }
    
    //3.objectInformationGet
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            [SDKLibCommand objectInformationGetWithDestination:node.address resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectInformationStatus * _Nonnull responseMessage) {
//                TeLogDebug(@"objectInformationGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            }];
//        }
//    }

    //4.firmwareUpdatePrepare
//    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
//    [oprationQueue addOperationWithBlock:^{
//        //这个block语句块在子线程中执行
//        NSLog(@"oprationQueue");
//        for (SigNodeModel *node in self.selectItemArray) {
//            if (node.state != DeviceStateOutOfLine) {
//                UInt32 firmwareID = 0xff000021;
//                NSData *firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
//                UInt64 objID = 0x8877665544332211;
//                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [SDKLibCommand firmwareUpdatePrepareWithDestination:node.address companyID:0x0211 firmwareID:firmwareIDData objectID:objID vendorValidationData:nil resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
//                        TeLogDebug(@"firmwareUpdatePrepare=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//                        dispatch_semaphore_signal(semaphore);
//                    }];
//                });
//                //Most provide 3 seconds to firmwareUpdatePrepare every node.
//                dispatch_semaphore_wait(semaphore, (dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 3.0))));
//            }
//        }
//    }];

    //5.firmwareUpdateStart
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            UInt16 cid = 0x0211;
//            UInt32 firmwareID = 0xff000021;
//            NSData *firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
//            [SDKLibCommand firmwareUpdateStartWithDestination:node.address updatePolicy:SigUpdatePolicyType_none companyID:cid firmwareID:firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
//                TeLogDebug(@"firmwareUpdateStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            }];
//        }
//    }

    //6.objectTransferStart
//    [self performSelector:@selector(testobjectTransferStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
    

    //7.objectBlockTransferStart
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            UInt64 objID = 0x1122334455667788;
//            NSData *otaData = [OTAFileSource.share getDataWithBinName:@"8258_mesh"];
//            NSData *blockData = [otaData subdataWithRange:NSMakeRange(0, 0x1000)];
//            UInt32 blockChecksum = [LibTools getCRC32OfData:blockData];
//            NSData *blockChecksumValue = [NSData dataWithBytes:&blockChecksum length:4];
////            UInt32 objectSize = (UInt32)otaData.length;//bin文件的总字节数
////            UInt8 blockSizeLog = 0x0C;//10^12=4096，表示meshOTA的一个block应该传递的最大bin文件字节数。
//            //11 22 33 44 55 66 77 88  00 00 00 01 00 61 a5 76 9e 00 10
//            //一个block为4096bytes,bin文件总大小为4096*0x20+1066,则blockNumber为0x00、0x01、0x02、0x03、···、0x21.
//            //chunkSize为当前block的bin文件最大字节数
//            //blockChecksumValue为CRC32校验
//            //currentBlockSize为当前block的bin文件字节数
//            [SDKLibCommand objectBlockTransferStartWithDestination:node.address objectID:objID blockNumber:0x00 chunkSize:0x1000 blockChecksumAlgorithm:SigBlockChecksumAlgorithmType_CRC32 blockChecksumValue:blockChecksumValue currentBlockSize:0x1000 resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectBlockTransferStatus * _Nonnull responseMessage) {
//                TeLogDebug(@"objectBlockTransferStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            }];
//        }
//    }

        //8.objectChunkTransfer
//    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
//    [oprationQueue addOperationWithBlock:^{
//        //这个block语句块在子线程中执行
//        NSLog(@"oprationQueue");
//        for (SigNodeModel *node in self.selectItemArray) {
//            if (node.state != DeviceStateOutOfLine) {
//                NSData *otaData = [OTAFileSource.share getDataWithBinName:@"8258_mesh"];
//                NSData *blockData = [otaData subdataWithRange:NSMakeRange(0, 0x1000)];
//                NSData *firstChunkData = [blockData subdataWithRange:NSMakeRange(0, 0x0100)];
//                [SDKLibCommand objectChunkTransferWithDestination:SigDataSource.share.getCurrentConnectedNode.address chunkNumber:0 firmwareImageData:firstChunkData resMax:0 retryCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//                }];
//            }
//        }
//    }];

    
    //9.objectBlockGet
    for (SigNodeModel *node in self.selectItemArray) {
        if (node.state != DeviceStateOutOfLine) {
            UInt64 objID = 0x1122334455667788;
            [SDKLibCommand objectBlockGetWithDestination:node.address objectID:objID blockNumber:0x00 resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectBlockStatus * _Nonnull responseMessage) {
                TeLogDebug(@"objectBlockGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            }];
        }
    }

    //10.firmwareUpdateGet
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            UInt16 cid = 0x0211;
//            UInt32 firmwareID = 0xff000021;
//            NSData *firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
//            [SDKLibCommand firmwareUpdateGetWithDestination:node.address companyID:cid firmwareID:firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
//                TeLogDebug(@"firmwareUpdateGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            }];
//        }
//    }

    //11.firmwareUpdateApply
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            UInt16 cid = 0x0211;
//            UInt32 firmwareID = 0xff000021;
//            NSData *firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
//            [SDKLibCommand firmwareUpdateApplyWithDestination:node.address companyID:cid firmwareID:firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
//                TeLogDebug(@"firmwareUpdateApply=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            }];
//        }
//    }

    //12.firmwareDistributionStop
//    for (SigNodeModel *node in self.selectItemArray) {
//        if (node.state != DeviceStateOutOfLine) {
//            UInt16 cid = 0x0211;
//            UInt32 firmwareID = 0xff000021;
//            NSData *firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
//            [SDKLibCommand firmwareDistributionStopWithDestination:node.address companyID:cid firmwareID:firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
//                TeLogDebug(@"firmwareDistributionStop=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            }];
//        }
//    }
    
    //=================test==================//
}

- (void)testobjectTransferStart {
    for (SigNodeModel *node in self.selectItemArray) {
        if (node.state != DeviceStateOutOfLine) {
            UInt64 objID = 0x1122334455667788;
            NSData *otaData = [OTAFileSource.share getDataWithBinName:@"8258_mesh"];
            UInt32 objectSize = (UInt32)otaData.length;//bin文件的总字节数
            UInt8 blockSizeLog = 0x0C;//10^12=4096，表示meshOTA的一个block应该传递的最大bin文件字节数。
            [SDKLibCommand objectTransferStartWithDestination:node.address objectID:objID objectSize:objectSize blockSizeLog:blockSizeLog resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectTransferStatus * _Nonnull responseMessage) {
                TeLogDebug(@"objectTransferStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            }];
        }
    }
}

- (IBAction)clickStartMeshOTA:(UIButton *)sender {
    if (![MeshOTAManager share].isMeshOTAing) {
        [self startMeshOTA];
    }else{
        [self showTips:@"Device is meshOTAing, needn't call repeat."];
    }
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
                    for (SigNodeModel *model in weakSelf.allItemArray) {
                        if (model.state != DeviceStateOutOfLine) {
                            if (![weakSelf.selectItemArray containsObject:model]) {
                                [weakSelf.selectItemArray addObject:model];
                            }
                        }
                    }
                } else {
                    [weakSelf.selectItemArray removeAllObjects];
                }
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }];
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            int vid = 0;
            if ([self.allItemVIDDict.allKeys containsObject:@(model.address)]) {
                vid = [self.allItemVIDDict[@(model.address)] intValue];
            }
            itemCell.titleLabel.text = [NSString stringWithFormat:@"adr:0x%X PID:0x%@    VID:%c%c",model.address,model.pid,vid&0xff,(vid>>8)&0xff];//显示两个字节的ASCII
            if (self.selectItemArray.count > 0) {
                itemCell.selectButton.selected = [self.selectItemArray containsObject:model];
            } else {
                itemCell.selectButton.selected = NO;
            }
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (model.state != DeviceStateOutOfLine) {
                    if ([weakSelf.selectItemArray containsObject:model]) {
                        [weakSelf.selectItemArray removeObject:model];
                    }else{
                        [weakSelf.selectItemArray addObject:model];
                    }
                    [weakSelf.tableView reloadData];
                }
            }];
        }
    } else {
        NSString *binString = self.binStringArray[indexPath.row];
        itemCell.titleLabel.text = binString;
        itemCell.selectButton.selected = indexPath.row == _binIndex;
        [itemCell.selectButton addAction:^(UIButton *button) {
            weakSelf.binIndex = indexPath.row;
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
                for (SigNodeModel *model in self.allItemArray) {
                    if (model.state != DeviceStateOutOfLine) {
                        if (![weakSelf.selectItemArray containsObject:model]) {
                            [weakSelf.selectItemArray addObject:model];
                        }
                    }
                }
            } else {
                [weakSelf.selectItemArray removeAllObjects];
            }
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            if (model.state != DeviceStateOutOfLine) {
                if ([self.selectItemArray containsObject:model]) {
                    [self.selectItemArray removeObject:model];
                }else{
                    [self.selectItemArray addObject:model];
                }
            }
        }
        [self.tableView reloadData];
    } else {
        if (_binIndex != indexPath.row) {
            _binIndex = indexPath.row;
            [self.tableView reloadData];
        }
    }
}

- (void)startMeshOTA{
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
    for (SigNodeModel *model in self.selectItemArray) {
        [tem addObject:@(model.address)];
    }
    
    __weak typeof(self) weakSelf = self;
    [MeshOTAManager.share startMeshOTAWithLocationAddress:SigDataSource.share.curLocationNodeModel.address cid:0x0211 deviceAddresses:tem otaData:data progressHandle:^(NSInteger progress) {
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
    }];
}

- (void)userAbled:(BOOL)able{
    self.startButton.enabled = able;
    self.tableView.userInteractionEnabled = able;
    [self setStartButtonEnable:able];
}

-(void)dealloc{
    TeLogDebug(@"%s",__func__);
}

@end
