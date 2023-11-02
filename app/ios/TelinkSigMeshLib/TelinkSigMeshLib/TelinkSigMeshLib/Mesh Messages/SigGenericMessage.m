/********************************************************************************************************
 * @file     SigGenericMessage.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/11/12
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

#import "SigGenericMessage.h"

@implementation SigGenericMessage

- (BOOL)isTransactionMessage {
    /*
SigGenericDeltaSet|SigGenericDeltaSetUnacknowledged|SigGenericLevelSet|SigGenericLevelSetUnacknowledged|SigGenericMoveSet|SigGenericMoveSetUnacknowledged|SigGenericOnOffSet|SigGenericOnOffSetUnacknowledged|SigGenericPowerLevelSet|SigGenericPowerLevelSetUnacknowledged
     */
    if ([self isMemberOfClass:[SigGenericOnOffSet class]] ||
        [self isMemberOfClass:[SigGenericOnOffSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigGenericLevelSet class]] ||
        [self isMemberOfClass:[SigGenericLevelSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigGenericDeltaSet class]] ||
        [self isMemberOfClass:[SigGenericDeltaSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigGenericMoveSet class]] ||
        [self isMemberOfClass:[SigGenericMoveSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigGenericPowerLevelSet class]] ||
        [self isMemberOfClass:[SigGenericPowerLevelSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigSceneRecall class]] ||
        [self isMemberOfClass:[SigSceneRecallUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightLightnessSet class]] ||
        [self isMemberOfClass:[SigLightLightnessSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightLightnessLinearSet class]] ||
        [self isMemberOfClass:[SigLightLightnessLinearSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightCTLSet class]] ||
        [self isMemberOfClass:[SigLightCTLSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightCTLTemperatureSet class]] ||
        [self isMemberOfClass:[SigLightCTLTemperatureSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightHSLHueSet class]] ||
        [self isMemberOfClass:[SigLightHSLHueSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightHSLSaturationSet class]] ||
        [self isMemberOfClass:[SigLightHSLSaturationSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightHSLSet class]] ||
        [self isMemberOfClass:[SigLightHSLSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightXyLSet class]] ||
        [self isMemberOfClass:[SigLightXyLSetUnacknowledged class]] ||
        [self isMemberOfClass:[SigLightLCLightOnOffSet class]] ||
        [self isMemberOfClass:[SigLightLCLightOnOffSetUnacknowledged class]]) {
        return YES;
    }
    return NO;
}

@end


@implementation SigAcknowledgedGenericMessage

/// The Type of the response message.
- (Class)responseType {
    TeLogDebug(@"注意：本类的子类未重写该方法。");
    return [SigAcknowledgedGenericMessage class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark - 3.2 Generic messages


#pragma mark - 3.2.1 Generic OnOff messages


#pragma mark 3.2.1.1 Generic OnOff Get, opcode:0x8201


/// Generic OnOff Get is an acknowledged message used to get
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   The response to the Generic OnOff Get message
/// is a Generic OnOff Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.42),
/// 3.2.1.1 Generic OnOff Get.
@implementation SigGenericOnOffGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnOffGet object.
 * @param   parameters    the hex data of SigGenericOnOffGet.
 * @return  return `nil` when initialize SigGenericOnOffGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericOnOffStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.1.2 Generic OnOff Set, opcode:0x8202


/// Generic OnOff Set is an acknowledged message used to set
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   The response to the Generic OnOff Set message
/// is a Generic OnOff Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.43),
/// 3.2.1.2 Generic OnOff Set.
@implementation SigGenericOnOffSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSet;
    }
    return self;
}

/// Creates the Generic OnOff Set message.
///
/// @param isOn The target value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSet;
        _isOn = isOn;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

/// Creates the Generic OnOff Set message.
/// @param isOn The target value of the Generic OnOff state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithIsOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSet;
        _isOn = isOn;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnOffSet object.
 * @param   parameters    the hex data of SigGenericOnOffSet.
 * @return  return `nil` when initialize SigGenericOnOffSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSet;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 4)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _isOn = tem == 0x01;
        memcpy(&tem, dataByte+1, 1);
        self.tid = tem;
        if (parameters.length == 4) {
            memcpy(&tem, dataByte+2, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+3, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _isOn ? 0x01 : 0x00;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericOnOffStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.1.3 Generic OnOff Set Unacknowledged, opcode:0x8203


/// Generic OnOff Set Unacknowledged is an unacknowledged message used to set
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.43),
/// 3.2.1.3 Generic OnOff Set Unacknowledged.
@implementation SigGenericOnOffSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic OnOff Set Unacknowledged message.
/// @param isOn  The target value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSetUnacknowledged;
        _isOn = isOn;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

/// Creates the Generic OnOff Set Unacknowledged message.
/// @param isOn  The target value of the Generic OnOff state.
/// @param transitionTime  The time that an element will take to transition to the target state from the present state.
/// @param delay  Message execution delay in 5 millisecond steps.
- (instancetype)initWithIsOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSetUnacknowledged;
        _isOn = isOn;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnOffSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericOnOffSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericOnOffSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffSetUnacknowledged;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 4)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _isOn = tem == 0x01;
        memcpy(&tem, dataByte+1, 1);
        self.tid = tem;
        if (parameters.length == 4) {
            memcpy(&tem, dataByte+2, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+3, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _isOn ? 0x01 : 0x00;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 3.2.1.4 Generic OnOff Status, opcode:0x8204


/// Generic OnOff Status is an unacknowledged message used to report
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.44),
/// 3.2.1.4 Generic OnOff Status.
@implementation SigGenericOnOffStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffStatus;
    }
    return self;
}

/// Creates the Generic OnOff Status message.
/// @param isOn  The current value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffStatus;
        _isOn = isOn;
        _targetState = NO;
        _remainingTime = nil;
    }
    return self;
}

/// Creates the Generic OnOff Status message.
/// @param isOn  The current value of the Generic OnOff state.
/// @param targetState  The target value of the Generic OnOff state.
/// @param remainingTime  The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithIsOn:(BOOL)isOn targetState:(BOOL)targetState remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffStatus;
        _isOn = isOn;
        _targetState = targetState;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnOffStatus object.
 * @param   parameters    the hex data of SigGenericOnOffStatus.
 * @return  return `nil` when initialize SigGenericOnOffStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffStatus;
        if (parameters == nil || parameters.length < 1) {
//            if (parameters == nil || (parameters.length != 1 && parameters.length != 3)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _isOn = tem == 0x01;
        if (parameters.length >= 3) {
            memcpy(&tem, dataByte+1, 1);
            _targetState = tem == 0x01;
            memcpy(&tem, dataByte+2, 1);
            _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem];
        } else {
            _targetState = NO;
            _remainingTime = nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _isOn ? 0x01 : 0x00;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem8 = _targetState ? 0x01 : 0x00;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark - 3.2.2 Generic Level messages


#pragma mark 3.2.2.1 Generic Level Get, opcode:0x8205


/// Generic Level Get is an acknowledged message used to get
/// the Generic Level state of an element (see Section 3.1.2).
/// @note   The response to the Generic Level Get message is
/// a Generic Level Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.44),
/// 3.2.2.1 Generic Level Get.
@implementation SigGenericLevelGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericLevelGet object.
 * @param   parameters    the hex data of SigGenericLevelGet.
 * @return  return `nil` when initialize SigGenericLevelGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericLevelStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.2.2 Generic Level Set, opcode:0x8206


/// Generic Level Set is an acknowledged message used to set the
/// Generic Level state of an element (see Section 3.1.2) to a new absolute value.
/// @note   The response to the Generic Level Set message is a
/// Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.44),
/// 3.2.2.2 Generic Level Set.
@implementation SigGenericLevelSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSet;
    }
    return self;
}

/// Creates the Generic Level Set message.
///
/// @param level  The target value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSet;
        _level = level;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
        _transitionTime = nil;
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param level The target value of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithLevel:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSet;
        _level = level;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericLevelSet object.
 * @param   parameters    the hex data of SigGenericLevelSet.
 * @return  return `nil` when initialize SigGenericLevelSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _level = tem16;
        memcpy(&tem, dataByte+2, 1);
        self.tid = tem;
        if (parameters.length == 5) {
            memcpy(&tem, dataByte+3, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+4, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _level;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericLevelStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.2.3 Generic Level Set Unacknowledged, opcode:0x8207


/// Generic Level Set Unacknowledged is an unacknowledged message used to set
/// the Generic Level state of an element (see Section 3.1.2) to a new absolute value.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.45),
/// 3.2.2.3 Generic Level Set Unacknowledged.
@implementation SigGenericLevelSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Level Set Unacknowledged message.
/// @param level  The target value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSetUnacknowledged;
        _level = level;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
        _transitionTime = nil;
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Level Set Unacknowledged message.
/// @param level  The target value of the Generic Level state.
/// @param transitionTime  The time that an element will take to transition to the target state from the present state.
/// @param delay  Message execution delay in 5 millisecond steps.
- (instancetype)initWithLevel:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSetUnacknowledged;
        _level = level;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericLevelSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericLevelSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericLevelSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _level = tem16;
        memcpy(&tem, dataByte+2, 1);
        self.tid = tem;
        if (parameters.length == 5) {
            memcpy(&tem, dataByte+3, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+4, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _level;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 3.2.2.8 Generic Level Status, opcode:0x8208


/// Generic Level Status is an unacknowledged message used to report
/// the Generic Level state of an element (see Section 3.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.48),
/// 3.2.2.8 Generic Level Status.
@implementation SigGenericLevelStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelStatus;
    }
    return self;
}

/// Creates the Generic Level Status message.
/// @param level  The current value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelStatus;
        _level = level;
        _targetLevel = 0;
        _remainingTime = nil;
    }
    return self;
}

/// Creates the Generic Level Status message.
/// @param level  The current value of the Generic Level state.
/// @param targetLevel  The target value of the Generic Level state.
/// @param remainingTime  The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithLevel:(UInt16)level targetLevel:(BOOL)targetLevel remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelStatus;
        _level = level;
        _targetLevel = targetLevel;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericLevelStatus object.
 * @param   parameters    the hex data of SigGenericLevelStatus.
 * @return  return `nil` when initialize SigGenericLevelStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericLevelStatus;
        if (parameters == nil || parameters.length < 2) {
//            if (parameters == nil || (parameters.length != 2 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _level = tem16;
        if (parameters.length >= 5) {
            memcpy(&tem16, dataByte+2, 2);
            _targetLevel = tem16;
            UInt8 tem = 0;
            memcpy(&tem, dataByte+4, 1);
            _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem];
        } else {
            _targetLevel = 0;
            _remainingTime = nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _level;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem16 = _targetLevel;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 3.2.2.4 Generic Delta Set, opcode:0x8209


/// Generic Delta Set is an acknowledged message used to set the
/// Generic Level state of an element (see Section 3.1.2) by a relative
/// value. The message is transactional – it supports changing the
/// state by a cumulative value with a sequence of messages that
/// are part of a transaction.
/// @note   The response to the Generic Delta Set message is
/// a Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.46),
/// 3.2.2.4 Generic Delta Set.
@implementation SigGenericDeltaSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSet;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param delta  The Delta change of the Generic Level state.
- (instancetype)initWithDelta:(UInt32)delta {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSet;
        _delta = delta;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param delta  The Delta change of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDelta:(UInt32)delta transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSet;
        _delta = delta;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericDeltaSet object.
 * @param   parameters    the hex data of SigGenericDeltaSet.
 * @return  return `nil` when initialize SigGenericDeltaSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSet;
        if (parameters == nil || (parameters.length != 5 && parameters.length != 7)) {
            return nil;
        }
        UInt32 tem32 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem32, dataByte, 4);
        _delta = tem32;
        memcpy(&tem, dataByte+4, 1);
        self.tid = tem;
        if (parameters.length == 7) {
            memcpy(&tem, dataByte+5, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+6, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = _delta;
    NSData *data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericLevelStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.2.5 Generic Delta Set Unacknowledged, opcode:0x820A


/// Generic Delta Set is an acknowledged message used to set the
/// Generic Level state of an element (see Section 3.1.2) by a relative
/// value. The message is transactional – it supports changing the
/// state by a cumulative value with a sequence of messages that
/// are part of a transaction.
/// @note   The response to the Generic Delta Set message is
/// a Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.46),
/// 3.2.2.5 Generic Delta Set Unacknowledged.
@implementation SigGenericDeltaSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Level Set Unacknowledged message.
/// @param delta  The Delta change of the Generic Level state.
- (instancetype)initWithDelta:(UInt32)delta {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSetUnacknowledged;
        _delta = delta;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Level Set Unacknowledged message.
/// @param delta  The Delta change of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDelta:(UInt32)delta transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSetUnacknowledged;
        _delta = delta;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericDeltaSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericDeltaSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericDeltaSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDeltaSet;
        if (parameters == nil || (parameters.length != 5 && parameters.length != 7)) {
            return nil;
        }
        UInt32 tem32 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem32, dataByte, 4);
        _delta = tem32;
        memcpy(&tem, dataByte+4, 1);
        self.tid = tem;
        if (parameters.length == 7) {
            memcpy(&tem, dataByte+5, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+6, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = _delta;
    NSData *data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 3.2.2.6 Generic Move Set, opcode:0x820B


/// Generic Move Set is an acknowledged message used to start a process of
/// changing the Generic Level state of an element (see Section 3.1.2) with a
/// defined transition speed.
/// @note   The response to the Generic Move Set message is a
/// Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.47),
/// 3.2.2.6 Generic Move Set.
@implementation SigGenericMoveSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSet;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSet;
        _deltaLevel = deltaLevel;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSet;
        _deltaLevel = deltaLevel;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericMoveSet object.
 * @param   parameters    the hex data of SigGenericMoveSet.
 * @return  return `nil` when initialize SigGenericMoveSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _deltaLevel = tem16;
        memcpy(&tem, dataByte+2, 1);
        self.tid = tem;
        if (parameters.length == 5) {
            memcpy(&tem, dataByte+3, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+4, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _deltaLevel;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericLevelStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.2.7 Generic Move Set Unacknowledged, opcode:0x820C


/// Generic Move Set Unacknowledged is an unacknowledged message
/// used to start a process of changing the Generic Level state of an
/// element (see Section 3.1.2) with a defined transition speed.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.47),
/// 3.2.2.7 Generic Move Set Unacknowledged.
@implementation SigGenericMoveSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSetUnacknowledged;
        _deltaLevel = deltaLevel;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSetUnacknowledged;
        _deltaLevel = deltaLevel;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericMoveSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericMoveSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericMoveSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericMoveSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _deltaLevel = tem16;
        memcpy(&tem, dataByte+2, 1);
        self.tid = tem;
        if (parameters.length == 5) {
            memcpy(&tem, dataByte+3, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+4, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _deltaLevel;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark - 3.2.3 Generic Default Transition Time messages


#pragma mark 3.2.3.1 Generic Default Transition Time Get, opcode:0x820D


/// Generic Default Transition Time Get is an acknowledged message
/// used to get the Generic Default Transition Time state of an
/// element (see Section 3.1.3).
/// @note   The response to the Generic Default Transition Time
/// Get message is a Generic Default Transition Time Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.48),
/// 3.2.3.1 Generic Default Transition Time Get.
@implementation SigGenericDefaultTransitionTimeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeGet object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeGet.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericDefaultTransitionTimeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.3.2 Generic Default Transition Time Set, opcode:0x820E


/// Generic Default Transition Time Set is an acknowledged message used to set
/// the Generic Default Transition Time state of an element (see Section 3.1.3).
/// @note   The response to the Generic Default Transition Time Set message
/// is a Generic Default Transition Time Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.3.2 Generic Default Transition Time Set.
@implementation SigGenericDefaultTransitionTimeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeSet;
    }
    return self;
}

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeSet;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeSet object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeSet.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeSet;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        self.transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.transitionTime.rawValue;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericDefaultTransitionTimeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.3.3 Generic Default Transition Time Set Unacknowledged, opcode:0x820F


/// Generic Default Transition Time Set Unacknowledged is an unacknowledged message
/// used to set the Generic Default Transition Time state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.3.3 Generic Default Transition Time Set Unacknowledged.
@implementation SigGenericDefaultTransitionTimeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeSetUnacknowledged;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        self.transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.transitionTime.rawValue;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 3.2.3.4 Generic Default Transition Time Status, opcode:0x8210


/// Generic Default Transition Time Set Unacknowledged is an unacknowledged message
/// used to set the Generic Default Transition Time state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.3.4 Generic Default Transition Time Status.
@implementation SigGenericDefaultTransitionTimeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeStatus;
    }
    return self;
}

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeStatus;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeStatus object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeStatus.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericDefaultTransitionTimeStatus;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        self.transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.transitionTime.rawValue;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 3.2.4 Generic OnPowerUp messages


#pragma mark 3.2.4.1 Generic OnPowerUp Get, opcode:0x8211


/// Generic OnPowerUp Get is an acknowledged message used to get
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   The response to the Generic OnPowerUp Get message
/// is a Generic OnPowerUp Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.4.1 Generic OnPowerUp Get.
@implementation SigGenericOnPowerUpGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnPowerUpGet object.
 * @param   parameters    the hex data of SigGenericOnPowerUpGet.
 * @return  return `nil` when initialize SigGenericOnPowerUpGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericOnPowerUpStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.4.4 Generic OnPowerUp Status, opcode:0x8212


/// Generic OnPowerUp Status is an unacknowledged message used to report
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.4.4 Generic OnPowerUp Status.
@implementation SigGenericOnPowerUpStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpStatus;
    }
    return self;
}

/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpStatus;
        _state = state;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnPowerUpStatus object.
 * @param   parameters    the hex data of SigGenericOnPowerUpStatus.
 * @return  return `nil` when initialize SigGenericOnPowerUpStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpStatus;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = (SigOnPowerUp)tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.state;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 3.2.4.2 Generic OnPowerUp Set, opcode:0x8213


/// Generic OnPowerUp Set is an acknowledged message used to set
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   The response to the Generic OnPowerUp Set message
/// is a Generic OnPowerUp Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.4.2 Generic OnPowerUp Set.
@implementation SigGenericOnPowerUpSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpSet;
    }
    return self;
}

/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpSet;
        _state = state;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnPowerUpSet object.
 * @param   parameters    the hex data of SigGenericOnPowerUpSet.
 * @return  return `nil` when initialize SigGenericOnPowerUpSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpSet;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = (SigOnPowerUp)tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericOnPowerUpStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.4.3 Generic OnPowerUp Set Unacknowledged, opcode:0x8214


/// Generic OnPowerUp Set Unacknowledged is an unacknowledged message used to set
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.4.3 Generic OnPowerUp Set Unacknowledged.
@implementation SigGenericOnPowerUpSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpSetUnacknowledged;
        _state = state;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericOnPowerUpSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericOnPowerUpSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericOnPowerUpSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnPowerUpSetUnacknowledged;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = (SigOnPowerUp)tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.state;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 3.2.5 Generic Power Level messages


#pragma mark 3.2.5.1 Generic Power Level Get, opcode:0x8215


/// Generic Power Level Get message is an acknowledged message used to get the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   The response to the Generic Power Level Get message is a Generic Power Level Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.5.1 Generic Power Level Get.
@implementation SigGenericPowerLevelGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerLevelGet object.
 * @param   parameters    the hex data of SigGenericPowerLevelGet.
 * @return  return `nil` when initialize SigGenericPowerLevelGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerLevelStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.5.2 Generic Power Level Set, opcode:0x8216


/// Generic Power Level Set is an acknowledged message used to set
/// the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   The response to the Generic Power Level Set message
/// is a Generic Power Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.51),
/// 3.2.5.1 3.2.5.2 Generic Power Level Set.
@implementation SigGenericPowerLevelSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSet;
    }
    return self;
}

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSet;
        _power = power;
        _transitionTime = nil;
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithPower:(UInt16)power transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSet;
        _power = power;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerLevelSet object.
 * @param   parameters    the hex data of SigGenericPowerLevelSet.
 * @return  return `nil` when initialize SigGenericPowerLevelSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _power = tem16;
        memcpy(&tem, dataByte+2, 1);
        self.tid = tem;
        if (parameters.length == 5) {
            memcpy(&tem, dataByte+3, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+4, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerLevelStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.5.3 Generic Power Level Set Unacknowledged, opcode:0x8217


/// Generic Power Level Set Unacknowledged is an unacknowledged message used to
/// set the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.51),
/// 3.2.5.3 Generic Power Level Set Unacknowledged.
@implementation SigGenericPowerLevelSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSetUnacknowledged;
        _power = power;
        _transitionTime = nil;
        _delay = 0;
    }
    return self;
}

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithPower:(UInt16)power transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSetUnacknowledged;
        _power = power;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerLevelSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericPowerLevelSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericPowerLevelSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _power = tem16;
        memcpy(&tem, dataByte+2, 1);
        self.tid = tem;
        if (parameters.length == 5) {
            memcpy(&tem, dataByte+3, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+4, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 3.2.5.4 Generic Power Level Status, opcode:0x8218


/// Generic Power Level Status is an unacknowledged message used to report
/// the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.52),
/// 3.2.5.4 Generic Power Level Status.
@implementation SigGenericPowerLevelStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelStatus;
    }
    return self;
}

/// Creates the Generic Power Level Status message.
///
/// @param power  The present value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelStatus;
        _power = power;
        _targetPower = 0;
        _transitionTime = nil;
    }
    return self;
}

/// Creates the Generic Power Level Status message.
///
/// @param power  The present value of the Generic Power Actual state.
/// @param targetPower The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithPower:(UInt16)power targetPower:(UInt16)targetPower transitionTime:(nullable SigTransitionTime *)transitionTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelStatus;
        _power = power;
        _targetPower = targetPower;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerLevelStatus object.
 * @param   parameters    the hex data of SigGenericPowerLevelStatus.
 * @return  return `nil` when initialize SigGenericPowerLevelStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLevelStatus;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 5)) {
            return nil;
        }
        UInt16 tem16 = 0;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _power = tem16;
        if (parameters.length == 5) {
            memcpy(&tem16, dataByte+2, 2);
            _targetPower = tem16;
            memcpy(&tem, dataByte+4, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
        } else {
            _targetPower = 0;
            _transitionTime = nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem16 = _targetPower;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 3.2.5.5 Generic Power Last Get, opcode:0x8219


/// Generic Power Last Get is an acknowledged message used to get
/// the Generic Power Last state of an element (see Section 3.1.5.1.1).
/// @note   The response to a Generic Power Last Get message is
/// a Generic Power Last Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.52),
/// 3.2.5.5 Generic Power Last Get.
@implementation SigGenericPowerLastGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLastGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerLastGet object.
 * @param   parameters    the hex data of SigGenericPowerLastGet.
 * @return  return `nil` when initialize SigGenericPowerLastGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLastGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerLastStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.5.6 Generic Power Last Status, opcode:0x821A


/// Generic Power Last Status is an unacknowledged message used to report
/// the Generic Power Last state of an element (see Section 3.1.5.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.52),
/// 3.2.5.6 Generic Power Last Status.
@implementation SigGenericPowerLastStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLastStatus;
    }
    return self;
}

/// Creates the Generic Power Last Status message.
/// @param power  The value of the Generic Power Last state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLastStatus;
        _power = power;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerLastStatus object.
 * @param   parameters    the hex data of SigGenericPowerLastStatus.
 * @return  return `nil` when initialize SigGenericPowerLastStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerLastStatus;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 2);
        _power = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 3.2.5.7 Generic Power Default Get, opcode:0x821B


/// Generic Power Default Get is an acknowledged message used to get
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   The response to a Generic Power Default Get message is
/// a Generic Power Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.7 Generic Power Default Get.
@implementation SigGenericPowerDefaultGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerDefaultGet object.
 * @param   parameters    the hex data of SigGenericPowerDefaultGet.
 * @return  return `nil` when initialize SigGenericPowerDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.5.10 Generic Power Default Status, opcode:0x821C


/// Generic Power Default Status is an unacknowledged message used to report
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.10 Generic Power Default Status.
@implementation SigGenericPowerDefaultStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultStatus;
    }
    return self;
}

/// Creates the Generic Power Default Status message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultStatus;
        _power = power;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerDefaultStatus object.
 * @param   parameters    the hex data of SigGenericPowerDefaultStatus.
 * @return  return `nil` when initialize SigGenericPowerDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultStatus;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 2);
        _power = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 3.2.5.11 Generic Power Range Get, opcode:0x821D


/// Generic Power Range Get is an acknowledged message used to get
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   The response to the Generic Power Range Get message is
/// a Generic Power Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.11 Generic Power Range Get.
@implementation SigGenericPowerRangeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerRangeGet object.
 * @param   parameters    the hex data of SigGenericPowerRangeGet.
 * @return  return `nil` when initialize SigGenericPowerRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.5.14 Generic Power Range Status, opcode:0x821E


/// Generic Power Range Status is an unacknowledged message used to report
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.54),
/// 3.2.5.14 Generic Power Range Status.
@implementation SigGenericPowerRangeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeStatus;
    }
    return self;
}

/// Creates the Generic Power Range Status message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeStatus;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/// Creates the Generic Power Range Status message.
/// @param status  Status Code for the requesting message.
/// @param request  The request received.
- (instancetype)initWithStatus:(SigGenericMessageStatus)status forSigGenericPowerRangeSetRequest:(SigGenericPowerRangeSet *)request {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeStatus;
        _status = status;
        _rangeMin = request.rangeMin;
        _rangeMax = request.rangeMax;
    }
    return self;
}

- (instancetype)initWithStatus:(SigGenericMessageStatus)status forSigGenericPowerRangeSetUnacknowledgedRequest:(SigGenericPowerRangeSetUnacknowledged *)request {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeStatus;
        _status = status;
        _rangeMin = request.rangeMin;
        _rangeMax = request.rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerRangeStatus object.
 * @param   parameters    the hex data of SigGenericPowerRangeStatus.
 * @return  return `nil` when initialize SigGenericPowerRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultStatus;
        if (parameters == nil || (parameters.length != 5)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = (SigGenericMessageStatus)tem8;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _rangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 3.2.5.8 Generic Power Default Set, opcode:0x821F


/// Generic Power Default Set is an acknowledged message used to set
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   The response to the Generic Power Default Set message is
/// a Generic Power Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.8 Generic Power Default Set.
@implementation SigGenericPowerDefaultSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultSet;
    }
    return self;
}

