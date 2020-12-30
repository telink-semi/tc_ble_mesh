/********************************************************************************************************
 * @file     OnOffItemCell.h
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
//  OnOffItemCell.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/11/30.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OnOffItemCell : UICollectionViewCell
@property (weak, nonatomic) IBOutlet UIView *onoffView;
@property (weak, nonatomic) IBOutlet UISwitch *onoffSwitch;
@property (weak, nonatomic) IBOutlet UILabel *onoffLabel;
@property (copy, nonatomic) void(^clickSwitchBlock)(UISwitch *uiswitch);
@end

NS_ASSUME_NONNULL_END
