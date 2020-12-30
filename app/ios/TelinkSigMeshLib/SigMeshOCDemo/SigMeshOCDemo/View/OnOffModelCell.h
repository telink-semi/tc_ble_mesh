//
//  OnOffModelCell.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/11/24.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class OnOffModelCell;

@protocol OnOffModelCellDelegate <NSObject>
@optional
- (void)onOffModelCell:(OnOffModelCell *)cell indexPath:(nonnull NSIndexPath *)indexPath didChangedValueWithValue:(BOOL)value;
@end


@interface OnOffModelCell : UITableViewCell
@property (strong, nonatomic) NSMutableArray <NSNumber *>*onoffAddressSource;
@property (strong, nonatomic) NSMutableArray <NSNumber *>*onoffStateSource;

@property (weak, nonatomic) id <OnOffModelCellDelegate>delegate;

- (void)refreshUI;
+ (CGFloat)getOnOffModelCellHightOfItemCount:(NSInteger)itemCount;

@end

NS_ASSUME_NONNULL_END
