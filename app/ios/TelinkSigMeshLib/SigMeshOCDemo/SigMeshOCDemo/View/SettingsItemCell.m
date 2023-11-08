/********************************************************************************************************
 * @file     SettingsItemCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/12
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SettingsItemCell.h"

#define kTitleHeight    (40.0)

@interface SettingsItemCell ()
@property (nonatomic, strong) UIView *bgView;
@property (nonatomic, strong) NSMutableArray <UIButton *>*itemButtons;
@end

@implementation SettingsItemCell

+ (CGFloat)getCellHeightWithItems:(NSArray <NSString *>*)items {
    return 10+5+kTitleHeight+5+10+(30+10)*items.count;
}

- (void)configUIWithTitle:(NSString *)title items:(NSArray <NSString *>*)items {
    //backgroundColor
    self.backgroundColor = [UIColor colorNamed:@"telinkTabBarBackgroundColor"];
    //removeFromSuperview
    if (self.bgView) {
        [self.bgView removeFromSuperview];
    }
    //init bgView
    self.bgView = [[UIView alloc] initWithFrame:CGRectMake(10, 10, self.frame.size.width-20, 5+kTitleHeight+5+(30+10)*items.count)];
    //cornerRadius
    self.bgView.layer.cornerRadius = 7;
    //borderWidth
    self.bgView.layer.borderWidth = 1;
    //borderColor
    self.bgView.layer.borderColor = [UIColor telinkBorderColor].CGColor;
    //masksToBounds
    self.bgView.layer.masksToBounds = YES;
    //backgroundColor
    self.bgView.backgroundColor = UIColor.telinkBackgroundWhite;
    //addSubview
    [self addSubview:self.bgView];

    //title label
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 5, self.bgView.frame.size.width-20-51-10-30, kTitleHeight)];
    titleLabel.text = title;
    titleLabel.numberOfLines = 0;
    titleLabel.textColor = [UIColor telinkTitleBlack];
    titleLabel.font = [UIFont boldSystemFontOfSize:13.0];
    [self.bgView addSubview:titleLabel];
    
    //tips button
    self.tipsButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.tipsButton setImage:[UIImage imageNamed:@"tishi"] forState:UIControlStateNormal];
    [self.bgView addSubview:self.tipsButton];

    if (items.count > 0) {
        //tips button
        self.tipsButton.frame = CGRectMake(self.bgView.frame.size.width-30-10, 10, 30, 30);
    } else {
        //switch button
        self.enableSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(self.bgView.frame.size.width-51-10, 10, 51, 31)];
        [self.bgView addSubview:self.enableSwitch];
        
        //tips button
        self.tipsButton.frame = CGRectMake(self.enableSwitch.frame.origin.x-10-30, 10, 30, 30);
    }

    //item view
    self.itemButtons = [NSMutableArray array];
    for (int i = 0; i < items.count; i++) {
        //init subView
        UIView *subView = [[UIView alloc] initWithFrame:CGRectMake(10, titleLabel.frame.origin.y + titleLabel.frame.size.height + (30+10)*i, self.bgView.frame.size.width-20, 40)];
        //backgroundColor
        subView.backgroundColor = [UIColor clearColor];
        //addSubview
        [self.bgView addSubview:subView];
        //add selectButton to Subview
        UIButton *selectButton = [UIButton buttonWithType:UIButtonTypeCustom];
        selectButton.frame = CGRectMake(0, 5, 30, 30);
        [selectButton setImage:[UIImage imageNamed:@"danxuan"] forState:UIControlStateNormal];
        [selectButton setImage:[UIImage imageNamed:@"danxuanxuanzhong"] forState:UIControlStateSelected];
        [selectButton addTarget:self action:@selector(clickSelectButton:) forControlEvents:UIControlEventTouchUpInside];
        [self.itemButtons addObject:selectButton];
        [subView addSubview:selectButton];
        //add itemLabel to Subview
        UILabel *itemLabel = [[UILabel alloc] initWithFrame:CGRectMake(selectButton.frame.origin.x+selectButton.frame.size.width, 5, subView.frame.size.width-30, 30)];
        itemLabel.text = items[i];
        itemLabel.textColor = [UIColor telinkTitleBlack];
        itemLabel.font = [UIFont systemFontOfSize:13.0];
        [subView addSubview:itemLabel];
    }
}

- (void)setSelectIndex:(NSInteger)selectIndex {
    _selectIndex = selectIndex;
    for (int i=0; i<self.itemButtons.count; i++) {
        UIButton *but = self.itemButtons[i];
        but.selected = i==selectIndex;
    }
}

- (void)clickSelectButton:(UIButton *)button {
    for (UIButton *but in self.itemButtons) {
        but.selected = NO;
    }
    button.selected = YES;
    if (self.backSelectIndexBlock) {
        self.backSelectIndexBlock([self.itemButtons indexOfObject:button]);
    }
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
