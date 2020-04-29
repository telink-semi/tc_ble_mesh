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
//  Created by Liangjiazhi on 2019/1/24.
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
        [Bluetooth.share cancelAllConnecttionWithComplete:^{
            [weakSelf loadJsonData:jsonData jaonName:jsonName];
        }];
    }];
    [self.navigationController pushViewController:vc animated:YES];

}

///加载json文件到本地(json data->SigDataSource，核心接口writeDataSourceToLib。)
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
            [weakSelf showTips:tipString];
            TeLog(@"%@",tipString);
        } else {
            NSString *tipString = [NSString stringWithFormat:@"import %@ fail!",name];
            [weakSelf showTips:tipString];
            TeLog(@"%@",tipString);
            return;
        }

        BOOL needChangeProvisionAddress = NO;
        BOOL hasPhoneUUID = NO;
        NSString *curPhoneUUID = [SigDataSource.share getCurrentProvisionerUUID];
        for (SigProvisionerModel *provision in SigDataSource.share.provisioners) {
            if ([provision.UUID isEqualToString:curPhoneUUID]) {
                hasPhoneUUID = YES;
                break;
            }
        }
        BOOL reStartSequenceNumber = NO;
        if (hasPhoneUUID) {
            // v3.1.0 存在
            BOOL isSameMesh = [SigDataSource.share.meshUUID isEqualToString:oldMeshUUID];
            if (isSameMesh) {
                // v3.1.0 存在，且为相同mesh网络，覆盖JSON，且使用本地的sno和kCurrentMeshProvisionAddress_key
                needChangeProvisionAddress = NO;
                reStartSequenceNumber = NO;;
            } else {
                // v3.1.0 存在，但为不同mesh网络，获取provision，修改为新的provisionLocation adress，sno从0开始
                needChangeProvisionAddress = YES;
                reStartSequenceNumber = YES;
            }
        } else {
            // v3.1.0 不存在，覆盖并新建provision
            needChangeProvisionAddress = NO;
            reStartSequenceNumber = YES;
        }
        if (needChangeProvisionAddress) {
            //修改provisionLocation adress
            UInt16 maxAddr = SigDataSource.share.curLocationNodeModel.address;
            for (SigNodeModel *node in SigDataSource.share.nodes) {
                NSInteger curMax = node.address + node.elements.count - 1;
                if (curMax > maxAddr) {
                    maxAddr = curMax;
                }
            }
            UInt16 newProvisionAddress = maxAddr + 1;
            [SigDataSource.share saveLocationProvisionAddress:newProvisionAddress];
            [SigDataSource.share changeLocationProvisionerNodeAddressToAddress:newProvisionAddress];
            TeLog(@"已经使用了address=0x%x作为本地地址",newProvisionAddress);
        } else {
            //新建或者覆盖
            if (reStartSequenceNumber) {
                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kCurrentMeshProvisionAddress_key];
                [SigDataSource.share setLocationSno:0];
            }
        }
        [SigDataSource.share checkExistLocationProvisioner];
        [SigDataSource.share writeDataSourceToLib];
        [SigDataSource.share.scanList removeAllObjects];
        [SigDataSource.share.matchsNodeIdentityArray removeAllObjects];
        [SigDataSource.share.noMatchsNodeIdentityArray removeAllObjects];
        init_json();
//        mesh_flash_retrieve();
    }];
}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
