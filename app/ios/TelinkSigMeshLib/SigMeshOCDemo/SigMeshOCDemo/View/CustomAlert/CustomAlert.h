/********************************************************************************************************
 * @file     CustomAlert.h
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

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    ItemType_Choose,
    ItemType_Input,
} ItemType;

@interface AlertItemModel : NSObject
@property (nonatomic,assign) ItemType itemType;
@property(nonatomic,strong) NSString *headerString;
@property(nonatomic,strong) NSString *defaultString;//choose的默认选中选，input的默认字符串
@property(nonatomic,strong) NSMutableArray <NSString *>*chooseItemsArray;//Item选项文本

@end

@interface CustomAlert : UIView
@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UILabel *detailLabel;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *tableVewHeightConstraint;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *leftBtn;
@property (weak, nonatomic) IBOutlet UIButton *rightBtn;

@property(nonatomic,strong) NSString *title;
@property(nonatomic,strong) NSString *detail;
@property(nonatomic,strong) NSMutableArray <AlertItemModel *>*itemArray;
@property(nonatomic,strong) NSString *leftBtnTitle;
@property(nonatomic,strong) NSString *rightBtnTitle;

- (UITextField * _Nullable)getTextFieldOfRow:(NSInteger)row;
- (BOOL)getSelectLeftOfRow:(NSInteger)row;

@end

NS_ASSUME_NONNULL_END
