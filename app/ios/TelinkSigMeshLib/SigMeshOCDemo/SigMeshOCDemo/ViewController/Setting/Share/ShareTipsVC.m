/********************************************************************************************************
 * @file     ShareTipsVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2021/1/26
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

#import "ShareTipsVC.h"
#import "UIViewController+Message.h"

@interface ShareTipsVC ()
@property (weak, nonatomic) IBOutlet UITextView *tipTextView;

@end

@implementation ShareTipsVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"Share Tips";
    self.tipTextView.text = @"Export JSON:\n\n1. Click EXPORT button to create a new json file.\n2. Iphone connect to computer that install iTunes.\n3. Click on the iTunes phone icon in the upper left corner of iTunes into the iphone interface.\n4. Select \"file sharing\" in the left of the iTunes, then find and click on the demo APP in the application of \"TelinkSigMesh\", wait for iTunes load file.\n5. After file is loaded, found file \"mesh-.json\"in the \"TelinkSigMesh\".You just must to drag the file to your computer.\n\n\nImport JSON:\n\n1. Iphone connect to computer that install iTunes.\n2. Click on the iTunes phone icon in the upper left corner of iTunes into the iphone interface.\n3. Select \"file sharing\" in the left of the iTunes, then find and click on the demo APP in the application of \"TelinkSigMesh\", wait for iTunes load file.\n4. After file is loaded, drag the files on the computer \"mesh.json\" into the right side of the \"TelinkSigMesh\", replace the old file and reopen the APP, the APP will load json data file automatically.\n5. Click IMPORT button to choose new json file and load it.\n\n\n\n导出JSON数据操作，步骤如下：\n\n1. 点击APP的EXPORT按钮，生成新的json文件。\n2. 将手机连接到安装了iTunes的电脑上。\n3. 点击iTunes左上角的手机图标进入iTunes设备详情界面。\n4. 选择iTunes左侧的“文件共享”，然后在应用中找到并点击demo APP “TelinkSigMesh”，等待iTunes加载文件。\n5. 文件加载完成后，在“TelinkSigMesh”的文稿中找到mesh的分享数据文件“mesh-.json”，把该文件拖到电脑即可。\n\n\n导入JSON数据操作，步骤如下：\n\n1. 将手机连接到安装了iTunes的电脑上。\n2. 点击iTunes左上角的手机图标进入iTunes设备详情界面。\n3. 选择iTunes左侧的“文件共享”，然后在应用中找到并点击demo APP “TelinkSigMesh”，等待iTunes加载文件。\n4. 文件加载完成后，将电脑上的json文件拖入右侧的“TelinkSigMesh”的文稿中。\n5. APP点击IMPORT按钮选择刚刚的JSON文件进行加载。";
}

@end
