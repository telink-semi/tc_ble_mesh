/********************************************************************************************************
 * @file     ShareInVC.m
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
//  ShareInVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2019/1/24.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "ShareInVC.h"
#import "FileChooseVC.h"
#import "UIViewController+Message.h"

@interface ShareInVC ()
@property (weak, nonatomic) IBOutlet UITextView *inTipTextView;

@end

@implementation ShareInVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.inTipTextView.text = @"Import JSON:\n\n1. Iphone connect to computer that install iTunes.\n2. Click on the iTunes phone icon in the upper left corner of iTunes into the iphone interface.\n3. Select \"file sharing\" in the left of the iTunes, then find and click on the demo APP in the application of \"TelinkSigMesh\", wait for iTunes load file.\n4. After file is loaded, drag the files on the computer \"mesh.json\" into the right side of the \"TelinkSigMesh\", replace the old file and reopen the APP, the APP will load json data file automatically.\n5. Click IMPORT button to choose new json file and load it.\n\n导入JSON数据操作，步骤如下：\n\n1. 将手机连接到安装了iTunes的电脑上。\n2. 点击iTunes左上角的手机图标进入iTunes设备详情界面。\n3. 选择iTunes左侧的“文件共享”，然后在应用中找到并点击demo APP “TelinkSigMesh”，等待iTunes加载文件。\n4. 文件加载完成后，将电脑上的json文件拖入右侧的“TelinkSigMesh”的文稿中。\n5. APP点击IMPORT按钮选择刚刚的JSON文件进行加载。";
}

- (IBAction)clickImportButton:(UIButton *)sender {
    FileChooseVC *vc = (FileChooseVC *)[UIStoryboard initVC:ViewControllerIdentifiers_FileChooseViewControllerID storybroad:@"Setting"];
    __weak typeof(self) weakSelf = self;
    [vc setBackJsonData:^(NSData * _Nonnull jsonData, NSString * _Nonnull jsonName) {
        [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
            TeLogDebug(@"SigBearer close %@",(successful?@"successful":@"fail"));
            [weakSelf loadJsonData:jsonData jaonName:jsonName];
        }];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

///加载json文件到本地(json data->SigDataSource。)
- (void)loadJsonData:(NSData *)data jaonName:(NSString *)name{
    NSOperationQueue *operation = [[NSOperationQueue alloc] init];
    __weak typeof(self) weakSelf = self;
    [operation addOperationWithBlock:^{
        NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSString *oldMeshUUID = SigDataSource.share.meshUUID;
        NSDictionary *dict = [LibTools getDictionaryWithJsonString:str];
        [SigDataSource.share setDictionaryToDataSource:dict];
        BOOL result = dict != nil;
        if (result) {
            NSString *tipString = [NSString stringWithFormat:@"import %@ success!",name];
            TeLogDebug(@"%@",tipString);
            [weakSelf showTips:tipString];
        } else {
            NSString *tipString = [NSString stringWithFormat:@"import %@ fail!",name];
            [weakSelf showTips:tipString];
            TeLogDebug(@"%@",tipString);
            return;
        }

        BOOL needChangeProvisionerAddress = NO;//修改手机本地节点的地址
        BOOL reStartSequenceNumber = NO;//修改手机本地节点使用的发包序列号sno
        BOOL hasPhoneUUID = NO;
        NSString *curPhoneUUID = [SigDataSource.share getCurrentProvisionerUUID];
        NSArray *provisioners = [NSArray arrayWithArray:SigDataSource.share.provisioners];
        for (SigProvisionerModel *provision in provisioners) {
            if ([provision.UUID isEqualToString:curPhoneUUID]) {
                hasPhoneUUID = YES;
                break;
            }
        }
        if (hasPhoneUUID) {
            // v3.1.0, 存在
            BOOL isSameMesh = [SigDataSource.share.meshUUID isEqualToString:oldMeshUUID];
            if (isSameMesh) {
                // v3.1.0, 存在，且为相同mesh网络，覆盖JSON，且使用本地的sno和ProvisionerAddress
                needChangeProvisionerAddress = NO;
                reStartSequenceNumber = NO;
            } else {
                // v3.1.0, 存在，但为不同mesh网络，获取provision，修改为新的ProvisionerAddress，sno从0开始
                needChangeProvisionerAddress = YES;
                reStartSequenceNumber = YES;
            }
        } else {
            // v3.1.0, 不存在，覆盖并新建provisioner
            needChangeProvisionerAddress = NO;
            reStartSequenceNumber = YES;
        }
        
        //重新计算sno
        if (reStartSequenceNumber) {
            [[NSUserDefaults standardUserDefaults] removeObjectForKey:kCurrentMeshProvisionAddress_key];
            [SigDataSource.share setLocationSno:0];
        }

        UInt16 maxAddr = SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress;
        NSArray *nodes = [NSArray arrayWithArray:SigDataSource.share.nodes];
        for (SigNodeModel *node in nodes) {
            NSInteger curMax = node.address + node.elements.count - 1;
            if (curMax > maxAddr) {
                maxAddr = curMax;
            }
        }
        if (needChangeProvisionerAddress) {
            //修改手机的本地节点的地址
            UInt16 newProvisionAddress = maxAddr + 1;
            [SigDataSource.share changeLocationProvisionerNodeAddressToAddress:newProvisionAddress];
            TeLogDebug(@"已经使用了address=0x%x作为本地地址",newProvisionAddress);
            //修改下一次添加设备使用的地址
            [SigDataSource.share saveLocationProvisionAddress:maxAddr + 1];
        } else {
            //修改下一次添加设备使用的地址
            [SigDataSource.share saveLocationProvisionAddress:maxAddr];
        }
        TeLogDebug(@"下一次添加设备可以使用的地址address=0x%x",SigDataSource.share.provisionAddress);

//        SigDataSource.share.curNetkeyModel = nil;
//        SigDataSource.share.curAppkeyModel = nil;
        [SigDataSource.share checkExistLocationProvisioner];
        [SigDataSource.share saveLocationData];
        [SigDataSource.share.scanList removeAllObjects];
    }];
}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

@end
