/********************************************************************************************************
 * @file     SDKLibCommand.m
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
//  SDKLibCommand.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/4.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SDKLibCommand.h"
//#import "SigMeshLib.h"

@interface SDKLibCommand ()
@property (nonatomic,strong) NSTimer *busyTimer;

@end

@implementation SDKLibCommand

- (instancetype)init{
    if (self = [super init]) {
        _retryCount = kAcknowledgeMessageDefaultRetryCount;
        _responseSourceArray = [NSMutableArray array];
        _timeout = kSDKLibCommandTimeout;
        _hadRetryCount = 0;
        _netkeyA = SigDataSource.share.curNetkeyModel;
        _appkeyA = SigDataSource.share.curAppkeyModel;
        _ivIndexA = SigDataSource.share.curNetkeyModel.ivIndex;
    }
    return self;
}



#pragma mark - config（open API）
/**
 function 1:AUTO if you need do provision , you should call this method, and it'll call back what you need
 
 @param address address of new device
 @param networkKey network key, which provsion need, you can see it as password of the mesh
 @param netkeyIndex netkey index
 @param appkeyModel appkey model
 @param unicastAddress address of remote device
 @param uuid uuid of remote device
 @param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
 @param isAuto 添加完成一个设备后，是否自动扫描添加下一个设备
 @param provisionSuccess call back when a device provision successful
 @param provisionFail call back when a device provision fail
 @param keyBindSuccess call back when a device keybind successful
 @param keyBindFail call back when a device keybind fail
 @param finish finish add the available devices list to the mesh
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish {
    [SigAddDeviceManager.share startAddDeviceWithNextAddress:address networkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel unicastAddress:unicastAddress uuid:uuid keyBindType:type productID:productID cpsData:cpsData isAutoAddNextDevice:isAuto provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail finish:finish];
}

/**
function 1:special if you need do provision , you should call this method, and it'll call back what you need

@param address address of new device
@param networkKey network key, which provsion need, you can see it as password of the mesh
@param netkeyIndex netkey index
@param peripheral device need add to mesh
@param provisionType ProvisionTpye_NoOOB or ProvisionTpye_StaticOOB.
@param staticOOBData oob for ProvisionTpye_StaticOOB.
@param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
@param provisionSuccess call back when a device provision successful
@param provisionFail call back when a device provision fail
@param keyBindSuccess call back when a device keybind successful
@param keyBindFail call back when a device keybind fail
*/
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail{
    [SigAddDeviceManager.share startAddDeviceWithNextAddress:(UInt16)address networkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel peripheral:peripheral provisionType:provisionType staticOOBData:staticOOBData keyBindType:type productID:productID cpsData:cpsData provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

+ (SigMessageHandle *)configAppKeyAddWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toAddress:(UInt16)address successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configAppKeyAddWithSigAppkeyModel:appkeyModel toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configAppKeyAddWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigAppKeyAdd *config = [[SigConfigAppKeyAdd alloc] initWithApplicationKey:appkeyModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseAppKeyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configAppKeyUpdateWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toAddress:(UInt16)address successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configAppKeyUpdateWithSigAppkeyModel:appkeyModel toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configAppKeyUpdateWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigAppKeyUpdate *config = [[SigConfigAppKeyUpdate alloc] initWithApplicationKey:appkeyModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseAppKeyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configAppKeyDeleteWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toAddress:(UInt16)address successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configAppKeyDeleteWithSigAppkeyModel:appkeyModel toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configAppKeyDeleteWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigAppKeyDelete *config = [[SigConfigAppKeyDelete alloc] initWithApplicationKey:appkeyModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseAppKeyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configAppKeyGetWithNetworkKeyIndex:(UInt16)networkKeyIndex toAddress:(UInt16)address successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configAppKeyGetWithNetworkKeyIndex:networkKeyIndex toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configAppKeyGetWithNetworkKeyIndex:(UInt16)networkKeyIndex toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigAppKeyGet *config = [[SigConfigAppKeyGet alloc] initWithNetworkKeyIndex:networkKeyIndex];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseAppKeyListCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configBeaconGetWithAddress:(UInt16)address successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configBeaconGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configBeaconGetWithNode:(SigNodeModel *)node successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigBeaconGet *config = [[SigConfigBeaconGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBeaconStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configBeaconSetWithSigSecureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState toAddress:(UInt16)address successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configBeaconSetWithSigSecureNetworkBeaconState:secureNetworkBeaconState toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configBeaconSetWithSigSecureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState toNode:(SigNodeModel *)node successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigBeaconSet *config = [[SigConfigBeaconSet alloc] initWithEnable:secureNetworkBeaconState];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBeaconStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configCompositionDataGetWithAddress:(UInt16)address successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configCompositionDataGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configCompositionDataGetWithNode:(SigNodeModel *)node successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigCompositionDataGet *config = [[SigConfigCompositionDataGet alloc] initWithPage:0xFF];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseCompositionDataStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configDefaultTtlGetWithAddress:(UInt16)address successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configDefaultTtlGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configDefaultTtlGetWithNode:(SigNodeModel *)node successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigDefaultTtlGet *config = [[SigConfigDefaultTtlGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseDefaultTtlStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configDefaultTtlSetWithTtl:(UInt8)ttl toAddress:(UInt16)address successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configDefaultTtlSetWithTtl:ttl toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configDefaultTtlSetWithTtl:(UInt8)ttl toNode:(SigNodeModel *)node successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigDefaultTtlSet *config = [[SigConfigDefaultTtlSet alloc] initWithTtl:ttl];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseDefaultTtlStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configFriendGetWithAddress:(UInt16)address successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configFriendGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configFriendGetWithNode:(SigNodeModel *)node successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigFriendGet *config = [[SigConfigFriendGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFriendStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configFriendSetWithSigNodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState toAddress:(UInt16)address successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configFriendSetWithSigNodeFeaturesState:nodeFeaturesState toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configFriendSetWithSigNodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState toNode:(SigNodeModel *)node successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigFriendSet *config = [[SigConfigFriendSet alloc] initWithEnable:nodeFeaturesState];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFriendStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configGATTProxyGetWithAddress:(UInt16)address successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configGATTProxyGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configGATTProxyGetWithNode:(SigNodeModel *)node successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigGATTProxyGet *config = [[SigConfigGATTProxyGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseGATTProxyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configGATTProxySetWithSigNodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState toAddress:(UInt16)address successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configGATTProxySetWithSigNodeGATTProxyState:nodeGATTProxyState toNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configGATTProxySetWithSigNodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState toNode:(SigNodeModel *)node successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigGATTProxySet *config = [[SigConfigGATTProxySet alloc] initWithEnable:nodeGATTProxyState];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseGATTProxyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configModelPublicationGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelPublicationGet *config = [[SigConfigModelPublicationGet alloc] initWithModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelPublicationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelPublicationSetWithSigPublish:(SigPublish *)publish modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelPublicationSet *config = [[SigConfigModelPublicationSet alloc] initWithPublish:publish toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelPublicationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelPublicationSetDisableWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelPublicationSet *config = [[SigConfigModelPublicationSet alloc] initWithDisablePublicationForModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelPublicationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelPublicationVirtualAddressSetWithSigPublish:(SigPublish *)publish modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelPublicationVirtualAddressSet *config = [[SigConfigModelPublicationVirtualAddressSet alloc] initWithPublish:publish toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelPublicationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0008) Op Code: (0000801B), parameters: (080000C000FF), accessPdu=801B080000C000FF
<--- Response - Access PDU, source:(0008)->destination: (0001) Op Code: (0000801F), parameters: (00080000C000FF), accessPdu=801F00080000C000FF receieved (decrypted using key: <SigDeviceKeySet: 0x282840300>)
*/
+ (SigMessageHandle *)configModelSubscriptionAddWithGroupAddress:(UInt16)groupAddress toNodeAddress:(UInt16)nodeAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionAdd *config = [[SigConfigModelSubscriptionAdd alloc] initWithGroupAddress:groupAddress toNodeElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = 1;
    command.retryCount = 2;
    return [SigMeshLib.share sendConfigMessage:config toDestination:nodeAddress command:command];
}

+ (SigMessageHandle *)configModelSubscriptionAddWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionAdd *config = [[SigConfigModelSubscriptionAdd alloc] initWithGroup:groupModel toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelSubscriptionDeleteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionDelete *config = [[SigConfigModelSubscriptionDelete alloc] initWithGroup:groupModel fromModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelSubscriptionDeleteAllWithSigModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionDeleteAll *config = [[SigConfigModelSubscriptionDeleteAll alloc] initFromModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelSubscriptionOverwriteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionOverwrite *config = [[SigConfigModelSubscriptionOverwrite alloc] initWithGroup:groupModel toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelSubscriptionVirtualAddressAddWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionVirtualAddressAdd *config = [[SigConfigModelSubscriptionVirtualAddressAdd alloc] initWithGroup:groupModel toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelSubscriptionVirtualAddressDeleteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionVirtualAddressDelete *config = [[SigConfigModelSubscriptionVirtualAddressDelete alloc] initWithGroup:groupModel fromModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelSubscriptionVirtualAddressOverwriteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelSubscriptionVirtualAddressOverwrite *config = [[SigConfigModelSubscriptionVirtualAddressOverwrite alloc] initWithGroup:groupModel fromModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelSubscriptionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configNetworkTransmitGetWithAddress:(UInt16)address successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configNetworkTransmitGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configNetworkTransmitGetWithNode:(SigNodeModel *)node successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNetworkTransmitGet *config = [[SigConfigNetworkTransmitGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNetworkTransmitStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configNetworkTransmitSetWithAddress:(UInt16)address networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configNetworkTransmitSetWithNode:node networkTransmitCount:networkTransmitCount networkTransmitIntervalSteps:networkTransmitIntervalSteps successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configNetworkTransmitSetWithNode:(SigNodeModel *)node networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNetworkTransmitSet *config = [[SigConfigNetworkTransmitSet alloc] initWithCount:networkTransmitCount steps:networkTransmitIntervalSteps];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNetworkTransmitStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configRelayGetWithAddress:(UInt16)address successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configRelayGetWithNode:node successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configRelayGetWithNode:(SigNodeModel *)node successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigRelayGet *config = [[SigConfigRelayGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRelayStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configRelaySetWithAddress:(UInt16)address relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configRelaySetWithNode:node relay:relay networkTransmitCount:networkTransmitCount networkTransmitIntervalSteps:networkTransmitIntervalSteps successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configRelaySetWithNode:(SigNodeModel *)node relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigRelaySet *config = [[SigConfigRelaySet alloc] initWithCount:networkTransmitCount steps:networkTransmitIntervalSteps];
    config.state = relay;
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRelayStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configSIGModelSubscriptionGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigSIGModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigSIGModelSubscriptionGet *config = [[SigConfigSIGModelSubscriptionGet alloc] initOfModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSIGModelSubscriptionListCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configVendorModelSubscriptionGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigVendorModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigVendorModelSubscriptionGet *config = [[SigConfigVendorModelSubscriptionGet alloc] initOfModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseVendorModelSubscriptionListCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configModelAppBindWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier toDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelAppBind *config = [[SigConfigModelAppBind alloc] initWithApplicationKeyIndex:applicationKeyIndex elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelAppStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

+ (SigMessageHandle *)configModelAppBindWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toModelIDModel:(SigModelIDModel *)modelIDModel toNode:(SigNodeModel *)node successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelAppBind *config = [[SigConfigModelAppBind alloc] initWithApplicationKey:appkeyModel toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelAppStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configModelAppUnbindWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toModelIDModel:(SigModelIDModel *)modelIDModel toNode:(SigNodeModel *)node successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigModelAppUnbind *config = [[SigConfigModelAppUnbind alloc] initWithApplicationKey:appkeyModel toModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseModelAppStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configNetKeyAddWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configNetKeyAddWithNode:node networkKey:networkKey successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configNetKeyAddWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNetKeyAdd *config = [[SigConfigNetKeyAdd alloc] initWithNetworkKey:networkKey];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNetKeyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configNetKeyDeleteWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configNetKeyDeleteWithNode:node networkKey:networkKey successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configNetKeyDeleteWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNetKeyDelete *config = [[SigConfigNetKeyDelete alloc] initWithNetworkKey:networkKey];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNetKeyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configNetKeyGetWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configNetKeyGetWithNode:node networkKey:networkKey successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configNetKeyGetWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNetKeyGet *config = [[SigConfigNetKeyGet alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNetKeyListCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configNetKeyUpdateWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        return [self configNetKeyUpdateWithNode:node networkKey:networkKey successCallback:successCallback resultCallback:resultCallback];
    } else {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return nil;
    }
}

+ (SigMessageHandle *)configNetKeyUpdateWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNetKeyUpdate *config = [[SigConfigNetKeyUpdate alloc] initWithNetworkKey:networkKey];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNetKeyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configNodeIdentityGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNodeIdentityGet *config = [[SigConfigNodeIdentityGet alloc] initWithNetKeyIndex:netKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNodeIdentityStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

+ (SigMessageHandle *)configNodeIdentitySetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNodeIdentitySet *config = [[SigConfigNodeIdentitySet alloc] initWithNetKeyIndex:netKeyIndex identity:identity];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNodeIdentityStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

+ (SigMessageHandle *)resetNodeWithNodeAddress:(UInt16)nodeAddress successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNodeReset *config = [[SigConfigNodeReset alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNodeResetStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:nodeAddress command:command];
}

+ (SigMessageHandle *)resetNode:(SigNodeModel *)node successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigNodeReset *config = [[SigConfigNodeReset alloc] init];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseNodeResetStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:node.address command:command];
}

+ (SigMessageHandle *)configSIGModelAppGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigSIGModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigSIGModelAppGet *config = [[SigConfigSIGModelAppGet alloc] initWithModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSIGModelAppListCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}

+ (SigMessageHandle *)configVendorModelAppGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigVendorModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    SigConfigVendorModelAppGet *config = [[SigConfigVendorModelAppGet alloc] initWithModel:modelIDModel];
    command.curMeshMessage = config;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseVendorModelAppListCallBack = successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:modelIDModel.parentElement.parentNode.address command:command];
}


#pragma mark - control and access（open API）

+ (SigMessageHandle *)genericOnOffGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericOnOffGet *message = [[SigGenericOnOffGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseOnOffStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericOnOffGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericOnOffGetWithDestination:node.address resMax:resMax successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)genericOnOffSetWithIsOn:(BOOL)isOn toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericOnOffSet *message = [[SigGenericOnOffSet alloc] initWithIsOn:isOn];
        msg = message;
    } else {
        SigGenericOnOffSetUnacknowledged *message = [[SigGenericOnOffSetUnacknowledged alloc] initWithIsOn:isOn];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseOnOffStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericOnOffSet:isOn withNode:node transitionTime:nil delay:0 resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)genericOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericOnOffSet *message = [[SigGenericOnOffSet alloc] initWithIsOn:isOn];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericOnOffSetUnacknowledged *message = [[SigGenericOnOffSetUnacknowledged alloc] initWithIsOn:isOn];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_ONOFF_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseOnOffStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericLevelGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericLevelGet *message = [[SigGenericLevelGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericLevelGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericLevelGet *message = [[SigGenericLevelGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericLevelSet:(UInt16)level toDestination:(UInt16)destination transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericLevelSet *message = [[SigGenericLevelSet alloc] initWithLevel:level];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericLevelSetUnacknowledged *message = [[SigGenericLevelSetUnacknowledged alloc] initWithLevel:level];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericLevelSet:(UInt16)level withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericLevelSet:level withNode:node transitionTime:nil delay:0 resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)genericLevelSet:(UInt16)level withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericLevelSet *message = [[SigGenericLevelSet alloc] initWithLevel:level];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericLevelSetUnacknowledged *message = [[SigGenericLevelSetUnacknowledged alloc] initWithLevel:level];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericDeltaSet:(UInt32)delta withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericDeltaSet:delta withNode:node transitionTime:nil delay:0 resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)genericDeltaSet:(UInt32)delta withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericDeltaSet *message = [[SigGenericDeltaSet alloc] initWithDelta:delta];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericDeltaSetUnacknowledged *message = [[SigGenericDeltaSetUnacknowledged alloc] initWithDelta:delta];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericDeltaSet:(UInt32)delta toDestination:(UInt16)destination transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericDeltaSet *message = [[SigGenericDeltaSet alloc] initWithDelta:delta];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericDeltaSetUnacknowledged *message = [[SigGenericDeltaSetUnacknowledged alloc] initWithDelta:delta];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericMoveSet:(UInt16)deltaLevel withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericMoveSet:deltaLevel withNode:node transitionTime:nil delay:0 resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)genericMoveSet:(UInt16)deltaLevel withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericMoveSet *message = [[SigGenericMoveSet alloc] initWithDeltaLevel:deltaLevel];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericMoveSetUnacknowledged *message = [[SigGenericMoveSetUnacknowledged alloc] initWithDeltaLevel:deltaLevel];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericDefaultTransitionTimeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericDefaultTransitionTimeGet *message = [[SigGenericDefaultTransitionTimeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_DEF_TRANSIT_TIME_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseDefaultTransitionTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericDefaultTransitionTimeSet:(nonnull SigTransitionTime *)transitionTime withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericDefaultTransitionTimeSet *message = [[SigGenericDefaultTransitionTimeSet alloc] initWithTransitionTime:transitionTime];
        msg = message;
    } else {
        SigGenericDefaultTransitionTimeSetUnacknowledged *message = [[SigGenericDefaultTransitionTimeSetUnacknowledged alloc] initWithTransitionTime:transitionTime];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_DEF_TRANSIT_TIME_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseDefaultTransitionTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)genericOnPowerUpGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericOnPowerUpGet *message = [[SigGenericOnPowerUpGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_ONOFF_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseOnPowerUpStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericOnPowerUpSet:(SigOnPowerUp)onPowerUp withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericOnPowerUpSet *message = [[SigGenericOnPowerUpSet alloc] initWithState:onPowerUp];
        msg = message;
    } else {
        SigGenericOnPowerUpSetUnacknowledged *message = [[SigGenericOnPowerUpSetUnacknowledged alloc] initWithState:onPowerUp];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_ONOFF_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseOnPowerUpStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:msg fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerLevelGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerLevelGet *message = [[SigGenericPowerLevelGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responsePowerLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerLevelSet:(UInt16)power withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericPowerLevelSet:power withNode:node transitionTime:nil delay:0 resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)genericPowerLevelSet:(UInt16)power withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericPowerLevelSet *message = [[SigGenericPowerLevelSet alloc] initWithPower:power];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigGenericPowerLevelSetUnacknowledged *message = [[SigGenericPowerLevelSetUnacknowledged alloc] initWithPower:power];
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLevelStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:msg fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerLastGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerLastGet *message = [[SigGenericPowerLastGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responsePowerLastStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerDefaultGet *message = [[SigGenericPowerDefaultGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responsePowerDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerRangeGet *message = [[SigGenericPowerRangeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responsePowerRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerDefaultSet:(UInt16)power withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericPowerDefaultSet *message = [[SigGenericPowerDefaultSet alloc] initWithPower:power];
        msg = message;
    } else {
        SigGenericPowerDefaultSetUnacknowledged *message = [[SigGenericPowerDefaultSetUnacknowledged alloc] initWithPower:power];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responsePowerDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:msg fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericPowerRangeSetWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigGenericPowerRangeSet *message = [[SigGenericPowerRangeSet alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        msg = message;
    } else {
        SigGenericPowerRangeSetUnacknowledged *message = [[SigGenericPowerRangeSetUnacknowledged alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_POWER_LEVEL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responsePowerRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:msg fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)genericBatteryGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericBatteryStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericBatteryGet *message = [[SigGenericBatteryGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_G_BAT_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBatteryStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorDescriptorGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorDescriptorGet *message = [[SigSensorDescriptorGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorDescriptorStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorDescriptorGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorDescriptorGet *message = [[SigSensorDescriptorGet alloc] initWithPropertyID:propertyID];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorDescriptorStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorGet *message = [[SigSensorGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorGet *message = [[SigSensorGet alloc] initWithPropertyID:propertyID];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorColumnGetWithPropertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorColumnStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorColumnGet *message = [[SigSensorColumnGet alloc] initWithPropertyID:propertyID rawValueX:rawValueX];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorColumnStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorSeriesGetWithPropertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorSeriesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorSeriesGet *message = [[SigSensorSeriesGet alloc] initWithPropertyID:propertyID rawValueX1Data:rawValueX1Data rawValueX2Data:rawValueX2Data];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorSeriesStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorCadenceGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorCadenceGet *message = [[SigSensorCadenceGet alloc] initWithPropertyID:propertyID];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorCadenceStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorCadenceSetWithPropertyID:(UInt16)propertyID cadenceData:(NSData *)cadenceData node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSensorCadenceSet *message = [[SigSensorCadenceSet alloc] init];
        message.cadenceData = cadenceData;
        msg = message;
    } else {
        SigSensorCadenceSetUnacknowledged *message = [[SigSensorCadenceSetUnacknowledged alloc] init];
        message.cadenceData = cadenceData;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorCadenceStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorSettingsGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorSettingsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorSettingsGet *message = [[SigSensorSettingsGet alloc] initWithPropertyID:propertyID];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorSettingsStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorSettingGetWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPpropertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorSettingGet *message = [[SigSensorSettingGet alloc] initWithPropertyID:propertyID settingPropertyID:settingPpropertyID];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorSettingStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sensorSettingSetWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPpropertyID settingRaw:(NSData *)settingRaw node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSensorSettingSet *message = [[SigSensorSettingSet alloc] initWithPropertyID:propertyID settingPropertyID:settingPpropertyID settingRaw:settingRaw];
        msg = message;
    } else {
        SigSensorSettingSetUnacknowledged *message = [[SigSensorSettingSetUnacknowledged alloc] initWithPropertyID:propertyID settingPropertyID:settingPpropertyID settingRaw:settingRaw];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SENSOR_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSensorSettingStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeGet *message = [[SigTimeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeSetWithSigTimeModel:(SigTimeModel *)timeModel toDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeSet *message = [[SigTimeSet alloc] initWithTimeModel:timeModel];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)timeSetWithSigTimeModel:(SigTimeModel *)timeModel node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeSet *message = [[SigTimeSet alloc] initWithTimeModel:timeModel];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeStatusWithSigTimeModel:(SigTimeModel *)timeModel toDestination:(UInt16)destination resMax:(int)resMax successCallback:(_Nullable responseTimeStatusMessageBlock)successCallback resultCallback:(_Nullable resultBlock)resultCallback {
    SigTimeStatus *message = [[SigTimeStatus alloc] init];
    message.timeModel = timeModel;
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)timeStatusWithSigTimeModel:(SigTimeModel *)timeModel node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeStatus *message = [[SigTimeStatus alloc] init];
    message.timeModel = timeModel;
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeRoleGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeRoleGet *message = [[SigTimeRoleGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeRoleStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeRoleSetWithNode:(SigNodeModel *)node timeRole:(SigTimeRole)timeRole resMax:(int)resMax successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeRoleSet *message = [[SigTimeRoleSet alloc] initWithTimeRole:timeRole];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeRoleStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeZoneGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeZoneGet *message = [[SigTimeZoneGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeZoneStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)timeZoneSetWithTimeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeZoneSet *message = [[SigTimeZoneSet alloc] initWithTimeZoneOffsetNew:timeZoneOffsetNew TAIOfZoneChange:TAIOfZoneChange];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTimeZoneStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)TAI_UTC_DeltaGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTAI_UTC_DeltaGet *message = [[SigTAI_UTC_DeltaGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTAI_UTC_DeltaStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)TAI_UTC_DeltaSetWithTAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTAI_UTC_DeltaSet *message = [[SigTAI_UTC_DeltaSet alloc] initWithTAI_UTC_DeltaNew:TAI_UTC_DeltaNew padding:padding TAIOfDeltaChange:TAIOfDeltaChange];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_TIME_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseTAI_UTC_DeltaStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sceneGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSceneGet *message = [[SigSceneGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCENE_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sceneRecallWithSceneNumber:(UInt16)sceneNumber node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self sceneRecallWithSceneNumber:sceneNumber transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)sceneRecallWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSceneRecall *message = [[SigSceneRecall alloc] initWithSceneNumber:sceneNumber transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigSceneRecallUnacknowledged *message = [[SigSceneRecallUnacknowledged alloc] initWithSceneNumber:sceneNumber transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)sceneRecallWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSceneRecall *message = [[SigSceneRecall alloc] initWithSceneNumber:sceneNumber transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigSceneRecallUnacknowledged *message = [[SigSceneRecallUnacknowledged alloc] initWithSceneNumber:sceneNumber transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCENE_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sceneRegisterGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSceneRegisterGet *message = [[SigSceneRegisterGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneRegisterStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)sceneRegisterGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSceneRegisterGet *message = [[SigSceneRegisterGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCENE_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneRegisterStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sceneStoreWithSceneNumber:(UInt16)sceneNumber toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSceneStore *message = [[SigSceneStore alloc] initWithSceneNumber:sceneNumber];
        msg = message;
    } else {
        SigSceneStoreUnacknowledged *message = [[SigSceneStoreUnacknowledged alloc] initWithSceneNumber:sceneNumber];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneRegisterStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)sceneStoreWithSceneNumber:(UInt16)sceneNumber node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSceneStore *message = [[SigSceneStore alloc] initWithSceneNumber:sceneNumber];
        msg = message;
    } else {
        SigSceneStoreUnacknowledged *message = [[SigSceneStoreUnacknowledged alloc] initWithSceneNumber:sceneNumber];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCENE_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneRegisterStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)sceneDeleteWithSceneNumber:(UInt16)sceneNumber toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSceneDelete *message = [[SigSceneDelete alloc] initWithSceneNumber:sceneNumber];
        msg = message;
    } else {
        SigSceneDeleteUnacknowledged *message = [[SigSceneDeleteUnacknowledged alloc] initWithSceneNumber:sceneNumber];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneRegisterStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)sceneDeleteWithSceneNumber:(UInt16)sceneNumber node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSceneDelete *message = [[SigSceneDelete alloc] initWithSceneNumber:sceneNumber];
        msg = message;
    } else {
        SigSceneDeleteUnacknowledged *message = [[SigSceneDeleteUnacknowledged alloc] initWithSceneNumber:sceneNumber];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCENE_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSceneRegisterStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)schedulerActionGetWithSchedulerIndex:(UInt8)schedulerIndex toDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSchedulerActionGet *message = [[SigSchedulerActionGet alloc] initWithIndex:schedulerIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSchedulerActionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)schedulerActionGetWithSchedulerIndex:(UInt8)schedulerIndex node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSchedulerActionGet *message = [[SigSchedulerActionGet alloc] initWithIndex:schedulerIndex];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCHED_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSchedulerActionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)schedulerActionGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSchedulerGet *message = [[SigSchedulerGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSchedulerStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)schedulerActionGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSchedulerGet *message = [[SigSchedulerGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCHED_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSchedulerStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)schedulerActionSetWithSchedulerModel:(SchedulerModel *)schedulerModel toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSchedulerActionSet *message = [[SigSchedulerActionSet alloc] initWithSchedulerModel:schedulerModel];
        msg = message;
    } else {
        SigSchedulerActionSetUnacknowledged *message = [[SigSchedulerActionSetUnacknowledged alloc] initWithSchedulerModel:schedulerModel];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSchedulerActionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)schedulerActionSetWithSchedulerModel:(SchedulerModel *)schedulerModel node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigSchedulerActionSet *message = [[SigSchedulerActionSet alloc] initWithSchedulerModel:schedulerModel];
        msg = message;
    } else {
        SigSchedulerActionSetUnacknowledged *message = [[SigSchedulerActionSetUnacknowledged alloc] initWithSchedulerModel:schedulerModel];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_SCHED_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseSchedulerActionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessGet *message = [[SigLightLightnessGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightLightnessGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLightnessGetWithDestination:node.address resMax:resMax successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightLightnessSetWithLightness:(UInt16)lightness toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLightnessSet *message = [[SigLightLightnessSet alloc] init];
        message.lightness = lightness;
        msg = message;
    } else {
        SigLightLightnessSetUnacknowledged *message = [[SigLightLightnessSetUnacknowledged alloc] init];
        message.lightness = lightness;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightLightnessSetWithLightness:(UInt16)lightness node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLightnessSetWithLightness:lightness transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightLightnessSetWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLightnessSet *message = [[SigLightLightnessSet alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightLightnessSetUnacknowledged *message = [[SigLightLightnessSetUnacknowledged alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessLinearGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessLinearGet *message = [[SigLightLightnessLinearGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessLinearStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessLinearSetWithLightness:(UInt16)lightness node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLightnessLinearSetWithLightness:lightness transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightLightnessLinearSetWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLightnessLinearSet *message = [[SigLightLightnessLinearSet alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightLightnessLinearSetUnacknowledged *message = [[SigLightLightnessLinearSetUnacknowledged alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessLinearStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessLastGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessLastGet *message = [[SigLightLightnessLastGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessLastStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessDefaultGet *message = [[SigLightLightnessDefaultGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessRangeGet *message = [[SigLightLightnessRangeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessDefaultSetWithLightness:(UInt16)lightness node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLightnessDefaultSet *message = [[SigLightLightnessDefaultSet alloc] initWithLightness:lightness];
        msg = message;
    } else {
        SigLightLightnessDefaultSetUnacknowledged *message = [[SigLightLightnessDefaultSetUnacknowledged alloc] initWithLightness:lightness];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLightnessRangeSetWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLightnessRangeSet *message = [[SigLightLightnessRangeSet alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        msg = message;
    } else {
        SigLightLightnessRangeSetUnacknowledged *message = [[SigLightLightnessRangeSetUnacknowledged alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHTNESS_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLightnessRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLGet *message = [[SigLightCTLGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightCTLGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLGet *message = [[SigLightCTLGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLSetWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightCTLSetWithLightness:lightness temperature:temperature deltaUV:deltaUV transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightCTLSetWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightCTLSet *message = [[SigLightCTLSet alloc] initWithCTLLightness:lightness CTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightCTLSetUnacknowledged *message = [[SigLightCTLSetUnacknowledged alloc] initWithCTLLightness:lightness CTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLTemperatureGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLTemperatureGet *message = [[SigLightCTLTemperatureGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLTemperatureStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightCTLTemperatureGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLTemperatureGet *message = [[SigLightCTLTemperatureGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLTemperatureStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLTemperatureRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLTemperatureRangeGet *message = [[SigLightCTLTemperatureRangeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLTemperatureRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLTemperatureSetWithTemperature:(UInt16)temperature deltaUV:(UInt16)deltaUV node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightCTLTemperatureSetWithTemperature:temperature deltaUV:deltaUV transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightCTLTemperatureSetWithTemperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightCTLTemperatureSet *message = [[SigLightCTLTemperatureSet alloc] initWithCTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightCTLTemperatureSetUnacknowledged *message = [[SigLightCTLTemperatureSetUnacknowledged alloc] initWithCTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLTemperatureStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLTemperatureSetWithTemperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightCTLTemperatureSet *message = [[SigLightCTLTemperatureSet alloc] initWithCTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightCTLTemperatureSetUnacknowledged *message = [[SigLightCTLTemperatureSetUnacknowledged alloc] initWithCTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLTemperatureStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightCTLDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLDefaultGet *message = [[SigLightCTLDefaultGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLDefaultSetWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightCTLDefaultSet *message = [[SigLightCTLDefaultSet alloc] initWithLightness:lightness temperature:temperature deltaUV:deltaUV];
        msg = message;
    } else {
        SigLightCTLDefaultSetUnacknowledged *message = [[SigLightCTLDefaultSetUnacknowledged alloc] initWithLightness:lightness temperature:temperature deltaUV:deltaUV];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightCTLTemperatureRangeSetWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightCTLTemperatureRangeSet *message = [[SigLightCTLTemperatureRangeSet alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        msg = message;
    } else {
        SigLightCTLTemperatureRangeSetUnacknowledged *message = [[SigLightCTLTemperatureRangeSetUnacknowledged alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_CTL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightCTLTemperatureRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLGet *message = [[SigLightHSLGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightHSLGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLGet *message = [[SigLightHSLGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLHueGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLHueGet *message = [[SigLightHSLHueGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_HUE_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLHueStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLHueSetWithHue:(UInt16)hue node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightHSLHueSetWithHue:hue transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightHSLHueSetWithHue:(UInt16)hue transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightHSLHueSet *message = [[SigLightHSLHueSet alloc] initWithHue:hue transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightHSLHueSetUnacknowledged *message = [[SigLightHSLHueSetUnacknowledged alloc] initWithHue:hue transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_HUE_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLHueStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLSaturationGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLSaturationGet *message = [[SigLightHSLSaturationGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_SAT_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLSaturationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLSaturationSetWithHue:(UInt16)saturation node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightHSLSaturationSetWithHue:saturation transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightHSLSaturationSetWithHue:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightHSLSaturationSet *message = [[SigLightHSLSaturationSet alloc] initWithSaturation:saturation transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightHSLSaturationSetUnacknowledged *message = [[SigLightHSLSaturationSetUnacknowledged alloc] initWithSaturation:saturation transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_SAT_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLSaturationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLSetWithHSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightHSLSetWithHSLLight:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightHSLSetWithHSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightHSLSet *message = [[SigLightHSLSet alloc] initWithHSLLightness:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightHSLSetUnacknowledged *message = [[SigLightHSLSetUnacknowledged alloc] initWithHSLLightness:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)lightHSLSetWithHSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightHSLSet *message = [[SigLightHSLSet alloc] initWithHSLLightness:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightHSLSetUnacknowledged *message = [[SigLightHSLSetUnacknowledged alloc] initWithHSLLightness:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLTargetGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLTargetGet *message = [[SigLightHSLTargetGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLTargetStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLDefaultGet *message = [[SigLightHSLDefaultGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLRangeGet *message = [[SigLightHSLRangeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLDefaultSetWithLight:(UInt16)light hue:(UInt16)hue saturation:(UInt16)saturation node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightHSLDefaultSet *message = [[SigLightHSLDefaultSet alloc] initWithLightness:light hue:hue saturation:saturation];
        msg = message;
    } else {
        SigLightHSLDefaultSetUnacknowledged *message = [[SigLightHSLDefaultSetUnacknowledged alloc] initWithLightness:light hue:hue saturation:saturation];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightHSLRangeSetWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightHSLRangeSet *message = [[SigLightHSLRangeSet alloc] initWithHueRangeMin:hueRangeMin hueRangeMax:hueRangeMax saturationRangeMin:saturationRangeMin saturationRangeMax:saturationRangeMax];
        msg = message;
    } else {
        SigLightHSLRangeSetUnacknowledged *message = [[SigLightHSLRangeSetUnacknowledged alloc] initWithHueRangeMin:hueRangeMin hueRangeMax:hueRangeMax saturationRangeMin:saturationRangeMin saturationRangeMax:saturationRangeMax];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_HSL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightHSLRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLGet *message = [[SigLightXyLGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLSetWithXyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightXyLSetWithXyLLightness:xyLLightness xyLx:xyLx xyLy:xyLy transitionTime:nil delay:0 node:node resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightXyLSetWithXyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightXyLSet *message = [[SigLightXyLSet alloc] initWithXyLLightness:xyLLightness xyLX:xyLx xyLY:xyLy transitionTime:transitionTime delay:delay];
        msg = message;
    } else {
        SigLightXyLSetUnacknowledged *message = [[SigLightXyLSetUnacknowledged alloc] initWithXyLLightness:xyLLightness xyLX:xyLx xyLY:xyLy transitionTime:transitionTime delay:delay];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLTargetGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLTargetGet *message = [[SigLightXyLTargetGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLTargetStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLDefaultGet *message = [[SigLightXyLDefaultGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLRangeGet *message = [[SigLightXyLRangeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLDefaultSetWithLightness:(UInt16)lightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightXyLDefaultSet *message = [[SigLightXyLDefaultSet alloc] initWithLightness:lightness xyLX:xyLx xyLY:xyLy];
        msg = message;
    } else {
        SigLightXyLDefaultSetUnacknowledged *message = [[SigLightXyLDefaultSetUnacknowledged alloc] initWithLightness:lightness xyLX:xyLx xyLY:xyLy];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLDefaultStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightXyLRangeSetWithXyLxRangeMin:(UInt16)xyLxRangeMin xyLxRangeMax:(UInt16)xyLxRangeMax xyLyRangeMin:(UInt16)xyLyRangeMin xyLyRangeMax:(UInt16)xyLyRangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightXyLRangeSet *message = [[SigLightXyLRangeSet alloc] initWithXyLXRangeMin:xyLxRangeMin xyLXRangeMax:xyLxRangeMax xyLYRangeMin:xyLyRangeMin xyLYRangeMax:xyLyRangeMax];
        msg = message;
    } else {
        SigLightXyLRangeSetUnacknowledged *message = [[SigLightXyLRangeSetUnacknowledged alloc] initWithXyLXRangeMin:xyLxRangeMin xyLXRangeMax:xyLxRangeMax xyLYRangeMin:xyLyRangeMin xyLYRangeMax:xyLyRangeMax];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_XYL_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightXyLRangeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCModeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCModeGet *message = [[SigLightLCModeGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCModeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCModeSetWithModeEnable:(BOOL)enable node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLCModeSet *message = [[SigLightLCModeSet alloc] initWithMode:enable?0x01:0x00];
        msg = message;
    } else {
        SigLightLCModeSetUnacknowledged *message = [[SigLightLCModeSetUnacknowledged alloc] initWithMode:enable?0x01:0x00];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCModeStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCOMGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCOMGet *message = [[SigLightLCOMGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCOMStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCOMSetWithModeEnable:(BOOL)enable node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLCOMSet *message = [[SigLightLCOMSet alloc] initWithMode:enable?0x01:0x00];
        msg = message;
    } else {
        SigLightLCOMSetUnacknowledged *message = [[SigLightLCOMSetUnacknowledged alloc] initWithMode:enable?0x01:0x00];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCOMStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCLightOnOffGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCLightOnOffGet *message = [[SigLightLCLightOnOffGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCLightOnOffStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCLightOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLCLightOnOffSet:isOn withNode:node transitionTime:nil delay:0 resMax:resMax ack:ack successCallback:successCallback resultCallback:resultCallback];
}

+ (SigMessageHandle *)lightLCLightOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLCLightOnOffSet *message = [[SigLightLCLightOnOffSet alloc] init];
        message.lightOnOff = isOn;
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
    } else {
        SigLightLCLightOnOffSetUnacknowledged *message = [[SigLightLCLightOnOffSetUnacknowledged alloc] init];
        message.lightOnOff = isOn;
        message.transitionTime = transitionTime;
        message.delay = delay;
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCLightOnOffStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCPropertyGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCPropertyGet *message = [[SigLightLCPropertyGet alloc] init];
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCPropertyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

+ (SigMessageHandle *)lightLCPropertySetWithPropertyID:(UInt16)propertyID propertyValue:(NSData *)propertyValue withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *msg;
    if (ack) {
        SigLightLCPropertySet *message = [[SigLightLCPropertySet alloc] initWithLightLCPropertyID:propertyID lightLCPropertyValue:propertyValue];
        msg = message;
    } else {
        SigLightLCPropertySetUnacknowledged *message = [[SigLightLCPropertySetUnacknowledged alloc] initWithLightLCPropertyID:propertyID lightLCPropertyValue:propertyValue];
        msg = message;
        if (successCallback != nil || resultCallback != nil) {
            TeLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
    }
//    SigModelIDModel *model = [node getModelIDModelWithModelID:SIG_MD_LIGHT_LC_SETUP_S];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = msg;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseLightLCPropertyStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
        return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:node.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];

//    return [SigMeshLib.share sendSigMeshMessage:message fromLocalElement:nil toModelIDModel:model command:command];
}

/// Sets the Filter Type on the connected GATT Proxy Node.
/// The filter will be emptied.
///
/// - parameter type: The new proxy filter type.
+ (void)setType:(SigProxyFilerType)type successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigSetFilterType *message = [[SigSetFilterType alloc] initWithType:type];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    command.timeout = kSDKLibCommandTimeout;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/// Resets the filter to an empty whitelist filter.
+ (void)resetFilterWithSuccessCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigSetFilterType *message = [[SigSetFilterType alloc] initWithType:SigProxyFilerType_whitelist];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/// Adds the given Address to the active filter.
///
/// - parameter address: The address to add to the filter.
+ (void)addAddressToFilterWithAddress:(UInt16)address successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    [self addAddressesToFilterWithAddresses:@[@(address)] successCallback:successCallback failCallback:failCallback];
}

/// Adds the given Addresses to the active filter.
///
/// - parameter addresses: The addresses to add to the filter.
+ (void)addAddressesToFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigAddAddressesToFilter *message = [[SigAddAddressesToFilter alloc] initWithAddresses:addresses];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    command.timeout = kSDKLibCommandTimeout;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/// Removes the given Address from the active filter.
///
/// - parameter address: The address to remove from the filter.
+ (void)removeAddressFromFilterWithAddress:(NSNumber *)address successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    [self removeAddressesFromFilterWithAddresses:@[address] successCallback:successCallback failCallback:failCallback];
}

/// Removes the given Addresses from the active filter.
///
/// - parameter addresses: The addresses to remove from the filter.
+ (void)removeAddressesFromFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigRemoveAddressesFromFilter *message = [[SigRemoveAddressesFromFilter alloc] initWithAddresses:addresses];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/// Adds all the addresses the Provisioner is subscribed to to the
/// Proxy Filter.
+ (void)setFilterForProvisioner:(SigProvisionerModel *)provisioner successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)failCallback {
    SigNodeModel *node = provisioner.node;
    if (!node) {
        TeLogError(@"provisioner.node = nil.");
        return;
    }
    NSMutableArray *addresses = [NSMutableArray array];
    for (SigElementModel *element in node.elements) {
        element.parentNode = node;
        // Add Unicast Addresses of all Elements of the Provisioner's Node.
        [addresses addObject:@(element.unicastAddress)];
        // Add all addresses that the Node's Models are subscribed to.
        for (SigModelIDModel *model in element.models) {
            if (model.subscribe && model.subscribe.count > 0) {
                for (NSString *addr in model.subscribe) {
                    UInt16 indAddr = [LibTools uint16From16String:addr];
                    [addresses addObject:@(indAddr)];
                }
            }
        }
    }

    // Add All Nodes group address.
    [addresses addObject:@(kMeshAddress_allNodes)];
    TeLogVerbose(@"filter addresses:%@",addresses);
    // Submit.
    __weak typeof(self) weakSelf = self;
    [self setType:SigProxyFilerType_whitelist successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
//        //逻辑1.for循环每次只添加一个地址
//        NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
//        [oprationQueue addOperationWithBlock:^{
//            //这个block语句块在子线程中执行
//            NSLog(@"oprationQueue");
//            __block BOOL hasFail = NO;
//            __block NSError *err = nil;
//            for (NSNumber *num in addresses) {
//                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//                [weakSelf addAddressesToFilterWithAddresses:@[num] successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
//                    TeLogInfo(@"responseMessage:.filterType=%d, .listSize=%d",responseMessage.filterType,responseMessage.listSize);
//                    dispatch_semaphore_signal(semaphore);
//                } failCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                    hasFail = YES;
//                    err = error;
//                    dispatch_semaphore_signal(semaphore);
//                }];
//                dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
//                if (hasFail) {
//                    break;
//                }
//            }
//            if (hasFail) {
//                if (failCallback) {
//                    failCallback(NO,err);
//                }
//            } else {
//                [weakSelf sendSecureNetworkBeacon];
//                if (successCallback) {
//                    successCallback(source,destination,responseMessage);
//                }
//            }
//        }];
        
        //逻辑2.一次添加多个地址
        [weakSelf addAddressesToFilterWithAddresses:addresses successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
            TeLogVerbose(@"responseMessage.listSize=%d",responseMessage.listSize);
            SigDataSource.share.unicastAddressOfConnected = source;
            [weakSelf sendSecureNetworkBeacon];
            if (successCallback) {
                successCallback(source,destination,responseMessage);
            }
        } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            if (failCallback) {
                failCallback(error==nil,error);
            }
        }];
    } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (failCallback) {
            failCallback(error==nil,error);
        }
    }];
}

#pragma mark - Mesh Firmware update (Mesh OTA)

/* Firmware Update Messages */

/*
 ---> Sending - Access PDU, source:(0001)->destination: (0002) Op Code: (0xB601), accessPdu=B6010001
 <--- Response - Access PDU, source:(0002)->destination: (0001) Op Code: (0xB602), accessPdu=B60201000611020100333100
 */
+ (SigMessageHandle *)firmwareUpdateInformationGetWithDestination:(UInt16)destination firstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateInformationGet *message = [[SigFirmwareUpdateInformationGet alloc] initWithFirstIndex:firstIndex entriesLimit:entriesLimit];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareInformationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0002) Op Code: (0xB603), accessPdu=B6030000000000
<--- Response - Access PDU, source:(0002)->destination: (0001) Op Code: (0xB604), accessPdu=B6040000
*/
+ (SigMessageHandle *)firmwareUpdateFirmwareMetadataCheckWithDestination:(UInt16)destination updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateFirmwareMetadataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateFirmwareMetadataCheck *message = [[SigFirmwareUpdateFirmwareMetadataCheck alloc] initWithUpdateFirmwareImageIndex:updateFirmwareImageIndex incomingFirmwareMetadata:incomingFirmwareMetadata];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareUpdateFirmwareMetadataStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB605), accessPdu=B605
<--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000B609), accessPdu=B6090002ff00001122334455667788110201003331
*/
+ (SigMessageHandle *)firmwareUpdateGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateGet *message = [[SigFirmwareUpdateGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

#warning 2020年04月16日09:31:37，已经修改，待更新log
/*
---> Sending - Access PDU, source:(0001)->destination: (0002) Op Code: (0xB606), accessPdu=B606FF000011223344556677880000000000
<--- Response - Access PDU, source:(0002)->destination: (0001) Op Code: (0xB609), accessPdu=B60920FF000000112233445566778800
*/
+ (SigMessageHandle *)firmwareUpdateStartWithDestination:(UInt16)destination updateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateStart *message = [[SigFirmwareUpdateStart alloc] initWithUpdateTTL:updateTTL updateTimeoutBase:updateTimeoutBase updateBLOBID:updateBLOBID updateFirmwareImageIndex:updateFirmwareImageIndex incomingFirmwareMetadata:incomingFirmwareMetadata];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)firmwareUpdateCancelWithDestination:(UInt16)destination companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateCancel *message = [[SigFirmwareUpdateCancel alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB608), accessPdu=B608
<--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000B609), accessPdu=B6090000ff00001122334455667788110201003331
*/
+ (SigMessageHandle *)firmwareUpdateApplyWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateApply *message = [[SigFirmwareUpdateApply alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)firmwareDistributionGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionGet *message = [[SigFirmwareDistributionGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)firmwareDistributionStartWithDestination:(UInt16)destination distributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(BOOL)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionStart *message = [[SigFirmwareDistributionStart alloc] initWithDistributionAppKeyIndex:distributionAppKeyIndex distributionTTL:distributionTTL distributionTimeoutBase:distributionTimeoutBase distributionTransferMode:distributionTransferMode updatePolicy:updatePolicy RFU:RFU distributionFirmwareImageIndex:distributionFirmwareImageIndex distributionMulticastAddress:distributionMulticastAddress];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0001) Op Code: (0xB60C), accessPdu=B60C
<--- Response - 
*/
+ (SigMessageHandle *)firmwareDistributionCancelWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionCancel *message = [[SigFirmwareDistributionCancel alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

//+ (SigMessageHandle *)firmwareDistributionDetailsGetWithDestination:(UInt16)destination status:(SigFirmwareDistributionStatusType)status companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionDetailsListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
//    SigFirmwareDistributionDetailsGet *message = [[SigFirmwareDistributionDetailsGet alloc] initWithStatus:status companyID:companyID firmwareID:firmwareID];
//    SDKLibCommand *command = [[SDKLibCommand alloc] init];
//    command.curMeshMessage = message;
//    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
//    command.responseFirmwareDistributionDetailsListCallBack = successCallback;
//    command.resultCallback = resultCallback;
//    command.responseMaxCount = resMax;
//    command.retryCount = retryCount;
//    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
//}

/* BLOB Transfer Messages */

+ (SigMessageHandle *)BLOBTransferGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBTransferGet *message = [[SigBLOBTransferGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBLOBTransferStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU,  source:(0001)->destination: (0002) Op Code: (0xB702), accessPdu=B702401122334455667788C41100000C7C01
<--- Response - Access PDU, source:(0002)->destination: (0001) Op Code: (0xB704), accessPdu=B7044001
*/
+ (SigMessageHandle *)BLOBTransferStartWithDestination:(UInt16)destination transferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBTransferStart *message = [[SigBLOBTransferStart alloc] initWithTransferMode:transferMode BLOBID:BLOBID BLOBSize:BLOBSize BLOBBlockSizeLog:BLOBBlockSizeLog MTUSize:MTUSize];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBLOBTransferStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)BLOBTransferAbortWithDestination:(UInt16)destination BLOBID:(UInt64)BLOBID resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigObjectTransferCancel *message = [[SigObjectTransferCancel alloc] initWithBLOBID:BLOBID];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBLOBTransferStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB705), accessPdu=B70500000001
<--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000007E), accessPdu=7E0000000001
*/
+ (SigMessageHandle *)BLOBBlockStartWithBlockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize toDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBBlockStart *message = [[SigBLOBBlockStart alloc] initWithBlockNumber:blockNumber chunkSize:chunkSize];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBLOBBlockStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)BLOBChunkTransferWithDestination:(UInt16)destination chunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount resultCallback:(resultBlock)resultCallback {
    SigBLOBChunkTransfer *message = [[SigBLOBChunkTransfer alloc] initWithChunkNumber:chunkNumber chunkData:chunkData];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    command.timeout = 10.0;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB707), accessPdu=B707
<--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000007E), accessPdu=7E4000000001
*/
+ (SigMessageHandle *)BLOBBlockGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBBlockGet *message = [[SigBLOBBlockGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBLOBBlockStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

/*
---> Sending - Access PDU, source:(0001)->destination: (0002) Op Code: (0xB70A), accessPdu=B70A
<--- Response - Access PDU, Access PDU, source:(0002)->destination: (0001) Op Code: (0xB70B), accessPdu=B70B0C0C10000001000003007C0101
*/
+ (SigMessageHandle *)BLOBInformationGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBInformationGet *message = [[SigBLOBInformationGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseBLOBInformationStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

#pragma mark - Remote Provision

+ (SigMessageHandle *)remoteProvisioningScanCapabilitiesGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanCapabilitiesGet *message = [[SigRemoteProvisioningScanCapabilitiesGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanCapabilitiesStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningScanGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanGet *message = [[SigRemoteProvisioningScanGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningScanStartWithScannedItemsLimit:(UInt8)scannedItemsLimit timeout:(UInt8)timeout UUID:(nullable NSData *)UUID destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanStart *message = [[SigRemoteProvisioningScanStart alloc] initWithScannedItemsLimit:scannedItemsLimit timeout:timeout UUID:UUID];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningScanStopWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanStop *message = [[SigRemoteProvisioningScanStop alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningExtendedScanStartWithADTypeFilterCount:(UInt8)ADTypeFilterCount ADTypeFilter:(nullable NSData *)ADTypeFilter UUID:(nullable NSData *)UUID timeout:(UInt8)timeout destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningExtendedScanStart *message = [[SigRemoteProvisioningExtendedScanStart alloc] initWithADTypeFilterCount:ADTypeFilterCount ADTypeFilter:ADTypeFilter UUID:UUID timeout:timeout];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningLinkGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningLinkGet *message = [[SigRemoteProvisioningLinkGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningLinkStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningLinkOpenWithUUID:(nullable NSData *)UUID destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningLinkOpen *message = [[SigRemoteProvisioningLinkOpen alloc] initWithUUID:UUID];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningLinkStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningLinkCloseWithReason:(SigRemoteProvisioningLinkCloseStatus)reason destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningLinkClose *message = [[SigRemoteProvisioningLinkClose alloc] initWithReason:reason];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningLinkStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}

+ (SigMessageHandle *)remoteProvisioningPDUSendWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningPDUSend *message = [[SigRemoteProvisioningPDUSend alloc] initWithOutboundPDUNumber:outboundPDUNumber provisioningPDU:provisioningPDU];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.resultCallback = resultCallback;
    command.responseMaxCount = resMax;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
}



#pragma mark - API by Telink

/// Get Online device, private use OnlineStatusCharacteristic(获取当前mesh网络的所有设备的在线、开关、亮度、色温状态（私有定制，需要特定的OnlineStatusCharacteristic）)

+ (nullable NSError *)telinkApiGetOnlineStatueFromUUIDWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericOnOffGet *message = [[SigGenericOnOffGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseOnOffStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    return [SigMeshLib.share sendTelinkApiGetOnlineStatueFromUUIDWithMessage:message command:command];
}

+ (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete {
    [SigBluetooth.share readOTACharachteristicWithTimeout:timeout complete:complete];
}

+ (void)cancelReadOTACharachteristic {
    [SigBluetooth.share cancelReadOTACharachteristic];
}

+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    #warning 2019年12月20日09:51:19，待完善ini接口
    NSError *err = nil;
    UInt16 op = model.opcode;
    if (model.opcode > 0xff) {
        op = ((model.opcode & 0xff) << 8) | (model.opcode >> 8);
    }
    if (model.vendorId > 0 && model.responseOpcode == 0) {
        model.retryCount = 0;
        TeLogWarn(@"change retryCount to 0.");
    }
    BOOL reliable = [self isReliableCommandWithOpcode:op vendorOpcodeResponse:model.responseOpcode];
    if (SigMeshLib.share.isBusyNow) {
        err = [NSError errorWithDomain:@"Telink sig mesh SDK is busy now." code:-1 userInfo:nil];
        TeLogError(@"tx cmd busy!........................");
        return err;
    }
    if ([SigHelper.share isUnicastAddress:model.address] && reliable && model.responseMax > 1) {
        model.responseMax = 1;
        TeLogWarn(@"change responseMax to 1.");
    }
    SigIniMeshMessage *message = [[SigIniMeshMessage alloc] initWithParameters:model.commandData];
    if (model.vendorId) {
        message.opCode = (op << 16) | ((model.vendorId & 0xff) << 8) | (model.vendorId >> 8);
    } else {
        message.opCode = op;
    }
    if (model.responseOpcode) {
        if (model.vendorId) {
            message.responseOpCode = (model.responseOpcode << 16) | ((model.vendorId & 0xff) << 8) | (model.vendorId >> 8);
        } else {
            message.responseOpCode = model.responseOpcode;
        }
    } else {
        message.responseOpCode = [SigHelper.share getResponseOpcodeWithSendOpcode:op];
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = model.responseMax;
    command.retryCount = model.retryCount;
    command.netkeyA = model.netkeyA;
    command.appkeyA = model.appkeyA;
    command.ivIndexA = model.ivIndexA;
    command.hadRetryCount = 0;
    if (model.timeout) {
        command.timeout = model.timeout;
    }
    if (model.appkeyA) {
        [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:model.address] usingApplicationKey:model.appkeyA command:command];
    } else {
        [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:model.address] usingApplicationKey:SigDataSource.share.curAppkeyModel command:command];
    }
    return err;
}

+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    IniCommandModel *model = [[IniCommandModel alloc] initWithIniCommandData:iniData];
    model.netkeyA = SigDataSource.share.curNetkeyModel;
    model.appkeyA = SigDataSource.share.curAppkeyModel;
    model.ivIndexA = SigDataSource.share.curNetkeyModel.ivIndex;
    return [self sendIniCommandModel:model successCallback:successCallback resultCallback:resultCallback];
}

+ (void)sendOpByINI:(UInt8 *)iniBuf length:(UInt32)length successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    [self sendOpINIData:[NSData dataWithBytes:iniBuf length:length] successCallback:successCallback resultCallback:resultCallback];
}

+ (BOOL)isReliableCommandWithOpcode:(UInt16)opcode vendorOpcodeResponse:(UInt32)vendorOpcodeResponse {
    if (IS_VENDOR_OP(opcode)) {
        return ((vendorOpcodeResponse & 0xff) != 0);
    } else {
        int responseOpCode = [SigHelper.share getResponseOpcodeWithSendOpcode:opcode];
        return responseOpCode != 0;
    }
}

+ (void)startMeshSDK {
    ///初始化本地存储的mesh网络数据
    [SigDataSource.share configData];

    //初始化ECC算法的公钥
    [SigEncryptionHelper.share eccInit];
    
    //初始化添加设备的参数
    [SigAddDeviceManager.share setNeedDisconnectBetweenProvisionToKeyBind:NO];
    
    ///初始化蓝牙
    [[SigBluetooth share] bleInit:^(CBCentralManager * _Nonnull central) {
        TeLogInfo(@"finish init SigBluetooth.");
        [SigMeshLib share];
        [SigMeshLib.share setNetworkManager:SigNetworkManager.share];
        [SigMeshLib.share setAcknowledgmentMessageInterval:5.0];
        [SigMeshLib.share setAcknowledgmentMessageTimeout:40.0];
    }];
    
//    ///默认为NO，连接速度更加快。设置为YES，表示扫描到的设备必须包含MacAddress，有些客户在添加流程需要通过MacAddress获取三元组信息。
//    [SigBluetooth.share setWaitScanRseponseEnabel:YES];
}

+ (BOOL)isBLEInitFinish {
    return [SigBluetooth.share isBLEInitFinish];
}

+ (void)sendSecureNetworkBeacon {
    SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:NO networkId:SigDataSource.share.curNetkeyModel.networkId ivIndex:[LibTools uint32From16String:SigDataSource.share.ivIndex] usingNetworkKey:SigDataSource.share.curNetkeyModel];
    //==========test=========//
    TeLogVerbose(@"==========ivIndex=0x%x",(unsigned int)[LibTools uint32From16String:SigDataSource.share.ivIndex]);
    //==========test=========//
    TeLogInfo(@"send SecureNetworkBeacon=%@",[LibTools convertDataToHexStr:beacon.pduData]);
    [SigBearer.share sendBlePdu:beacon ofType:SigPduType_meshBeacon];
}

+ (void)statusNowTime {
    //time_auth = 1;//每次无条件接受这个时间指令。
    UInt64 seconds = (UInt64)[LibTools secondsFrome2000];
    [NSTimeZone resetSystemTimeZone]; // 重置手机系统的时区
    NSInteger offset = [NSTimeZone localTimeZone].secondsFromGMT;
    UInt8 zoneOffset = (UInt8)(offset/60/15+64);//时区=分/15+64
//    TeLogInfo(@"设置秒数：%llu，时区：%d",seconds,zoneOffset);
    SigTimeModel *timeModel = [[SigTimeModel alloc] initWithTAISeconds:seconds subSeconds:0 uncertainty:0 timeAuthority:1 TAI_UTC_Delta:0 timeZoneOffset:zoneOffset];
    [self timeStatusWithSigTimeModel:timeModel toDestination:kMeshAddress_allNodes resMax:0 successCallback:nil resultCallback:nil];
}

+ (void)publishNodeTimeModelWithNodeAddress:(UInt16)address successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    //publish time model
    UInt32 option = SIG_MD_TIME_S;
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node == nil) {
        TeLogError(@"there has not a node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return;
    }
    NSArray *elementAddresses = [node getAddressesWithModelID:@(option)];
    if (elementAddresses.count > 0) {
        TeLog(@"SDK need publish time");
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            UInt16 eleAdr = [elementAddresses.firstObject intValue];
            //周期，20秒上报一次。ttl:0xff（表示采用节点默认参数）。
            SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:2 intervalSteps:5];
            SigPublish *publish = [[SigPublish alloc] initWithDestination:kMeshAddress_allNodes withKeyIndex:SigDataSource.share.curAppkeyModel.index friendshipCredentialsFlag:0 ttl:0xff periodSteps:kTimePublishInterval periodResolution:1 retransmit:retransmit];
            SigModelIDModel *modelID = [node getModelIDModelWithModelID:option andElementAddress:eleAdr];
            [SDKLibCommand configModelPublicationSetWithSigPublish:publish modelIDModel:modelID successCallback:successCallback resultCallback:resultCallback] ;
        });
    }else{
        TeLogError(@"there has not a time model of node that address is 0x%x",address);
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"there has not a time model of node that address is 0x%x",address] code:0 userInfo:nil];
            resultCallback(NO,error);
        }
        return;
    }
}

/// Add Single Device (provision+keyBind)
/// @param configModel all config message of add device.
/// @param provisionSuccess callback when provision success.
/// @param provisionFail callback when provision fail.
/// @param keyBindSuccess callback when keybind success.
/// @param keyBindFail callback when keybind fail.
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    [SigAddDeviceManager.share startAddDeviceWithSigAddConfigModel:configModel provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

/// provision
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    if (provisionType == ProvisionTpye_NoOOB) {
        TeLogVerbose(@"start noOob provision.");
        [SigProvisioningManager.share provisionWithUnicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex provisionSuccess:provisionSuccess fail:fail];
    } else if (provisionType == ProvisionTpye_StaticOOB) {
        TeLogVerbose(@"start staticOob provision.");
        [SigProvisioningManager.share provisionWithUnicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex staticOobData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
    } else {
        TeLogError(@"unsupport provision type.");
    }
}

/// keybind
/// @param peripheral CBPeripheral of CoreBluetooth will be keybind.
/// @param unicastAddress address of new device.
/// @param appkey appkey
/// @param appkeyIndex appkeyIndex
/// @param netkeyIndex netkeyIndex
/// @param keyBindType KeyBindTpye_Normal means add appkey and model bind, KeyBindTpye_Fast means just add appkey.
/// @param productID the productID info need to save in node when keyBindType is KeyBindTpye_Fast.
/// @param cpsData the elements info need to save in node when keyBindType is KeyBindTpye_Fast.
/// @param keyBindSuccess callback when keybind success.
/// @param fail callback when provision fail.
- (void)startKeyBindWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex netkeyIndex:(UInt16)netkeyIndex keyBindType:(KeyBindTpye)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail {
    [SigBearer.share connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
        if (successful) {
            SigAppkeyModel *appkeyModel = [SigDataSource.share getAppkeyModelWithAppkeyIndex:appkeyIndex];
            if (!appkeyModel || ![appkeyModel.getDataKey isEqualToData:appkey] || netkeyIndex != appkeyModel.boundNetKey) {
                TeLogVerbose(@"appkey is error.");
                if (fail) {
                    NSError *err = [NSError errorWithDomain:@"appkey is error." code:-1 userInfo:nil];
                    fail(err);
                }
                return;
            }
            [SigKeyBindManager.share keyBind:unicastAddress appkeyModel:appkeyModel keyBindType:keyBindType productID:productID cpsData:cpsData keyBindSuccess:keyBindSuccess fail:fail];
        } else {
            if (fail) {
                NSError *err = [NSError errorWithDomain:@"connect fail." code:-1 userInfo:nil];
                fail(err);
            }
        }
    }];
}

@end
