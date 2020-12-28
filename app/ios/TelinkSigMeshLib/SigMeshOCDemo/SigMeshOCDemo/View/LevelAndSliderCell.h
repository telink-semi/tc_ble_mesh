//
//  LevelAndSliderCell.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/11/24.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class LevelAndSliderCell;

@protocol LevelAndSliderCellDelegate <NSObject>
@optional
- (void)levelAndSliderCell:(LevelAndSliderCell *)cell didChangedValueWithValue:(CGFloat)value;
- (void)levelAndSliderCellDidClickedDeleteButton:(LevelAndSliderCell *)cell;
- (void)levelAndSliderCellDidClickedAddButton:(LevelAndSliderCell *)cell;
@end


@interface LevelAndSliderCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UILabel *showLevelLabel;
@property (weak, nonatomic) IBOutlet UILabel *showValueLabel;
@property (weak, nonatomic) IBOutlet UISlider *valueSlider;

@property (weak, nonatomic) id <LevelAndSliderCellDelegate>delegate;

@end

NS_ASSUME_NONNULL_END