/// Creates the Generic Power Default Set message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultSet;
        _power = power;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerDefaultSet object.
 * @param   parameters    the hex data of SigGenericPowerDefaultSet.
 * @return  return `nil` when initialize SigGenericPowerDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultSet;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 2);
        _power = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 3.2.5.9 Generic Power Default Set Unacknowledged, opcode:0x8220


/// Generic Power Default Set Unacknowledged is an unacknowledged message used to set
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.9 Generic Power Default Set Unacknowledged.
@implementation SigGenericPowerDefaultSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Power Default Set message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultSetUnacknowledged;
        _power = power;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericPowerDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericPowerDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerDefaultSetUnacknowledged;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 2);
        _power = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _power;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 3.2.5.12 Generic Power Range Set, opcode:0x8221


/// Generic Power Range Set is an acknowledged message used to set
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   The response to the Generic Power Range Set message is
/// a Generic Power Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.54),
/// 3.2.5.12 Generic Power Range Set.
@implementation SigGenericPowerRangeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeSet;
    }
    return self;
}

/// Creates the Generic Power Range Set message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeSet;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerRangeSet object.
 * @param   parameters    the hex data of SigGenericPowerRangeSet.
 * @return  return `nil` when initialize SigGenericPowerRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeSet;
        if (parameters == nil || (parameters.length != 4)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+0, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _rangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericPowerRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 3.2.5.13 Generic Power Range Set Unacknowledged, opcode:0x8222


/// Generic Power Range Set Unacknowledged is an unacknowledged message used to set
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.54),
/// 3.2.5.13 Generic Power Range Set Unacknowledged.
@implementation SigGenericPowerRangeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeSetUnacknowledged;
    }
    return self;
}

/// Creates the Generic Power Range Set message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeSetUnacknowledged;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericPowerRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericPowerRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericPowerRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericPowerRangeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 4)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+0, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _rangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 3.2.6 Generic Battery messages


#pragma mark 3.2.6.1 Generic Battery Get, opcode:0x8223


/// Generic Battery Get message is an acknowledged message used to get
/// the Generic Battery state of an element (see Section 3.1.6).
/// @note   The response to the Generic Battery Get message is a Generic
/// Battery Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.55),
/// 3.2.6.1 Generic Battery Get.
@implementation SigGenericBatteryGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericBatteryGet;
    }
    return self;
}

