/********************************************************************************************************
 * @file     CustomShareAlert.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/18
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CustomShareAlert.h"
#import "ShareItemCell.h"
#import "ShareButtonCell.h"
#import "UIButton+extension.h"

@implementation ShareAlertItemModel
@end

@interface CustomShareAlert ()<UITableViewDelegate,UITableViewDataSource>

@end

@implementation CustomShareAlert

- (void)awakeFromNib {
    [super awakeFromNib];
    self.layer.cornerRadius = 7;
    self.layer.masksToBounds = YES;
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass(ShareItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(ShareItemCell.class)];
    [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass(ShareButtonCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(ShareButtonCell.class)];
    _itemArray = [NSMutableArray array];
    _tableView.dataSource = self;
    _tableView.delegate = self;
}

- (void)setTitle:(NSString *)title {
    _title = title;
    self.titleLabel.text = title;
}

- (void)setItemArray:(NSMutableArray<ShareAlertItemModel *> *)itemArray {
    _itemArray = itemArray;
    self.tableVewHeightConstraint.constant = [self getHightOfItemArray:itemArray];
}

- (CGFloat)getAlertHightWithItemArray:(NSMutableArray<ShareAlertItemModel *> *)itemArray {
    return [self getHightOfItemArray:itemArray] + 45;
}

- (CGFloat)getHightOfItemArray:(NSMutableArray<ShareAlertItemModel *> *)itemArray {
    BOOL hasDelete = NO;
    NSInteger titleCount = 0;
    NSArray *array = [NSArray arrayWithArray:itemArray];
    for (ShareAlertItemModel *model in array) {
        if (model.itemType == ShareItemType_button) {
            hasDelete = YES;
        } else {
            titleCount += 1;
        }
    }
    CGFloat hight = 45 * titleCount + 70 * (hasDelete ? 1 : 0);
    return hight;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.itemArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ShareAlertItemModel *model = self.itemArray[indexPath.row];
    __weak typeof(self) weakSelf = self;
    if (model.itemType == ShareItemType_button) {
        ShareButtonCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(ShareButtonCell.class)];
        cell.accessoryType = UITableViewCellAccessoryNone;
        [cell.deleteButton setTitle:model.textString forState:UIControlStateNormal];
        [cell.deleteButton addAction:^(UIButton *button) {
            [weakSelf clickIndex:indexPath.row];
        }];
        return cell;
    } else {
        ShareItemCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(ShareItemCell.class)];
        cell.titleLabel.text = model.textString;
        cell.titleLabel.textColor = model.textColor;
        cell.accessoryType = model.itemType == ShareItemType_icon_text_next ? UITableViewCellAccessoryDisclosureIndicator : UITableViewCellAccessoryNone;
        cell.iconWidthConstraint.constant = model.itemType == ShareItemType_text ? 0 : 25;
        if (model.itemType != ShareItemType_text) {
            cell.iconImageView.image = model.iconImage;
        }
        return cell;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    [self clickIndex:indexPath.row];
}

- (void)clickIndex:(NSInteger)index {
    if (self.backClickIndexBlock) {
        self.backClickIndexBlock(index);
    }
}

@end
