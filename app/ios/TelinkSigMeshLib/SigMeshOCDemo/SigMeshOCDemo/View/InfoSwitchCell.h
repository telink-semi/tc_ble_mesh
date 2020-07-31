//
//  InfoSwitchCell.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/6/5.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface InfoSwitchCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UILabel *showLabel;
@property (weak, nonatomic) IBOutlet UISwitch *showSwitch;

@end

NS_ASSUME_NONNULL_END