/**
 * @brief   Initialize SigGenericBatteryGet object.
 * @param   parameters    the hex data of SigGenericBatteryGet.
 * @return  return `nil` when initialize SigGenericBatteryGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericBatteryGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigGenericBatteryStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 3.2.6.2 Generic Battery Status, opcode:0x8224


/// Generic Battery Status is an unacknowledged message used to report
/// the Generic Battery state of an element (see Section 3.1.6).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.55),
/// 3.2.6.2 Generic Battery Status.
@implementation SigGenericBatteryStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericBatteryStatus;
    }
    return self;
}

- (instancetype)initWithBatteryLevel:(UInt8)batteryLevel timeToDischarge:(UInt32)timeToDischarge andCharge:(UInt32)timeToCharge batteryPresence:(SigBatteryPresence)batteryPresence batteryIndicator:(SigBatteryIndicator)batteryIndicator batteryChargingState:(SigBatteryChargingState)batteryChargingState batteryServiceability:(SigBatteryServiceability)batteryServiceability {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericBatteryStatus;
        _batteryLevel = batteryLevel != 0xFF ? MIN(batteryLevel, 100) : 0xFF;
        _timeToDischarge = timeToDischarge != 0xFFFFFF ? MIN(timeToDischarge, 0xFFFFFE) : 0xFFFFFF;
        _timeToCharge = timeToCharge != 0xFFFFFF ? MIN(timeToCharge, 0xFFFFFE) : 0xFFFFFF;
        _flags = (batteryServiceability << 6) | (batteryChargingState << 4) | (batteryIndicator << 2) | (batteryPresence);
    }
    return self;
}

/**
 * @brief   Initialize SigGenericBatteryStatus object.
 * @param   parameters    the hex data of SigGenericBatteryStatus.
 * @return  return `nil` when initialize SigGenericBatteryStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericBatteryStatus;
        if (parameters == nil || parameters.length != 8) {
            return nil;
        }else{
            UInt8 tem1 = 0;
            UInt8 tem2 = 0;
            UInt8 tem3 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem1, dataByte, 1);
            _batteryLevel = tem1;
            memcpy(&tem1, dataByte+1, 1);
            memcpy(&tem2, dataByte+2, 1);
            memcpy(&tem3, dataByte+3, 1);
            _timeToDischarge = (UInt32)tem1 | ((UInt32)tem2 << 8) |  ((UInt32)tem3 << 16);
            memcpy(&tem1, dataByte+4, 1);
            memcpy(&tem2, dataByte+5, 1);
            memcpy(&tem3, dataByte+6, 1);
            _timeToCharge = (UInt32)tem1 | ((UInt32)tem2 << 8) |  ((UInt32)tem3 << 16);
            memcpy(&tem1, dataByte+7, 1);
            _flags = tem1;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _batteryLevel;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt32 tem = _timeToDischarge;
    data = [NSData dataWithBytes:&tem length:4];
    [mData appendData:[data subdataWithRange:NSMakeRange(0, 3)]];
    tem = _timeToCharge;
    data = [NSData dataWithBytes:&tem length:4];
    [mData appendData:[data subdataWithRange:NSMakeRange(0, 3)]];
    tem8 = _flags;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// Whether the battery level is known.
- (BOOL)isBatteryLevelKnown {
    return _batteryLevel != 0xFF;
}

/// Whether the time to discharge is known.
- (BOOL)isTimeToDischargeKnown {
    return _timeToDischarge != 0xFFFFFF;
}

/// Whether the time to charge is known.
- (BOOL)isTimeToChargeKnown {
    return _timeToCharge != 0xFFFFFF;
}

/// Presence of the battery.
- (SigBatteryPresence)batteryPresence {
    return (SigBatteryPresence)(_flags & 0x03);
}

/// Charge level of the battery.
- (SigBatteryIndicator)batteryIndicator {
    return (SigBatteryIndicator)((_flags >> 2) & 0x03);
}

/// Whether the battery is charging.
- (SigBatteryChargingState)batteryChargingState {
    return (SigBatteryChargingState)((_flags >> 4) & 0x03);
}

/// The serviceability of the battery.
- (SigBatteryServiceability)batteryServiceability {
    return (SigBatteryServiceability)((_flags >> 6) & 0x03);
}

@end


#pragma mark - 4.2 Sensor messages


#pragma mark 4.2.1 Sensor Descriptor Get, opcode:0x8230


/// Sensor Descriptor Get is an acknowledged message used to get the Sensor Descriptor state of all sensors within an element (see Section 4.1.1).
/// @note   The response to a Sensor Descriptor Get message is a Sensor Descriptor Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.114),
/// 4.2.1 Sensor Descriptor Get.
@implementation SigSensorDescriptorGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorDescriptorGet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorDescriptorGet;
        _propertyID = propertyID;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorDescriptorGet object.
 * @param   parameters    the hex data of SigSensorDescriptorGet.
 * @return  return `nil` when initialize SigSensorDescriptorGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorDescriptorGet;
        if ((parameters == nil || parameters.length == 0) || (parameters != nil && parameters.length == 2)) {
            if (parameters != nil) {
                UInt16 tem16 = 0;
                Byte *dataByte = (Byte *)parameters.bytes;
                memcpy(&tem16, dataByte, 2);
                _propertyID = tem16;
            }
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorDescriptorStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.2 Sensor Descriptor Status, opcode:0x51


/// The Sensor Descriptor Status is an unacknowledged message used to report
/// a sequence of the Sensor Descriptor states of an element (see Section 4.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.114),
/// 4.2.2 Sensor Descriptor Status.
@implementation SigSensorDescriptorStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorDescriptorStatus;
        _descriptorModels = [NSMutableArray array];
    }
    return self;
}

/**
 * @brief   Initialize SigSensorDescriptorStatus object.
 * @param   parameters    the hex data of SigSensorDescriptorStatus.
 * @return  return `nil` when initialize SigSensorDescriptorStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorDescriptorStatus;
        if (parameters != nil && (parameters.length == 2 || parameters.length % 8 == 0)) {
            _descriptorModels = [NSMutableArray array];
            if (parameters.length == 2) {
                [_descriptorModels addObject:[[SigSensorDescriptorModel alloc] initWithDescriptorParameters:parameters]];
            } else {
                for (int i=0; i<parameters.length/8; i++) {
                    [_descriptorModels addObject:[[SigSensorDescriptorModel alloc] initWithDescriptorParameters:[parameters subdataWithRange:NSMakeRange(i*8, 8)]]];
                }
            }
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSArray *descriptorModels = [NSArray arrayWithArray:_descriptorModels];
    for (SigSensorDescriptorModel *model in descriptorModels) {
        [mData appendData:model.getDescriptorParameters];
    }
    return mData;
}

@end


#pragma mark 4.2.13 Sensor Get, opcode:0x8231


/// Sensor Get is an acknowledged message used to get the Sensor Data state (see Section 4.1.4).
/// @note   The response to the Sensor Get message is a Sensor Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.120),
/// 4.2.13 Sensor Get.
@implementation SigSensorGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorGet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorGet;
        _propertyID = propertyID;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorGet object.
 * @param   parameters    the hex data of SigSensorGet.
 * @return  return `nil` when initialize SigSensorGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorGet;
        if ((parameters == nil || parameters.length == 0) || (parameters != nil && parameters.length == 2)) {
            if (parameters != nil) {
                UInt16 tem16 = 0;
                Byte *dataByte = (Byte *)parameters.bytes;
                memcpy(&tem16, dataByte, 2);
                _propertyID = tem16;
            }
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.14 Sensor Status, opcode:0x52


/// Sensor Status is an unacknowledged message used to report
/// the Sensor Data state of an element (see Section 4.1.4).
/// @note   The message contains a Sensor Data state, defined
/// by the Sensor Descriptor state (see Section 4.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.120),
/// 4.2.14 Sensor Status.
@implementation SigSensorStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorStatus object.
 * @param   parameters    the hex data of SigSensorStatus.
 * @return  return `nil` when initialize SigSensorStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorStatus;
        _marshalledSensorData = parameters;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _marshalledSensorData;
}

@end


#pragma mark 4.2.15 Sensor Column Get, opcode:0x8232


/// Sensor Column Get is an acknowledged message used to get
/// the Sensor Series Column state (see Section 4.1.5).
/// @note   The response to the Sensor Column Get message is
/// a Sensor Column Status message (see Section 4.2.16).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.122),
/// 4.2.15 Sensor Column Get.
@implementation SigSensorColumnGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorColumnGet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorColumnGet;
        _propertyID = propertyID;
        _rawValueX = rawValueX;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorColumnGet object.
 * @param   parameters    the hex data of SigSensorColumnGet.
 * @return  return `nil` when initialize SigSensorColumnGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceGet;
        if ((parameters == nil || parameters.length == 0) || (parameters != nil && parameters.length > 2)) {
            if (parameters != nil) {
                UInt16 tem16 = 0;
                Byte *dataByte = (Byte *)parameters.bytes;
                memcpy(&tem16, dataByte, 2);
                _propertyID = tem16;
                _rawValueX = [parameters subdataWithRange:NSMakeRange(2, parameters.length - 2)];
            }
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        [mData appendData:_rawValueX];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorColumnStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.16 SensorColumnStatus, opcode:0x53


/// Sensor Column Status is an unacknowledged message used to report
/// the Sensor Series Column state of an element (see Section 4.1.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.122),
/// 4.2.16 SensorColumnStatus.
@implementation SigSensorColumnStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorColumnStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorColumnStatus object.
 * @param   parameters    the hex data of SigSensorColumnStatus.
 * @return  return `nil` when initialize SigSensorColumnStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorColumnStatus;
        _columnData = parameters;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _columnData;
}

@end


#pragma mark 4.2.17 Sensor Series Get, opcode:0x8233


/// Sensor Series Get is an acknowledged message used to get
/// a sequence of the Sensor Series Column states (see Section 4.1.5).
/// @note   The response to the Sensor Series Get message is
/// a Sensor Series Status message (see Section 4.2.18).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.123),
/// 4.2.17 Sensor Series Get.
@implementation SigSensorSeriesGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSeriesGet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSeriesGet;
        _propertyID = propertyID;
        _rawValueX1Data = rawValueX1Data;
        _rawValueX2Data = rawValueX2Data;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSeriesGet object.
 * @param   parameters    the hex data of SigSensorSeriesGet.
 * @return  return `nil` when initialize SigSensorSeriesGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSeriesGet;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            _seriesData = parameters;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _seriesData;
//    if (_propertyID != 0) {
//        NSMutableData *mData = [NSMutableData data];
//        UInt16 tem16 = _propertyID;
//        NSData *data = [NSData dataWithBytes:&tem16 length:2];
//        [mData appendData:data];
//        return mData;
//    } else {
//        return nil;
//    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorSeriesStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.18 Sensor Series Status, opcode:0x54


/// Sensor Series Status is an unacknowledged message used to report a
/// sequence of the Sensor Series Column states of an element (see Section 4.1.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.123),
/// 4.2.18 Sensor Series Status.
@implementation SigSensorSeriesStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSeriesStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSeriesStatus object.
 * @param   parameters    the hex data of SigSensorSeriesStatus.
 * @return  return `nil` when initialize SigSensorSeriesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSeriesStatus;
        _seriesData = parameters;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _seriesData;
}

@end


#pragma mark 4.2.3 Sensor Cadence Get, opcode:0x8234


/// Sensor Cadence Get is an acknowledged message used to get
/// the Sensor Cadence state of an element (see Section 4.1.3).
/// @note   The response to the Sensor Cadence Get message is
/// a Sensor Cadence Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.115),
/// 4.2.3 Sensor Cadence Get.
@implementation SigSensorCadenceGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSet;
        _propertyID = propertyID;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorCadenceGet object.
 * @param   parameters    the hex data of SigSensorCadenceGet.
 * @return  return `nil` when initialize SigSensorCadenceGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSet;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorCadenceStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.4 Sensor Cadence Set, opcode:0x55


/// Sensor Cadence Set is an acknowledged message used to set
/// the Sensor Cadence state of an element (see Section 4.1.3).
/// @note   The response to the Sensor Cadence Set message is
/// a Sensor Cadence Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.115),
/// 4.2.4 Sensor Cadence Set.
@implementation SigSensorCadenceSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSet;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorCadenceSet object.
 * @param   parameters    the hex data of SigSensorCadenceSet.
 * @return  return `nil` when initialize SigSensorCadenceSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSet;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            _cadenceData = parameters;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _cadenceData;
//    if (_propertyID != 0) {
//        NSMutableData *mData = [NSMutableData data];
//        UInt16 tem16 = _propertyID;
//        NSData *data = [NSData dataWithBytes:&tem16 length:2];
//        [mData appendData:data];
//        return mData;
//    } else {
//        return nil;
//    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorCadenceStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.5 Sensor Cadence Set Unacknowledged, opcode:0x56


/// Sensor Cadence Set is an acknowledged message used to set
/// the Sensor Cadence state of an element (see Section 4.1.3).
/// @note   The response to the Sensor Cadence Set message is
/// a Sensor Cadence Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.116),
/// 4.2.5 Sensor Cadence Set Unacknowledged.
@implementation SigSensorCadenceSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSetUnacknowledged;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorCadenceSetUnacknowledged object.
 * @param   parameters    the hex data of SigSensorCadenceSetUnacknowledged.
 * @return  return `nil` when initialize SigSensorCadenceSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceSetUnacknowledged;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            _cadenceData = parameters;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _cadenceData;
//    if (_propertyID != 0) {
//        NSMutableData *mData = [NSMutableData data];
//        UInt16 tem16 = _propertyID;
//        NSData *data = [NSData dataWithBytes:&tem16 length:2];
//        [mData appendData:data];
//        return mData;
//    } else {
//        return nil;
//    }
}

@end


#pragma mark 4.2.6 Sensor Cadence Status, opcode:0x57


/// The Sensor Cadence Status is an unacknowledged message used to report
/// the Sensor Cadence state of an element (see Section 4.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.117),
/// 4.2.6 Sensor Cadence Status.
@implementation SigSensorCadenceStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorCadenceStatus object.
 * @param   parameters    the hex data of SigSensorCadenceStatus.
 * @return  return `nil` when initialize SigSensorCadenceStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorCadenceStatus;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            _cadenceData = parameters;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _cadenceData;
//    if (_propertyID != 0) {
//        NSMutableData *mData = [NSMutableData data];
//        UInt16 tem16 = _propertyID;
//        NSData *data = [NSData dataWithBytes:&tem16 length:2];
//        [mData appendData:data];
//        return mData;
//    } else {
//        return nil;
//    }
}

@end


#pragma mark 4.2.7 Sensor Settings Get, opcode:0x8235


/// Sensor Settings Get is an acknowledged message used to get
/// the list of Sensor Setting states of an element (see Section 4.1.2).
/// @note   The response to the Sensor Settings Get message is a
/// Sensor Settings Status message (see Section 4.2.8).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.117),
/// 4.2.7 Sensor Settings Get.
@implementation SigSensorSettingsGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingsGet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingsGet;
        _propertyID = propertyID;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSettingsGet object.
 * @param   parameters    the hex data of SigSensorSettingsGet.
 * @return  return `nil` when initialize SigSensorSettingsGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingsGet;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorSettingsStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.8 Sensor Settings Status, opcode:0x58


/// The Sensor Settings Status is an unacknowledged message used to report
/// a list of the Sensor Setting states of an element (see Section 4.1.2).
/// @note   The message is sent as a response to the Sensor Settings Get
/// message or is sent as an unsolicited message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.118),
/// 4.2.8 Sensor Settings Status.
@implementation SigSensorSettingsStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingsStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSettingsStatus object.
 * @param   parameters    the hex data of SigSensorSettingsStatus.
 * @return  return `nil` when initialize SigSensorSettingsStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingsStatus;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            _settingsData = parameters;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _settingsData;
//    if (_propertyID != 0) {
//        NSMutableData *mData = [NSMutableData data];
//        UInt16 tem16 = _propertyID;
//        NSData *data = [NSData dataWithBytes:&tem16 length:2];
//        [mData appendData:data];
//        return mData;
//    } else {
//        return nil;
//    }
}

@end


#pragma mark 4.2.9 Sensor Setting Get, opcode:0x8236


/// Sensor Setting Get is an acknowledged message used to get
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   The response to the Sensor Setting Get message is
/// a Sensor Setting Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.118),
/// 4.2.9 Sensor Setting Get.
@implementation SigSensorSettingGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingGet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingGet;
        _propertyID = propertyID;
        _settingPropertyID = settingPropertyID;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSettingGet object.
 * @param   parameters    the hex data of SigSensorSettingGet.
 * @return  return `nil` when initialize SigSensorSettingGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingGet;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorSettingStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.10 Sensor Setting Set, opcode:0x59


/// Sensor Setting Set is an acknowledged message used to set
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   The response to the Sensor Setting Set message is
/// a Sensor Setting Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.118),
/// 4.2.10 Sensor Setting Set.
@implementation SigSensorSettingSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingSet;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingSet;
        _propertyID = propertyID;
        _settingPropertyID = settingPropertyID;
        _settingRaw = settingRaw;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSettingSet object.
 * @param   parameters    the hex data of SigSensorSettingSet.
 * @return  return `nil` when initialize SigSensorSettingSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingSet;
        if (parameters != nil && parameters.length >= 4) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _settingPropertyID = tem16;
            _settingRaw = [parameters subdataWithRange:NSMakeRange(4, parameters.length-4)];
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _settingPropertyID;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        [mData appendData:_settingRaw];
        return mData;
    } else {
        return nil;
    }
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSensorSettingStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 4.2.11 Sensor Setting Set Unacknowledged, opcode:0x5A


/// Sensor Setting Set Unacknowledged is an unacknowledged message used to set
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.119),
/// 4.2.11 Sensor Setting Set Unacknowledged.
@implementation SigSensorSettingSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingSetUnacknowledged;
        _propertyID = propertyID;
        _settingPropertyID = settingPropertyID;
        _settingRaw = settingRaw;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSettingSetUnacknowledged object.
 * @param   parameters    the hex data of SigSensorSettingSetUnacknowledged.
 * @return  return `nil` when initialize SigSensorSettingSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingSetUnacknowledged;
        if (parameters != nil && parameters.length >= 4) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _settingPropertyID = tem16;
            _settingRaw = [parameters subdataWithRange:NSMakeRange(4, parameters.length-4)];
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _settingPropertyID;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        [mData appendData:_settingRaw];
        return mData;
    } else {
        return nil;
    }
}

@end


#pragma mark 4.2.12 Sensor Setting Status, opcode:0x5B


/// Sensor Setting Status is an unacknowledged message used to report
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   The message is sent as a response to the Sensor Setting
/// Get and Sensor Setting Set messages or sent as an unsolicited message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.119),
/// 4.2.12 Sensor Setting Status.
@implementation SigSensorSettingStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingStatus;
    }
    return self;
}

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingAccess:(SigSensorSettingAccessType)settingAccess settingRaw:(NSData *)settingRaw {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingStatus;
        _propertyID = propertyID;
        _settingPropertyID = settingPropertyID;
        _settingAccess = settingAccess;
        _settingRaw = settingRaw;
    }
    return self;
}

/**
 * @brief   Initialize SigSensorSettingStatus object.
 * @param   parameters    the hex data of SigSensorSettingStatus.
 * @return  return `nil` when initialize SigSensorSettingStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sensorSettingStatus;
        if (parameters != nil && parameters.length >= 5) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _settingPropertyID = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+4, 1);
            _settingAccess = tem8;
            _settingRaw = [parameters subdataWithRange:NSMakeRange(5, parameters.length-5)];
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_propertyID != 0) {
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = _propertyID;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _settingPropertyID;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _settingAccess;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        [mData appendData:_settingRaw];
        return mData;
    } else {
        return nil;
    }
}

@end


#pragma mark - 5.2.1 Time messages


#pragma mark 5.2.1.1 Time Get, opcode:0x8237


/// Time Get is a message used to get the Time
/// state (see Section 5.1.1) of neighbor nodes.
/// @note   The response to the Time Get
/// message is a Time Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.142),
/// 5.2.1.1 Time Get.
@implementation SigTimeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeGet object.
 * @param   parameters    the hex data of SigTimeGet.
 * @return  return `nil` when initialize SigTimeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTimeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.2 Time Set, opcode:0x5C


/// Time Set is an acknowledged message used to set
/// the Time state of an element (see Section 5.1.1).
/// @note   The response to the Time Set message
/// is a Time Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.143),
/// 5.2.1.2 Time Set.
@implementation SigTimeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeSet;
    }
    return self;
}

- (instancetype)initWithTimeModel:(SigTimeModel *)timeModel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeSet;
        _timeModel = timeModel;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeSet object.
 * @param   parameters    the hex data of SigTimeSet.
 * @return  return `nil` when initialize SigTimeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeSet;
        if (parameters == nil || parameters.length != 10) {
            return nil;
        }else{
            _timeModel = [[SigTimeModel alloc] initWithParameters:parameters];
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _timeModel.getTimeParameters;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTimeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.3 Time Status, opcode:0x5D


/// Time Status is an unacknowledged message used to report
/// the Time state of an element (see Section 5.1.1).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.143),
/// 5.2.1.3 Time Status.
@implementation SigTimeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeStatus object.
 * @param   parameters    the hex data of SigTimeStatus.
 * @return  return `nil` when initialize SigTimeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeStatus;
        if (parameters == nil || parameters.length != 10) {
            return nil;
        }else{
            _timeModel = [[SigTimeModel alloc] initWithParameters:parameters];
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _timeModel.getTimeParameters;
}

@end


#pragma mark 5.2.1.10 Time Role Get, opcode:0x8238


/// Time Status is an unacknowledged message used to report
/// the Time state of an element (see Section 5.1.1).
/// @note   The response to the Time Role Get message is
/// a Time Role Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.10 Time Role Get.
@implementation SigTimeRoleGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleGet;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeRoleGet object.
 * @param   parameters    the hex data of SigTimeRoleGet.
 * @return  return `nil` when initialize SigTimeRoleGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTimeRoleStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.11 Time Role Set, opcode:0x8239


/// Time Role Set is an acknowledged message used to set
/// the Time Role state of an element (see Section 5.1.2).
/// @note   The response to the Time Role Set message
/// is a Time Role Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.1.11 Time Role Set.
@implementation SigTimeRoleSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleSet;
    }
    return self;
}

- (instancetype)initWithTimeRole:(SigTimeRole)timeRole {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleSet;
        _timeRole = timeRole;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeRoleSet object.
 * @param   parameters    the hex data of SigTimeRoleSet.
 * @return  return `nil` when initialize SigTimeRoleSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleSet;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _timeRole = (SigTimeRole)tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _timeRole;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTimeRoleStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.12 Time Role Status, opcode:0x823A


/// Time Role Status is an unacknowledged message used to report
/// the Time state of an element (see Section 5.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.1.12 Time Role Status.
@implementation SigTimeRoleStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeRoleStatus object.
 * @param   parameters    the hex data of SigTimeRoleStatus.
 * @return  return `nil` when initialize SigTimeRoleStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeRoleStatus;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _timeRole = (SigTimeRole)tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _timeRole;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 5.2.1.4 Time Zone Get, opcode:0x823B


/// Time Zone Get is an acknowledged message used to get
/// the Time Zone Offset Current state (see Section 5.1.1.5),
/// the Time Zone Offset New state (see Section 5.1.1.6), and
/// the TAI of Zone Change state (see Section 5.1.1.7).
/// @note   The response to the Time Zone Get message is
/// a Time Zone Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.144),
/// 5.2.1.4 Time Zone Get.
@implementation SigTimeZoneGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneGet;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeZoneGet object.
 * @param   parameters    the hex data of SigTimeZoneGet.
 * @return  return `nil` when initialize SigTimeZoneGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTimeZoneStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.5 Time Zone Set, opcode:0x823C


/// Time Zone Set is an acknowledged message used to set
/// the Time Zone Offset New state (see Section 5.1.1.6) and
/// the TAI of Zone Change state (see Section 5.1.1.7).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.144),
/// 5.2.1.5 Time Zone Set.
@implementation SigTimeZoneSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneSet;
    }
    return self;
}

- (instancetype)initWithTimeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneSet;
        _timeZoneOffsetNew = timeZoneOffsetNew;
        _TAIOfZoneChange = TAIOfZoneChange;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeZoneSet object.
 * @param   parameters    the hex data of SigTimeZoneSet.
 * @return  return `nil` when initialize SigTimeZoneSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneSet;
        if (parameters == nil || parameters.length != 6) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _timeZoneOffsetNew = tem8;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte+1, 5);
            _TAIOfZoneChange = tem64;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _timeZoneOffsetNew;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt64 tem64 = _TAIOfZoneChange & 0xFFFFFFFFFF;
    data = [NSData dataWithBytes:&tem64 length:5];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTimeRoleStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.6 Time Zone Status, opcode:0x823D


/// Time Zone Status is an unacknowledged message used to report
/// the Time Zone Offset Current state (see Section 5.1.1.5), the Time
/// Zone Offset New state (see Section 5.1.1.6), and the TAI of Zone
/// Change state (see Section 5.1.1.7).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.144),
/// 5.2.1.6 Time Zone Status.
@implementation SigTimeZoneStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigTimeZoneStatus object.
 * @param   parameters    the hex data of SigTimeZoneStatus.
 * @return  return `nil` when initialize SigTimeZoneStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_timeZoneStatus;
        if (parameters == nil || parameters.length != 7) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _timeZoneOffsetCurrent = tem8;
            memcpy(&tem8, dataByte+1, 1);
            _timeZoneOffsetNew = tem8;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte+2, 5);
            _TAIOfZoneChange = tem64;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _timeZoneOffsetCurrent;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _timeZoneOffsetNew;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt64 tem64 = _TAIOfZoneChange & 0xFFFFFFFFFF;
    data = [NSData dataWithBytes:&tem64 length:5];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 5.2.1.7 TAI-UTC Delta Get, opcode:0x823E


/// TAI-UTC Delta Get is an acknowledged message used to get
/// the TAI-UTC Delta Current state (see Section 5.1.1.8), the
/// TAI-UTC Delta New state (see Section 5.1.1.9), and the TAI of
/// Delta Change state (see Section 5.1.1.10).
/// @note   The response to the TAI-UTC Delta Get message
/// is a TAI-UTC Delta Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.7 TAI-UTC Delta Get.
@implementation SigTAI_UTC_DeltaGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaGet;
    }
    return self;
}

/**
 * @brief   Initialize SigTAI_UTC_DeltaGet object.
 * @param   parameters    the hex data of SigTAI_UTC_DeltaGet.
 * @return  return `nil` when initialize SigTAI_UTC_DeltaGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTAI_UTC_DeltaStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.8 TAI-UTC Delta Set, opcode:0x823F


/// TAI-UTC Delta Set is an acknowledged message used to set
/// the TAI-UTC Delta New state (see Section 5.1.1.9) and the TAI
/// of Delta Change state (see Section 5.1.1.10).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.8 TAI-UTC Delta Set.
@implementation SigTAI_UTC_DeltaSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaSet;
    }
    return self;
}

- (instancetype)initWithTAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaSet;
        _TAI_UTC_DeltaNew = TAI_UTC_DeltaNew;
        _padding = padding;
        _TAIOfDeltaChange = TAIOfDeltaChange;
    }
    return self;
}

/**
 * @brief   Initialize SigTAI_UTC_DeltaSet object.
 * @param   parameters    the hex data of SigTAI_UTC_DeltaSet.
 * @return  return `nil` when initialize SigTAI_UTC_DeltaSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaSet;
        if (parameters == nil || parameters.length != 7) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _TAI_UTC_DeltaNew = (tem16 >> 1) & 0x7FFF;
            _padding = tem16 & 0b1;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte+2, 5);
            _TAIOfDeltaChange = tem64;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = ((_TAI_UTC_DeltaNew & 0x7FFF) << 1) | (_padding & 0b1);
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt64 tem64 = _TAIOfDeltaChange & 0xFFFFFFFFFF;
    data = [NSData dataWithBytes:&tem64 length:5];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigTAI_UTC_DeltaStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.1.9 TAI-UTC Delta Status, opcode:0x8240


/// TAI-UTC Delta Status is an unacknowledged message used to report
/// the TAI-UTC Delta Current state (see Section 5.1.1.8), the TAI-UTC
/// Delta New state (see Section 5.1.1.9), and the TAI of Delta Change
/// state (see Section 5.1.1.10).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.9 TAI-UTC Delta Status.
@implementation SigTAI_UTC_DeltaStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigTAI_UTC_DeltaStatus object.
 * @param   parameters    the hex data of SigTAI_UTC_DeltaStatus.
 * @return  return `nil` when initialize SigTAI_UTC_DeltaStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_TAI_UTC_DeltaStatus;
        if (parameters == nil || parameters.length != 9) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _TAI_UTC_DeltaCurrent = (tem16 >> 1) & 0x7FFF;
            _paddingCurrent = tem16 & 0b1;
            memcpy(&tem16, dataByte+2, 2);
            _TAI_UTC_DeltaNew = (tem16 >> 1) & 0x7FFF;
            _paddingNew = tem16 & 0b1;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte+4, 5);
            _TAIOfDeltaChange = tem64;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = ((_TAI_UTC_DeltaCurrent & 0x7FFF) << 1) | (_paddingCurrent & 0b1);
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = ((_TAI_UTC_DeltaNew & 0x7FFF) << 1) | (_paddingNew & 0b1);
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt64 tem64 = _TAIOfDeltaChange & 0xFFFFFFFFFF;
    data = [NSData dataWithBytes:&tem64 length:5];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 5.2.2 Scene messages


#pragma mark 5.2.2.5 Scene Get, opcode:0x8241


/// Scene Get is an acknowledged message used to get
/// the current status of a currently active scene (see
/// Section 5.1.3.2) of an element.
/// @note   The response to the Scene Get message is
/// a Scene Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.148),
/// 5.2.2.5 Scene Get.
@implementation SigSceneGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneGet;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneGet object.
 * @param   parameters    the hex data of SigSceneGet.
 * @return  return `nil` when initialize SigSceneGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSceneStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.2.3 Scene Recall, opcode:0x8242


/// Scene Recall is an acknowledged message that is used to recall
/// the current state of an element from a previously stored scene.
/// @note   The response to the Scene Recall message is a Scene
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.147),
/// 5.2.2.3 Scene Recall.
@implementation SigSceneRecall

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRecall;
    }
    return self;
}

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRecall;
        _sceneNumber = sceneNumber;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneRecall object.
 * @param   parameters    the hex data of SigSceneRecall.
 * @return  return `nil` when initialize SigSceneRecall object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRecall;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _sceneNumber = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _sceneNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSceneStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.2.4 Scene Recall Unacknowledged, opcode:0x8243


/// Scene Recall Unacknowledged is an unacknowledged message used to recall
/// the current state of an element from a previously stored Scene.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.147),
/// 5.2.2.4 Scene Recall Unacknowledged.
@implementation SigSceneRecallUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRecallUnacknowledged;
    }
    return self;
}

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRecallUnacknowledged;
        _sceneNumber = sceneNumber;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneRecallUnacknowledged object.
 * @param   parameters    the hex data of SigSceneRecallUnacknowledged.
 * @return  return `nil` when initialize SigSceneRecallUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRecallUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _sceneNumber = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _sceneNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 5.2.2.6 Scene Status, opcode:0x5E


/// Scene Status is an unacknowledged message used to report the current
/// status of a currently active scene (see Section 5.1.3.2) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.148),
/// 5.2.2.6 Scene Status.
@implementation SigSceneStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStatus;
    }
    return self;
}

- (instancetype)initWithStatusCode:(SigSceneResponseStatus)statusCode currentScene:(UInt16)currentScene targetScene:(UInt16)targetScene remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStatus;
        _currentScene = currentScene;
        _targetScene = targetScene;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneStatus object.
 * @param   parameters    the hex data of SigSceneStatus.
 * @return  return `nil` when initialize SigSceneStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStatus;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 6)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte, 1);
            _statusCode = (SigSceneResponseStatus)tem8;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte+1, 2);
            _currentScene = tem16;
            if (parameters.length == 6) {
                memcpy(&tem16, dataByte+3, 2);
                _targetScene = tem16;
                memcpy(&tem8, dataByte+5, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _statusCode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _currentScene;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem16 = _targetScene;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 5.2.2.7 Scene Register Get, opcode:0x8244


/// Scene Register Get is an acknowledged message used to get the
/// current status of the Scene Register (see Section 5.1.3.1) of an element.
/// @note   The response to the Scene Register Get message is a
/// Scene Register Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.148),
/// 5.2.2.7 Scene Register Get.
@implementation SigSceneRegisterGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRegisterGet;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneRegisterGet object.
 * @param   parameters    the hex data of SigSceneRegisterGet.
 * @return  return `nil` when initialize SigSceneRegisterGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRegisterGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSceneRegisterStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 5.2.2.8 Scene Register Status, opcode:0x8245


/// Scene Register Status is an unacknowledged message that is used to report
/// the current status of the Scene Register (see Section 5.1.3.1) of an element.
/// @note   The message uses a single-octet Opcode to maximize the payload size.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.149),
/// 5.2.2.8 Scene Register Status.
@implementation SigSceneRegisterStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRegisterStatus;
    }
    return self;
}

- (instancetype)initWithStatusCode:(SigSceneResponseStatus)statusCode currentScene:(UInt16)currentScene targetScene:(UInt16)targetScene scenes:(NSMutableArray <NSNumber *>*)scenes {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRegisterStatus;
        _statusCode = statusCode;
        _currentScene = currentScene;
        _scenes = [NSMutableArray arrayWithArray:scenes];
    }
    return self;
}

/**
 * @brief   Initialize SigSceneRegisterStatus object.
 * @param   parameters    the hex data of SigSceneRegisterStatus.
 * @return  return `nil` when initialize SigSceneRegisterStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneRegisterStatus;
        if (parameters == nil || (parameters.length < 3)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte, 1);
            _statusCode = (SigSceneResponseStatus)tem8;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte+1, 2);
            _currentScene = tem16;
            if (_scenes == nil) {
                _scenes = [NSMutableArray array];
            }
            while (parameters.length >= 3+(2*(_scenes.count + 1))) {
                memcpy(&tem16, dataByte+3+2*_scenes.count, 2);
                [_scenes addObject:@(tem16)];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _statusCode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _currentScene;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_scenes != nil && _scenes.count != 0) {
        for (int i=0; i<_scenes.count; i++) {
            tem16 = [_scenes[i] intValue];
            data = [NSData dataWithBytes:&tem16 length:2];
            [mData appendData:data];
        }
    }
    return mData;
}

@end


#pragma mark 5.2.2.1 Scene Store, opcode:0x8246


/// Scene Store is an acknowledged message used to store the current state
/// of an element as a Scene, which can be recalled later.
/// @note   The response to the Scene Store message is a Scene Register
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.2.1 Scene Store.
@implementation SigSceneStore

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStore;
    }
    return self;
}

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStore;
        _sceneNumber = sceneNumber;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneStore object.
 * @param   parameters    the hex data of SigSceneStore.
 * @return  return `nil` when initialize SigSceneStore object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStore;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte, 2);
            _sceneNumber = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _sceneNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSceneRegisterStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 5.2.2.2 Scene Store Unacknowledged, opcode:0x8247


/// Scene Store Unacknowledged is an unacknowledged message used to store
/// the current state of an element as a Scene, which can be recalled later.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.2.2 Scene Store Unacknowledged.
@implementation SigSceneStoreUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStoreUnacknowledged;
    }
    return self;
}

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStoreUnacknowledged;
        _sceneNumber = sceneNumber;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneStoreUnacknowledged object.
 * @param   parameters    the hex data of SigSceneStoreUnacknowledged.
 * @return  return `nil` when initialize SigSceneStoreUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneStoreUnacknowledged;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte, 2);
            _sceneNumber = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _sceneNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 5.2.2.9 Scene Delete, opcode:0x829E


/// Scene Delete is an acknowledged message used to delete a Scene from
/// the Scene Register state (see Section 5.1.3.1) of an element.
/// @note   The response to the Scene Delete message is a Scene Register
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.149),
/// 5.2.2.9 Scene Delete.
@implementation SigSceneDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneDelete;
    }
    return self;
}

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneDelete;
        _sceneNumber = sceneNumber;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneDelete object.
 * @param   parameters    the hex data of SigSceneDelete.
 * @return  return `nil` when initialize SigSceneDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneDelete;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte, 2);
            _sceneNumber = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _sceneNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSceneRegisterStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 5.2.2.10 Scene Delete Unacknowledged, opcode:0x829F


/// Scene Delete Unacknowledged is an unacknowledged message used to delete
/// a scene from the Scene Register state (see Section 5.1.3.1) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.149),
/// 5.2.2.10 Scene Delete Unacknowledged.
@implementation SigSceneDeleteUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneDeleteUnacknowledged;
    }
    return self;
}

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneDeleteUnacknowledged;
        _sceneNumber = sceneNumber;
    }
    return self;
}

/**
 * @brief   Initialize SigSceneDeleteUnacknowledged object.
 * @param   parameters    the hex data of SigSceneDeleteUnacknowledged.
 * @return  return `nil` when initialize SigSceneDeleteUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_sceneDeleteUnacknowledged;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte, 2);
            _sceneNumber = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _sceneNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 5.2.3 Scheduler messages


#pragma mark 5.2.3.3 Scheduler Action Get, opcode:0x8248


/// Scheduler Action Get is an acknowledged message used to report
/// the action defined by the entry of the Schedule Register state of an
/// element (see Section 5.1.4.2), identified by the Index field.
/// @note   The response to the Scheduler Action Get message is a
/// Scheduler Action Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.150),
/// 5.2.3.3 Scheduler Action Get.
@implementation SigSchedulerActionGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionGet;
    }
    return self;
}

- (instancetype)initWithIndex:(UInt8)index {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionGet;
        _index = index;
    }
    return self;
}

/**
 * @brief   Initialize SigSchedulerActionGet object.
 * @param   parameters    the hex data of SigSchedulerActionGet.
 * @return  return `nil` when initialize SigSchedulerActionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionGet;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte, 1);
            _index = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _index;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSchedulerActionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 5.2.3.6 Scheduler Action Status, opcode:0x5F


/// Scheduler Action Status is an unacknowledged message used to report
/// the entry of the Schedule Register state of an element (see Section 5.1.4.2),
/// identified by the Index field.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.151),
/// 5.2.3.6 Scheduler Action Status.
@implementation SigSchedulerActionStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionStatus;
    }
    return self;
}

- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionStatus;
        _schedulerModel = schedulerModel;
    }
    return self;
}

/**
 * @brief   Initialize SigSchedulerActionStatus object.
 * @param   parameters    the hex data of SigSchedulerActionStatus.
 * @return  return `nil` when initialize SigSchedulerActionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionStatus;
        if (parameters == nil || (parameters.length != 10)) {
            return nil;
        }else{
            SchedulerModel *model = [[SchedulerModel alloc] init];
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte, 8);
            [model setSchedulerData:tem64];
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte+8, 2);
            model.sceneId = tem16;
            _schedulerModel = model;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt64 tem64 = _schedulerModel.schedulerData;
    NSData *data = [NSData dataWithBytes:&tem64 length:8];
    [mData appendData:data];
    UInt16 tem16 = _schedulerModel.sceneId;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 5.2.3.1 Scheduler Get, opcode:0x8249


/// Scheduler Get is an acknowledged message used to get the
/// current Schedule Register state of an element (see Section 5.1.4.2).
/// @note   The response to the Scheduler Get message is a
/// Scheduler Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.150),
/// 5.2.3.1 Scheduler Get.
@implementation SigSchedulerGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerGet;
    }
    return self;
}

/**
 * @brief   Initialize SigSchedulerGet object.
 * @param   parameters    the hex data of SigSchedulerGet.
 * @return  return `nil` when initialize SigSchedulerGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSchedulerStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 5.2.3.2 Scheduler Status, opcode:0x824A


/// Scheduler Status is an unacknowledged message used to report
/// the current Schedule Register state of an element (see Section 5.1.4.2).
/// @note   The message shall be sent as a response to the
/// Scheduler Get message (see Section 5.2.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.151),
/// 5.2.3.2 Scheduler Status.
@implementation SigSchedulerStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerStatus;
        _schedulers = [NSMutableArray array];
    }
    return self;
}

- (instancetype)initWithSchedulers:(NSMutableArray <NSNumber *>*)schedulers {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerStatus;
        _schedulers = [NSMutableArray arrayWithArray:schedulers];
    }
    return self;
}

/**
 * @brief   Initialize SigSchedulerStatus object.
 * @param   parameters    the hex data of SigSchedulerStatus.
 * @return  return `nil` when initialize SigSchedulerStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerStatus;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            NSMutableArray *array = [NSMutableArray array];
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte, 2);
            for (int i=0; i<0xF; i++) {
                if (((tem16 >> i) & 0b1) == 1) {
                    [array addObject:@(0xF-i)];
                }
            }
            _schedulers = array;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = 0;
    NSArray *schedulers = [NSArray arrayWithArray:_schedulers];
    for (NSNumber *num in schedulers) {
        UInt16 schedulerID = (UInt16)num.intValue;
        tem16 |= (1 << (0xF-schedulerID));
    }
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 5.2.3.4 Scheduler Action Set, opcode:0x60


/// Scheduler Action Set is an acknowledged message used to set
/// the entry of the Schedule Register state of an element (see
/// Section 5.1.4.2), identified by the Index field.
/// @note   The response to the Scheduler Action Set message
/// is a Scheduler Action Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.150),
/// 5.2.3.4 Scheduler Action Set.
@implementation SigSchedulerActionSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionSet;
    }
    return self;
}

- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionSet;
        _schedulerModel = schedulerModel;
    }
    return self;
}

/**
 * @brief   Initialize SigSchedulerActionSet object.
 * @param   parameters    the hex data of SigSchedulerActionSet.
 * @return  return `nil` when initialize SigSchedulerActionSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionSet;
        if (parameters == nil || (parameters.length != 10)) {
            return nil;
        }else{
            SchedulerModel *model = [[SchedulerModel alloc] init];
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte, 8);
            [model setSchedulerData:tem64];
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte+8, 2);
            model.sceneId = tem16;
            _schedulerModel = model;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt64 tem64 = _schedulerModel.schedulerData;
    NSData *data = [NSData dataWithBytes:&tem64 length:8];
    [mData appendData:data];
    UInt16 tem16 = _schedulerModel.sceneId;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSchedulerActionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 5.2.3.5 Scheduler Action Set Unacknowledged, opcode:0x61


/// Scheduler Action Set Unacknowledged is an unacknowledged message used to set
/// the entry of the Schedule Register state of an element (see Section 5.1.4.2), identified
/// by the Index field.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.151),
/// 5.2.3.5 Scheduler Action Set Unacknowledged.
@implementation SigSchedulerActionSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionSetUnacknowledged;
        _schedulerModel = schedulerModel;
    }
    return self;
}

/**
 * @brief   Initialize SigSchedulerActionSetUnacknowledged object.
 * @param   parameters    the hex data of SigSchedulerActionSetUnacknowledged.
 * @return  return `nil` when initialize SigSchedulerActionSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_schedulerActionSetUnacknowledged;
        if (parameters == nil || (parameters.length != 10)) {
            return nil;
        }else{
            SchedulerModel *model = [[SchedulerModel alloc] init];
            Byte *dataByte = (Byte *)parameters.bytes;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte, 8);
            [model setSchedulerData:tem64];
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte+8, 2);
            model.sceneId = tem16;
            _schedulerModel = model;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt64 tem64 = _schedulerModel.schedulerData;
    NSData *data = [NSData dataWithBytes:&tem64 length:8];
    [mData appendData:data];
    UInt16 tem16 = _schedulerModel.sceneId;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.1 Light Lightness messages


#pragma mark 6.3.1.1 Light Lightness Get, opcode:0x824B


/// Light Lightness Get is an acknowledged message used to get
/// the Light Lightness Actual state of an element (see Section 6.1.2.1).
/// @note   The response to the Light Lightness Get message is
/// a Light Lightness Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.199),
/// 6.3.1.1 Light Lightness Get.
@implementation SigLightLightnessGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessGet object.
 * @param   parameters    the hex data of SigLightLightnessGet.
 * @return  return `nil` when initialize SigLightLightnessGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.2 Light Lightness Set, opcode:0x824C


/// The Light Lightness Set is an acknowledged message used to set
/// the Light Lightness Actual state of an element (see Section 6.1.2).
/// @note   The response to the Light Lightness Set message is a
/// Light Lightness Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.200),
/// 6.3.1.2 Light Lightness Set.
@implementation SigLightLightnessSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessSet;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessSet;
        _lightness = lightness;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessSet object.
 * @param   parameters    the hex data of SigLightLightnessSet.
 * @return  return `nil` when initialize SigLightLightnessSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.3 Light Lightness Set Unacknowledged, opcode:0x824D


/// The Light Lightness Set Unacknowledged is an unacknowledged message used to set
/// the Light Lightness Actual state of an element (see Section 6.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.200),
/// 6.3.1.3 Light Lightness Set Unacknowledged.
@implementation SigLightLightnessSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessSetUnacknowledged;
        _lightness = lightness;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.1.4 Light Lightness Status, opcode:0x824E


/// The Light Lightness Status is an unacknowledged message used to report
/// the Light Lightness Actual state of an element (see Section 6.1.2.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.201),
/// 6.3.1.4 Light Lightness Status.
@implementation SigLightLightnessStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessStatus;
    }
    return self;
}

- (instancetype)initWithPresentLightness:(UInt16)presentLightness targetLightness:(UInt16)targetLightness remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessStatus;
        _presentLightness = presentLightness;
        _targetLightness = targetLightness;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessStatus object.
 * @param   parameters    the hex data of SigLightLightnessStatus.
 * @return  return `nil` when initialize SigLightLightnessStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessStatus;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _presentLightness = tem16;
            if (parameters.length == 5) {
                memcpy(&tem16, dataByte+2, 2);
                _targetLightness = tem16;
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+4, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _presentLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem16 = _targetLightness;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.1.5 Light Lightness Linear Get, opcode:0x824F


/// Light Lightness Linear Get is an acknowledged message used to get
/// the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   The response to the Light Lightness Linear Get message
/// is a Light Lightness Linear Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.201),
/// 6.3.1.5 Light Lightness Linear Get.
@implementation SigLightLightnessLinearGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessLinearGet object.
 * @param   parameters    the hex data of SigLightLightnessLinearGet.
 * @return  return `nil` when initialize SigLightLightnessLinearGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessLinearStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.6 Light Lightness Linear Set, opcode:0x8250


/// The Light Lightness Linear Set is an acknowledged message used to set
/// the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   The response to the Light Lightness Linear Set message is a
/// Light Lightness Linear Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.201),
/// 6.3.1.6 Light Lightness Linear Set.
@implementation SigLightLightnessLinearSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearSet;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearSet;
        _lightness = lightness;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessLinearSet object.
 * @param   parameters    the hex data of SigLightLightnessLinearSet.
 * @return  return `nil` when initialize SigLightLightnessLinearSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
//v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessLinearStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.7 Light Lightness Linear Set Unacknowledged, opcode:0x8251


/// The Light Lightness Linear Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.202),
/// 6.3.1.7 Light Lightness Linear Set Unacknowledged.
@implementation SigLightLightnessLinearSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearSetUnacknowledged;
        _lightness = lightness;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessLinearSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessLinearSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessLinearSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.1.8 Light Lightness Linear Status, opcode:0x8252


/// The Light Lightness Linear Status is an unacknowledged message used to report
/// the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.203),
/// 6.3.1.8 Light Lightness Linear Status.
@implementation SigLightLightnessLinearStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearStatus;
    }
    return self;
}

- (instancetype)initWithPresentLightness:(UInt16)presentLightness targetLightness:(UInt16)targetLightness remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearStatus;
        _presentLightness = presentLightness;
        _targetLightness = targetLightness;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessLinearStatus object.
 * @param   parameters    the hex data of SigLightLightnessLinearStatus.
 * @return  return `nil` when initialize SigLightLightnessLinearStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLinearStatus;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _presentLightness = tem16;
            if (parameters.length == 5) {
                memcpy(&tem16, dataByte+2, 2);
                _targetLightness = tem16;
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+4, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _presentLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem16 = _targetLightness;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.1.9 Light Lightness Last Get, opcode:0x8253


/// Light Lightness Last Get is an acknowledged message used to get
/// the Light Lightness Last state of an element (see Section 6.1.2.3).
/// @note   The response to the Light Lightness Last Get message
/// is a Light Lightness Last Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.203),
/// 6.3.1.9 Light Lightness Last Get.
@implementation SigLightLightnessLastGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLastGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessLastGet object.
 * @param   parameters    the hex data of SigLightLightnessLastGet.
 * @return  return `nil` when initialize SigLightLightnessLastGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLastGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessLastStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.10 Light Lightness Last Status, opcode:0x8254


/// Light Lightness Last Status is an unacknowledged message used to report
/// the Light Lightness Last state of an element (see Section 6.1.2.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.203),
/// 6.3.1.10 Light Lightness Last Status.
@implementation SigLightLightnessLastStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLastStatus;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLastStatus;
        _lightness = lightness;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessLastStatus object.
 * @param   parameters    the hex data of SigLightLightnessLastStatus.
 * @return  return `nil` when initialize SigLightLightnessLastStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessLastStatus;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.1.11 Light Lightness Default Get, opcode:0x8255


/// Light Lightness Default Get is an acknowledged message used to get
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   The response to the Light Lightness Default Get message is
/// a Light Lightness Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.11 Light Lightness Default Get.
@implementation SigLightLightnessDefaultGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessDefaultGet object.
 * @param   parameters    the hex data of SigLightLightnessDefaultGet.
 * @return  return `nil` when initialize SigLightLightnessDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.14 Light Lightness Default Status, opcode:0x8256


/// Light Lightness Default Status is an unacknowledged message used to report
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.14 Light Lightness Default Status.
@implementation SigLightLightnessDefaultStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultStatus;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultStatus;
        _lightness = lightness;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessDefaultStatus object.
 * @param   parameters    the hex data of SigLightLightnessDefaultStatus.
 * @return  return `nil` when initialize SigLightLightnessDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultStatus;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.1.15 Light Lightness Range Get, opcode:0x8257


/// The Light Lightness Range Get is an acknowledged message used to get
/// the Light Lightness Range state of an element (see Section 6.1.2.5).
/// @note   The response to the Light Lightness Range Get message is a
/// Light Lightness Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.205),
/// 6.3.1.15 Light Lightness Range Get.
@implementation SigLightLightnessRangeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessRangeGet object.
 * @param   parameters    the hex data of SigLightLightnessRangeGet.
 * @return  return `nil` when initialize SigLightLightnessRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.18 Light Lightness Range Status, opcode:0x8258


/// Light Lightness Default Status is an unacknowledged message used to report
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.1.18 Light Lightness Range Status.
@implementation SigLightLightnessRangeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeStatus;
    }
    return self;
}

- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeStatus;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessRangeStatus object.
 * @param   parameters    the hex data of SigLightLightnessRangeStatus.
 * @return  return `nil` when initialize SigLightLightnessRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeStatus;
        if (parameters == nil || (parameters.length != 5)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _statusCode = (SigGenericMessageStatus)tem8;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _statusCode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _rangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.1.12 Light Lightness Default Set, opcode:0x8259


/// The Light Lightness Default Set is an acknowledged message used to set
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   The response to the Light Lightness Default Set message is a
/// Light Lightness Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.12 Light Lightness Default Set
@implementation SigLightLightnessDefaultSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultSet;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultSet;
        _lightness = lightness;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessDefaultSet object.
 * @param   parameters    the hex data of SigLightLightnessDefaultSet.
 * @return  return `nil` when initialize SigLightLightnessDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultSet;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.13 Light Lightness Default Set Unacknowledged, opcode:0x825A


/// The Light Lightness Default Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.13 Light Lightness Default Set Unacknowledged.
@implementation SigLightLightnessDefaultSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultSetUnacknowledged;
        _lightness = lightness;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessDefaultSetUnacknowledged;
        if (parameters == nil || (parameters.length != 2)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.1.16 Light Lightness Range Set, opcode:0x825B


/// Light Lightness Range Set is an acknowledged message used to set
/// the Light Lightness Range state of an element (see Section 6.1.2.5).
/// @note   The response to the Light Lightness Range Get message
/// is a Light Lightness Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.205),
/// 6.3.1.16 Light Lightness Range Set.
@implementation SigLightLightnessRangeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeSet;
    }
    return self;
}

/// Creates the Light Lightness Range Set message.
/// @param rangeMin  The value of the Lightness Range Min field of the Light Lightness Range state.
/// @param rangeMax  The value of the Lightness Range Max field of the Light Lightness Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeSet;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessRangeSet object.
 * @param   parameters    the hex data of SigLightLightnessRangeSet.
 * @return  return `nil` when initialize SigLightLightnessRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeSet;
        if (parameters == nil || (parameters.length != 4)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+0, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _rangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLightnessRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.1.17 Light Lightness Range Set Unacknowledged, opcode:0x825C


/// Light Lightness Range Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Range state of an element (see Section 6.1.2.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.205),
/// 6.3.1.17 Light Lightness Range Set Unacknowledged.
@implementation SigLightLightnessRangeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeSetUnacknowledged;
    }
    return self;
}

/// Creates the Light Lightness Range Set message.
/// @param rangeMin  The value of the Lightness Range Min field of the Light Lightness Range state.
/// @param rangeMax  The value of the Lightness Range Max field of the Light Lightness Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeSetUnacknowledged;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLightnessRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightLightnessRangeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 4)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+0, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _rangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.2 Light CTL Messages


#pragma mark 6.3.2.1 Light CTL Get, opcode:0x825D


/// Light CTL Get is an acknowledged message used to get the Light CTL state of an element (see Section 6.1.3).
/// @note   The response to the Light CTL Get message is a Light CTL Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.2.1 Light CTL Get.
@implementation SigLightCTLGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLGet object.
 * @param   parameters    the hex data of SigLightCTLGet.
 * @return  return `nil` when initialize SigLightCTLGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.2 Light CTL Set, opcode:0x825E


/// Light CTL Set is an acknowledged message used to set the Light CTL
/// Lightness state, Light CTL Temperature state, and the Light CTL Delta
/// UV state of an element (see Section 6.1.3.6, Section 6.1.3.1, and
/// Section 6.1.3.4).
/// @note   The response to the Light CTL Set message is a Light CTL
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.2.2 Light CTL Set.
@implementation SigLightCTLSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLSet;
    }
    return self;
}

- (instancetype)initWithCTLLightness:(UInt16)CTLLightness CTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLSet;
        _CTLLightness = CTLLightness;
        _CTLTemperature = CTLTemperature;
        _CTLDeltaUV = CTLDeltaUV;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLSet object.
 * @param   parameters    the hex data of SigLightCTLSet.
 * @return  return `nil` when initialize SigLightCTLSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLSet;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _CTLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _CTLTemperature = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _CTLDeltaUV = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+6, 1);
            self.tid = tem8;
            if (parameters.length == 9) {
                memcpy(&tem8, dataByte+7, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+8, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _CTLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _CTLTemperature;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _CTLDeltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.3 Light CTL Set Unacknowledged, opcode:0x825F


/// Light CTL Set Unacknowledged is an unacknowledged message used to
/// set the Light CTL Lightness state, Light CTL Temperature state, and the
/// Light CTL Delta UV state of an element (see Section 6.1.3.6, Section 6.1.3.1,
/// and Section 6.1.3.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.2.3 Light CTL Set Unacknowledged
@implementation SigLightCTLSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithCTLLightness:(UInt16)CTLLightness CTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLSetUnacknowledged;
        _CTLLightness = CTLLightness;
        _CTLTemperature = CTLTemperature;
        _CTLDeltaUV = CTLDeltaUV;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLSetUnacknowledged;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _CTLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _CTLTemperature = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _CTLDeltaUV = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+6, 1);
            self.tid = tem8;
            if (parameters.length == 9) {
                memcpy(&tem8, dataByte+7, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+8, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _CTLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _CTLTemperature;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _CTLDeltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.2.4 Light CTL Status, opcode:0x8260


/// The Light CTL Status is an unacknowledged message used to report
/// the Light CTL Lightness and the Light CTL Temperature state of an
/// element (see Section 6.1.3.6 and 6.1.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.208),
/// 6.3.2.4 Light CTL Status.
@implementation SigLightCTLStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLStatus;
    }
    return self;
}

- (instancetype)initWithPresentCTLLightness:(UInt16)presentCTLLightness presentCTLTemperature:(UInt16)presentCTLTemperature targetCTLLightness:(UInt16)targetCTLLightness targetCTLTemperature:(UInt16)targetCTLTemperature remainingTime:(SigTransitionTime * _Nullable )remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLStatus;
        _presentCTLLightness = presentCTLLightness;
        _presentCTLTemperature = presentCTLTemperature;
        _targetCTLLightness = targetCTLLightness;
        _targetCTLTemperature = targetCTLTemperature;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLStatus object.
 * @param   parameters    the hex data of SigLightCTLStatus.
 * @return  return `nil` when initialize SigLightCTLStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLStatus;
        if (parameters == nil || (parameters.length != 4 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _presentCTLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _presentCTLTemperature = tem16;
            if (parameters.length == 9) {
                memcpy(&tem16, dataByte+4, 2);
                _targetCTLLightness = tem16;
                memcpy(&tem16, dataByte+6, 2);
                _targetCTLTemperature = tem16;
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+8, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _presentCTLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _presentCTLTemperature;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem16 = _targetCTLLightness;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _targetCTLTemperature;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.2.5 Light CTL Temperature Get, opcode:0x8261


/// The Light CTL Status is an unacknowledged message used to report
/// the Light CTL Lightness and the Light CTL Temperature state of an
/// element (see Section 6.1.3.6 and 6.1.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.208),
/// 6.3.2.5 Light CTL Temperature Get.
@implementation SigLightCTLTemperatureGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureGet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureGet.
 * @return  return `nil` when initialize SigLightCTLTemperatureGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLTemperatureStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.9 Light CTL Temperature Range Get, opcode:0x8262


/// The Light CTL Temperature Range Get is an acknowledged message used to
/// get the Light CTL Temperature Range state of an element (see Section 6.1.3.3).
/// @note   The response to the Light CTL Temperature Range Get message is
/// a Light CTL Temperature Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.210),
/// 6.3.2.9 Light CTL Temperature Range Get.
@implementation SigLightCTLTemperatureRangeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureRangeGet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeGet.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLTemperatureRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.12 Light CTL Temperature Range Status, opcode:0x8263


/// Light CTL Temperature Range Status is an unacknowledged message used to report
/// the Light CTL Temperature Range state of an element (see Section 6.1.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.211),
/// 6.3.2.12 Light CTL Temperature Range Status.
@implementation SigLightCTLTemperatureRangeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeStatus;
    }
    return self;
}

- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeStatus;
        _statusCode = statusCode;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureRangeStatus object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeStatus.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeStatus;
        if (parameters == nil || (parameters.length != 5)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _statusCode = (SigGenericMessageStatus)tem8;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _statusCode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _rangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.2.6 Light CTL Temperature Set, opcode:0x8264


/// The Light CTL Temperature Set is an acknowledged message used to set
/// the Light CTL Temperature state and the Light CTL Delta UV state of an
/// element (see Section 6.1.3.1 and 6.1.3.4).
/// @note   The response to the Light CTL Temperature Set message is a
/// Light CTL Temperature Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.208),
/// 6.3.2.6 Light CTL Temperature Set.
@implementation SigLightCTLTemperatureSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureSet;
    }
    return self;
}

- (instancetype)initWithCTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureSet;
        _CTLTemperature = CTLTemperature;
        _CTLDeltaUV = CTLDeltaUV;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureSet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureSet.
 * @return  return `nil` when initialize SigLightCTLTemperatureSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureSet;
        if (parameters == nil || (parameters.length != 5 && parameters.length != 7)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _CTLTemperature = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _CTLDeltaUV = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+4, 1);
            self.tid = tem8;
            if (parameters.length == 7) {
                memcpy(&tem8, dataByte+5, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+6, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _CTLTemperature;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _CTLDeltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLTemperatureStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.7 Light CTL Temperature Set Unacknowledged, opcode:0x8265


/// The Light CTL Temperature Set Unacknowledged is an unacknowledged message
/// used to set the Light CTL Temperature state and the Light CTL Delta UV state of an
/// element (see Section 6.1.3.1 and 6.1.3.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.209),
/// 6.3.2.7 Light CTL Temperature Set Unacknowledged.
@implementation SigLightCTLTemperatureSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithCTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureSetUnacknowledged;
        _CTLTemperature = CTLTemperature;
        _CTLDeltaUV = CTLDeltaUV;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLTemperatureSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLTemperatureSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureSetUnacknowledged;
        if (parameters == nil || (parameters.length != 5 && parameters.length != 7)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _CTLTemperature = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _CTLDeltaUV = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+4, 1);
            self.tid = tem8;
            if (parameters.length == 7) {
                memcpy(&tem8, dataByte+5, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+6, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _CTLTemperature;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _CTLDeltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.2.8 Light CTL Temperature Status, opcode:0x8266


/// Light CTL Temperature Status is an unacknowledged message used to report
/// the Light CTL Temperature and Light CTL Delta UV state of an element (see
/// Section 6.1.3.1 and 6.1.3.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.210),
/// 6.3.2.8 Light CTL Temperature Status.
@implementation SigLightCTLTemperatureStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureStatus;
    }
    return self;
}

- (instancetype)initWithPresentCTLTemperature:(UInt16)presentCTLTemperature presentCTLDeltaUV:(UInt16)presentCTLDeltaUV targetCTLTemperature:(UInt16)targetCTLTemperature targetCTLDeltaUV:(UInt16)targetCTLDeltaUV remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureStatus;
        _presentCTLTemperature = presentCTLTemperature;
        _presentCTLDeltaUV = presentCTLDeltaUV;
        _targetCTLTemperature = targetCTLTemperature;
        _targetCTLDeltaUV = targetCTLDeltaUV;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureStatus object.
 * @param   parameters    the hex data of SigLightCTLTemperatureStatus.
 * @return  return `nil` when initialize SigLightCTLTemperatureStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureStatus;
        if (parameters == nil || (parameters.length != 4 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _presentCTLTemperature = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _presentCTLDeltaUV = tem16;
            if (parameters.length == 9) {
                memcpy(&tem16, dataByte+4, 2);
                _targetCTLTemperature = tem16;
                memcpy(&tem16, dataByte+6, 2);
                _targetCTLDeltaUV = tem16;
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+8, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _presentCTLTemperature;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _presentCTLDeltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem16 = _targetCTLTemperature;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _targetCTLDeltaUV;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.2.13 Light CTL Default Get, opcode:0x8267


/// Light CTL Default Get is an acknowledged message used to get the Light CTL Temperature Default and Light CTL Delta UV Default states of an element (see Section 6.1.3).
/// @note   The response to the Light CTL Default Get message is a Light CTL Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.212),
/// 6.3.2.13 Light CTL Default Get.
@implementation SigLightCTLDefaultGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLDefaultGet object.
 * @param   parameters    the hex data of SigLightCTLDefaultGet.
 * @return  return `nil` when initialize SigLightCTLDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.16 Light CTL Default Status, opcode:0x8268


/// The Light CTL Default Status is an unacknowledged message used to report
/// the Light CTL Temperature Default and the Light CTL Delta UV Default states
/// of an element (see Section 6.1.3.2 and 6.1.3.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.213),
/// 6.3.2.16 Light CTL Default Status.
@implementation SigLightCTLDefaultStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultStatus;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultStatus;
        _lightness = lightness;
        _temperature = temperature;
        _deltaUV = deltaUV;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLDefaultStatus object.
 * @param   parameters    the hex data of SigLightCTLDefaultStatus.
 * @return  return `nil` when initialize SigLightCTLDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultStatus;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _temperature = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _deltaUV = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _temperature;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _deltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.2.14 Light CTL Default Set, opcode:0x8269


/// The Light CTL Default Set is an acknowledged message used to set the
/// Light CTL Temperature Default state and the Light CTL Delta UV Default
/// state of an element (see Section 6.1.3.2 and 6.1.3.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.212),
/// 6.3.2.14 Light CTL Default Set.
@implementation SigLightCTLDefaultSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultSet;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultSet;
        _lightness = lightness;
        _temperature = temperature;
        _deltaUV = deltaUV;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLDefaultSet object.
 * @param   parameters    the hex data of SigLightCTLDefaultSet.
 * @return  return `nil` when initialize SigLightCTLDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultSet;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _temperature = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _deltaUV = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _temperature;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _deltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.14 Light CTL Default Set, opcode:0x826A


/// The Light CTL Default Set Unacknowledged is an unacknowledged message
/// used to set the Light CTL Temperature Default state and the Light CTL Delta
/// UV Default state of an element (see Section 6.1.3.2 and 6.1.3.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.212),
/// 6.3.2.14 Light CTL Default Set.
@implementation SigLightCTLDefaultSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultSetUnacknowledged;
        _lightness = lightness;
        _temperature = temperature;
        _deltaUV = deltaUV;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLDefaultSetUnacknowledged;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _temperature = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _deltaUV = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _temperature;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _deltaUV;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.2.10 Light CTL Temperature Range Set, opcode:0x826B


/// Light CTL Temperature Range Set is an acknowledged message
///  used to set the Light CTL Temperature Range state of an
///  element (see Section 6.1.3.3).
/// @note   The response to the Light CTL Temperature Range Get
/// message is a Light CTL Temperature Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.210),
/// 6.3.2.10 Light CTL Temperature Range Set.
@implementation SigLightCTLTemperatureRangeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeSet;
    }
    return self;
}

- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeSet;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureRangeSet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeSet.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeSet;
        if (parameters == nil || (parameters.length != 4)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+0, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _rangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightCTLTemperatureRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.2.11 Light CTL Temperature Range Set Unacknowledged, opcode:0x826C


/// Light CTL Temperature Range Set Unacknowledged is an unacknowledged
/// message used to set the Light CTL Temperature Range state of an element
/// (see Section 6.1.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.211),
/// 6.3.2.11 Light CTL Temperature Range Set Unacknowledged.
@implementation SigLightCTLTemperatureRangeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeSetUnacknowledged;
        _rangeMin = rangeMin;
        _rangeMax = rangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightCTLTemperatureRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightCTLTemperatureRangeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 4)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+0, 2);
        _rangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _rangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _rangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _rangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.3 Light HSL messages


#pragma mark 6.3.3.1 Light HSL Get, opcode:0x826D


/// The Light HSL Get is an acknowledged message used to get
/// the Light HSL Lightness (see Section 6.1.4.5), Light HSL Hue
/// (see Section 6.1.4.1), and Light HSL Saturation (see
/// Section 6.1.4.2) states of an element.
/// @note   The response to the Light HSL Get message is
/// a Light HSL Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.213),
/// 6.3.3.1 Light HSL Get.
@implementation SigLightHSLGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLGet object.
 * @param   parameters    the hex data of SigLightHSLGet.
 * @return  return `nil` when initialize SigLightHSLGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.7 Light HSL Hue Get, opcode:0x826E


/// The Light HSL Hue Get is an acknowledged message used to get
/// the Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   The response to the Light HSL Hue Get message is a
/// Light HSL Hue Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.216),
/// 6.3.3.7 Light HSL Hue Get.
@implementation SigLightHSLHueGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLHueGet object.
 * @param   parameters    the hex data of SigLightHSLHueGet.
 * @return  return `nil` when initialize SigLightHSLHueGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLHueStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.8 Light HSL Hue Set, opcode:0x826F


/// The Light HSL Hue Set is an acknowledged message used to set the
/// target Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   The response to the Light HSL Hue Set message is a Light
/// HSL Hue Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.216),
/// 6.3.3.8 Light HSL Hue Set.
@implementation SigLightHSLHueSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueSet;
    }
    return self;
}

- (instancetype)initWithHue:(UInt16)hue transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueSet;
        _hue = hue;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLHueSet object.
 * @param   parameters    the hex data of SigLightHSLHueSet.
 * @return  return `nil` when initialize SigLightHSLHueSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _hue = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _hue;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLHueStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.9 Light HSL Hue Set Unacknowledged, opcode:0x8270


/// The Light HSL Hue Set Unacknowledged is an unacknowledged message
/// used to set the target Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.216),
/// 6.3.3.9 Light HSL Hue Set Unacknowledged.
@implementation SigLightHSLHueSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithHue:(UInt16)hue transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueSetUnacknowledged;
        _hue = hue;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLHueSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLHueSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLHueSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _hue = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _hue;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.3.10 Light HSL Hue Status, opcode:0x8271


/// The Light HSL Hue Status is an unacknowledged message used
/// to report the Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.217),
/// 6.3.3.10 Light HSL Hue Status.
@implementation SigLightHSLHueStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueStatus;
    }
    return self;
}

- (instancetype)initWithPresentHue:(UInt16)presentHue targetHue:(UInt16)targetHue transitionTime:(SigTransitionTime *)transitionTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueStatus;
        _presentHue = presentHue;
        _targetHue = targetHue;
        _transitionTime = transitionTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLHueStatus object.
 * @param   parameters    the hex data of SigLightHSLHueStatus.
 * @return  return `nil` when initialize SigLightHSLHueStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLHueStatus;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _presentHue = tem16;
            if (parameters.length == 5) {
                memcpy(&tem16, dataByte+2, 2);
                _targetHue = tem16;
                memcpy(&tem8, dataByte+4, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _presentHue;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_transitionTime) {
        tem16 = _targetHue;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.3.11 Light HSL Saturation Get, opcode:0x8272


/// The Light HSL Saturation Get is an acknowledged message used to
/// get the Light HSL Saturation state of an element (see Section 6.1.4.2).
/// @note   The response to the Light HSL Saturation Get message is
/// a Light HSL Saturation Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.217),
/// 6.3.3.11 Light HSL Saturation Get.
@implementation SigLightHSLSaturationGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLSaturationGet object.
 * @param   parameters    the hex data of SigLightHSLSaturationGet.
 * @return  return `nil` when initialize SigLightHSLSaturationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLSaturationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.12 Light HSL Saturation Set, opcode:0x8273


/// The Light HSL Saturation Set is an acknowledged message used to
/// set the target Light HSL Saturation state of an element (see
/// Section 6.1.4.2).
/// @note   The response to the Light HSL Saturation Set message is
/// a Light HSL Saturation Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.218),
/// 6.3.3.12 Light HSL Saturation Set.
@implementation SigLightHSLSaturationSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationSet;
    }
    return self;
}

- (instancetype)initWithSaturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationSet;
        _saturation = saturation;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLSaturationSet object.
 * @param   parameters    the hex data of SigLightHSLSaturationSet.
 * @return  return `nil` when initialize SigLightHSLSaturationSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationSet;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _saturation = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _saturation;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLSaturationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.13 Light HSL Saturation Set Unacknowledged, opcode:0x8274


/// The Light HSL Saturation Set Unacknowledged is an unacknowledged message
/// used to set the target Light HSL Saturation state of an element (see Section 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.218),
/// 6.3.3.13 Light HSL Saturation Set Unacknowledged.
@implementation SigLightHSLSaturationSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithSaturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationSetUnacknowledged;
        _saturation = saturation;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLSaturationSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLSaturationSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLSaturationSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationSetUnacknowledged;
        if (parameters == nil || (parameters.length != 3 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _saturation = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            self.tid = tem8;
            if (parameters.length == 5) {
                memcpy(&tem8, dataByte+3, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+4, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _saturation;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.3.14 Light HSL Saturation Status, opcode:0x8275


/// The Light HSL Saturation Status is an unacknowledged message used to
/// report the Light HSL Saturation state of an element (see Section 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.219),
/// 6.3.3.14 Light HSL Saturation Status.
@implementation SigLightHSLSaturationStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationStatus;
    }
    return self;
}

- (instancetype)initWithPresentSaturation:(UInt16)presentSaturation targetSaturation:(UInt16)targetSaturation remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationStatus;
        _presentSaturation = presentSaturation;
        _targetSaturation = targetSaturation;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLSaturationStatus object.
 * @param   parameters    the hex data of SigLightHSLSaturationStatus.
 * @return  return `nil` when initialize SigLightHSLSaturationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSaturationStatus;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 5)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _presentSaturation = tem16;
            if (parameters.length == 5) {
                memcpy(&tem16, dataByte+2, 2);
                _targetSaturation = tem16;
                memcpy(&tem8, dataByte+4, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _presentSaturation;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime) {
        tem16 = _targetSaturation;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.3.2 Light HSL Set, opcode:0x8276


/// The Light HSL Set is an acknowledged message used to set
/// the Light HSL Lightness (see Section 6.1.4.5) state, Light HSL
/// Hue state (see Section 6.1.4.1), and the Light HSL Saturation
/// state (see Section 6.1.4.2) of an element.
/// @note   The response to the Light HSL Set message is a
/// Light HSL Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.213),
/// 6.3.3.2 Light HSL Set.
@implementation SigLightHSLSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSet;
    }
    return self;
}

- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSet;
        _HSLLightness = HSLLightness;
        _HSLHue = HSLHue;
        _HSLSaturation = HSLSaturation;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLSet object.
 * @param   parameters    the hex data of SigLightHSLSet.
 * @return  return `nil` when initialize SigLightHSLSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSet;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _HSLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _HSLHue = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _HSLSaturation = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+6, 1);
            self.tid = tem8;
            if (parameters.length == 9) {
                memcpy(&tem8, dataByte+7, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+8, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _HSLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLHue;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLSaturation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.3 Light HSL Set Unacknowledged, opcode:0x8277


/// The Light HSL Set Unacknowledged is an unacknowledged message
/// used to set the Light HSL Lightness (see Section 6.1.4.5) state, Light
/// HSL Hue state (see Section 6.1.4.1), and the Light HSL Saturation
/// state (see Section 6.1.4.2) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.214),
/// 6.3.3.3 Light HSL Set Unacknowledged.
@implementation SigLightHSLSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSetUnacknowledged;
        _HSLLightness = HSLLightness;
        _HSLHue = HSLHue;
        _HSLSaturation = HSLSaturation;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLSetUnacknowledged;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _HSLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _HSLHue = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _HSLSaturation = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+6, 1);
            self.tid = tem8;
            if (parameters.length == 9) {
                memcpy(&tem8, dataByte+7, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+8, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _HSLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLHue;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLSaturation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.3.4 Light HSL Status, opcode:0x8278


/// Light HSL Status is an unacknowledged message used to report
/// the Light HSL Lightness, Light HSL Hue, and Light HSL Saturation
/// states of an element (see Section 6.1.4.5, 6.1.4.1, and 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.215),
/// 6.3.3.4 Light HSL Status.
@implementation SigLightHSLStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLStatus;
    }
    return self;
}

- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation remainingTime:(SigTransitionTime * _Nullable )remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLStatus;
        _HSLLightness = HSLLightness;
        _HSLHue = HSLHue;
        _HSLSaturation = HSLSaturation;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLStatus object.
 * @param   parameters    the hex data of SigLightHSLStatus.
 * @return  return `nil` when initialize SigLightHSLStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLStatus;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _HSLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _HSLHue = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _HSLSaturation = tem16;
            if (parameters.length == 7) {
                memcpy(&tem8, dataByte+6, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _HSLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLHue;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLSaturation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime) {
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.3.5 Light HSL Target Get, opcode:0x8279


/// Light HSL Target Get is an acknowledged message used to get the
/// target Light HSL Lightness (see Section 6.1.4.5), Light HSL Hue (see
/// Section 6.1.4.1), and Light HSL Saturation (see Section 6.1.4.2) states
/// of an element.
/// @note   For example, it may be used when an element reports it is
/// in transition to target Light HSL Lightness (see Section 6.1.4.5), Light
/// HSL Hue (see Section 6.1.4.1), or Light HSL Saturation (see
/// Section 6.1.4.2) states by including a positive Remaining Time field in
/// the Light HSL Status message (see Section 6.3.3.4), the Light Lightness
/// Status message (see Section 6.3.1.4), or the Light xyL Status message
/// (see Section 6.3.4.4).
/// @note   The response to the Light HSL Target Get message is a
/// Light HSL Target Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.215),
/// 6.3.3.5 Light HSL Target Get.
@implementation SigLightHSLTargetGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLTargetGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLTargetGet object.
 * @param   parameters    the hex data of SigLightHSLTargetGet.
 * @return  return `nil` when initialize SigLightHSLTargetGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLTargetGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLTargetStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.6 Light HSL Target Status, opcode:0x827A


/// The Light HSL Target Status is an unacknowledged message used to
/// report the target Light HSL Lightness, Light HSL Hue, and Light HSL
/// Saturation states of an element (see Section 6.1.4.5, 6.1.4.1 and 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.215),
/// 6.3.3.6 Light HSL Target Status.
@implementation SigLightHSLTargetStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLTargetStatus;
    }
    return self;
}

- (instancetype)initWithHSLLightnessTarget:(UInt16)HSLLightnessTarget HSLHueTarget:(UInt16)HSLHueTarget HSLSaturationTarget:(UInt16)HSLSaturationTarget remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLTargetStatus;
        _HSLLightnessTarget = HSLLightnessTarget;
        _HSLHueTarget = HSLHueTarget;
        _HSLSaturationTarget = HSLSaturationTarget;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLTargetStatus object.
 * @param   parameters    the hex data of SigLightHSLTargetStatus.
 * @return  return `nil` when initialize SigLightHSLTargetStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLTargetStatus;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _HSLLightnessTarget = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _HSLHueTarget = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _HSLSaturationTarget = tem16;
            if (parameters.length == 7) {
                memcpy(&tem8, dataByte+6, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _HSLLightnessTarget;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLHueTarget;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _HSLSaturationTarget;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime) {
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.3.15 Light HSL Default Get, opcode:0x827B


/// Light HSL Default Get is an acknowledged message used to get
/// the Light Lightness Default (see Section 6.1.2.4), the Light HSL
/// Hue Default (see Section 6.1.4.2), and Light HSL Saturation Default
/// (see Section 6.1.4.5) states of an element.
/// @note   The response to the Light HSL Default Get message is
/// a Light HSL Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.219),
/// 6.3.3.15 Light HSL Default Get.
@implementation SigLightHSLDefaultGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLDefaultGet object.
 * @param   parameters    the hex data of SigLightHSLDefaultGet.
 * @return  return `nil` when initialize SigLightHSLDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


/// Light HSL Default Status is an unacknowledged message used to report
/// the Light Lightness Default (see Section 6.1.2.4), the Light HSL Hue Default
/// (see Section 6.1.4.2), and Light HSL Saturation Default (see Section 6.1.4.5)
/// states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.220),
/// 6.3.3.18 Light HSL Default Status.
@implementation SigLightHSLDefaultStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultStatus;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultStatus;
        _lightness = lightness;
        _hue = hue;
        _saturation = saturation;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLDefaultStatus object.
 * @param   parameters    the hex data of SigLightHSLDefaultStatus.
 * @return  return `nil` when initialize SigLightHSLDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultStatus;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _hue = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _saturation = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _hue;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.3.19 Light HSL Range Get, opcode:0x827D


/// The Light HSL Range Get is an acknowledged message used to get
/// the Light HSL Hue Range (see Section 6.1.4.3) and Light HSL Saturation
/// Range (see Section 6.1.4.6) states of an element.
/// @note   The response to the Light HSL Range Get message is a
/// Light HSL Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.221),
/// 6.3.3.19 Light HSL Range Get.
@implementation SigLightHSLRangeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLRangeGet object.
 * @param   parameters    the hex data of SigLightHSLRangeGet.
 * @return  return `nil` when initialize SigLightHSLRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.22 Light HSL Range Status, opcode:0x827E


/// Light HSL Range Status is an unacknowledged message used to report
/// the Light HSL Hue Range (see Section 6.1.4.3) and Light HSL Saturation
/// Range (see Section 6.1.4.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.222),
/// 6.3.3.22 Light HSL Range Status.
@implementation SigLightHSLRangeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeStatus;
    }
    return self;
}

- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode hueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeStatus;
        _statusCode = statusCode;
        _hueRangeMin = hueRangeMin;
        _hueRangeMax = hueRangeMax;
        _saturationRangeMin = saturationRangeMin;
        _saturationRangeMax = saturationRangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLRangeStatus object.
 * @param   parameters    the hex data of SigLightHSLRangeStatus.
 * @return  return `nil` when initialize SigLightHSLRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeStatus;
        if (parameters == nil || (parameters.length != 9)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _statusCode = (SigGenericMessageStatus)tem8;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _hueRangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _hueRangeMax = tem;
        memcpy(&tem, dataByte+5, 2);
        _saturationRangeMin = tem;
        memcpy(&tem, dataByte+7, 2);
        _saturationRangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _statusCode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _hueRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _hueRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturationRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturationRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.3.16 Light HSL Default Set, opcode:0x827F


/// Light HSL Default Set is an acknowledged message used to set
/// the Light Lightness Default (see Section 6.1.2.4), the Light HSL
/// Hue Default (see Section 6.1.4.2), and Light HSL Saturation
/// Default (see Section 6.1.4.5) states of an element.
/// @note   The response to the Light HSL Default Set message is
/// a Light HSL Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.219),
/// 6.3.3.16 Light HSL Default Set.
@implementation SigLightHSLDefaultSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultSet;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultSet;
        _lightness = lightness;
        _hue = hue;
        _saturation = saturation;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLDefaultSet object.
 * @param   parameters    the hex data of SigLightHSLDefaultSet.
 * @return  return `nil` when initialize SigLightHSLDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultSet;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _hue = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _saturation = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _hue;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.17 Light HSL Default Set Unacknowledged, opcode:0x8280


/// Light HSL Default Set Unacknowledged is an unacknowledged message used
/// to set the Light Lightness Default (see Section 6.1.2.4), the Light HSL Hue
/// Default (see Section 6.1.4.2), and Light HSL Saturation Default (see Section 6.1.4.5)
/// states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.220),
/// 6.3.3.17 Light HSL Default Set Unacknowledged.
@implementation SigLightHSLDefaultSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultSetUnacknowledged;
        _lightness = lightness;
        _hue = hue;
        _saturation = saturation;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLDefaultSetUnacknowledged;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _hue = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _saturation = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _hue;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.3.20 Light HSL Range Set, opcode:0x8281


/// Light HSL Range Set is an acknowledged message used to set the
/// Light HSL Hue Range (see Section 6.1.4.3) and Light HSL Saturation
/// Range (see Section 6.1.4.6) states of an element.
/// @note   The response to the Light HSL Range Set message is a
/// Light HSL Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.221),
/// 6.3.3.20 Light HSL Range Set.
@implementation SigLightHSLRangeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeSet;
    }
    return self;
}

- (instancetype)initWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeSet;
        _hueRangeMin = hueRangeMin;
        _hueRangeMax = hueRangeMax;
        _saturationRangeMin = saturationRangeMin;
        _saturationRangeMax = saturationRangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLRangeSet object.
 * @param   parameters    the hex data of SigLightHSLRangeSet.
 * @return  return `nil` when initialize SigLightHSLRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeSet;
        if (parameters == nil || (parameters.length != 8)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte, 2);
        _hueRangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _hueRangeMax = tem;
        memcpy(&tem, dataByte+4, 2);
        _saturationRangeMin = tem;
        memcpy(&tem, dataByte+6, 2);
        _saturationRangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _hueRangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _hueRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturationRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturationRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightHSLRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.3.21 Light HSL Range Set Unacknowledged, opcode:0x82


/// Light HSL Range Set Unacknowledged is an unacknowledged message
/// used to set the Light HSL Hue Range (see Section 6.1.4.3) and Light HSL
/// Saturation Range (see Section 6.1.4.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.221),
/// 6.3.3.21 Light HSL Range Set Unacknowledged.
@implementation SigLightHSLRangeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeSetUnacknowledged;
        _hueRangeMin = hueRangeMin;
        _hueRangeMax = hueRangeMax;
        _saturationRangeMin = saturationRangeMin;
        _saturationRangeMax = saturationRangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightHSLRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightHSLRangeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 8)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte, 2);
        _hueRangeMin = tem;
        memcpy(&tem, dataByte+2, 2);
        _hueRangeMax = tem;
        memcpy(&tem, dataByte+4, 2);
        _saturationRangeMin = tem;
        memcpy(&tem, dataByte+6, 2);
        _saturationRangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _hueRangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _hueRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturationRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _saturationRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.4 Light xyL messages


#pragma mark 6.3.4.1 Light xyL Get, opcode:0x8283


/// The Light xyL Get is an acknowledged message used to get
/// the Light xyL Lightness (see Section 6.1.5.5), Light xyL x (see
/// Section 6.1.5.1), and Light xyL y (see Section 6.1.5.2) states
/// of an element.
/// @note   Upon receiving a Light xyL Get message, the element
/// shall respond with a Light xyL Status message.
/// @note   The response to the Light xyL Get message is a Light
/// xyL Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.223),
/// 6.3.4.1 Light xyL Get.
@implementation SigLightXyLGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLGet object.
 * @param   parameters    the hex data of SigLightXyLGet.
 * @return  return `nil` when initialize SigLightXyLGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.2 Light xyL Set, opcode:0x8284


/// The Light xyL Set is an acknowledged message used to set the Light
/// xyL Lightness (see Section 6.1.5.5), Light xyL x state (see Section 6.1.5.1),
/// and the Light xyL y states (see Section 6.1.5.2) of an element.
/// @note   The response to the Light xyL Set message is a Light xyL
/// Status message. The structure of the message is defined in the following table.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.223),
/// 6.3.4.2 Light xyL Set.
@implementation SigLightXyLSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLSet;
    }
    return self;
}

- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLSet;
        _xyLLightness = xyLLightness;
        _xyLX = xyLX;
        _xyLY = xyLY;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLSet object.
 * @param   parameters    the hex data of SigLightXyLSet.
 * @return  return `nil` when initialize SigLightXyLSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLSet;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _xyLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _xyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _xyLY = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+6, 1);
            self.tid = tem8;
            if (parameters.length == 9) {
                memcpy(&tem8, dataByte+7, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+8, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _xyLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.3 Light xyL Set Unacknowledged, opcode:0x8285


/// The Light xyL Set Unacknowledged is an unacknowledged message used to
/// set the Light xyL Lightness (see Section 6.1.5.5), Light xyL x (see Section 6.1.5.1),
/// and the Light xyL y states (see Section 6.1.5.2) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.224),
/// 6.3.4.3 Light xyL Set Unacknowledged.
@implementation SigLightXyLSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLSetUnacknowledged;
        _xyLLightness = xyLLightness;
        _xyLX = xyLX;
        _xyLY = xyLY;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightXyLSetUnacknowledged.
 * @return  return `nil` when initialize SigLightXyLSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLSetUnacknowledged;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _xyLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _xyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _xyLY = tem16;
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+6, 1);
            self.tid = tem8;
            if (parameters.length == 9) {
                memcpy(&tem8, dataByte+7, 1);
                _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
                memcpy(&tem8, dataByte+8, 1);
                _delay = tem8;
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _xyLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.4.4 Light xyL Status, opcode:0x8286


/// The Light xyL Status is an unacknowledged message used to report
/// the Light xyL Lightness, Light xyL x, and Light xyL y states of an
/// element (see Section 6.1.5.5, 6.1.5.1 and 6.1.5.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.224),
/// 6.3.4.4 Light xyL Status.
@implementation SigLightXyLStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLStatus;
    }
    return self;
}

- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLStatus;
        _xyLLightness = xyLLightness;
        _xyLX = xyLX;
        _xyLY = xyLY;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLStatus object.
 * @param   parameters    the hex data of SigLightXyLStatus.
 * @return  return `nil` when initialize SigLightXyLStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLStatus;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 7)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _xyLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _xyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _xyLY = tem16;
            if (parameters.length == 7) {
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+6, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _xyLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.4.5 Light xyL Target Get, opcode:0x8287


/// The Light xyL Target Get is an acknowledged message used to get the
/// target Light xyL Lightness (see Section 6.1.5.5), Light xyL x (see
/// Section 6.1.5.1), and Light xyL y (see Section 6.1.5.2) states of an element.
/// @note   For example, it may be used when an element reports it is in
/// transition to new Light xyL Lightness (see Section 6.1.5.5), Light xyL x
/// (see Section 6.1.5.1), or Light xyL y (see Section 6.1.5.2) states by including
/// a positive Remaining Time field in the Light xyL Status message (see
/// Section 6.3.4.4), Light Lightness Status message (see Section 6.3.1.4),
/// or the Light HSL Status message (see Section 6.3.3.4).
/// @note   The response to the Light xyL Target Get message is a Light
/// xyL Target Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.225),
/// 6.3.4.5 Light xyL Target Get.
@implementation SigLightXyLTargetGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLTargetGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLTargetGet object.
 * @param   parameters    the hex data of SigLightXyLTargetGet.
 * @return  return `nil` when initialize SigLightXyLTargetGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLTargetGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLTargetStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.6 Light xyL Target Status, opcode:0x8288


/// Light xyL Target Status is an unacknowledged message used to report
/// the target Light xyL Lightness, Light xyL x, and Light xyL y states of an
/// element (see Section 6.1.5.5, 6.1.5.1 and 6.1.5.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.225),
/// 6.3.4.6 Light xyL Target Status.
@implementation SigLightXyLTargetStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLTargetStatus;
    }
    return self;
}

- (instancetype)initWithTargetXyLLightness:(UInt16)targetXyLLightness targetXyLX:(UInt16)targetXyLX targetXyLY:(UInt16)targetXyLY remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLTargetStatus;
        _targetXyLLightness = targetXyLLightness;
        _targetXyLX = targetXyLX;
        _targetXyLY = targetXyLY;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLTargetStatus object.
 * @param   parameters    the hex data of SigLightXyLTargetStatus.
 * @return  return `nil` when initialize SigLightXyLTargetStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLTargetStatus;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 7)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _targetXyLLightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _targetXyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _targetXyLY = tem16;
            if (parameters.length == 7) {
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+6, 1);
                _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem8];
            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _targetXyLLightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _targetXyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _targetXyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_remainingTime != nil) {
        UInt8 tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 6.3.4.7 Light xyL Default Get, opcode:0x8289


/// Light xyL Default Get is an acknowledged message used to get the
/// Light Lightness Default (see Section 6.1.2.4), the Light xyL x Default
/// (see Section 6.1.5.2), and Light xyL y Default (see Section 6.1.5.5)
/// states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.225),
/// 6.3.4.7 Light xyL Default Get.
@implementation SigLightXyLDefaultGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLDefaultGet object.
 * @param   parameters    the hex data of SigLightXyLDefaultGet.
 * @return  return `nil` when initialize SigLightXyLDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.10 Light xyL Default Status, opcode:0x828A


/// Light xyL Default Status is an unacknowledged message used to report
/// the Light Lightness Default (see Section 6.1.2.4), the Light xyL x Default
/// (see Section 6.1.5.2), and Light xyL y Default (see Section 6.1.5.5) states
/// of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.226),
/// 6.3.4.10 Light xyL Default Status.
@implementation SigLightXyLDefaultStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultStatus;
    }
    return self;
}

- (instancetype)initWithXyLLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultStatus;
        _lightness = lightness;
        _xyLX = xyLX;
        _xyLY = xyLY;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLDefaultStatus object.
 * @param   parameters    the hex data of SigLightXyLDefaultStatus.
 * @return  return `nil` when initialize SigLightXyLDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultStatus;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _xyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _xyLY = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.4.11 Light xyL Range Get, opcode:0x828B


/// The Light xyL Range Get is an acknowledged message used to get
/// the Light xyL x Range (see Section 6.1.5.3) and Light xyL y Range
/// (see Section 6.1.5.6) states of an element.
/// @note   The response to the Light xyL Range Get message is a
/// Light xyL Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.227),
/// 6.3.4.11 Light xyL Range Get.
@implementation SigLightXyLRangeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLRangeGet object.
 * @param   parameters    the hex data of SigLightXyLRangeGet.
 * @return  return `nil` when initialize SigLightXyLRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.14 Light xyL Range Status, opcode:0x828C


/// Light xyL Range Status is an unacknowledged message used to report
/// the Light xyL x Range (see Section 6.1.5.3) and Light xyL y Range (see
/// Section 6.1.5.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.228),
/// 6.3.4.14 Light xyL Range Status.
@implementation SigLightXyLRangeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeStatus;
    }
    return self;
}

- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode xyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeStatus;
        _statusCode = statusCode;
        _xyLXRangeMin = xyLXRangeMin;
        _xyLXRangeMax = xyLXRangeMax;
        _xyLYRangeMin = xyLYRangeMin;
        _xyLYRangeMax = xyLYRangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLRangeStatus object.
 * @param   parameters    the hex data of SigLightXyLRangeStatus.
 * @return  return `nil` when initialize SigLightXyLRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeStatus;
        if (parameters == nil || (parameters.length != 9)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _statusCode = (SigGenericMessageStatus)tem8;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _xyLXRangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _xyLXRangeMax = tem;
        memcpy(&tem, dataByte+5, 2);
        _xyLYRangeMin = tem;
        memcpy(&tem, dataByte+7, 2);
        _xyLYRangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _statusCode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _xyLXRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLXRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLYRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLYRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.4.8 Light xyL Default Set, opcode:0x828D


/// Light xyL Default Set is an acknowledged message used to set
/// the Light Lightness Default (see Section 6.1.2.4), the Light xyL x
/// Default (see Section 6.1.5.2), and Light xyL y Default (see
/// Section 6.1.5.5) states of an element.
/// @note   The response to the Light xyL Default Set message is
/// a Light xyL Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.226),
/// 6.3.4.8 Light xyL Default Set.
@implementation SigLightXyLDefaultSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultSet;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultSet;
        _lightness = lightness;
        _xyLX = xyLX;
        _xyLY = xyLY;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLDefaultSet object.
 * @param   parameters    the hex data of SigLightXyLDefaultSet.
 * @return  return `nil` when initialize SigLightXyLDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultSet;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _xyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _xyLY = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLDefaultStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.9 Light xyL Default Set Unacknowledged, opcode:0x828E


/// Light xyL Default Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Default (see Section 6.1.2.4), the Light
/// xyL x Default (see Section 6.1.5.2), and Light xyL y Default (see
/// Section 6.1.5.5) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.226),
/// 6.3.4.9 Light xyL Default Set Unacknowledged.
@implementation SigLightXyLDefaultSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultSetUnacknowledged;
        _lightness = lightness;
        _xyLX = xyLX;
        _xyLY = xyLY;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightXyLDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightXyLDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLDefaultSetUnacknowledged;
        if (parameters == nil || (parameters.length != 6)) {
            return nil;
        }else{
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightness = tem16;
            memcpy(&tem16, dataByte+2, 2);
            _xyLX = tem16;
            memcpy(&tem16, dataByte+4, 2);
            _xyLY = tem16;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightness;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLX;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLY;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.4.12 Light xyL Range Set, opcode:0x828F


/// Light xyL Range Set is an acknowledged message used to set
/// the Light xyL x Range (see Section 6.1.5.3) and Light xyL y
/// Range (see Section 6.1.5.6) states of an element.
/// @note   The response to the Light xyL Range Set message is
/// a Light xyL Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.227),
/// 6.3.4.12 Light xyL Range Set.
@implementation SigLightXyLRangeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeSet;
    }
    return self;
}

- (instancetype)initWithXyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeSet;
        _xyLXRangeMin = xyLXRangeMin;
        _xyLXRangeMax = xyLXRangeMax;
        _xyLYRangeMin = xyLYRangeMin;
        _xyLYRangeMax = xyLYRangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLRangeSet object.
 * @param   parameters    the hex data of SigLightXyLRangeSet.
 * @return  return `nil` when initialize SigLightXyLRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeSet;
        if (parameters == nil || (parameters.length != 9)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _xyLXRangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _xyLXRangeMax = tem;
        memcpy(&tem, dataByte+5, 2);
        _xyLYRangeMin = tem;
        memcpy(&tem, dataByte+7, 2);
        _xyLYRangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _xyLXRangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLXRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLYRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLYRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightXyLRangeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.4.13 Light xyL Range Set Unacknowledged, opcode:0x8290


/// Light xyL Range Set Unacknowledged is an unacknowledged message
/// used to set the Light xyL x Range (see Section 6.1.5.3) and Light xyL y
/// Range (see Section 6.1.5.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.227),
/// 6.3.4.13 Light xyL Range Set Unacknowledged.
@implementation SigLightXyLRangeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithXyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeSetUnacknowledged;
        _xyLXRangeMin = xyLXRangeMin;
        _xyLXRangeMax = xyLXRangeMax;
        _xyLYRangeMin = xyLYRangeMin;
        _xyLYRangeMax = xyLYRangeMax;
    }
    return self;
}

/**
 * @brief   Initialize SigLightXyLRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightXyLRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightXyLRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_lightXyLRangeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 9)) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem = 0;
        memcpy(&tem, dataByte+1, 2);
        _xyLXRangeMin = tem;
        memcpy(&tem, dataByte+3, 2);
        _xyLXRangeMax = tem;
        memcpy(&tem, dataByte+5, 2);
        _xyLYRangeMin = tem;
        memcpy(&tem, dataByte+7, 2);
        _xyLYRangeMax = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _xyLXRangeMin;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLXRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLYRangeMin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _xyLYRangeMax;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.5 Light LC messages - 6.3.5.1 Light LC Mode messages


#pragma mark 6.3.5.1.1 Light LC Mode Get, opcode:0x8291


/// Light LC Mode Get is an acknowledged message used to get
/// the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   The response to the Light LC Mode Get message is
/// a Light LC Mode Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.1 Light LC Mode Get.
@implementation SigLightLCModeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCModeGet object.
 * @param   parameters    the hex data of SigLightLCModeGet.
 * @return  return `nil` when initialize SigLightLCModeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCModeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.5.1.2 Light LC Mode Set, opcode:0x8292


/// The Light LC Mode Set is an acknowledged message used to set
/// the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   The response to the Light LC Mode Set message is a
/// Light LC Mode Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.2 Light LC Mode Set.
@implementation SigLightLCModeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeSet;
    }
    return self;
}

- (instancetype)initWithMode:(UInt8)mode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeSet;
        _mode = mode;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCModeSet object.
 * @param   parameters    the hex data of SigLightLCModeSet.
 * @return  return `nil` when initialize SigLightLCModeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeSet;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _mode = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _mode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCModeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.5.1.3 Light LC Mode Set Unacknowledged, opcode:0x8293


/// The Light LC Mode Set Unacknowledged is an unacknowledged message
/// used to set the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.3 Light LC Mode Set Unacknowledged.
@implementation SigLightLCModeSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithMode:(UInt8)mode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeSetUnacknowledged;
        _mode = mode;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCModeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCModeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCModeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeSetUnacknowledged;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _mode = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _mode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.5.1.4 Light LC Mode Status, opcode:0x8294


/// The Light LC Mode Status is an unacknowledged message used to report
/// the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.4 Light LC Mode Status.
@implementation SigLightLCModeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeStatus;
    }
    return self;
}

- (instancetype)initWithMode:(UInt8)mode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeStatus;
        _mode = mode;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCModeStatus object.
 * @param   parameters    the hex data of SigLightLCModeStatus.
 * @return  return `nil` when initialize SigLightLCModeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCModeStatus;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _mode = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _mode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.5 Light LC messages - 6.3.5.2 Light LC Occupancy Mode messages


#pragma mark 6.3.5.2.1 Light LC OM Get, opcode:0x8295


/// Light LC OM Get is an acknowledged message used to get
/// the Light LC Occupancy Mode state of an element (see
/// Section 6.2.3.2).
/// @note   The response to the Light LC OM Get message is
/// a Light LC OM Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.1 Light LC OM Get.
@implementation SigLightLCOMGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCOMGet object.
 * @param   parameters    the hex data of SigLightLCOMGet.
 * @return  return `nil` when initialize SigLightLCOMGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMGet;
        if (parameters == nil || (parameters.length == 0)) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCOMStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.5.2.2 Light LC OM Set, opcode:0x8296


/// The Light LC OM Set is an acknowledged message used to set
/// the Light LC Occupancy Mode state of an element (see
/// Section 6.2.3.2).
/// @note   The response to the Light LC OM Set message is a
/// Light LC OM Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.2 Light LC OM Set.
@implementation SigLightLCOMSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMSet;
    }
    return self;
}

- (instancetype)initWithMode:(UInt8)mode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMSet;
        _mode = mode;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCOMSet object.
 * @param   parameters    the hex data of SigLightLCOMSet.
 * @return  return `nil` when initialize SigLightLCOMSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMSet;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _mode = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _mode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCOMStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 6.3.5.2.3 Light LC OM Set Unacknowledged, opcode:0x8297


/// The Light LC OM Set Unacknowledged is an unacknowledged
/// message used to set the Light LC Occupancy Mode state of an
/// element (see Section 6.2.3.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.3 Light LC OM Set Unacknowledged.
@implementation SigLightLCOMSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMSetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithMode:(UInt8)mode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMSetUnacknowledged;
        _mode = mode;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCOMSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCOMSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCOMSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMSetUnacknowledged;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _mode = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _mode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 6.3.5.2.4 Light LC OM Status, opcode:0x8298


/// The Light LC OM Status is an unacknowledged message used to report
/// the Light LC Occupancy Mode state of an element (see Section 6.2.3.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.4 Light LC OM Status.
@implementation SigLightLCOMStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMStatus;
    }
    return self;
}

- (instancetype)initWithMode:(UInt8)mode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMStatus;
        _mode = mode;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCOMStatus object.
 * @param   parameters    the hex data of SigLightLCOMStatus.
 * @return  return `nil` when initialize SigLightLCOMStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCOMStatus;
        if (parameters == nil || (parameters.length != 1)) {
            return nil;
        }else{
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _mode = tem8;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _mode;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 6.3.5 Light LC messages - 6.3.5.3 Light LC Light OnOff messages


#pragma mark 6.3.5.3.1 Light LC Light OnOff Get, opcode:0x8299


/// Light LC Light OnOff Get is an acknowledged message used to get
/// the Light LC Light OnOff state of an element (see Section 6.2.3.3).
/// @note   The response to the Light LC Light OnOff Get message is
/// a Light LC Light OnOff Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.231),
/// 6.3.5.3.1 Light LC Light OnOff Get.
@implementation SigLightLCLightOnOffGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCLightOnOffGet object.
 * @param   parameters    the hex data of SigLightLCLightOnOffGet.
 * @return  return `nil` when initialize SigLightLCLightOnOffGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCLightOnOffStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 6.3.5.3.2 Light LC Light OnOff Set, opcode:0x829A


/// The Light LC Light OnOff Set is an acknowledged message used to set
/// the Light LC Light OnOff state of an element (see Section 6.2.3.3).
/// @note   The response to the Light LC Light OnOff Set message is a
/// Light LC Light OnOff Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.231),
/// 6.3.5.3.2 Light LC Light OnOff Set.
@implementation SigLightLCLightOnOffSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffSet;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

- (instancetype)initWithLightOnOff:(BOOL)lightOnOff transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffSet;
        _lightOnOff = lightOnOff;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCLightOnOffSet object.
 * @param   parameters    the hex data of SigLightLCLightOnOffSet.
 * @return  return `nil` when initialize SigLightLCLightOnOffSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffSet;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 4)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _lightOnOff = tem == 0x01;
        memcpy(&tem, dataByte+1, 1);
        self.tid = tem;
        if (parameters.length == 4) {
            memcpy(&tem, dataByte+2, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+3, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _lightOnOff ? 0x01 : 0x00;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCLightOnOffStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 6.3.5.3.3 Light LC Light OnOff Set Unacknowledged, opcode:0x829B


/// The Light LC Light OnOff Set Unacknowledged is an unacknowledged
/// message used to set the Light LC Light OnOff state of an element (see
/// Section 6.2.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.231),
/// 6.3.5.3.3 Light LC Light OnOff Set Unacknowledged.
@implementation SigLightLCLightOnOffSetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffSet;
        //v3.2.3后，_transitionTime为nil，默认不带渐变参数。
//        _transitionTime = [[SigTransitionTime alloc] initWithSetps:0 stepResolution:0];
        _delay = 0;
    }
    return self;
}

- (instancetype)initWithLightOnOff:(BOOL)lightOnOff transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffSet;
        _lightOnOff = lightOnOff;
        _transitionTime = transitionTime;
        _delay = delay;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCLightOnOffSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCLightOnOffSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCLightOnOffSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffSet;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 4)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _lightOnOff = tem == 0x01;
        memcpy(&tem, dataByte+1, 1);
        self.tid = tem;
        if (parameters.length == 4) {
            memcpy(&tem, dataByte+2, 1);
            _transitionTime = [[SigTransitionTime alloc] initWithRawValue:tem];
            memcpy(&tem, dataByte+3, 1);
            _delay = tem;
        } else {
            _transitionTime = nil;
            _delay = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _lightOnOff ? 0x01 : 0x00;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.tid;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_transitionTime != nil) {
        tem8 = _transitionTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _delay;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    //v3.2.3后，_transitionTime为nil，则不补0000了。
//    else{
//        tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:1];
//        [mData appendData:data];
//        [mData appendData:data];
//    }
    return mData;
}

@end


#pragma mark 6.3.5.3.4 Light LC Light OnOff Status, opcode:0x829C


/// The Light LC Light OnOff Status is an unacknowledged message used to report
/// the Light LC Light OnOff state of an element (see Section 6.2.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.232),
/// 6.3.5.3.4 Light LC Light OnOff Status.
@implementation SigLightLCLightOnOffStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffStatus;
    }
    return self;
}

- (instancetype)initWithPresentLightOnOff:(BOOL)presentLightOnOff targetLightOnOff:(BOOL)targetLightOnOff remainingTime:(nullable SigTransitionTime *)remainingTime {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCLightOnOffStatus;
        _presentLightOnOff = presentLightOnOff;
        _targetLightOnOff = targetLightOnOff;
        _remainingTime = remainingTime;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCLightOnOffStatus object.
 * @param   parameters    the hex data of SigLightLCLightOnOffStatus.
 * @return  return `nil` when initialize SigLightLCLightOnOffStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_genericOnOffStatus;
        if (parameters == nil || (parameters.length != 1 && parameters.length != 3)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _presentLightOnOff = tem == 0x01;
        if (parameters.length == 3) {
            memcpy(&tem, dataByte+1, 1);
            _targetLightOnOff = tem == 0x01;
            memcpy(&tem, dataByte+2, 1);
            _remainingTime = [[SigTransitionTime alloc] initWithRawValue:tem];
        } else {
            _targetLightOnOff = NO;
            _remainingTime = nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _presentLightOnOff ? 0x01 : 0x00;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_remainingTime != nil) {
        tem8 = _targetLightOnOff ? 0x01 : 0x00;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _remainingTime.rawValue;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark - 6.3.6 Light LC Property Messages
/*
 Light LC Property messages operate on Light LC states defined
 in Section 6.2.3 by referring to device properties defined by
 these states and providing Raw values for the device properties.
 */

