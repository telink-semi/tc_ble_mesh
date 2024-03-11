/********************************************************************************************************
 * @file     SigAuthenticationModel.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/23
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
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _authenticationMethod = AuthenticationMethod_noOob;
    }
    return self;
}

///For Static OOB
- (instancetype)initWithStaticOobCallback:(provideStaticKeyCallback)callback{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _authenticationMethod = AuthenticationMethod_staticOob;
        _provideStaticKeyBlock = callback;
    }
    return self;
}

///For Output OOB, OutputAction is OutputAction_outputAlphanumeric.
- (instancetype)initWithOutputAlphanumericOfOutputOobCallback:(provideAlphanumericCallback)callback{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _authenticationMethod = AuthenticationMethod_outputOob;
        _outputAction = OutputAction_outputAlphanumeric;
        _provideAlphanumericBlock = callback;
    }
    return self;
}

///For Output OOB, OutputAction is not OutputAction_outputAlphanumeric.
- (instancetype)initWithOutputAction:(OutputAction)outputAction outputOobCallback:(provideNumericCallback)callback{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _authenticationMethod = AuthenticationMethod_outputOob;
        _outputAction = outputAction;
        _provideNumericBlock = callback;
    }
    return self;
}

///For Input OOB, InputAction is InputAction_inputAlphanumeric.
- (instancetype)initWithInputAlphanumericOfInputOobCallback:(displayAlphanumericCallback)callback{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _authenticationMethod = AuthenticationMethod_inputOob;
        _inputAction = InputAction_inputAlphanumeric;
        _displayAlphanumericBlock = callback;
    }
    return self;
}

///For Input OOB, InputAction is not InputAction_inputAlphanumeric.
- (instancetype)initWithInputAction:(InputAction)inputAction inputOobCallback:(displayNumberCallback)callback{
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _authenticationMethod = AuthenticationMethod_inputOob;
        _inputAction = inputAction;
        _displayNumberBlock = callback;
    }
    return self;
}

@end
