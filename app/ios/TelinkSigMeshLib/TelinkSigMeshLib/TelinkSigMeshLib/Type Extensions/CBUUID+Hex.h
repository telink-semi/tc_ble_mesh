//
//  CBUUID+Hex.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/16.
//  Copyright © 2019 Telink. All rights reserved.
//

//#import <AppKit/AppKit.h>


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CBUUID (Hex)

/// Creates the UUID from a 32-character hexadecimal string.
- (instancetype)initWithHex:(NSString *)hex;

/// Returns the uuidString without dashes.
- (NSString *)getHex;

/// The UUID as Data.
- (NSData *)getData;

@end

NS_ASSUME_NONNULL_END
