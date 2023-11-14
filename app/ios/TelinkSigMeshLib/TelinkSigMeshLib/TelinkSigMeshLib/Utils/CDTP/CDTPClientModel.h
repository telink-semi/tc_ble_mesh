/********************************************************************************************************
 * @file     CDTPClientModel.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/6/30
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#import "OTSBaseModel.h"

NS_ASSUME_NONNULL_BEGIN

@protocol CDTPClientDelegate <NSObject>
@optional

/*!
 *  @method onDiscoverServicePeripheral:advertisementData:RSSI:
 *
 *  @param peripheral           A <code>CBPeripheral</code> object.
 *  @param advertisementData    A dictionary containing any advertisement and scan response data.
 *  @param RSSI                 The current RSSI of <i>peripheral</i>, in dBm. A value of <code>127</code> is reserved and indicates the RSSI
 *                                was not available.
 *
 *  @discussion                 This method is invoked while scanning, upon the discovery of <i>peripheral</i> by <i>central</i>. A discovered peripheral must
 *                              be retained in order to use it; otherwise, it is assumed to not be of interest and will be cleaned up by the central manager. For
 *                              a list of <i>advertisementData</i> keys, see {@link CBAdvertisementDataLocalNameKey} and other similar constants.
 *
 *  @seealso                    CBAdvertisementData.h
 *
 */
- (void)onDiscoverServicePeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI;

/// A callback called whenever the CDTP Client Write some object data to CDTP Service.
/// @param progress The progress of write object. Range of progress is 0.0~1.0.
- (void)onClientWriteProgress:(float)progress;

/// A callback called whenever the CDTP Client read some object data from CDTP Service.
/// @param progress The progress of read object. Range of progress is 0.0~1.0.
- (void)onClientReadProgress:(float)progress;

/// This method is called back when an exception is triggered when the CDTP Client write Object data to the CDTP Service. error is nil means Write success.
/// @param error Error of Write progress.
- (void)onClientWriteFinishWithError:(NSError * __nullable )error;

/// This method is called back when an exception is triggered when the CDTP Client read Object data from the CDTP Service. error is nil means read success.
/// @param data The CDTP mesh network object data.
/// @param error Error of read progress.
- (void)onClientReadFinishWithData:(NSData * __nullable )data error:(NSError * __nullable )error;

@end


@interface CDTPClientModel : NSObject

/// The delegate will receive callbacks whenever the state of CDTP Client had change.
@property (nonatomic, weak) id <CDTPClientDelegate>delegate;

/**
 * @brief   Initialize CDTPClientModel object.
 * @param   result  The handle of Initialize ble success.
 * @return  return `nil` when initialize CDTPClientModel object fail.
 */
- (instancetype)initWithBleInitResult:(bleInitSuccessCallback)result;

/// CDTP Client start scan CDTP Service.
- (void)startScanCDTPService;

/// CDTP Client stop scan CDTP Service.
- (void)stopScanCDTPService;

/// CDTP Client read mesh network object from CDTP Service.
/// - Parameter peripheral: the bluetooth Peripheral object of CDTP Service.
- (void)startReadMeshObjectFromServicePeripheral:(CBPeripheral *)peripheral;

/// CDTP Client write mesh network object to CDTP Service.
/// - Parameters:
///   - object: The mesh network CDTP object.
///   - peripheral: the bluetooth Peripheral object of CDTP Service.
- (void)startWriteMeshObject:(ObjectModel *)object toServicePeripheral:(CBPeripheral *)peripheral;

/// CDTP Client stop CDTP Share progress.
- (void)endShare;

@end

NS_ASSUME_NONNULL_END
