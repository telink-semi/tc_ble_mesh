/********************************************************************************************************
 * @file     TableCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/11/23
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

#import "TableCell.h"

@implementation TableCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    [self configurationCornerWithBgView:_bgView];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setModel:(SigForwardingTableModel *)model {
    _model = model;
    for (UIView *subview in self.bgView.subviews) {
        if (subview != _originBgView && subview != _targetBgView && subview != _backwardBgView && subview != _routeBgView && subview != _nodeBgView) {
            [subview removeFromSuperview];
        }
    }
    _originLabel.text = [NSString stringWithFormat:@"Name-%@, Adr-0x%04X", [SigDataSource.share getNodeWithAddress:model.tableSource].name, model.tableSource];
    _targetLabel.text = [NSString stringWithFormat:@"Name-%@, Adr-0x%04X", [SigDataSource.share getNodeWithAddress:model.tableDestination].name, model.tableDestination];
    for (int i=0; i<model.entryNodeAddress.count; i++) {
        UILabel *nodeLabel = nil;
        if (i == 0) {
            nodeLabel = [self getNodeNameLabelWithView:_nodeBgView];
        } else {
            UIView *nextNodeBgView = [[UIView alloc] initWithFrame:CGRectMake(0, _nodeBgView.frame.origin.y+40*i, _nodeBgView.frame.size.width, _nodeBgView.frame.size.height)];
            [_bgView addSubview:nextNodeBgView];
            UIImageView *nextImageView = [[UIImageView alloc] initWithFrame:CGRectMake(25, 10, 20, 20)];
            nextImageView.image = [UIImage imageNamed:@"dengs"];
            [nextNodeBgView addSubview:nextImageView];
            UILabel *oldLabel = [self getNodeNameLabelWithView:_nodeBgView];
            nodeLabel = [[UILabel alloc] initWithFrame:oldLabel.frame];
            nodeLabel.font = oldLabel.font;
            nodeLabel.textColor = oldLabel.textColor;
            [nextNodeBgView addSubview:nodeLabel];
        }
        nodeLabel.text = [NSString stringWithFormat:@"Name-%@, Adr-0x%04X", [SigDataSource.share getNodeWithAddress:[model.entryNodeAddress[i] intValue]].name, [model.entryNodeAddress[i] intValue]];
    }
}

- (UILabel *)getNodeNameLabelWithView:(UIView *)view {
    UILabel *label = nil;
    for (UIView *subview in view.subviews) {
        if ([subview isMemberOfClass:UILabel.class]) {
            label = (UILabel *)subview;
            break;
        }
    }
    return label;
}

+ (CGFloat)getHeightOfModel:(SigForwardingTableModel *)model {
    CGFloat height = 7+7+45*4+40*model.entryNodeAddress.count;
    return height;
}

@end
