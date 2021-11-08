/********************************************************************************************************
 * @file     CustomAlert.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/9/16
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
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
