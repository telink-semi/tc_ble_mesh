/********************************************************************************************************
 * @file     OnOffModelCell.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/11/24
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "OnOffModelCell.h"
#import "OnOffItemCell.h"

@interface OnOffModelCell ()<UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@end

@implementation OnOffModelCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_OnOffItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_OnOffItemCellID];
    self.collectionView.dataSource = self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];
}

- (void)changedValue:(UISwitch *)sender indexPath:(nonnull NSIndexPath *)indexPath {
    if ([self.delegate respondsToSelector:@selector(onOffModelCell:indexPath:didChangedValueWithValue:)]) {
        [self.delegate onOffModelCell:self indexPath:indexPath didChangedValueWithValue:sender.on];
    }
}

#pragma mark - UICollectionViewDelegate
- (nonnull __kindof UICollectionViewCell *)collectionView:(nonnull UICollectionView *)collectionView cellForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
    OnOffItemCell *item = [collectionView dequeueReusableCellWithReuseIdentifier:CellIdentifiers_OnOffItemCellID forIndexPath:indexPath];
    NSNumber *ele_adr = self.onoffAddressSource[indexPath.item];
    item.onoffLabel.text = [NSString stringWithFormat:@"ele adr:0x%X",ele_adr.intValue];
    __weak typeof(self) weakSelf = self;
    [item setClickSwitchBlock:^(UISwitch * _Nonnull uiswitch) {
        [weakSelf changedValue:uiswitch indexPath:indexPath];
//        //0.不带渐变且无回包写法：
//        //attention: resMax change to 0, because node detail vc needn't response.
////        [DemoCommand switchOnOffWithIsOn:swift.isOn address:ele_adr.intValue responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
//        //1.不带渐变写法：
//        [DemoCommand switchOnOffWithIsOn:swift.isOn address:ele_adr.intValue responseMaxCount:1 ack:YES successCallback:nil resultCallback:nil];
//
//        //2.带渐变写法：Sending message:SigGenericOnOffSet->Access PDU, source:(0x0001)->destination: (0x0002) Op Code: (0x8202), accessPdu=820200174100
////        SigTransitionTime *transitionTime = [[SigTransitionTime alloc] initWithSetps:1 stepResolution:SigStepResolution_seconds];
////        [SDKLibCommand genericOnOffSetDestination:ele_adr.intValue isOn:swift.isOn transitionTime:transitionTime delay:0 retryCount:2 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
////            TelinkLogInfo(@"source=0x%x,destination=0x%x,responseMessage=%@",source,destination,responseMessage);
////        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
////            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
////        }];
    }];
    //Attention: panel has 8 onoff button, but node just has one onoff data, developer should fix that when your app has panel.
//    if (indexPath.item == 0) {
//        item.onoffSwitch.on = self.model.state == DeviceStateOn;
//    }
    if (indexPath.item < self.onoffStateSource.count) {
        item.onoffSwitch.on = [self.onoffStateSource[indexPath.item] boolValue];
    }
    return item;
}

- (NSInteger)collectionView:(nonnull UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.onoffAddressSource.count;
}

- (void)refreshUI {
    [self.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

+ (CGFloat)getOnOffModelCellHeightOfItemCount:(NSInteger)itemCount {
    CGRect rx = [UIScreen mainScreen].bounds;
    CGFloat a = floorf((rx.size.width - 8) / (CGFloat)(70 + 8));
    CGFloat  h = (70.0 + 8) * ceilf(itemCount/a);
    return h + 8;
}

@end
