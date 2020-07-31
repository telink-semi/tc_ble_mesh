/********************************************************************************************************
 * @file     SchedulerCell.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  SchedulerCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/9/25.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
