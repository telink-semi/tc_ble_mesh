//
//  CBUUID+Hex.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "CBUUID+Hex.h"

//#import <AppKit/AppKit.h>


@implementation CBUUID (Hex)

/// Creates the UUID from a 32-character hexadecimal string.
- (instancetype)initWithHex:(NSString *)hex {
    if(self = [super init]){
        if (hex.length != 32) {
            return nil;
        }
        NSString *uuidString = [NSString stringWithFormat:@"%@_%@_%@_%@_%@",[hex substringWithRange:NSMakeRange(0, 8)],[hex substringWithRange:NSMakeRange(8, 4)],[hex substringWithRange:NSMakeRange(12, 4)],[hex substringWithRange:NSMakeRange(16, 4)],[hex substringWithRange:NSMakeRange(20, 12)]];
        CBUUID *uuid = [CBUUID UUIDWithString:uuidString];
        if(uuid == nil){
            return nil;
        }
        return uuid;
    }
    return nil;
}

/// Returns the uuidString without dashes.
- (NSString *)getHex {
    return [self.UUIDString stringByReplacingOccurrencesOfString:@"-" withString:@""];
}

/// The UUID as Data.
- (NSData *)getData {
    return [LibTools nsstringToHex:self.UUIDString];
}

@end
