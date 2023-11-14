/********************************************************************************************************
 * @file     SchedulerCell.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/25
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SchedulerCell.h"

@interface SchedulerCell ()
@property (nonatomic, strong) NSArray *monthStrings;
@property (nonatomic, strong) NSArray *weekStrings;
@end

@implementation SchedulerCell{
    __weak IBOutlet UILabel *_nameLabel;
    __weak IBOutlet UILabel *_detailStateLabel;

}

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    self.monthStrings = @[@"Jan",@"Feb",@"Mar",@"Apr",@"May",@"Jun",@"Jul",@"Aug",@"Sept",@"Oct",@"Nov",@"Dec"];
    self.weekStrings = @[@"Mon",@"Tues",@"Wed",@"Thur",@"Fri",@"Sat",@"Sun"];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:NO animated:animated];
    // Configure the view for the selected state
}

- (void)updateContent:(SchedulerModel *)model{
    _nameLabel.text = [NSString stringWithFormat:@"SchedulerID:0x%llX",model.schedulerID];
    NSString *year,*month=@"",*day,*hour,*minute,*second,*week=@"",*action;
    //year
    year = [NSString stringWithFormat:@"%llu",model.year];
    if (model.year >= 0 && model.year <= 0x63) {
        year = [NSString stringWithFormat:@"%llu",model.year];
    }else if (model.year == 0x64){
        year = @"Any";
    }
    //month
    for (int i=0; i<12; i++) {
        BOOL isExist = ((model.month>>i)&0x1)==1;
        if (isExist) {
            if (month.length != 0) {
                month = [month stringByAppendingString:@"、"];
            }
            month = [month stringByAppendingString:self.monthStrings[i]];
        }
    }
    if (month.length == 0) {
        month = [NSString stringWithFormat:@"%llu",model.month];
    }
    //day
    day = [NSString stringWithFormat:@"%llu",model.day];
    if (model.day >= 1 && model.day <= 0x1F) {
        day = [NSString stringWithFormat:@"%llu",model.day];
    }else if (model.day == 0){
        day = @"Any";
    }
    //hour
    hour = [NSString stringWithFormat:@"%llu",model.hour];
    if (model.hour >= 0 && model.hour <= 0x17) {
        hour = [NSString stringWithFormat:@"%llu",model.hour];
    }else if (model.hour == 0x18){
        hour = @"Any";
    }else if (model.hour == 0x19){
        hour = @"Once a day";
    }
    //minute
    minute = [NSString stringWithFormat:@"%llu",model.minute];
    if (model.minute >= 0 && model.minute <= 0x3B) {
        minute = [NSString stringWithFormat:@"%llu",model.minute];
    }else if (model.minute == 0x3C){
        minute = @"Any";
    }else if (model.minute == 0x3D){
        minute = @"Every 15 minutes";
    }else if (model.minute == 0x3E){
        minute = @"Every 20 minutes";
    }else if (model.minute == 0x3F){
        minute = @"Once an hour";
    }
    //second
    second = [NSString stringWithFormat:@"%llu",model.second];
    if (model.second >= 0 && model.second <= 0x3B) {
        second = [NSString stringWithFormat:@"%llu",model.second];
    }else if (model.second == 0x3C){
        second = @"Any";
    }else if (model.second == 0x3D){
        second = @"Every 15 seconds";
    }else if (model.second == 0x3E){
        second = @"Every 20 seconds";
    }else if (model.second == 0x3F){
        second = @"Once an minute";
    }
    //week
    for (int i=0; i<7; i++) {
        BOOL isExist = ((model.week>>i)&0x1)==1;
        if (isExist) {
            if (week.length != 0) {
                week = [week stringByAppendingString:@"、"];
            }
            week = [week stringByAppendingString:self.weekStrings[i]];
        }
    }
    if (week.length == 0) {
        week = [NSString stringWithFormat:@"%llu",model.week];
    }
    //action
    action = [NSString stringWithFormat:@"%lu",(unsigned long)model.action];
    if (model.action == 0x0) {
        action = @"Turn Off";
    }else if (model.action == 0x1){
        action = @"Turn On";
    }else if (model.action == 0x2){
        action = [NSString stringWithFormat:@"Scene -- %llu",model.sceneId];
    }else if (model.action == 0xF){
        action = @"No action";
    }

    _detailStateLabel.text = [NSString stringWithFormat:@"Year:%@\nMonth:%@\nDay:%@\nHour:%@\nMinute:%@\nSecond:%@\nWeek:%@\nAction:%@",year,month,day,hour,minute,second,week,action];
}

- (IBAction)clickEditButton:(UIButton *)sender {
    if (self.clickEditBlock) {
        self.clickEditBlock();
    }
}

@end