#pragma mark 6.3.6.1 Light LC Property Get, opcode:0x829D


/// Light LC Property Get is an acknowledged message used to get
/// the Light LC Property state of an element (see Section 6.2).
/// @note   The response to the Light LC Property Get message
/// is a Light LC Property Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.232),
/// 6.3.6.1 Light LC Property Get.
@implementation SigLightLCPropertyGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertyGet;
    }
    return self;
}

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertyGet;
        _lightLCPropertyID = lightLCPropertyID;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCPropertyGet object.
 * @param   parameters    the hex data of SigLightLCPropertyGet.
 * @return  return `nil` when initialize SigLightLCPropertyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertyGet;
        if (parameters != nil && parameters.length == 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightLCPropertyID = tem16;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightLCPropertyID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCPropertyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 6.3.6.2 Light LC Property Set, opcode:0x62


/// The Light LC Property Set is an acknowledged message used to set
/// the Light LC Property state of an element (see Section 6.2).
/// @note   The response to the Light LC Property Set message is a
/// Light LC Property Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.233),
/// 6.3.6.2 Light LC Property Set.
@implementation SigLightLCPropertySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertySet;
    }
    return self;
}

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertySet;
        _lightLCPropertyID = lightLCPropertyID;
        _lightLCPropertyValue = lightLCPropertyValue;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCPropertySet object.
 * @param   parameters    the hex data of SigLightLCPropertySet.
 * @return  return `nil` when initialize SigLightLCPropertySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertySet;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightLCPropertyID = tem16;
            _lightLCPropertyValue = [parameters subdataWithRange:NSMakeRange(2, parameters.length - 2)];
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightLCPropertyID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_lightLCPropertyValue != nil) {
        [mData appendData:_lightLCPropertyValue];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLightLCPropertyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 6.3.6.3 Light LC Property Set Unacknowledged, opcode:0x63


/// The Light LC Property Set Unacknowledged is an unacknowledged
/// message used to set the Light LC Property state of an element (see
/// Section 6.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.233),
/// 6.3.6.3 Light LC Property Set Unacknowledged.
@implementation SigLightLCPropertySetUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertySetUnacknowledged;
    }
    return self;
}

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertySetUnacknowledged;
        _lightLCPropertyID = lightLCPropertyID;
        _lightLCPropertyValue = lightLCPropertyValue;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCPropertySetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCPropertySetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCPropertySetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertySetUnacknowledged;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightLCPropertyID = tem16;
            _lightLCPropertyValue = [parameters subdataWithRange:NSMakeRange(2, parameters.length - 2)];
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightLCPropertyID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_lightLCPropertyValue != nil) {
        [mData appendData:_lightLCPropertyValue];
    }
    return mData;
}

