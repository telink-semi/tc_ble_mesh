/********************************************************************************************************
 * @file     CustomAlert.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/9/16
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

#import "CustomAlert.h"
#import "ChooseItemCell.h"
#import "InputItemCell.h"
#import "UIButton+extension.h"

@implementation AlertItemModel
@end

@interface CustomAlert ()<UITableViewDelegate,UITableViewDataSource>

@end

@implementation CustomAlert

- (void)awakeFromNib {
    [super awakeFromNib];
    self.layer.cornerRadius = 7;
    self.layer.masksToBounds = YES;
    _tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([ChooseItemCell class]) bundle:nil] forCellReuseIdentifier:@"ChooseItemCell"];
    [_tableView registerNib:[UINib nibWithNibName:NSStringFromClass([InputItemCell class]) bundle:nil] forCellReuseIdentifier:@"InputItemCell"];
    _tableView.dataSource = self;
    _tableView.delegate = self;
    _itemArray = [NSMutableArray array];
}

- (void)setTitle:(NSString *)title {
    _title = title;
    self.titleLabel.text = title;
}

- (void)setDetail:(NSString *)detail {
    _detail = detail;
    self.detailLabel.text = detail;
}

- (void)setItemArray:(NSMutableArray<AlertItemModel *> *)itemArray {
    _itemArray = itemArray;
    self.tableVewHeightConstraint.constant = 45 * itemArray.count;
}

- (void)setLeftBtnTitle:(NSString *)leftBtnTitle {
    _leftBtnTitle = leftBtnTitle;
    [self.leftBtn setTitle:_leftBtnTitle forState:UIControlStateNormal];
}

- (void)setRightBtnTitle:(NSString *)rightBtnTitle {
    _rightBtnTitle = rightBtnTitle;
    [self.rightBtn setTitle:_rightBtnTitle forState:UIControlStateNormal];
}

- (UITextField *)getTextFieldOfRow:(NSInteger)row {
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:row inSection:0];
    UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
    if ([cell isKindOfClass:[InputItemCell class]]) {
        return ((InputItemCell *)cell).inputTF;
    }
    return nil;
}

- (BOOL)getSelectLeftOfRow:(NSInteger)row {
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:row inSection:0];
    UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
    if ([cell isKindOfClass:[ChooseItemCell class]]) {
        return ((ChooseItemCell *)cell).leftButton.isSelected;
    }
    return NO;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.itemArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    AlertItemModel *model = self.itemArray[indexPath.row];
    if (model.itemType == ItemType_Choose) {
        ChooseItemCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ChooseItemCell"];
        cell.headerLabel.text = model.headerString;
        cell.leftLabel.text = model.chooseItemsArray.firstObject;
        cell.rightLabel.text = model.chooseItemsArray.lastObject;
        cell.leftButton.selected = [model.chooseItemsArray.firstObject isEqualToString:model.defaultString];
        cell.rightButton.selected = [model.chooseItemsArray.lastObject isEqualToString:model.defaultString];
        [cell.leftButton addAction:^(UIButton *button) {
            cell.leftButton.selected = YES;
            cell.rightButton.selected = NO;
        }];
        [cell.rightButton addAction:^(UIButton *button) {
            cell.rightButton.selected = YES;
            cell.leftButton.selected = NO;
        }];
        return cell;
    } else {
        InputItemCell *cell = [tableView dequeueReusableCellWithIdentifier:@"InputItemCell"];
        cell.headerLabel.text = model.headerString;
        cell.inputTF.text = model.defaultString;
        return cell;
    }
}


@end
