/********************************************************************************************************
 * @file     NSData+Compression.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/10/9
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/*
 Compression Algorithm: rfc1951
 Reference: DEFLATE Compressed Data Format Specification, RFC 1951, version 1.3  (https://specificationrefs.bluetooth.com/ext-ref/IETF/RFC1951.pdf)
 */
@interface NSData (Compression)

/// This method will apply the gzip deflate algorithm and return the compressed data.
/// The compression level is a floating point value between 0.0 and 1.0,
///with 0.0 meaning no compression and 1.0 meaning maximum compression.
///A value of 0.1 will provide the fastest possible compression. If you supply a negative value,
///this will apply the default compression level, which is equivalent to a value of around 0.7.
- (nullable NSData *)compressionDataWithCompressionLevel:(float)level;

/// This method is equivalent to calling `compressionDataWithCompressionLevel:` with the default compression level.
- (nullable NSData *)compressionData;

/// This method will unzip data that was compressed using the deflate algorithm and return the result.
- (nullable NSData *)decompressionData;

@end

NS_ASSUME_NONNULL_END
