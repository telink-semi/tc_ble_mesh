/********************************************************************************************************
 * @file     CustomAlert.h
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

- (UITextField *)getTextFieldOfRow:(NSInteger)row;
- (BOOL)getSelectLeftOfRow:(NSInteger)row;

@end

NS_ASSUME_NONNULL_END