@end


#pragma mark 6.3.6.4 Light LC Property Status, opcode:0x64


/// The Light LC Property Status is an unacknowledged message used to
/// report the Light LC Property state of an element (see Section 6.2).
/// @note   The message is sent as a response to the Light LC Property
/// Get and Light LC Property Set messages or may be sent as an
/// unsolicited message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.233),
/// 6.3.6.4 Light LC Property Status.
@implementation SigLightLCPropertyStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertyStatus;
    }
    return self;
}

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertyStatus;
        _lightLCPropertyID = lightLCPropertyID;
        _lightLCPropertyValue = lightLCPropertyValue;
    }
    return self;
}

/**
 * @brief   Initialize SigLightLCPropertyStatus object.
 * @param   parameters    the hex data of SigLightLCPropertyStatus.
 * @return  return `nil` when initialize SigLightLCPropertyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LightLCPropertyStatus;
        if (parameters != nil && parameters.length >= 2) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _lightLCPropertyID = tem16;
            _lightLCPropertyValue = [parameters subdataWithRange:NSMakeRange(2, parameters.length - 2)];
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _lightLCPropertyID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_lightLCPropertyValue != nil) {
        [mData appendData:_lightLCPropertyValue];
    }
    return mData;
}

@end


#pragma mark - 8.4.1 Firmware Update model messages
/*
 This section defines the format of messages exchanged
 between the Firmware Update Client and Firmware Update
 Server models.
 */


