/********************************************************************************************************
 * @file     SigAuthenticationModel.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  SigAuthenticationModel.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/23.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigAuthenticationModel.h"

@interface SigAuthenticationModel ()
@property (nonatomic,copy) provideStaticKeyCallback provideStaticKeyBlock;
@property (nonatomic,copy) provideAlphanumericCallback provideAlphanumericBlock;
@property (nonatomic,copy) provideNumericCallback provideNumericBlock;
@property (nonatomic,copy) displayAlphanumericCallback displayAlphanumericBlock;
@property (nonatomic,copy) displayNumberCallback displayNumberBlock;

@end

@implementation SigAuthenticationModel

///For No OOB
- (instancetype)initWithNoOob{
    if (self = [super init]) {
        _authenticationMethod = AuthenticationMethod_noOob;
    }
    return self;
}

///For Static OOB
- (instancetype)initWithStaticOobCallback:(provideStaticKeyCallback)callback{
    if (self = [super init]) {
        _authenticationMethod = AuthenticationMethod_staticOob;
        _provideStaticKeyBlock = callback;
    }
    return self;
}

///For Output OOB, OutputAction is OutputAction_outputAlphanumeric.
- (instancetype)initWithOutputAlphanumericOfOutputOobCallback:(provideAlphanumericCallback)callback{
    if (self = [super init]) {
        _authenticationMethod = AuthenticationMethod_outputOob;
        _outputAction = OutputAction_outputAlphanumeric;
        _provideAlphanumericBlock = callback;
    }
    return self;
}

///For Output OOB, OutputAction is not OutputAction_outputAlphanumeric.
- (instancetype)initWithOutputAction:(OutputAction)outputAction outputOobCallback:(provideNumericCallback)callback{
    if (self = [super init]) {
        _authenticationMethod = AuthenticationMethod_outputOob;
        _outputAction = outputAction;
        _provideNumericBlock = callback;
    }
    return self;
}

///For Input OOB, InputAction is InputAction_inputAlphanumeric.
- (instancetype)initWithInputAlphanumericOfInputOobCallback:(displayAlphanumericCallback)callback{
    if (self = [super init]) {
        _authenticationMethod = AuthenticationMethod_inputOob;
        _inputAction = InputAction_inputAlphanumeric;
        _displayAlphanumericBlock = callback;
    }
    return self;
}

///For Input OOB, InputAction is not InputAction_inputAlphanumeric.
- (instancetype)initWithInputAction:(InputAction)inputAction inputOobCallback:(displayNumberCallback)callback{
    if (self = [super init]) {
        _authenticationMethod = AuthenticationMethod_inputOob;
        _inputAction = inputAction;
        _displayNumberBlock = callback;
    }
    return self;
}

- (void)handelAuthentication:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION {
    switch (self.authenticationMethod) {
        case AuthenticationMethod_staticOob:
            if (self.provideStaticKeyBlock) {
                self.provideStaticKeyBlock();
            }
            break;
        case AuthenticationMethod_outputOob:
            if (self.outputAction == OutputAction_outputAlphanumeric) {
                if (self.provideAlphanumericBlock) {
                    self.provideAlphanumericBlock((UInt8)firstArg);
                }
            } else {
                if (self.provideNumericBlock) {
                    // 定义一个指向个数可变的参数列表指针；
                    va_list args;
//                    // 用于存放取出的参数
//                    id arg;
                    // 初始化变量刚定义的va_list变量，这个宏的第二个参数是第一个可变参数的前一个参数，是一个固定的参数
                    va_start(args, firstArg);
//                    OutputAction secondArg;
//                    // 遍历全部参数 va_arg返回可变的参数(a_arg的第二个参数是你要返回的参数的类型)
//                    while ((arg = va_arg(args, id))) {
//                        NSLog(@"%@", arg);
//                        secondArg = (OutputAction)arg;
//                        break;
//                    }
                    OutputAction secondArg = (OutputAction)va_arg(args, id);
                    self.provideNumericBlock((UInt8)firstArg,secondArg);
                    // 清空参数列表，并置参数指针args无效
                    va_end(args);
                }
            }
            break;
        case AuthenticationMethod_inputOob:
            if (self.inputAction == InputAction_inputAlphanumeric) {
                if (self.displayAlphanumericBlock) {
                    self.displayAlphanumericBlock((NSString *)firstArg);
                }
            } else {
                if (self.displayNumberBlock) {
                    va_list args;
                    va_start(args, firstArg);
                    InputAction secondArg = (InputAction)va_arg(args, id);

                    self.displayNumberBlock([(NSNumber *)firstArg intValue],secondArg);
                }
            }
            break;
        default:
            break;
    }
}

@end
