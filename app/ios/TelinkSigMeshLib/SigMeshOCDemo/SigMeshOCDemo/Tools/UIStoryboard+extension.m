/********************************************************************************************************
 * @file     UIStoryboard+extension.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  UIStoryboard+extension.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "UIStoryboard+extension.h"

@implementation UIStoryboard (extension)

+ (UIViewController *)initVC:(NSString *)name{
    return [self initVC:name storybroad:@"Main"];
}

+ (UIViewController *)initVC:(NSString *)name storybroad:(NSString *)storybroadName{
    UIStoryboard *storybroad = [UIStoryboard storyboardWithName:storybroadName bundle:nil];
    UIViewController *vc = [storybroad instantiateViewControllerWithIdentifier:name];
    return vc;
}

@end