#pragma mark 8.4.1.1 Firmware Update Information Get, opcode:0xB601


/// The Firmware Update Information Get message is an acknowledged
/// message used to get information about the firmware images installed
/// on a node.
/// @note   The response to the Firmware Update Information Get
/// message is a Firmware Update Information Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.91),
/// 8.4.1.1 Firmware Update Information Get.
@implementation SigFirmwareUpdateInformationGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateInformationGet;
        _firstIndex = 0;
        _entriesLimit = 0;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateInformationGet object.
 * @param   parameters    the hex data of SigFirmwareUpdateInformationGet.
 * @return  return `nil` when initialize SigFirmwareUpdateInformationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateInformationGet;
        if (parameters != nil && parameters.length > 2) {
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            _firstIndex = tem8;
            memcpy(&tem8, dataByte+1, 1);
            _entriesLimit = tem8;
        }else{
            return nil;
        }
    }
    return self;
}

- (instancetype)initWithFirstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateInformationGet;
        _firstIndex = firstIndex;
        _entriesLimit = entriesLimit;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _firstIndex;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _entriesLimit;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareUpdateInformationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.1.2 Firmware Update Information Status, opcode:0xB602


/// The Firmware Update Information Status message is an unacknowledged
/// message used to report information about firmware images installed on a node.
/// @note   The Firmware Update Information Status message is sent in
/// response to a Firmware Update Information Get message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.92),
/// 8.4.1.2 Firmware Update Information Status.
@implementation SigFirmwareUpdateInformationStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateInformationStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateInformationStatus object.
 * @param   parameters    the hex data of SigFirmwareUpdateInformationStatus.
 * @return  return `nil` when initialize SigFirmwareUpdateInformationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateInformationStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters.length < 2 + 3) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _firmwareInformationListCount = tem8;
        memcpy(&tem8, dataByte+1, 1);
        _firstIndex = tem8;
        NSInteger index = 2;
        _firmwareInformationList = [NSMutableArray array];
        while (parameters.length > index) {
            SigFirmwareInformationEntryModel *model = [[SigFirmwareInformationEntryModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(index, parameters.length - index)]];
            if (model) {
                [_firmwareInformationList addObject:model];
                index += model.parameters.length;
            } else {
                break;
            }
            if (_firmwareInformationList.count >= _firmwareInformationListCount) {
                break;
            }
        }
    }
    return self;
}

@end


#pragma mark 8.4.1.3 Firmware Update Firmware Metadata Check, opcode:0xB603


/// The Firmware Update Firmware Metadata Check message is an acknowledged
/// message, sent to a Firmware Update Server, to check whether the node can
/// accept a firmware update.
/// @note   The response to the Firmware Update Firmware Metadata Check
/// message is a Firmware Update Firmware Metadata Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.93),
/// 8.4.1.3 Firmware Update Firmware Metadata Check.
@implementation SigFirmwareUpdateFirmwareMetadataCheck

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateFirmwareMetadataCheck;
    }
    return self;
}

- (instancetype)initWithUpdateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateFirmwareMetadataCheck;
        if (incomingFirmwareMetadata && incomingFirmwareMetadata.length) {
            _incomingFirmwareMetadata = [NSData dataWithData:incomingFirmwareMetadata];
        }
        _updateFirmwareImageIndex = updateFirmwareImageIndex;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = updateFirmwareImageIndex;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        if (incomingFirmwareMetadata && incomingFirmwareMetadata.length) {
            [mData appendData:incomingFirmwareMetadata];
        }
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateFirmwareMetadataCheck object.
 * @param   parameters    the hex data of SigFirmwareUpdateFirmwareMetadataCheck.
 * @return  return `nil` when initialize SigFirmwareUpdateFirmwareMetadataCheck object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateFirmwareMetadataCheck;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length == 0) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _updateFirmwareImageIndex = tem8;
        if (parameters.length > 1) {
            _incomingFirmwareMetadata = [parameters subdataWithRange:NSMakeRange(1, parameters.length - 1)];
        } else {
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareUpdateFirmwareMetadataStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.1.4 Firmware Update Firmware Metadata Status, opcode:0xB604


/// The Firmware Update Firmware Metadata Status message is an unacknowledged
/// message sent to a Firmware Update Client that is used to report whether a
/// Firmware Update Server can accept a firmware update.
/// @note   The Firmware Update Firmware Metadata Status message is sent in
/// response to a Firmware Update Firmware Metadata Check message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.93),
/// 8.4.1.4 Firmware Update Firmware Metadata Status.
@implementation SigFirmwareUpdateFirmwareMetadataStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateFirmwareMetadataStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateFirmwareMetadataStatus object.
 * @param   parameters    the hex data of SigFirmwareUpdateFirmwareMetadataStatus.
 * @return  return `nil` when initialize SigFirmwareUpdateFirmwareMetadataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateFirmwareMetadataStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8 & 0b111;
        _additionalInformation = (tem8 >> 3) & 0b11111;
        memcpy(&tem8, dataByte+1, 1);
        _updateFirmwareImageIndex = tem8;
    }
    return self;
}

@end


#pragma mark 8.4.1.5 Firmware Update Get, opcode:0xB605


/// The Firmware Update Get message is an acknowledged message
/// used to get the current status of the Firmware Update Server.
/// @note   The response to the Firmware Update Get message is a
/// Firmware Update Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.94),
/// 8.4.1.5 Firmware Update Get.
@implementation SigFirmwareUpdateGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateGet;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateGet object.
 * @param   parameters    the hex data of SigFirmwareUpdateGet.
 * @return  return `nil` when initialize SigFirmwareUpdateGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareUpdateStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.1.6 Firmware Update Start, opcode:0xB606


/// The Firmware Update Start message is an acknowledged message used to
/// start a firmware update on a Firmware Update Server.
/// @note   The response to the Firmware Update Start message is a Firmware
/// Update Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.94),
/// 8.4.1.6 Firmware Update Start.
@implementation SigFirmwareUpdateStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateStart;
    }
    return self;
}

- (instancetype)initWithUpdateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateStart;
        _updateTTL = updateTTL;
        _updateTimeoutBase = updateTimeoutBase;
        _updateBLOBID = updateBLOBID;
        _updateFirmwareImageIndex = updateFirmwareImageIndex;
        _incomingFirmwareMetadata = [NSData dataWithData:incomingFirmwareMetadata];
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = updateTTL;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        UInt16 tem16 = updateTimeoutBase;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt64 tem64 = updateBLOBID;
        data = [NSData dataWithBytes:&tem64 length:8];
        [mData appendData:data];
        tem8 = updateFirmwareImageIndex;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        if (incomingFirmwareMetadata && incomingFirmwareMetadata.length) {
            [mData appendData:incomingFirmwareMetadata];
        }
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateStart object.
 * @param   parameters    the hex data of SigFirmwareUpdateStart.
 * @return  return `nil` when initialize SigFirmwareUpdateStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateStart;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 1 + 2 + 8 + 1 + 2) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt64 tem64 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        memcpy(&tem16, dataByte + 1, 2);
        memcpy(&tem64, dataByte + 3, 8);
        _updateTTL = tem8;
        _updateTimeoutBase = tem16;
        _updateBLOBID = tem64;
        memcpy(&tem8, dataByte + 11, 1);
        _updateFirmwareImageIndex = tem8;
        if (_incomingFirmwareMetadata && _incomingFirmwareMetadata.length > 0) {
            _incomingFirmwareMetadata = [parameters subdataWithRange:NSMakeRange(12, parameters.length - 12)];
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareUpdateStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.1.7 Firmware Update Cancel, opcode:0xB607


/// The Firmware Update Cancel message is an acknowledged message
/// used to cancel a firmware update and delete any stored information
/// about the update on a Firmware Update Server.
/// @note   The response to a Firmware Update Cancel message is a
/// Firmware Update Status message.
/// @note   The Firmware Update Cancel message has no parameters.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.7 Firmware Update Cancel.
@implementation SigFirmwareUpdateCancel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateCancel;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateCancel object.
 * @param   parameters    the hex data of SigFirmwareUpdateCancel.
 * @return  return `nil` when initialize SigFirmwareUpdateCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateCancel;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareUpdateStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.1.8 Firmware Update Apply, opcode:0xB608


/// The Firmware Update Apply message is an acknowledged message
/// used to apply a firmware image that has been transferred to a
/// Firmware Update Server.
/// @note   The response to a Firmware Update Apply message is a
/// Firmware Update Status message.
/// @note   The Firmware Update Apply message has no parameters.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.8 Firmware Update Apply.
@implementation SigFirmwareUpdateApply

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateApply;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateApply object.
 * @param   parameters    the hex data of SigFirmwareUpdateApply.
 * @return  return `nil` when initialize SigFirmwareUpdateApply object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateApply;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareUpdateStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.1.9 Firmware Update Status, opcode:0xB609


/// The Firmware Update Status message is an unacknowledged message
/// sent by a Firmware Update Server to report the status of a firmware update.
/// @note   A Firmware Updates Status message is sent in response to
/// a Firmware Update Get message, a Firmware Update Start message,
/// a Firmware Update Cancel message, or a Firmware Update Apply message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.9 Firmware Update Status.
@implementation SigFirmwareUpdateStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareUpdateStatus object.
 * @param   parameters    the hex data of SigFirmwareUpdateStatus.
 * @return  return `nil` when initialize SigFirmwareUpdateStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareUpdateStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || (parameters.length != 1 && parameters.length <= 13)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8 & 0b111;
        _RFU1 = (tem8 >> 3) & 0b11;
        _updatePhase = (tem8 >> 5) & 0b111;
        if (parameters.length > 1) {
            memcpy(&tem8, dataByte+1, 1);
            _updateTTL = tem8;
            memcpy(&tem8, dataByte+2, 1);
            _additionalInformation = tem8 & 0b11111;
            _RFU2 = (tem8 >> 5) & 0b111;
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte + 3, 2);
            _updateTimeoutBase = tem16;
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte + 5, 8);
            _updateBLOBID = tem64;
            memcpy(&tem8, dataByte+13, 1);
            _updateFirmwareImageIndex = tem8;
        }
    }
    return self;
}

@end


#pragma mark - 8.4.2 Firmware Distribution model messages


#pragma mark 8.4.2.8 Firmware Distribution Get, opcode:0xB60A


/// The Firmware Distribution Get message is an acknowledged message
/// sent by a Firmware Distribution Client to get the state of the current
/// firmware image distribution on a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Get message is a
/// Firmware Distribution Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.100),
/// 8.4.2.8 Firmware Distribution Get.
@implementation SigFirmwareDistributionGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionGet;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionGet.
 * @return  return `nil` when initialize SigFirmwareDistributionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.9 Firmware Distribution Start, opcode:0xB60B


/// The Firmware Distribution Start message is an acknowledged message
/// sent by a Firmware Distribution Client to start the firmware image
/// distribution to the Updating nodes in the Distribution Receivers List.
/// @note   The response to a Firmware Distribution Start message is a
/// Firmware Distribution Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.100),
/// 8.4.2.9 Firmware Distribution Start.
@implementation SigFirmwareDistributionStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionStart;
    }
    return self;
}

- (instancetype)initWithDistributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(SigUpdatePolicyType)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionStart;
        _distributionAppKeyIndex = distributionAppKeyIndex;
        _distributionTTL = distributionTTL;
        _distributionTimeoutBase = distributionTimeoutBase;
        _distributionTransferMode = distributionTransferMode;
        _updatePolicy = updatePolicy;
        _RFU = RFU;
        _distributionFirmwareImageIndex = distributionFirmwareImageIndex;
        _distributionMulticastAddress = [NSData dataWithData:distributionMulticastAddress];
        
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = distributionAppKeyIndex;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = distributionTTL;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = distributionTimeoutBase;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem8 = distributionTransferMode | ((updatePolicy == SigUpdatePolicyType_verifyAndApply ? 1 : 0) << 2) | ((RFU&0b11111) << 3);
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = distributionFirmwareImageIndex;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        [mData appendData:distributionMulticastAddress];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionStart object.
 * @param   parameters    the hex data of SigFirmwareDistributionStart.
 * @return  return `nil` when initialize SigFirmwareDistributionStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionStart;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || (parameters.length != 10 && parameters.length != 24)) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _distributionAppKeyIndex = tem16;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2, 1);
        _distributionTTL = tem8;
        memcpy(&tem16, dataByte+3, 2);
        _distributionTimeoutBase = tem16;
        memcpy(&tem8, dataByte+5, 1);
        _distributionTransferMode = tem8 & 0b11;
        _updatePolicy = (tem8 >> 2) & 0b1;
        _RFU = tem8 >> 3;
        memcpy(&tem16, dataByte+6, 2);
        _distributionFirmwareImageIndex = tem16;
        if (parameters.length == 10) {
            _distributionMulticastAddress = [parameters subdataWithRange:NSMakeRange(8, 2)];
        } else if (parameters.length >= 24){
            _distributionMulticastAddress = [parameters subdataWithRange:NSMakeRange(8, 16)];
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.10 Firmware Distribution Cancel, opcode:0xB60C


/// The Firmware Distribution Cancel message is an acknowledged message
/// sent by a Firmware Distribution Client to stop the firmware image distribution
/// from a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Cancel message is a
/// Firmware Distribution Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.101),
/// 8.4.2.10 Firmware Distribution Cancel.
@implementation SigFirmwareDistributionCancel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCancel;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionCancel object.
 * @param   parameters    the hex data of SigFirmwareDistributionCancel.
 * @return  return `nil` when initialize SigFirmwareDistributionCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCancel;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.11 Firmware Distribution Apply, opcode:0xB60D


/// The Firmware Distribution Apply message is an acknowledged message
/// sent from a Firmware Distribution Client to a Firmware Distribution Server
/// to apply the firmware image on the Updating nodes.
/// @note   The response to a Firmware Distribution Apply message is a
/// Firmware Distribution Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.101),
/// 8.4.2.11 Firmware Distribution Apply.
@implementation SigFirmwareDistributionApply

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionApply;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionApply object.
 * @param   parameters    the hex data of SigFirmwareDistributionApply.
 * @return  return `nil` when initialize SigFirmwareDistributionApply object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionApply;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.12 Firmware Distribution Status, opcode:0xB60E


/// The Firmware Distribution Status message is an unacknowledged message
/// sent by a Firmware Distribution Server to report the status of a firmware
/// image distribution.
/// @note   A Firmware Distribution Status message is sent as a response to
/// a Firmware Distribution Get message, Firmware Distribution Start message,
/// Firmware Distribution Cancel message, or Firmware Distribution Apply message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.102),
/// 8.4.2.12 Firmware Distribution Status.
@implementation SigFirmwareDistributionStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || (parameters.length != 2 && parameters.length != 12)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        memcpy(&tem8, dataByte+1, 1);
        _distributionPhase = tem8;
        if (parameters.length == 12) {
            UInt16 tem16 = 0;
            memcpy(&tem16, dataByte + 2, 2);
            _distributionMulticastAddress = tem16;
            memcpy(&tem16, dataByte + 4, 2);
            _distributionAppKeyIndex = tem16;
            memcpy(&tem8, dataByte+6, 1);
            _distributionTTL = tem8;
            memcpy(&tem16, dataByte + 7, 2);
            _distributionTimeoutBase = tem16;
            memcpy(&tem8, dataByte+9, 1);
            _distributionTransferMode = tem8 & 0b11;
            _updatePolicy = (tem8 >> 2) & 0b1;
            _RFU = (tem8 >> 3) & 0b11111;
            memcpy(&tem16, dataByte + 10, 2);
            _distributionFirmwareImageIndex = tem16;
        }
    }
    return self;
}

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status distributionPhase:(SigDistributionPhaseState)distributionPhase distributionMulticastAddress:(UInt16)distributionMulticastAddress distributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(SigUpdatePolicyType)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionStatus;
        _status = status;
        _distributionPhase = distributionPhase;
        _distributionMulticastAddress = distributionMulticastAddress;
        _distributionAppKeyIndex = distributionAppKeyIndex;
        _distributionTTL = distributionTTL;
        _distributionTimeoutBase = distributionTimeoutBase;
        _distributionTransferMode = distributionTransferMode;
        _updatePolicy = updatePolicy;
        _RFU = RFU;
        _distributionFirmwareImageIndex = distributionFirmwareImageIndex;

        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        tem8 = status;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = distributionPhase;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = distributionMulticastAddress;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = distributionAppKeyIndex;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem8 = distributionTTL;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = distributionTimeoutBase;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem8 = (distributionTransferMode & 0b11) | ((updatePolicy & 0b1) << 2) | ((RFU & 0b11111) << 3);
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = distributionFirmwareImageIndex;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 8.4.2.13 Firmware Distribution Upload Get, opcode:0xB616


/// The Firmware Distribution Upload Get message is an acknowledged
/// message sent by a Firmware Distribution Client to check the status of
/// a firmware image upload to a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Upload Get message
/// is a Firmware Distribution Upload Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.103),
/// 8.4.2.13 Firmware Distribution Upload Get.
@implementation SigFirmwareDistributionUploadGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadGet;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionUploadGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadGet.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionUploadStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.14 Firmware Distribution Upload Start, opcode:0xB617


/// The Firmware Distribution Upload Start message is an acknowledged message sent
/// by a Firmware Distribution Client to start a firmware image upload to a Firmware
/// Distribution Server.
/// @note   The response to a Firmware Distribution Upload Start message is a
/// Firmware Distribution Upload Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.103),
/// 8.4.2.14 Firmware Distribution Upload Start.
@implementation SigFirmwareDistributionUploadStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadStart;
    }
    return self;
}

