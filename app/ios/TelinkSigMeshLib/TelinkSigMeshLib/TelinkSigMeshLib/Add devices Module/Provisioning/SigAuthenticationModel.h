/********************************************************************************************************
 * @file     SigAuthenticationModel.h
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
//  SigAuthenticationModel.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/23.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// The user shall provide 16 byte OOB Static Key.
typedef NSData *_Nonnull(^provideStaticKeyCallback)(void);
/// The user shall provide a number.
typedef int (^provideNumericCallback)(UInt8 maximumNumberOfDigits,OutputAction outputAction);
/// The user shall provide an alphanumeric text.
typedef NSString *_Nonnull(^provideAlphanumericCallback)(UInt8 maximumNumberOfCharacters);
/// The application should display this number to the user. User should perform selected action given number of times, or enter the number on the remote device.
typedef void (^displayNumberCallback)(int value,InputAction inputAction);
/// The application should display the text to the user. User should enter the text on the provisioning device.
typedef void (^displayAlphanumericCallback)(NSString *text);

@interface SigAuthenticationModel : NSObject
@property (nonatomic, assign) AuthenticationMethod authenticationMethod;
@property (nonatomic, assign) OutputAction outputAction;
@property (nonatomic, assign) InputAction inputAction;

///For No OOB
- (instancetype)initWithNoOob;

///For Static OOB
- (instancetype)initWithStaticOobCallback:(provideStaticKeyCallback)callback;

///For Output OOB, OutputAction is OutputAction_outputAlphanumeric.
- (instancetype)initWithOutputAlphanumericOfOutputOobCallback:(provideAlphanumericCallback)callback;

///For Output OOB, OutputAction is not OutputAction_outputAlphanumeric.
- (instancetype)initWithOutputAction:(OutputAction)outputAction outputOobCallback:(provideNumericCallback)callback;

///For Input OOB, InputAction is InputAction_inputAlphanumeric.
- (instancetype)initWithInputAlphanumericOfInputOobCallback:(displayAlphanumericCallback)callback;

///For Input OOB, InputAction is not InputAction_inputAlphanumeric.
- (instancetype)initWithInputAction:(InputAction)inputAction inputOobCallback:(displayNumberCallback)callback;

- (void)handelAuthentication:(id)firstArg, ... NS_REQUIRES_NIL_TERMINATION;

@end

NS_ASSUME_NONNULL_END
