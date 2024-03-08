/********************************************************************************************************
 * @file     CustomCircularButton.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/11/15
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

#import "CustomCircularButton.h"
#import "UIColor+Telink.h"

@implementation CustomCircularButton

- (instancetype)init {
    if (self = [super init]) {
        [self createUI];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self createUI];
    }
    return self;
}

- (void)createUI {
    self.backgroundView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 60, 60)];
    self.backgroundView.backgroundColor = HEX(#8F7BCB);
    [self addSubview:self.backgroundView];
    self.customButton = [UIButton buttonWithType:UIButtonTypeSystem];
    self.customButton.frame = CGRectMake(0, 0, 60, 60);
    [self addSubview:self.customButton];
    /// Set fillet related parameters.
    [self setCornerRadius];
}

/// Set fillet related parameters.
- (void)setCornerRadius {
    self.backgroundView.layer.cornerRadius = self.backgroundView.bounds.size.width/2;
    self.backgroundView.layer.masksToBounds = YES;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