- (instancetype)initWithUploadTTL:(UInt8)uploadTTL uploadTimeoutBase:(UInt16)uploadTimeoutBase uploadBLOBID:(UInt64)uploadBLOBID uploadFirmwareSize:(UInt32)uploadFirmwareSize uploadFirmwareMetadataLength:(UInt8)uploadFirmwareMetadataLength uploadFirmwareMetadata:(NSData *)uploadFirmwareMetadata uploadFirmwareID:(NSData *)uploadFirmwareID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadStart;
        _uploadTTL = uploadTTL;
        _uploadTimeoutBase = uploadTimeoutBase;
        _uploadBLOBID = uploadBLOBID;
        _uploadFirmwareSize = uploadFirmwareSize;
        _uploadFirmwareMetadataLength = uploadFirmwareMetadataLength;
        if (uploadFirmwareMetadata && uploadFirmwareMetadata.length > 0) {
            _uploadFirmwareMetadata = [NSData dataWithData:uploadFirmwareMetadata];
        }
        if (uploadFirmwareID && uploadFirmwareID.length > 0) {
            _uploadFirmwareID = [NSData dataWithData:uploadFirmwareID];
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt32 tem32 = 0;
        UInt64 tem64 = 0;
        NSMutableData *mData = [NSMutableData data];
        tem8 = uploadTTL;
        [mData appendData:[NSData dataWithBytes:&tem8 length:1]];
        tem16 = uploadTimeoutBase;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
        tem64 = uploadBLOBID;
        [mData appendData:[NSData dataWithBytes:&tem64 length:8]];
        tem32 = uploadFirmwareSize;
        [mData appendData:[NSData dataWithBytes:&tem32 length:4]];
        tem8 = uploadFirmwareMetadataLength;
        [mData appendData:[NSData dataWithBytes:&tem8 length:1]];
        if (uploadFirmwareMetadata && uploadFirmwareMetadata.length > 0) {
            [mData appendData:uploadFirmwareMetadata];
        }
        if (uploadFirmwareID && uploadFirmwareID.length > 0) {
            [mData appendData:uploadFirmwareID];
        }
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionUploadStart object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadStart.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadStart;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 17) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt32 tem32 = 0;
        UInt64 tem64 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _uploadTTL = tem8;
        memcpy(&tem16, dataByte + 1, 2);
        _uploadTimeoutBase = tem16;
        memcpy(&tem64, dataByte + 3, 8);
        _uploadBLOBID = tem64;
        memcpy(&tem32, dataByte + 11, 4);
        _uploadFirmwareSize = tem32;
        memcpy(&tem8, dataByte + 15, 1);
        _uploadFirmwareMetadataLength = tem8;
        if (_uploadFirmwareMetadataLength == 0) {
            _uploadFirmwareID = [parameters subdataWithRange:NSMakeRange(16, parameters.length - 16)];
        } else {
            if (parameters.length >= 16 + _uploadFirmwareMetadataLength) {
                _uploadFirmwareMetadata = [parameters subdataWithRange:NSMakeRange(16, _uploadFirmwareMetadataLength)];
                _uploadFirmwareID = [parameters subdataWithRange:NSMakeRange(16 + _uploadFirmwareMetadataLength, parameters.length - 16 - _uploadFirmwareMetadataLength)];
            }
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionUploadStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.15 Firmware Distribution Upload OOB Start, opcode:0xB618


/// The Firmware Distribution Upload OOB Start message is an acknowledged message
/// sent by a Firmware Distribution Client to start a firmware image upload to a Firmware
/// Distribution Server using an out-of-band mechanism.
/// @note   The response to a Firmware Distribution Upload OOB Start message is a
/// Firmware Distribution Upload Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.15 Firmware Distribution Upload OOB Start.
@implementation SigFirmwareDistributionUploadOOBStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadOOBStart;
    }
    return self;
}

- (instancetype)initWithUploadURILength:(UInt8)uploadURILength uploadURI:(NSData *)uploadURI uploadFirmwareID:(NSData *)uploadFirmwareID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadOOBStart;
        _uploadURILength = uploadURILength;
        _uploadURI = uploadURI;
        if (uploadURI && uploadURI.length > 0) {
            _uploadURI = [NSData dataWithData:uploadURI];
        }
        if (uploadFirmwareID && uploadFirmwareID.length > 0) {
            _uploadFirmwareID = [NSData dataWithData:uploadFirmwareID];
        }
        UInt8 tem8 = 0;
        NSMutableData *mData = [NSMutableData data];
        tem8 = uploadURILength;
        [mData appendData:[NSData dataWithBytes:&tem8 length:1]];
        if (uploadURI && uploadURI.length > 0) {
            [mData appendData:uploadURI];
        }
        if (uploadFirmwareID && uploadFirmwareID.length > 0) {
            [mData appendData:uploadFirmwareID];
        }
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionUploadOOBStart object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadOOBStart.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadOOBStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadOOBStart;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _uploadURILength = tem8;
        if (_uploadURILength > 0) {
            if (parameters.length >= 1 + _uploadURILength) {
                _uploadURI = [parameters subdataWithRange:NSMakeRange(1, _uploadURILength)];
                _uploadFirmwareID = [parameters subdataWithRange:NSMakeRange(1 + _uploadURILength, parameters.length - 1 - _uploadURILength)];
            }
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionUploadStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.16 Firmware Distribution Upload Cancel, opcode:0xB619


/// The Firmware Distribution Upload Cancel message is an acknowledged
/// message sent by a Firmware Distribution Client to stop a firmware image
/// upload to a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Upload Cancel
/// message is a Firmware Distribution Upload Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.16 Firmware Distribution Upload Cancel.
@implementation SigFirmwareDistributionUploadCancel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadCancel;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionUploadCancel object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadCancel.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadCancel;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionUploadStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.17 Firmware Distribution Upload Status, opcode:0xB61A


/// The Firmware Distribution Upload Status message is an unacknowledged message
/// sent by a Firmware Distribution Server to report the status of a firmware image upload.
/// @note   A Firmware Distribution Upload Status message is sent as a response to a
/// Firmware Distribution Upload Get message, Firmware Distribution Upload Start message,
/// Firmware Distribution Upload OOB Start message, or Firmware Distribution
/// Upload Cancel message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.17 Firmware Distribution Upload Status.
@implementation SigFirmwareDistributionUploadStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionUploadStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || (parameters.length != 2 && parameters.length < 4)) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        memcpy(&tem8, dataByte+1, 1);
        _uploadPhase = tem8;
        if (parameters.length >= 4) {
            memcpy(&tem8, dataByte + 2, 1);
            _uploadProgress = tem8 & 0x7F;
            _uploadType = (tem8 >> 7) & 0b1;
            _uploadFirmwareID = [parameters subdataWithRange:NSMakeRange(3, parameters.length - 3)];
        }
    }
    return self;
}

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status uploadPhase:(SigFirmwareUploadPhaseStateType)uploadPhase uploadProgress:(UInt8)uploadProgress uploadType:(BOOL)uploadType uploadFirmwareID:(NSData *)uploadFirmwareID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionUploadStatus;
        _status = status;
        _uploadPhase = uploadPhase;
        _uploadProgress = uploadProgress;
        if (uploadFirmwareID && uploadFirmwareID.length > 0) {
            _uploadFirmwareID = [NSData dataWithData:uploadFirmwareID];
        }

        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        tem8 = status;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = uploadPhase;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = uploadProgress | ((uploadType==YES?1:0) << 7);
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        if (uploadFirmwareID && uploadFirmwareID.length > 0) {
            [mData appendData:uploadFirmwareID];
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 8.4.2.18 Firmware Distribution Firmware Get, opcode:0xB61B


/// The Firmware Distribution Firmware Get message is an acknowledged message
/// sent by a Firmware Distribution Client to check whether a specific firmware
/// image is stored on a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Get message is a
/// Firmware Distribution Firmware Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.105),
/// 8.4.2.18 Firmware Distribution Firmware Get.
@implementation SigFirmwareDistributionFirmwareGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareGet;
    }
    return self;
}

- (instancetype)initWithFirmwareID:(NSData *)firmwareID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareGet;
        if (firmwareID && firmwareID.length > 0) {
            _firmwareID = [NSData dataWithData:firmwareID];
        }
        NSMutableData *mData = [NSMutableData data];
        if (firmwareID && firmwareID.length > 0) {
            [mData appendData:firmwareID];
        }
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareGet.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareGet;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length == 0) {
            return nil;
        }
        _firmwareID = [NSData dataWithData:parameters];
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionFirmwareStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 8.4.2.22 Firmware Distribution Firmware Status, opcode:0xB61C


/// The Firmware Distribution Firmware Status message is an unacknowledged message
/// sent by a Firmware Distribution Server to report the status of an operation on a stored
/// firmware image.
/// @note   A Firmware Distribution Firmware Status message is sent in response to a
/// Firmware Distribution Firmware Get message, a Firmware Distribution Firmware Get
/// By Index message, a Firmware Distribution Firmware Delete message, or a Firmware
/// Distribution Firmware Delete All message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.22 Firmware Distribution Firmware Status.
@implementation SigFirmwareDistributionFirmwareStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        memcpy(&tem16, dataByte+1, 2);
        _entryCount = tem16;
        memcpy(&tem16, dataByte+3, 2);
        _distributionFirmwareImageIndex = tem16;
        if (parameters.length > 5) {
            _firmwareID = [NSData dataWithData:[parameters subdataWithRange:NSMakeRange(5, parameters.length - 5)]];
        }
    }
    return self;
}

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status entryCount:(UInt16)entryCount distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex firmwareID:(NSData *)firmwareID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareStatus;
        _status = status;
        _entryCount = entryCount;
        _distributionFirmwareImageIndex = distributionFirmwareImageIndex;
        if (firmwareID && firmwareID.length > 0) {
            _firmwareID = [NSData dataWithData:firmwareID];
        }

        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        tem8 = status;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = entryCount;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = distributionFirmwareImageIndex;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        if (firmwareID && firmwareID.length > 0) {
            [mData appendData:firmwareID];
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 8.4.2.19 Firmware Distribution Firmware Get By Index, opcode:0xB61D


/// The Firmware Distribution Firmware Get By Index message is an acknowledged message
/// sent by a Firmware Distribution Client to check which firmware image is stored in a particular
/// entry in the Firmware Images List state on a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Get By Index message is a
/// Firmware Distribution Firmware Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.105),
/// 8.4.2.19 Firmware Distribution Firmware Get By Index.
@implementation SigFirmwareDistributionFirmwareGetByIndex

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareGetByIndex;
    }
    return self;
}

- (instancetype)initWithDistributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareGetByIndex;
        _distributionFirmwareImageIndex = distributionFirmwareImageIndex;
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = 0;
        tem16 = distributionFirmwareImageIndex;
        [mData appendData:[NSData dataWithBytes:&tem16 length:2]];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareGetByIndex object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareGetByIndex.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareGetByIndex object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareGetByIndex;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _distributionFirmwareImageIndex = tem16;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionFirmwareStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 8.4.2.20 Firmware Distribution Firmware Delete, opcode:0xB61E


/// The Firmware Distribution Firmware Delete message is an acknowledged message
/// sent by a Firmware Distribution Client to delete a stored firmware image on a
/// Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Delete message is a
/// Firmware Distribution Firmware Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.20 Firmware Distribution Firmware Delete.
@implementation SigFirmwareDistributionFirmwareDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareDelete;
    }
    return self;
}

- (instancetype)initWithFirmwareID:(NSData *)firmwareID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareDelete;
        if (firmwareID && firmwareID.length > 0) {
            _firmwareID = [NSData dataWithData:firmwareID];
        }
        NSMutableData *mData = [NSMutableData data];
        if (firmwareID && firmwareID.length > 0) {
            [mData appendData:firmwareID];
        }
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareDelete object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareDelete.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareDelete;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length == 0) {
            return nil;
        }
        _firmwareID = [NSData dataWithData:parameters];
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionFirmwareStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 8.4.2.21 Firmware Distribution Firmware Delete All, opcode:0xB61F


/// The Firmware Distribution Firmware Delete All message is an acknowledged message
/// sent by a Firmware Distribution Client to delete all firmware images stored on a
/// Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Delete All message is a
/// Firmware Distribution Firmware Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.21 Firmware Distribution Firmware Delete All.
@implementation SigFirmwareDistributionFirmwareDeleteAll

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareDeleteAll;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareDeleteAll object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareDeleteAll.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareDeleteAll object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionFirmwareDeleteAll;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionUploadStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.4 Firmware Distribution Receivers Get, opcode:0xB60F


/// The Firmware Distribution Receivers Get message is an acknowledged message
/// sent by the Firmware Distribution Client to get the firmware distribution status of
/// each Updating node.
/// @note   The response to a Firmware Distribution Receivers Get message is a
/// Firmware Distribution Receivers List message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.4 Firmware Distribution Receivers Get.
@implementation SigFirmwareDistributionReceiversGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversGet;
    }
    return self;
}

- (instancetype)initWithFirstIndex:(UInt16)firstIndex entriesLimit:(UInt16)entriesLimit {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversGet;
        _firstIndex = firstIndex;
        _entriesLimit = entriesLimit;
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = firstIndex;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = entriesLimit;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionReceiversGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversGet.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversGet;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 2);
        _firstIndex = tem;
        memcpy(&tem, dataByte + 2, 2);
        _entriesLimit = tem;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionReceiversList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.5 Firmware Distribution Receivers List, opcode:0xB610


/// The Firmware Distribution Receivers List message is an unacknowledged message sent
/// by the Firmware Distribution Server to report the firmware distribution status of each receiver.
/// @note   A Firmware Distribution Receivers List message is sent as a response to a
/// Firmware Distribution Receivers Get message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.98),
/// 8.4.2.5 Firmware Distribution Receivers List.
@implementation SigFirmwareDistributionReceiversList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversList;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionReceiversList object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversList.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversList;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 4 || ((parameters.length - 4) % 5) != 0) {
            return nil;
        }
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 2);
        _receiversListCount = tem;
        memcpy(&tem, dataByte + 2, 2);
        _firstIndex = tem;

        NSMutableArray *mArray = [NSMutableArray array];
        while ((mArray.count * 5 + 4) < parameters.length) {
            SigUpdatingNodeEntryModel *model = [[SigUpdatingNodeEntryModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(mArray.count * 5 + 4, 5)]];
            [mArray addObject:model];
        }
        _receiversList = mArray;
    }
    return self;
}

- (instancetype)initWithReceiversListCount:(UInt16)receiversListCount firstIndex:(UInt16)firstIndex receiversList:(NSArray <SigUpdatingNodeEntryModel *>*)receiversList {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversList;
        _receiversListCount = receiversListCount;
        _firstIndex = firstIndex;
        _receiversList = [NSMutableArray arrayWithArray:receiversList];

        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = 0;
        tem16 = receiversListCount;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = firstIndex;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        
        for (SigUpdatingNodeEntryModel *model in receiversList) {
            if (model && model.parameters && model.parameters.length > 0) {
                [mData appendData:model.parameters];
            }
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 8.4.2.1 Firmware Distribution Receivers Add, opcode:0xB611


/// The Firmware Distribution Receivers Add message is an acknowledged message sent
/// by a Firmware Distribution Client to add new entries to the Distribution Receivers List
/// state of a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Receivers Add message is a
/// Firmware Distribution Receivers Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.96),
/// 8.4.2.1 Firmware Distribution Receivers Add.
@implementation SigFirmwareDistributionReceiversAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversAdd;
    }
    return self;
}

- (instancetype)initWithReceiverEntrysList:(NSArray <SigReceiverEntryModel *>*)receiverEntrysList {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversAdd;
        if (receiverEntrysList && receiverEntrysList.count) {
            _receiverEntrysList = [NSMutableArray arrayWithArray:receiverEntrysList];
            NSMutableData *mData = [NSMutableData data];
            for (SigReceiverEntryModel *model in receiverEntrysList) {
                [mData appendData:model.parameters];
            }
            self.parameters = mData;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionReceiversAdd object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversAdd.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversAdd;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length == 0 || (parameters.length % 3) != 0) {
            return nil;
        }
        NSMutableArray *mArray = [NSMutableArray array];
        while (mArray.count*3 < parameters.length) {
            SigReceiverEntryModel *model = [[SigReceiverEntryModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(mArray.count*3, 3)]];
            [mArray addObject:model];
        }
        _receiverEntrysList = mArray;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionReceiversStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.2 Firmware Distribution Receivers Delete All, opcode:0xB612


/// The Firmware Distribution Receivers Delete All message is an acknowledged
/// message sent by a Firmware Distribution Client to remove all entries from the
/// Distribution Receivers List state of a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Receivers Delete All message
/// is a Firmware Distribution Receivers Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.2 Firmware Distribution Receivers Delete All.
@implementation SigFirmwareDistributionReceiversDeleteAll

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversDeleteAll;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionReceiversDeleteAll object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversDeleteAll.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversDeleteAll object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversDeleteAll;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionReceiversStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.3 Firmware Distribution Receivers Status, opcode:0xB613


/// The Firmware Distribution Receivers Status message is an unacknowledged
/// message sent by a Firmware Distribution Server to report the current size of
/// the Distribution Receivers List state.
/// @note   A Firmware Distribution Receivers Status message is sent as a
/// response to a Firmware Distribution Receivers Add message or a Firmware
/// Distribution Receivers Delete All message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.3 Firmware Distribution Receivers Status.
@implementation SigFirmwareDistributionReceiversStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionReceiversStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 3) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        memcpy(&tem, dataByte + 1, 2);
        _receiversListCount = tem;
    }
    return self;
}

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status receiversListCount:(UInt16)receiversListCount {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionReceiversStatus;
        _status = status;
        _receiversListCount = receiversListCount;
        
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        tem8 = status;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = receiversListCount;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 8.4.2.6 Firmware Distribution Capabilities Get, opcode:0xB614


/// The Firmware Distribution Capabilities Get message is an acknowledged message
/// sent by a Firmware Distribution Client to get the capabilities of a Firmware
/// Distribution Server.
/// @note   The response to a Firmware Distribution Capabilities Get message is a
/// Firmware Distribution Capabilities Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.99),
/// 8.4.2.6 Firmware Distribution Capabilities Get.
@implementation SigFirmwareDistributionCapabilitiesGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCapabilitiesGet;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionCapabilitiesGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionCapabilitiesGet.
 * @return  return `nil` when initialize SigFirmwareDistributionCapabilitiesGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCapabilitiesGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigFirmwareDistributionCapabilitiesStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}
@end


#pragma mark 8.4.2.7 Firmware Distribution Capabilities Status, opcode:0xB615


/// The Firmware Distribution Capabilities Status message is an unacknowledged
/// message sent by a Firmware Distribution Server to report Distributor capabilities.
/// @note   A Firmware Distribution Capabilities Status message is sent as a
/// response to a Firmware Distributor Capabilities Get message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.99),
/// 8.4.2.7 Firmware Distribution Capabilities Status.
@implementation SigFirmwareDistributionCapabilitiesStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCapabilitiesStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigFirmwareDistributionCapabilitiesStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionCapabilitiesStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionCapabilitiesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCapabilitiesStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 17) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt32 tem32 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _maxDistributionReceiversListSize = tem16;
        memcpy(&tem16, dataByte + 2, 2);
        _maxFirmwareImagesListSize = tem16;
        memcpy(&tem32, dataByte + 4, 4);
        _maxFirmwareImageSize = tem32;
        memcpy(&tem32, dataByte + 8, 4);
        _maxUploadSpace = tem32;
        memcpy(&tem32, dataByte + 12, 4);
        _remainingUploadSpace = tem32;
        memcpy(&tem8, dataByte + 16, 1);
        _outOfBandRetrievalSupported = tem8;
        if (parameters.length > 17) {
            _supportedURISchemeNames = [parameters subdataWithRange:NSMakeRange(17, parameters.length - 17)];
        }
    }
    return self;
}

- (instancetype)initWithMaxDistributionReceiversListSize:(UInt16)maxDistributionReceiversListSize maxFirmwareImagesListSize:(UInt16)maxFirmwareImagesListSize maxFirmwareImageSize:(UInt32)maxFirmwareImageSize maxUploadSpace:(UInt32)maxUploadSpace remainingUploadSpace:(UInt32)remainingUploadSpace outOfBandRetrievalSupported:(UInt8)outOfBandRetrievalSupported {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_FirmwareDistributionCapabilitiesStatus;
        _maxDistributionReceiversListSize = maxDistributionReceiversListSize;
        _maxFirmwareImagesListSize = maxFirmwareImagesListSize;
        _maxFirmwareImageSize = maxFirmwareImageSize;
        _maxUploadSpace = maxUploadSpace;
        _remainingUploadSpace = remainingUploadSpace;
        _outOfBandRetrievalSupported = outOfBandRetrievalSupported;
        
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt32 tem32 = 0;
        tem16 = maxDistributionReceiversListSize;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = maxFirmwareImagesListSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem32 = maxFirmwareImageSize;
        data = [NSData dataWithBytes:&tem32 length:4];
        [mData appendData:data];
        tem32 = maxUploadSpace;
        data = [NSData dataWithBytes:&tem32 length:4];
        [mData appendData:data];
        tem32 = remainingUploadSpace;
        data = [NSData dataWithBytes:&tem32 length:4];
        [mData appendData:data];
        tem8 = outOfBandRetrievalSupported;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark - 7.3.1 BLOB Transfer messages


#pragma mark 7.3.1.1 BLOB Transfer Get, opcode:0xB701


/// BLOB Transfer Get is an acknowledged message used to get
/// the state of a BLOB transfer, if any, on a BLOB Transfer Server.
/// @note   The response to a BLOB Transfer Get message is a
/// BLOB Transfer Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.1 BLOB Transfer Get.
@implementation SigBLOBTransferGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferGet;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBTransferGet object.
 * @param   parameters    the hex data of SigBLOBTransferGet.
 * @return  return `nil` when initialize SigBLOBTransferGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBLOBTransferStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 7.3.1.2 BLOB Transfer Start, opcode:0xB702


/// BLOB Transfer Start is an acknowledged message used to start
/// a new BLOB transfer.
/// @note   The response to a BLOB Transfer Start message is a
/// BLOB Transfer Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.2 BLOB Transfer Start.
@implementation SigBLOBTransferStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferStart;
    }
    return self;
}

- (instancetype)initWithTransferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferStart;
        _RFU = 0;
        _transferMode = transferMode;
        _BLOBID = BLOBID;
        _BLOBSize = BLOBSize;
        _BLOBBlockSizeLog = BLOBBlockSizeLog;
        _MTUSize = MTUSize;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = (_transferMode << 6) | _RFU;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        UInt64 tem64 = BLOBID;
        data = [NSData dataWithBytes:&tem64 length:8];
        [mData appendData:data];
        UInt32 tem32 = BLOBSize;
        data = [NSData dataWithBytes:&tem32 length:4];
        [mData appendData:data];
        tem8 = BLOBBlockSizeLog;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        UInt16 tem16 = MTUSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBTransferStart object.
 * @param   parameters    the hex data of SigBLOBTransferStart.
 * @return  return `nil` when initialize SigBLOBTransferStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferStart;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length != 17) {
            return nil;
        }
        UInt64 tem64 = 0;
        UInt32 tem32 = 0;
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _RFU = tem8 & 0b111111;
        _transferMode = (tem8 >> 6) & 0b11;

        memcpy(&tem64, dataByte+1, 8);
        memcpy(&tem32, dataByte+1+8, 4);
        memcpy(&tem8, dataByte+1+8+4, 1);
        memcpy(&tem16, dataByte+1+8+4+1, 2);
        _BLOBID = tem64;
        _BLOBSize = tem32;
        _BLOBBlockSizeLog = tem8;
        _MTUSize = tem16;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBLOBTransferStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 7.3.1.3 BLOB Transfer Cancel, opcode:0xB703


/// BLOB Transfer Cancel is an acknowledged message used to cancel
/// the ongoing BLOB transfer.
/// @note   The response to the BLOB Transfer Cancel message is a
/// BLOB Transfer Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.3 BLOB Transfer Cancel.
@implementation SigObjectTransferCancel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferCancel;
    }
    return self;
}

- (instancetype)initWithBLOBID:(UInt64)BLOBID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferCancel;
        _BLOBID = BLOBID;
        NSMutableData *mData = [NSMutableData data];
        UInt64 tem64 = BLOBID;
        NSData *data = [NSData dataWithBytes:&tem64 length:8];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigObjectTransferCancel object.
 * @param   parameters    the hex data of SigObjectTransferCancel.
 * @return  return `nil` when initialize SigObjectTransferCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferCancel;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length != 8) {
            return nil;
        }
        UInt64 tem64 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem64, dataByte, 8);
        _BLOBID = tem64;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBLOBTransferStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 7.3.1.4 BLOB Transfer Status, opcode:0xB704


/// BLOB Transfer Status is an unacknowledged message used to report
/// the state of the BLOB Transfer Server.
/// @note   The message is sent in response to a BLOB Transfer Get message,
/// a BLOB Transfer Start message, or a BLOB Transfer Cancel message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.29),
/// 7.3.1.4 BLOB Transfer Status.
@implementation SigBLOBTransferStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBTransferStatus object.
 * @param   parameters    the hex data of SigBLOBTransferStatus.
 * @return  return `nil` when initialize SigBLOBTransferStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8 & 0b1111;
        _RFU = (tem8 >> 4) & 0b11;
        _transferMode = (tem8 >> 6) & 0b11;
        memcpy(&tem8, dataByte+1, 1);
        _transferPhase = tem8;
        if (parameters.length >= 1+1+8) {
            UInt64 tem64 = 0;
            memcpy(&tem64, dataByte + 1 + 1, 8);
            _BLOBID = tem64;
            if (parameters.length >= 1+1+8+4+1+2+1) {
                UInt32 tem32 = 0;
                memcpy(&tem32, dataByte + 1 + 1 + 8, 4);
                _BLOBSize = tem32;
                memcpy(&tem8, dataByte+1+1+8+4, 1);
                _blockSizeLog = tem8;
                UInt16 tem16 = 0;
                memcpy(&tem16, dataByte+1+1+8+4+1, 2);
                _transferMTUSize = tem16;
                _blocksNotReceived = [parameters subdataWithRange:NSMakeRange(1+1+8+4+1+2, parameters.length - (1+1+8+4+1+2))];
            }
        }
    }
    return self;
}

- (instancetype)initWithStatus:(SigBLOBTransferStatusType)status RFU:(UInt8)RFU transferMode:(SigTransferModeState)transferMode transferPhase:(SigTransferPhaseState)transferPhase BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize blockSizeLog:(UInt8)blockSizeLog transferMTUSize:(UInt16)transferMTUSize blocksNotReceived:(NSData *)blocksNotReceived {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBTransferStatus;
        _status = status;
        _RFU = RFU;
        _transferMode = transferMode;
        _transferPhase = transferPhase;
        _BLOBID = BLOBID;
        _BLOBSize = BLOBSize;
        _blockSizeLog = blockSizeLog;
        _transferMTUSize = transferMTUSize;
        _blocksNotReceived = blocksNotReceived;
        if (blocksNotReceived && blocksNotReceived.length > 0) {
            _blocksNotReceived = [NSData dataWithData:blocksNotReceived];
        }

        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt32 tem32 = 0;
        UInt64 tem64 = 0;
        tem8 = (status & 0b1111) | ((RFU & 0b11) << 4) | ((transferMode & 0b11) << 6);
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = transferPhase;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem64 = BLOBID;
        data = [NSData dataWithBytes:&tem64 length:8];
        [mData appendData:data];
        tem32 = BLOBSize;
        data = [NSData dataWithBytes:&tem32 length:4];
        [mData appendData:data];
        tem8 = blockSizeLog;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = transferMTUSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        if (blocksNotReceived && blocksNotReceived.length > 0) {
            [mData appendData:blocksNotReceived];
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 3.1.3.1.6 BLOB Block Start, opcode:0xB705


/// BLOB Block Start is an acknowledged message used to start
/// the transfer of an incoming block to the server.
/// @note   The response to the BLOB Block Start message is
/// a BLOB Block Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.6 BLOB Block Start.
@implementation SigBLOBBlockStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockStart;
    }
    return self;
}

