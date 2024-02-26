/********************************************************************************************************
 * @file     UIView+Frame.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/12/12
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

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIView (Frame)

/** 坐标 origin */
@property (nonatomic, assign) CGPoint origin;
/** 坐标 size */
@property (nonatomic, assign) CGSize size;

/** 坐标 width */
@property (nonatomic, assign) CGFloat width;
/** 坐标 height */
@property (nonatomic, assign) CGFloat height;

/** 坐标 x */
@property (nonatomic, assign) CGFloat x;
/** 坐标 y */
@property (nonatomic, assign) CGFloat y;
/** 坐标 maxX */
@property (nonatomic, assign) CGFloat maxX;
/** 坐标 maxY */
@property (nonatomic, assign) CGFloat maxY;
/** 坐标 centerX */
@property (nonatomic, assign) CGFloat centerX;
/** 坐标 centerY */
@property (nonatomic, assign) CGFloat centerY;

@end

NS_ASSUME_NONNULL_END
