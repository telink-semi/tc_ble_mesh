/********************************************************************************************************
 * @file     ExtendBearerModeCell.h
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/6/28
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

typedef void(^BackExtendBearerModeCallback)(SigTelinkExtendBearerMode extendBearerMode);

@interface ExtendBearerModeCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UIButton *noExtendButton;
@property (weak, nonatomic) IBOutlet UIButton *extendGattOnlyButton;
@property (weak, nonatomic) IBOutlet UIButton *extendGattAndAdvButton;

@property (nonatomic, assign) SigTelinkExtendBearerMode extendBearerMode;
@property (nonatomic, copy) BackExtendBearerModeCallback block;

@end

NS_ASSUME_NONNULL_END
