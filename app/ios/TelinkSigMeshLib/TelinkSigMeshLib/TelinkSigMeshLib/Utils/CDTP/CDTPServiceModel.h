/********************************************************************************************************
 * @file     CDTPServiceModel.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/6/30
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class ObjectModel;

@protocol CDTPServiceDelegate <NSObject>
@optional

/// A callback called whenever the CDTP Client Write some object data to CDTP Service.
/// @param progress The progress of write object. Range of progress is 0.0~1.0
/// @param objectModel The mesh network object.
- (void)onServiceWriteProgress:(float)progress objectModel:(ObjectModel *)objectModel;

/// A callback called whenever the CDTP Client Read some object data from CDTP Service.
/// @param progress The progress of read object. Range of progress is 0.0~1.0, 1.0 mean success.
- (void)onServiceReadProgress:(float)progress;

/// This method is called back when an exception is triggered when the CDTP Client write Object data to the CDTP Service.
/// @param error Error of Write progress.
- (void)onServiceWriteError:(NSError *)error;

/// This method is called back when an exception is triggered when the CDTP Client read Object data from the CDTP Service.
/// @param error Error of Read progress.
- (void)onServiceReadError:(NSError *)error;

@end

@interface CDTPServiceModel : NSObject

/// The delegate will receive callbacks whenever the state of CDTP Service had change.
@property (nonatomic, weak) id <CDTPServiceDelegate>delegate;
/// The dictionary of the share mesh network.
@property (nonatomic, strong) NSDictionary *shareMeshDictionary;

/**
 * @brief   Initialize CDTPServiceModel object.
 * @param   shareMeshDictionary  The dictionary of the share mesh network.
 * @return  return `nil` when initialize CDTPServiceModel object fail.
 */
- (instancetype)initWithShareMeshDictionary:(NSDictionary *)shareMeshDictionary;

/// CDTP Service start Advertising.
- (void)startAdvertising;

/// CDTP Service stop Advertising.
- (void)stopAdvertising;

/// CDTP Service stop CDTP Share progress.
- (void)endShare;

@end

NS_ASSUME_NONNULL_END
