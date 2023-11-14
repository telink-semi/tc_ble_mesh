/********************************************************************************************************
 * @file     RemoteElementCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/8/10
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "RemoteElementCell.h"
#import "NSString+extension.h"

@implementation RemoteElementCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (IBAction)clickAddressList:(UIButton *)sender {
    [self pushPublicAddressList];
}

- (void)pushPublicAddressList {
    NSArray *groups = [NSArray arrayWithArray:SigDataSource.share.getAllShowGroupList];
    //判断model合法性
    NSString *modelIdString = self.modelTF.text.removeAllSapceAndNewlines;
    if ([modelIdString isEqualToString:@"1002"]) {
        NSMutableArray *mArray = [NSMutableArray arrayWithArray:SigDataSource.share.getAllExtendGroupList];
        [mArray addObjectsFromArray:SigDataSource.share.getAllInvalidGroupList];
        groups = mArray;
    }
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Select Public Address" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    for (SigGroupModel *group in groups) {
        UIAlertAction *alertT = [UIAlertAction actionWithTitle:[NSString stringWithFormat:@"%@(0x%@)", group.name, group.address] style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            weakSelf.pubAdrTF.text = group.address;
        }];
        [actionSheet addAction:alertT];
    }
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"Cancel");
    }];
    [actionSheet addAction:alertF];
    actionSheet.popoverPresentationController.sourceView = self.pubAdrTF;
    actionSheet.popoverPresentationController.sourceRect =  self.pubAdrTF.frame;
    [self.vc presentViewController:actionSheet animated:YES completion:nil];
}

@end
