/********************************************************************************************************
 * @file     ShareOutVC.m
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
//  ShareOutVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2019/1/24.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "ShareOutVC.h"
#import "UIViewController+Message.h"

@interface ShareOutVC ()
@property (weak, nonatomic) IBOutlet UITextView *outTipTextView;

@end

@implementation ShareOutVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.outTipTextView.text = @"Export JSON:\n\n1. Click EXPORT button to create a new json file.\n2. Iphone connect to computer that install iTunes.\n3. Click on the iTunes phone icon in the upper left corner of iTunes into the iphone interface.\n4. Select \"file sharing\" in the left of the iTunes, then find and click on the demo APP in the application of \"TelinkSigMesh\", wait for iTunes load file.\n5. After file is loaded, found file \"mesh-.json\"in the \"TelinkSigMesh\".You just must to drag the file to your computer.\n\n导出JSON数据操作，步骤如下：\n\n1. 点击APP的EXPORT按钮，生成新的json文件。\n2. 将手机连接到安装了iTunes的电脑上。\n3. 点击iTunes左上角的手机图标进入iTunes设备详情界面。\n4. 选择iTunes左侧的“文件共享”，然后在应用中找到并点击demo APP “TelinkSigMesh”，等待iTunes加载文件。\n5. 文件加载完成后，在“TelinkSigMesh”的文稿中找到mesh的分享数据文件“mesh-.json”，把该文件拖到电脑即可。";
}

- (IBAction)clickExportButton:(UIButton *)sender {
    //导出json文件名为“mesh-时间.json”
    NSDate *date = [NSDate date];
    NSDateFormatter *f = [[NSDateFormatter alloc] init];
    f.dateFormat = @"yyyy-MM-dd-HH:mm:ss.SSS";
    f.locale = [[NSLocale alloc] initWithLocaleIdentifier:@"zh_CN"];
    NSString *dstr = [f stringFromDate:date];
    NSString *jsonName = [NSString stringWithFormat:@"mesh-%@.json",dstr];
    
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:jsonName];

    NSFileManager *manager = [[NSFileManager alloc] init];
    BOOL exist = [manager fileExistsAtPath:path];
    if (!exist) {
        BOOL ret = [manager createFileAtPath:path contents:nil attributes:nil];
        if (ret) {
            NSDictionary *jsonDict = [SigDataSource.share getFormatDictionaryFromDataSource];
            NSData *tempData = [LibTools getJSONDataWithDictionary:jsonDict];
//            NSData *tempData = [SigDataSource.share.getJsonStringFromeDataSource dataUsingEncoding:NSUTF8StringEncoding];
            NSFileHandle *handle = [NSFileHandle fileHandleForWritingAtPath:path];
            [handle truncateFileAtOffset:0];
            [handle writeData:tempData];
            [handle closeFile];
            
            NSString *tipString = [NSString stringWithFormat:@"export %@ success!",jsonName];
            [self showTips:tipString];
            TeLogDebug(@"%@",tipString);
        } else {
            NSString *tipString = [NSString stringWithFormat:@"export %@ fail!",jsonName];
            [self showTips:tipString];
            TeLogDebug(@"%@",tipString);
        }
    }
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
