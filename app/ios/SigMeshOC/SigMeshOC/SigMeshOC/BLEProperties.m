/********************************************************************************************************
 * @file     BLEProperties.m 
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
//  BLEProperties.m
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "BLEProperties.h"

@interface BLEProperties()
@property (nonatomic,strong) NSOperationQueue *queue;
@property (nonatomic,strong) NSTimer *loopTimer;
@end

@implementation BLEProperties

- (instancetype)init{
    self = [super init];
    if (self) {
        
        _store = [[BLEStore alloc] init];
        _commandHandle = [[SDKLibCommand alloc] init];
        _state = StateNormal;
                
        _queue = [[NSOperationQueue alloc] init];
        __weak typeof(self) weakSelf = self;
        [_queue addOperationWithBlock:^{
            init_json();
            master_clock_init();
            mesh_init_all();//this api need use 150ms.
            //app has exist mesh.json at this time, load json.
            [SigDataSource.share writeDataSourceToLib];

            NSRunLoop *loop = NSRunLoop.currentRunLoop;
            weakSelf.loopTimer = [NSTimer scheduledTimerWithTimeInterval:kLoopTimeInterval target:weakSelf selector:@selector(meshLibProcess) userInfo:nil repeats:YES];
            [loop addTimer:weakSelf.loopTimer forMode:NSRunLoopCommonModes];
            [loop run];
        }];
        
    }
    return self;
}

- (void)meshLibProcess{
    Thread_main_process();
}

@end
