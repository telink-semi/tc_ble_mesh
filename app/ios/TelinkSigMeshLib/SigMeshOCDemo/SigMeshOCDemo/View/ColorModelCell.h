//
//  ColorModelCell.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/11/23.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ColorManager.h"
#import "ColorPickerView.h"

NS_ASSUME_NONNULL_BEGIN

@class ColorModelCell;

@protocol ColorModelCellDelegate <NSObject>
@optional
- (void)colorModelCell:(ColorModelCell *)cell didChangedColorWithColor:(UIColor *)color rgbModel:(RGBModel *)rgbModel hsvModel:(HSVModel *)hsvModel hslModel:(HSLModel *)hslModel;
@end


@interface ColorModelCell : UITableViewCell
@property (weak, nonatomic) IBOutlet ColorPickerView *colorPicker;
@property (strong, nonatomic) UIColor *colorModel;
@property (strong, nonatomic) RGBModel *rgbModel;
@property (strong, nonatomic) HSVModel *hsvModel;
@property (strong, nonatomic) HSLModel *hslModel;

@property (weak, nonatomic) id <ColorModelCellDelegate>delegate;

@end

NS_ASSUME_NONNULL_END