- (instancetype)initWithBlockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockStart;
        _blockNumber = blockNumber;
        _chunkSize = chunkSize;
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = blockNumber;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = chunkSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBBlockStart object.
 * @param   parameters    the hex data of SigBLOBBlockStart.
 * @return  return `nil` when initialize SigBLOBBlockStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockStart;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 4) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte, 2);
        _blockNumber = tem16;
        memcpy(&tem16, dataByte+2, 2);
        _chunkSize = tem16;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBLOBBlockStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 3.1.3.1.8 BLOB Chunk Transfer, opcode:0x7D


/// BLOB Chunk Transfer is an unacknowledged message used to
/// deliver a chunk of the current block to a BLOB Transfer Server.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.8 BLOB Chunk Transfer.
@implementation SigBLOBChunkTransfer

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBChunkTransfer;
    }
    return self;
}

- (instancetype)initWithChunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData sendBySegmentPdu:(BOOL)sendBySegmentPdu {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBChunkTransfer;
        _chunkNumber = chunkNumber;
        _chunkData = [NSData dataWithData:chunkData];
        _sendBySegmentPdu = sendBySegmentPdu;
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = chunkNumber;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        [mData appendData:chunkData];
        self.parameters = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBChunkTransfer object.
 * @param   parameters    the hex data of SigBLOBChunkTransfer.
 * @return  return `nil` when initialize SigBLOBChunkTransfer object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBChunkTransfer;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 2+1) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _chunkNumber = tem16;
        if (parameters.length >= 2+1) {
            _chunkData = [parameters subdataWithRange:NSMakeRange(2, parameters.length-2)];
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _chunkNumber;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    [mData appendData:_chunkData];
    return mData;
}

- (BOOL)isSegmented {
    //v3.3.0特殊处理：最后一个包如果为unsegment包，需使用segment包进行发送。即所有SigBLOBChunkTransfer都使用segment的发送发送即可。
    return _sendBySegmentPdu;
}

@end


#pragma mark 3.1.3.1.5 BLOB Block Get, opcode:0xB707


/// BLOB Block Get is an acknowledged message used to retrieve the phase
/// of the transfer and to find out which block is active (i.e., being transferred),
/// if any, and get the status of that block.
/// @note   The response to the BLOB Block Get message is a BLOB Block
/// Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.29),
/// 3.1.3.1.5 BLOB Block Get.
@implementation SigBLOBBlockGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockGet;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBBlockGet object.
 * @param   parameters    the hex data of SigBLOBBlockGet.
 * @return  return `nil` when initialize SigBLOBBlockGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBLOBBlockStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 7.3.1.8 BLOB Partial Block Report, opcode:0x7C


/// BLOB Partial Block Report is an unacknowledged message used
/// by the BLOB Transfer Server to request chunks.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.31),
/// 7.3.1.8 BLOB Partial Block Report.
@implementation SigBLOBPartialBlockReport

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBPartialBlockReport;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBPartialBlockReport object.
 * @param   parameters    the hex data of SigBLOBPartialBlockReport.
 * @return  return `nil` when initialize SigBLOBPartialBlockReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBPartialBlockReport;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil) {
            return nil;
        }
        if (parameters.length > 0) {
            NSArray *array = [LibTools getNumberListFromUTF8EncodeData:parameters];
            if (array) {
                _encodedMissingChunks = [NSMutableArray arrayWithArray:array];
            } else {
                _encodedMissingChunks = [NSMutableArray array];
            }
        }
    }
    return self;
}

@end


#pragma mark 3.1.3.1.7 BLOB Block Status, opcode:0x7E


/// BLOB Block Status is an unacknowledged message used to report
/// the status of the active block (i.e., the block being transferred), if any.
/// @note   The message is sent as a response to the BLOB Block
/// Start message or the BLOB Block Get message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.7 BLOB Block Status.
@implementation SigBLOBBlockStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBBlockStatus object.
 * @param   parameters    the hex data of SigBLOBBlockStatus.
 * @return  return `nil` when initialize SigBLOBBlockStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8 & 0b1111;
        _RFU = (tem8 >> 4) & 0b11;
        _format = (tem8 >> 6) & 0b11;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _blockNumber = tem16;
        memcpy(&tem16, dataByte+3, 2);
        _chunkSize = tem16;
        if (_format == SigBLOBBlockFormatType_someChunksMissing) {
            if (parameters.length > 5) {
                NSMutableArray *array = [NSMutableArray array];
                UInt16 addressesLength = parameters.length - 5;
                UInt16 index = 0;
                while (addressesLength > index) {
                    memcpy(&tem8, dataByte+5+index, 1);
                    for (int i=0; i<8; i++) {
                        BOOL exist = (tem8 >> i) & 1;
                        if (exist) {
                            [array addObject:@(i+8*index)];
                        }
                    }
                    index++;
                }
                _missingChunksList = array;
            }
        } else if (_format == SigBLOBBlockFormatType_encodedMissingChunks) {
            if (parameters.length > 5) {
                NSMutableArray *array = [NSMutableArray arrayWithArray:[LibTools getNumberListFromUTF8EncodeData:[parameters subdataWithRange:NSMakeRange(5, parameters.length-5)]]];
                _encodedMissingChunksList = array;
            }
        }
    }
    return self;
}

- (instancetype)initWithStatus:(SigBLOBBlockStatusType)status RFU:(UInt8)RFU format:(SigBLOBBlockFormatType)format blockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize missingChunksList:(NSArray <NSNumber *>*)missingChunksList encodedMissingChunksList:(NSArray <NSNumber *>*)encodedMissingChunksList {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBBlockStatus;
        _status = status;
        _RFU = RFU;
        _format = format;
        _blockNumber = blockNumber;
        _chunkSize = chunkSize;
        if (missingChunksList) {
            _missingChunksList = [NSArray arrayWithArray:missingChunksList];
        } else {
            _missingChunksList = [NSArray array];
        }
        if (encodedMissingChunksList) {
            _encodedMissingChunksList = [NSArray arrayWithArray:encodedMissingChunksList];
        } else {
            _encodedMissingChunksList = [NSArray array];
        }

        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        tem8 = (status & 0b1111) | ((RFU & 0b11) << 4) | ((format & 0b11) << 6);
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = blockNumber;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = chunkSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        //暂时不处理该逻辑，因为需要知道当前BLOB的chunks个数。
        if (format == SigBLOBBlockFormatType_someChunksMissing) {
            TeLogError(@"暂时不处理该逻辑，因为需要知道当前BLOB的chunks个数!!!");
//            if (missingChunksList && missingChunksList.count > 0) {
//                for (NSNumber *num in missingChunksList) {
//
//                }
//                [mData appendData:blocksNotReceived];
//            }
        } else if (format == SigBLOBBlockFormatType_encodedMissingChunks) {
            if (encodedMissingChunksList && encodedMissingChunksList.count > 0) {
                NSData *encodedMissingChunksListData = [LibTools getUTF8EncodeDataFromNumberList:encodedMissingChunksList];
                [mData appendData:encodedMissingChunksListData];
            }
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 7.3.1.10 BLOB Information Get, opcode:0xB70A


/// BLOB Information Get is an acknowledged message used to get
/// the Capabilities state of the BLOB Transfer Server.
/// @note   The response to the BLOB Information Get message is
/// a BLOB Information Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.32),
/// 7.3.1.10 BLOB Information Get.
@implementation SigBLOBInformationGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBInformationGet;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBInformationGet object.
 * @param   parameters    the hex data of SigBLOBInformationGet.
 * @return  return `nil` when initialize SigBLOBInformationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBInformationGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBLOBInformationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 7.3.1.11 BLOB Information Status, opcode:0xB70B


/// BLOB Information Get is an acknowledged message used to get
/// the Capabilities state of the BLOB Transfer Server.
/// @note   The response to the BLOB Information Get message is
/// a BLOB Information Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.32),
/// 7.3.1.11 BLOB Information Status.
@implementation SigBLOBInformationStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBInformationStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigBLOBInformationStatus object.
 * @param   parameters    the hex data of SigBLOBInformationStatus.
 * @return  return `nil` when initialize SigBLOBInformationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBInformationStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 13) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _minBlockSizeLog = tem8;
        memcpy(&tem8, dataByte+1, 1);
        _maxBlockSizeLog = tem8;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+2, 2);
        _maxChunksNumber = tem16;
        memcpy(&tem16, dataByte+4, 2);
        _maxChunkSize = tem16;
        UInt32 tem32 = 0;
        memcpy(&tem32, dataByte+6, 4);
        _maxBLOBSize = tem32;
        memcpy(&tem16, dataByte+10, 2);
        _MTUSize = tem16;
        memcpy(&tem8, dataByte+12, 1);
        _supportedTransferMode.value = tem8;
    }
    return self;
}

- (instancetype)initWithMinBlockSizeLog:(UInt8)minBlockSizeLog maxBlockSizeLog:(UInt8)maxBlockSizeLog maxChunksNumber:(UInt16)maxChunksNumber maxChunkSize:(UInt16)maxChunkSize maxBLOBSize:(UInt32)maxBLOBSize MTUSize:(UInt16)MTUSize supportedTransferMode:(struct SigSupportedTransferMode)supportedTransferMode {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BLOBInformationStatus;
        _minBlockSizeLog = minBlockSizeLog;
        _maxBlockSizeLog = maxBlockSizeLog;
        _maxChunksNumber = maxChunksNumber;
        _maxChunkSize = maxChunkSize;
        _maxBLOBSize = maxBLOBSize;
        _MTUSize = MTUSize;
        _supportedTransferMode = supportedTransferMode;

        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        UInt32 tem32 = 0;
        tem8 = minBlockSizeLog;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = maxBlockSizeLog;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = maxChunksNumber;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = maxChunkSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem32 = maxBLOBSize;
        data = [NSData dataWithBytes:&tem32 length:4];
        [mData appendData:data];
        tem16 = MTUSize;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem8 = supportedTransferMode.value;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark - 4.3.X Bridge messages
/*
 The Bridge messages are used to configure the behavior of
 a Subnet Bridge node.
 The Bridge messages shall be encrypted and authenticated
 using the DevKey of the Subnet Bridge node.
 */


#pragma mark 4.3.X.1 SUBNET_BRIDGE_GET, opcode:0xBF70


/// The SUBNET_BRIDGE_GET message is an acknowledged message used to get
/// the current Subnet Bridge state of a node (see Section 4.2.X).
/// @note   The response to a SUBNET_BRIDGE_GET message is a
/// SUBNET_BRIDGE_STATUS message.
/// @note   This message has no parameters.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.1 SUBNET_BRIDGE_GET.
@implementation SigSubnetBridgeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigSubnetBridgeGet object.
 * @param   parameters    the hex data of SigSubnetBridgeGet.
 * @return  return `nil` when initialize SigSubnetBridgeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSubnetBridgeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.X.2 SUBNET_BRIDGE_SET, opcode:0xBF71


/// The SUBNET_BRIDGE_SET message is an acknowledged message used to set
/// the Subnet Bridge state of a node (see Section 4.2.X).
/// @note   The response to a SUBNET_BRIDGE_SET message is a
/// SUBNET_BRIDGE_STATUS message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.2 SUBNET_BRIDGE_SET.
@implementation SigSubnetBridgeSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeSet;
    }
    return self;
}

/**
 * @brief   Initialize SigSubnetBridgeSet object.
 * @param   parameters    the hex data of SigSubnetBridgeSet.
 * @return  return `nil` when initialize SigSubnetBridgeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeSet;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length < 1) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte, 1);
        _subnetBridge = tem8;
    }
    return self;
}

- (instancetype)initWithSubnetBridge:(SigSubnetBridgeStateValues)subnetBridge {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeSet;
        _subnetBridge = subnetBridge;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = subnetBridge;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSubnetBridgeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.X.3 SUBNET_BRIDGE_STATUS, opcode:0xBF72


/// The SUBNET_BRIDGE_STATUS message is an unacknowledged message
/// used to report the current Subnet Bridge state of a node (see Section 4.2.X).
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.3 SUBNET_BRIDGE_STATUS.
@implementation SigSubnetBridgeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSubnetBridgeStatus object.
 * @param   parameters    the hex data of SigSubnetBridgeStatus.
 * @return  return `nil` when initialize SigSubnetBridgeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeStatus;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length != 8) {
            return nil;
        }
        SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] initWithParameters:parameters];
        _subnetBridge = model;
    }
    return self;
}

- (instancetype)initWithSubnetBridge:(SigSubnetBridgeModel *)subnetBridge {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SubnetBridgeStatus;
        _subnetBridge = subnetBridge;
        self.parameters = [NSData dataWithData:subnetBridge.parameters];
    }
    return self;
}

@end


#pragma mark 4.3.X.4 BRIDGING_TABLE_ADD, opcode:0xBF73


/// The BRIDGING_TABLE_ADD message is an acknowledged message used to add
/// entries to the Bridging Table state (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_ADD message is a
/// BRIDGING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.4 BRIDGING_TABLE_ADD.
@implementation SigBridgeTableAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableAdd;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeTableAdd object.
 * @param   parameters    the hex data of SigBridgeTableAdd.
 * @return  return `nil` when initialize SigBridgeTableAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableAdd;
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        if (parameters == nil || parameters.length != 8) {
            return nil;
        }
        SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] initWithParameters:parameters];
        _subnetBridge = model;
    }
    return self;
}

- (instancetype)initWithSubnetBridge:(SigSubnetBridgeModel *)subnetBridge {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableAdd;
        _subnetBridge = subnetBridge;
        self.parameters = [NSData dataWithData:subnetBridge.parameters];
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBridgeTableStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.X.5 BRIDGING_TABLE_REMOVE, opcode:0xBF74


/// The BRIDGING_TABLE_REMOVE message is an acknowledged message
/// used to remove entries from the Bridging Table state (see Section 4.2.X+1)
/// of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_REMOVE message is a
/// BRIDGING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.5 BRIDGING_TABLE_REMOVE.
@implementation SigBridgeTableRemove

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableRemove;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeTableRemove object.
 * @param   parameters    the hex data of SigBridgeTableRemove.
 * @return  return `nil` when initialize SigBridgeTableRemove object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableRemove;
        if (parameters == nil || parameters.length != 7) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        SigDirectionsFieldValues value = SigDirectionsFieldValues_prohibited;
        NSData *data = [NSData dataWithBytes:&value length:1];
        NSMutableData *mData = [NSMutableData dataWithData:parameters];
        [mData appendData:data];
        SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] initWithParameters:mData];
        _netKeyIndex1 = model.netKeyIndex1;
        _netKeyIndex2 = model.netKeyIndex2;
        _address1 = model.address1;
        _address2 = model.address2;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 address1:(UInt16)address1 address2:(UInt16)address2 {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableRemove;
        _netKeyIndex1 = netKeyIndex1;
        _netKeyIndex2 = netKeyIndex2;
        _address1 = address1;
        _address2 = address2;
        SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] initWithDirections:SigDirectionsFieldValues_prohibited netKeyIndex1:netKeyIndex1 netKeyIndex2:netKeyIndex2 address1:address1 address2:address2];
        self.parameters = [NSData dataWithData:[model.parameters subdataWithRange:NSMakeRange(1, 7)]];
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBridgeTableStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.X.6 BRIDGING_TABLE_STATUS, opcode:0xBF75


/// The BRIDGING_TABLE_STATUS message is an unacknowledged message
/// used to report the status of the most recent operation on the Bridging Table
/// state (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.12),
/// 4.3.X.6 BRIDGING_TABLE_STATUS.
@implementation SigBridgeTableStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeTableStatus object.
 * @param   parameters    the hex data of SigBridgeTableStatus.
 * @return  return `nil` when initialize SigBridgeTableStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableStatus;
        if (parameters == nil || parameters.length != 9) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(1, 8)]];
        _subnetBridge = model;
    }
    return self;
}

- (instancetype)initWithStatus:(SigConfigMessageStatus)status subnetBridge:(SigSubnetBridgeModel *)subnetBridge {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableStatus;
        _status = status;
        _subnetBridge = subnetBridge;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = status;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        [mData appendData:subnetBridge.parameters];
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 4.3.X.7 BRIDGED_SUBNETS_GET, opcode:0xBF76


/// The BRIDGED_SUBNETS_GET message is an acknowledged message used to get
/// a filtered set of pairs of NetKey Indexes (see Table 4.Y+12) extracted from the
/// Bridging Table state entries of a Subnet Bridge node.
/// @note   The response to a BRIDGED_SUBNETS_GET message is a
/// BRIDGED_SUBNETS_LIST message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.13),
/// 4.3.X.7 BRIDGED_SUBNETS_GET.
@implementation SigBridgeSubnetsGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeSubnetsGet;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeSubnetsGet object.
 * @param   parameters    the hex data of SigBridgeSubnetsGet.
 * @return  return `nil` when initialize SigBridgeSubnetsGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeSubnetsGet;
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _filter = tem8 >> 6;
        _prohibited = (tem8 >> 4) & 0b11;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = tem16 & 0xFFF;
        memcpy(&tem8, dataByte+2, 1);
        _startIndex = tem8;
    }
    return self;
}

- (instancetype)initWithFilter:(SigFilterFieldValues)filter prohibited:(UInt8)prohibited netKeyIndex:(UInt16)netKeyIndex startIndex:(UInt8)startIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeSubnetsGet;
        _filter = filter;
        _prohibited = prohibited;
        _netKeyIndex = netKeyIndex;
        _startIndex = startIndex;
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        tem16 = ((filter & 0b11) << 14) | ((prohibited & 0b11) << 12) | (startIndex & 0xFFF);
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        NSMutableData *mData = [NSMutableData dataWithData:data];
        tem8 = startIndex;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBridgeSubnetsList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.X.8 BRIDGED_SUBNETS_LIST, opcode:0xBF77


/// The BRIDGED_SUBNETS_LIST message is an unacknowledged message used
/// to report a filtered set of pairs of NetKey Indexes extracted from the Bridging
/// Table state entries of a Subnet Bridge node.
/// @note   This message is a response to the BRIDGED_SUBNETS_GET message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.14),
/// 4.3.X.8 BRIDGED_SUBNETS_LIST.
@implementation SigBridgeSubnetsList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeSubnetsList;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeSubnetsList object.
 * @param   parameters    the hex data of SigBridgeSubnetsList.
 * @return  return `nil` when initialize SigBridgeSubnetsList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeSubnetsList;
        if (parameters == nil || parameters.length < 3) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _filter = tem8 >> 6;
        _prohibited = (tem8 >> 4) & 0b11;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = tem16 & 0xFFF;
        memcpy(&tem8, dataByte+2, 1);
        _startIndex = tem8;
        NSMutableArray *mArray = [NSMutableArray array];
        while (3 * (_bridgedSubnetsList.count + 1) + 3 >= parameters.length) {
            SigBridgeSubnetModel *model = [[SigBridgeSubnetModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(3 + 3 * _bridgedSubnetsList.count, 3)]];
            [mArray addObject:model];
        }
        _bridgedSubnetsList = mArray;
    }
    return self;
}

- (instancetype)initWithFilter:(SigFilterFieldValues)filter prohibited:(UInt8)prohibited netKeyIndex:(UInt16)netKeyIndex startIndex:(UInt8)startIndex bridgedSubnetsList:(NSArray <SigBridgeSubnetModel *>*)bridgedSubnetsList {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeSubnetsList;
        _filter = filter;
        _prohibited = prohibited;
        _netKeyIndex = netKeyIndex;
        _startIndex = startIndex;
        if (bridgedSubnetsList) {
            _bridgedSubnetsList = [NSArray arrayWithArray:bridgedSubnetsList];
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        tem16 = ((filter & 0b11) << 14) | ((prohibited & 0b11) << 12) | (startIndex & 0xFFF);
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        NSMutableData *mData = [NSMutableData dataWithData:data];
        tem8 = startIndex;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        for (SigBridgeSubnetModel *model in bridgedSubnetsList) {
            if (model.parameters) {
                [mData appendData:model.parameters];
            }
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 4.3.X.9 BRIDGING_TABLE_GET, opcode:0xBF78


/// The BRIDGING_TABLE_GET message is an acknowledged message used to get
/// the list of addresses and allowed traffic directions of the Bridging Table state
/// entries (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_GET message is a
/// BRIDGING_TABLE_LIST message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.14),
/// 4.3.X.9 BRIDGING_TABLE_GET.
@implementation SigBridgeTableGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableGet;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeTableGet object.
 * @param   parameters    the hex data of SigBridgeTableGet.
 * @return  return `nil` when initialize SigBridgeTableGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableGet;
        if (parameters == nil || parameters.length != 5) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        SigBridgeSubnetModel *model = [[SigBridgeSubnetModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(0, 3)]];
        _netKeyIndex1 = model.netKeyIndex1;
        _netKeyIndex2 = model.netKeyIndex2;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte+3, 2);
        _startIndex = tem16;

    }
    return self;
}

- (instancetype)initWithNetKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 startIndex:(UInt16)startIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableGet;
        _netKeyIndex1 = netKeyIndex1;
        _netKeyIndex2 = netKeyIndex2;
        _startIndex = startIndex;
        SigBridgeSubnetModel *model = [[SigBridgeSubnetModel alloc] initWithNetKeyIndex1:netKeyIndex1 netKeyIndex2:netKeyIndex2];
        UInt16 tem16 = startIndex;
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        NSMutableData *mData = [NSMutableData dataWithData:model.parameters];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBridgeTableList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.X.10 BRIDGING_TABLE_LIST, opcode:0xBF79


/// The BRIDGING_TABLE_LIST message is an unacknowledged message used to
/// report the list of addresses and allowed traffic directions of the Bridging Table
/// state entries (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.15),
/// 4.3.X.10 BRIDGING_TABLE_LIST.
@implementation SigBridgeTableList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableList;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgeTableList object.
 * @param   parameters    the hex data of SigBridgeTableList.
 * @return  return `nil` when initialize SigBridgeTableList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableList;
        if (parameters == nil || parameters.length < 6) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        SigBridgeSubnetModel *model = [[SigBridgeSubnetModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(1, 3)]];
        _netKeyIndex1 = model.netKeyIndex1;
        _netKeyIndex2 = model.netKeyIndex2;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+4, 2);
        _startIndex = tem16;
        NSMutableArray *mArray = [NSMutableArray array];
        while (6 + 5 * (mArray.count + 1) <= parameters.length) {
            SigBridgedAddressesModel *addressesModel = [[SigBridgedAddressesModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(6+5*mArray.count, 5)]];
            [mArray addObject:addressesModel];
        }
        _bridgedAddressesList = mArray;
    }
    return self;
}

- (instancetype)initWithStatus:(UInt8)status netKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 startIndex:(UInt16)startIndex bridgedAddressesList:(NSArray <SigBridgedAddressesModel *>*)bridgedAddressesList {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgeTableList;
        _status = status;
        _netKeyIndex1 = netKeyIndex1;
        _netKeyIndex2 = netKeyIndex2;
        _startIndex = startIndex;
        if (bridgedAddressesList) {
            _bridgedAddressesList = [NSArray arrayWithArray:bridgedAddressesList];
        }
        
        UInt16 tem8 = status;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        NSMutableData *mData = [NSMutableData dataWithData:data];
        SigBridgeSubnetModel *model = [[SigBridgeSubnetModel alloc] initWithNetKeyIndex1:netKeyIndex1 netKeyIndex2:netKeyIndex2];
        [mData appendData:model.parameters];
        UInt16 tem16 = startIndex;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        if (bridgedAddressesList && bridgedAddressesList.count > 0) {
            for (SigBridgedAddressesModel *addressesModel in bridgedAddressesList) {
                if (addressesModel.parameters) {
                    [mData appendData:addressesModel.parameters];
                }
            }
        }
        self.parameters = mData;
    }
    return self;
}

@end


#pragma mark 4.3.11.11 BRIDGING_TABLE_SIZE_GET, opcode:0xBF7A


/// A BRIDGING_TABLE_SIZE_GET message is an acknowledged message used to get
/// the Bridging Table Size state (see Section 4.2.43) of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_SIZE_GET message is a
/// BRIDGING_TABLE_SIZE_STATUS message.
/// @note   This message has no parameters.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.414),
/// 4.3.11.11 BRIDGING_TABLE_SIZE_GET.
@implementation SigBridgingTableSizeGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgingTableSizeGet;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgingTableSizeGet object.
 * @param   parameters    the hex data of SigBridgingTableSizeGet.
 * @return  return `nil` when initialize SigBridgingTableSizeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgingTableSizeGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigBridgingTableSizeStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.11.12 BRIDGING_TABLE_SIZE_STATUS, opcode:0xBF7B


/// A BRIDGING_TABLE_SIZE_STATUS message is an unacknowledged message used to
/// report the Bridging Table Size state (see Section 4.2.43) of a Subnet Bridge node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.414),
/// 4.3.11.12 BRIDGING_TABLE_SIZE_STATUS.
@implementation SigBridgingTableSizeStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgingTableSizeStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgingTableSizeStatus object.
 * @param   parameters    the hex data of SigBridgingTableSizeStatus.
 * @return  return `nil` when initialize SigBridgingTableSizeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgingTableSizeStatus;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        if (parameters) {
            self.parameters = [NSData dataWithData:parameters];
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _bridgingTableSize = tem16;
    }
    return self;
}

/**
 * @brief   Initialize SigBridgingTableSizeStatus object.
 * @param   bridgingTableSize    Bridging Table Size state.
 * The Bridging_Table_Size field indicates the Bridging Table Size state of the node,
 * as defined in Section 4.2.43.
 * The Bridging Table Size state is a 2-octet value indicating the maximum number of Bridging
 * Table state entries that the Subnet Bridge node can support. Multiple Bridging Table state
 * entries can be stored for a single pair of subnets.
 * The Bridging Table Size state value shall be at least 16.
 * @return  return `nil` when initialize SigBridgingTableSizeStatus object fail.
 */
- (instancetype)initWithBridgingTableSize:(UInt16)bridgingTableSize {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_BridgingTableSizeStatus;
        _bridgingTableSize = bridgingTableSize;
        UInt16 tem16 = bridgingTableSize;
        NSMutableData *mData = [NSMutableData data];
        NSData *data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.parameters = mData;
    }
    return self;
}

@end


@implementation SigTelinkOnlineStatusMessage
- (instancetype)initWithAddress:(UInt16)address state:(DeviceState)state brightness:(UInt8)brightness temperature:(UInt8)temperature {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _address = address;
        _state = state;
        _brightness = brightness;
        _temperature = temperature;
    }
    return self;
}
@end
