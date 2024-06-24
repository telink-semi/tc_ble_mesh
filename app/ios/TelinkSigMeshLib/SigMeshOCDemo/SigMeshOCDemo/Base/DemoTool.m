/********************************************************************************************************
 * @file     DemoTool.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/11/22
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

#import "DemoTool.h"

@implementation DemoTool

/// Get Node State Image
/// - Parameter unicastAddress: the unicast address of node
+ (UIImage *)getNodeStateImageWithUnicastAddress:(UInt16)unicastAddress {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:unicastAddress];
    NSString *iconName = @"";
    switch (node.state) {
        case DeviceStateOutOfLine:
            iconName = @"dengo";
            break;
        case DeviceStateOff:
            iconName = @"dengn";
            break;
        case DeviceStateOn:
            iconName = @"dengs";
            break;
        default:
            break;
    }
    if (!node.isKeyBindSuccess) {
        iconName = @"dengo";
    }
    //传感器sensor特殊处理
    if (node.isSensor) {
        iconName = @"ic_sensor_off";
        if (node.isMotionSensor) {
            iconName = SigBearer.share.isOpen ? @"ic_motion_sensor_online" : @"ic_motion_sensor";
        } else if (node.isAmbientLightSensor) {
            iconName = SigBearer.share.isOpen ? @"ic_ambient_light_online" : @"ic_ambient_light";
        } else if (node.sensorDataArray.count > 0 && SigBearer.share.isOpen) {
            iconName = @"ic_sensor";
        }
    } else if (node.isLPN) {
        iconName = @"ic_battery-20-bluetooth";
    } else if (node.isRemote) {
        //遥控器remote特殊处理
        iconName = @"ic_rmt";
    }
    return [UIImage imageNamed:iconName];
}

/// Get Node State string
/// - Parameter unicastAddress: the unicast address of node
+ (NSString *)getNodeStateStringWithUnicastAddress:(UInt16)unicastAddress {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:unicastAddress];
    NSString *state = @"";
    switch (node.state) {
        case DeviceStateOutOfLine:
            state = @"OFFLINE";
            break;
        case DeviceStateOff:
            state = @"OFF";
            break;
        case DeviceStateOn:
            state = @"ON";
            break;
        default:
            break;
    }
    return state;
}

#ifdef kIsTelinkCloudSigMeshLib
+ (NSString *)getStateStringWithState:(ResponseShareState)state {
    NSString *tem = @"";
    switch (state) {
        case ResponseShareStateForbidden:
            tem = @"FORBIDDEN";
            break;
        case ResponseShareStateMember:
            tem = @"MEMBER";
            break;
        case ResponseShareStateVisitor:
            tem = @"VISITOR";
            break;
        case ResponseShareStateApplying:
            tem = @"APPLYING";
            break;

        default:
            break;
    }
    return tem;
}

+ (UIImage *)getStateImageWithState:(ResponseShareState)state {
    UIImage *image = nil;
    switch (state) {
        case ResponseShareStateForbidden:
            image = [UIImage imageNamed:@"ic_forbidden"];
            break;
        case ResponseShareStateMember:
            image = [UIImage imageNamed:@"ic_member"];
            break;
        case ResponseShareStateVisitor:
            image = [UIImage imageNamed:@"ic_visitor"];
            break;
        case ResponseShareStateApplying:
            image = nil;
            break;

        default:
            break;
    }
    return image;
}

+ (UIColor *)getStateColorWithState:(ResponseShareState)state {
    UIColor *tem = [UIColor blackColor];
    switch (state) {
        case ResponseShareStateForbidden:
            tem = HEX(#D63F21);
            break;
        case ResponseShareStateMember:
            tem = HEX(#67A65D);
            break;
        case ResponseShareStateVisitor:
            tem = HEX(#92BC52);
            break;
        case ResponseShareStateApplying:
            tem = HEX(#F1C141);
            break;

        default:
            break;
    }
    return tem;
}
#endif

@end
