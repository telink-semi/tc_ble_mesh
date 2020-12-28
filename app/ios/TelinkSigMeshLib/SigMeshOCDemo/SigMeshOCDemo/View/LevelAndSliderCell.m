//
//  LevelAndSliderCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/11/24.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "LevelAndSliderCell.h"

@implementation LevelAndSliderCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
}

- (IBAction)changedValue:(UISlider *)sender {
    if ([self.delegate respondsToSelector:@selector(levelAndSliderCell:didChangedValueWithValue:)]) {
        [self.delegate levelAndSliderCell:self didChangedValueWithValue:sender.value];
    }
}

- (IBAction)clickDelete:(UIButton *)sender {
    if ([self.delegate respondsToSelector:@selector(levelAndSliderCellDidClickedDeleteButton:)]) {
        [self.delegate levelAndSliderCellDidClickedDeleteButton:self];
    }
}

- (IBAction)clickAdd:(UIButton *)sender {
    if ([self.delegate respondsToSelector:@selector(levelAndSliderCellDidClickedAddButton:)]) {
        [self.delegate levelAndSliderCellDidClickedAddButton:self];
    }
}

@end
