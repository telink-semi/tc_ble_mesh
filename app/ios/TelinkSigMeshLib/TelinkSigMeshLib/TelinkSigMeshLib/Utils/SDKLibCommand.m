/********************************************************************************************************
 * @file     SDKLibCommand.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/4
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

#import "SDKLibCommand.h"
#import "SigECCEncryptHelper.h"
#import "SigKeyBindManager.h"

@implementation SDKLibCommand

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self setDefaultCommandParameter];
    }
    return self;
}

/**
 * @brief   Initialize SDKLibCommand object.
 * @param   message    the message of command.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   responseAllMessageCallBack    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  return `nil` when initialize SigScanRspModel object fail.
 */
- (instancetype)initWithMessage:(SigBaseMeshMessage *)message retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock _Nullable)responseAllMessageCallBack resultCallback:(resultBlock)resultCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self setDefaultCommandParameter];
        _curMeshMessage = message;
        _retryCount = retryCount;
        _responseMaxCount = responseMaxCount;
        _responseAllMessageCallBack = (responseAllMessageBlock)responseAllMessageCallBack;
        _resultCallback = resultCallback;
    }
    return self;
}

- (void)setDefaultCommandParameter {
    _retryCount = kAcknowledgeMessageDefaultRetryCount;
    _responseSourceArray = [NSMutableArray array];
    _timeout = SigDataSource.share.defaultReliableIntervalOfNotLPN;
    _hadRetryCount = 0;
    _hadReceiveAllResponse = NO;
    _tidPosition = 0;
    _tid = 0;
    _curNetkey = SigMeshLib.share.dataSource.curNetkeyModel;
    _curAppkey = SigMeshLib.share.dataSource.curAppkeyModel;
    _curIvIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
    _sendBySegmentPDU = NO;
    _unsegmentedMessageLowerTransportPDUMaxLength = SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength;
}

#pragma mark - 4.3.2 Configuration messages


/**
 * @brief   Config AppKey Add.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.37 Config AppKey Add, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configAppKeyAddWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigAppKeyAdd *config = [[SigConfigAppKeyAdd alloc] initWithApplicationKey:appkeyModel];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseAppKeyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config AppKey Add.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   sendBySegmentPdu    Whether to use segment to send this command.
 * @param   unsegmentedMessageLowerTransportPDUMaxLength    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.37 Config AppKey Add, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configAppKeyAddWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel sendBySegmentPdu:(BOOL)sendBySegmentPdu unsegmentedMessageLowerTransportPDUMaxLength:(UInt16)unsegmentedMessageLowerTransportPDUMaxLength retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigAppKeyAdd *config = [[SigConfigAppKeyAdd alloc] initWithApplicationKey:appkeyModel];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseAppKeyStatusCallBack = successCallback;
    command.sendBySegmentPDU = sendBySegmentPdu;
    command.unsegmentedMessageLowerTransportPDUMaxLength = unsegmentedMessageLowerTransportPDUMaxLength;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config AppKey Update.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.38 Config AppKey Update, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configAppKeyUpdateWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigAppKeyUpdate *config = [[SigConfigAppKeyUpdate alloc] initWithApplicationKey:appkeyModel];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseAppKeyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config AppKey Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.39 Config AppKey Delete , seeAlso: Mesh_v1.0.pdf  (page.170)
 */
+ (SigMessageHandle *)configAppKeyDeleteWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigAppKeyDelete *config = [[SigConfigAppKeyDelete alloc] initWithApplicationKey:appkeyModel];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseAppKeyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config AppKey Get.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    the networkKeyIndex of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.41 Config AppKey Get , seeAlso: Mesh_v1.0.pdf  (page.171)
 */
+ (SigMessageHandle *)configAppKeyGetWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigAppKeyGet *config = [[SigConfigAppKeyGet alloc] initWithNetworkKeyIndex:networkKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseAppKeyListCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Beacon Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.1 Config Beacon Get , seeAlso: Mesh_v1.0.pdf  (page.154)
 */
+ (SigMessageHandle *)configBeaconGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigBeaconGet *config = [[SigConfigBeaconGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBeaconStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Beacon Set.
 * @param   destination    the unicastAddress of destination.
 * @param   secureNetworkBeaconState    New Secure Network Beacon state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.2 Config Beacon Set , seeAlso: Mesh_v1.0.pdf  (page.154)
 */
+ (SigMessageHandle *)configBeaconSetWithDestination:(UInt16)destination secureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigBeaconSet *config = [[SigConfigBeaconSet alloc] initWithEnable:secureNetworkBeaconState];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBeaconStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Composition Data Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.4 Config Composition Data Get, seeAlso: Mesh_v1.0.pdf  (page.154)
 */
+ (SigMessageHandle *)configCompositionDataGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigCompositionDataGet *config = [[SigConfigCompositionDataGet alloc] initWithPage:0xFF];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseCompositionDataStatusCallBack = successCallback;
//    command.timeout = 3.0;//GATT-LPN固件返回SigConfigCompositionData比较慢，当前接口设置超时为3秒。
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Default TTL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.6 Config Default TTL Get, seeAlso: Mesh_v1.0.pdf  (page.155)
 */
+ (SigMessageHandle *)configDefaultTtlGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigDefaultTtlGet *config = [[SigConfigDefaultTtlGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseDefaultTtlStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Default TTL Set.
 * @param   destination    the unicastAddress of destination.
 * @param   ttl    New Default TTL value.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.7 Config Default TTL Set, seeAlso: Mesh_v1.0.pdf  (page.155)
 */
+ (SigMessageHandle *)configDefaultTtlSetWithDestination:(UInt16)destination ttl:(UInt8)ttl retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigDefaultTtlSet *config = [[SigConfigDefaultTtlSet alloc] initWithTtl:ttl];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseDefaultTtlStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Friend Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.55 Config Friend Get, seeAlso: Mesh_v1.0.pdf  (page.176)
 */
+ (SigMessageHandle *)configFriendGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigFriendGet *config = [[SigConfigFriendGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFriendStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Friend Set.
 * @param   destination    the unicastAddress of destination.
 * @param   nodeFeaturesState    New Friend state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.56 Config Friend Set, seeAlso: Mesh_v1.0.pdf  (page.176)
 */
+ (SigMessageHandle *)configFriendSetWithDestination:(UInt16)destination nodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigFriendSet *config = [[SigConfigFriendSet alloc] initWithEnable:nodeFeaturesState];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFriendStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config GATT Proxy Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.9 Config GATT Proxy Get, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configGATTProxyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigGATTProxyGet *config = [[SigConfigGATTProxyGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseGATTProxyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config GATT Proxy Set.
 * @param   destination    the unicastAddress of destination.
 * @param   nodeGATTProxyState    New GATT Proxy state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.10 Config GATT Proxy Set, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configGATTProxySetWithDestination:(UInt16)destination nodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigGATTProxySet *config = [[SigConfigGATTProxySet alloc] initWithEnable:nodeGATTProxyState];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseGATTProxyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Key Refresh Phase Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.58 Config Key Refresh Phase Get, seeAlso: Mesh_v1.0.pdf  (page.177)
 */
+ (SigMessageHandle *)configKeyRefreshPhaseGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigKeyRefreshPhaseStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigKeyRefreshPhaseGet *config = [[SigConfigKeyRefreshPhaseGet alloc] initWithNetKeyIndex:netKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseKeyRefreshPhaseStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Key Refresh Phase Set.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex    NetKey Index.
 * @param   transition    New Key Refresh Phase Transition.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.59 Config Key Refresh Phase Set, seeAlso: Mesh_v1.0.pdf  (page.177)
 */
+ (SigMessageHandle *)configKeyRefreshPhaseSetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex transition:(SigControllableKeyRefreshTransitionValues)transition retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigKeyRefreshPhaseStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigKeyRefreshPhaseSet *config = [[SigConfigKeyRefreshPhaseSet alloc] initWithNetKeyIndex:netKeyIndex transition:transition];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseKeyRefreshPhaseStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Publication Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.15 Config Model Publication Get, seeAlso: Mesh_v1.0.pdf  (page.157)
 */
+ (SigMessageHandle *)configModelPublicationGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelPublicationGet *config = [[SigConfigModelPublicationGet alloc] initWithElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelPublicationStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Publication Set.
 * @param   destination    the unicastAddress of destination.
 * @param   publish    new publosh config value of node.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.16 Config Model Publication Set, seeAlso: Mesh_v1.0.pdf  (page.158)
 */
+ (SigMessageHandle *)configModelPublicationSetWithDestination:(UInt16)destination publish:(SigPublish *)publish elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelPublicationSet *config = [[SigConfigModelPublicationSet alloc] initWithPublish:publish toElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelPublicationStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Publication Virtual Address Set.
 * @param   destination    the unicastAddress of destination.
 * @param   publish    new publosh config value of node.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.17 Config Model Publication Virtual Address Set, seeAlso: Mesh_v1.0.pdf  (page.159)
 */
+ (SigMessageHandle *)configModelPublicationVirtualAddressSetWithDestination:(UInt16)destination publish:(SigPublish *)publish elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelPublicationVirtualAddressSet *config = [[SigConfigModelPublicationVirtualAddressSet alloc] initWithPublish:publish toElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelPublicationStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/*
 sample data
---> Sending - Access PDU, source:(0001)->destination: (0008) Op Code: (0000801B), parameters: (080000C000FF), accessPdu=801B080000C000FF
<--- Response - Access PDU, source:(0008)->destination: (0001) Op Code: (0000801F), parameters: (00080000C000FF), accessPdu=801F00080000C000FF received (decrypted using key: <SigDeviceKeySet: 0x282840300>)
*/
/**
 * @brief   Config Model Subscription Add.
 * @param   destination    the unicastAddress of destination.
 * @param   groupAddress    Value of the address.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.19 Config Model Subscription Add, seeAlso: Mesh_v1.0.pdf  (page.161)
 */
+ (SigMessageHandle *)configModelSubscriptionAddWithDestination:(UInt16)destination toGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionAdd *config = [[SigConfigModelSubscriptionAdd alloc] initWithGroupAddress:groupAddress toElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Subscription Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   groupAddress    Value of the address.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.21 Config Model Subscription Delete, seeAlso: Mesh_v1.0.pdf  (page.162)
 */
+ (SigMessageHandle *)configModelSubscriptionDeleteWithDestination:(UInt16)destination groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionDelete *config = [[SigConfigModelSubscriptionDelete alloc] initWithGroupAddress:groupAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Subscription Delete All.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.25 Config Model Subscription Delete All, seeAlso: Mesh_v1.0.pdf  (page.164)
 */
+ (SigMessageHandle *)configModelSubscriptionDeleteAllWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionDeleteAll *config = [[SigConfigModelSubscriptionDeleteAll alloc] initWithElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Subscription Overwrite.
 * @param   destination    the unicastAddress of destination.
 * @param   groupAddress    Value of the address.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.23 Config Model Subscription Overwrite, seeAlso: Mesh_v1.0.pdf  (page.163)
 */
+ (SigMessageHandle *)configModelSubscriptionOverwriteWithDestination:(UInt16)destination groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionOverwrite *config = [[SigConfigModelSubscriptionOverwrite alloc] initWithGroupAddress:groupAddress elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Subscription Virtual Address Add.
 * @param   destination    the unicastAddress of destination.
 * @param   virtualLabel    Value of the Label UUID.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.20 Config Model Subscription Virtual Address Add, seeAlso: Mesh_v1.0.pdf  (page.162)
 */
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressAddWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionVirtualAddressAdd *config = [[SigConfigModelSubscriptionVirtualAddressAdd alloc] initWithVirtualLabel:virtualLabel elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Subscription Virtual Address Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   virtualLabel    Value of the Label UUID.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.22 Config Model Subscription Virtual Address Delete, seeAlso: Mesh_v1.0.pdf  (page.163)
 */
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressDeleteWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionVirtualAddressDelete *config = [[SigConfigModelSubscriptionVirtualAddressDelete alloc] initWithVirtualLabel:virtualLabel elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model Subscription Virtual Address Overwrite.
 * @param   destination    the unicastAddress of destination.
 * @param   virtualLabel    Value of the Label UUID.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.24 Config Model Subscription Virtual Address Overwrite, seeAlso: Mesh_v1.0.pdf  (page.164)
 */
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressOverwriteWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelSubscriptionVirtualAddressOverwrite *config = [[SigConfigModelSubscriptionVirtualAddressOverwrite alloc] initWithVirtualLabel:virtualLabel elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Network Transmit Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.69 Config Network Transmit Get, seeAlso: Mesh_v1.0.pdf  (page.182)
 */
+ (SigMessageHandle *)configNetworkTransmitGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNetworkTransmitGet *config = [[SigConfigNetworkTransmitGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNetworkTransmitStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Network Transmit Set.
 * @param   destination    the unicastAddress of destination.
 * @param   networkTransmitCount    Number of transmissions for each Network PDU originating from the node.
 * @param   networkTransmitIntervalSteps    Number of 10-millisecond steps between transmissions.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.70 Config Network Transmit Set, seeAlso: Mesh_v1.0.pdf  (page.182)
 */
+ (SigMessageHandle *)configNetworkTransmitSetWithDestination:(UInt16)destination networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNetworkTransmitSet *config = [[SigConfigNetworkTransmitSet alloc] initWithCount:networkTransmitCount steps:networkTransmitIntervalSteps];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNetworkTransmitStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Relay Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.12 Config Relay Get, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configRelayGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigRelayGet *config = [[SigConfigRelayGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseRelayStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Relay Set.
 * @param   destination    the unicastAddress of destination.
 * @param   relay    the value of relay.
 * @param   networkTransmitCount    Number of retransmissions on advertising bearer for each Network PDU relayed by the node.
 * @param   networkTransmitIntervalSteps    Number of 10-millisecond steps between retransmissions.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.13 Config Relay Set, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configRelaySetWithDestination:(UInt16)destination relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigRelaySet *config = [[SigConfigRelaySet alloc] initWithCount:networkTransmitCount steps:networkTransmitIntervalSteps];
    config.state = relay;
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseRelayStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config SIG Model Subscription Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.27 Config SIG Model Subscription Get, seeAlso: Mesh_v1.0.pdf  (page.165)
 */
+ (SigMessageHandle *)configSIGModelSubscriptionGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigSIGModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigSIGModelSubscriptionGet *config = [[SigConfigSIGModelSubscriptionGet alloc] initWithElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSIGModelSubscriptionListCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Vendor Model Subscription Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.29 Config Vendor Model Subscription Get, seeAlso: Mesh_v1.0.pdf  (page.166)
 */
+ (SigMessageHandle *)configVendorModelSubscriptionGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigVendorModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigVendorModelSubscriptionGet *config = [[SigConfigVendorModelSubscriptionGet alloc] initWithElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseVendorModelSubscriptionListCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Low Power Node PollTimeout Get.
 * @param   destination    the unicastAddress of destination.
 * @param   LPNAddress    The unicast address of the Low Power node.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.67 Config Low Power Node PollTimeout Get, seeAlso: Mesh_v1.0.pdf  (page.181)
 */
+ (SigMessageHandle *)configLowPowerNodePollTimeoutGetWithDestination:(UInt16)destination LPNAddress:(UInt16)LPNAddress retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigLowPowerNodePollTimeoutStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigLowPowerNodePollTimeoutGet *config = [[SigConfigLowPowerNodePollTimeoutGet alloc] initWithLPNAddress:LPNAddress];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLowPowerNodePollTimeoutStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Heartbeat Publication Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.61 Config Heartbeat Publication Get, seeAlso: Mesh_v1.0.pdf  (page.178)
 */
+ (SigMessageHandle *)configHeartbeatPublicationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigHeartbeatPublicationGet *config = [[SigConfigHeartbeatPublicationGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseHeartbeatPublicationStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Heartbeat Publication Set.
 * @param   destination    the unicastAddress of destination.
 * @param   countLog    Number of Heartbeat messages to be sent.
 * @param   periodLog    Period for sending Heartbeat messages.
 * @param   ttl    TTL to be used when sending Heartbeat messages.
 * @param   features    Bit field indicating features that trigger Heartbeat messages when changed.
 * @param   netKeyIndex    NetKey Index.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.62 Config Heartbeat Publication Set, seeAlso: Mesh_v1.0.pdf  (page.179)
 */
+ (SigMessageHandle *)configHeartbeatPublicationSetWithDestination:(UInt16)destination countLog:(UInt8)countLog periodLog:(UInt8)periodLog ttl:(UInt8)ttl features:(SigFeatures)features netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigHeartbeatPublicationSet *config = [[SigConfigHeartbeatPublicationSet alloc] initWithDestination:destination countLog:countLog periodLog:periodLog ttl:ttl features:features netKeyIndex:netKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseHeartbeatPublicationStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Heartbeat Subscription Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.64 Config Heartbeat Subscription Get, seeAlso: Mesh_v1.0.pdf  (page.180)
 */
+ (SigMessageHandle *)configHeartbeatSubscriptionGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigHeartbeatSubscriptionGet *config = [[SigConfigHeartbeatSubscriptionGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseHeartbeatSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Heartbeat Subscription Set.
 * @param   destination    the unicastAddress of destination.
 * @param   source    Source address for Heartbeat messages.
 * @param   periodLog    Period for receiving Heartbeat messages.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.65 Config Heartbeat Subscription Set, seeAlso: Mesh_v1.0.pdf  (page.180)
 */
+ (SigMessageHandle *)configHeartbeatSubscriptionSetWithDestination:(UInt16)destination source:(UInt16)source periodLog:(UInt8)periodLog retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigHeartbeatSubscriptionSet *config = [[SigConfigHeartbeatSubscriptionSet alloc] initWithSource:source destination:destination periodLog:periodLog];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseHeartbeatSubscriptionStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model App Bind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.46 Config Model App Bind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppBindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelAppBind *config = [[SigConfigModelAppBind alloc] initWithApplicationKeyIndex:applicationKeyIndex elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelAppStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model App Unbind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.47 Config Model App Unbind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppUnbindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelAppUnbind *config = [[SigConfigModelAppUnbind alloc] initWithApplicationKeyIndex:applicationKeyIndex elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelAppStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model App Bind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   isVendorModelID    YES means Vendor Model ID, NO means SIG Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.46 Config Model App Bind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppBindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelAppBind *config = [[SigConfigModelAppBind alloc] initWithApplicationKeyIndex:applicationKeyIndex elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier isVendorModelID:isVendorModelID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelAppStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Model App Unbind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   isVendorModelID    YES means Vendor Model ID, NO means SIG Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.47 Config Model App Unbind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppUnbindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigModelAppUnbind *config = [[SigConfigModelAppUnbind alloc] initWithApplicationKeyIndex:applicationKeyIndex elementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier isVendorModelID:isVendorModelID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseModelAppStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config NetKey Add.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    NetKey Index.
 * @param   networkKeyData    NetKey.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.31 Config NetKey Add, seeAlso: Mesh_v1.0.pdf  (page.167)
 */
+ (SigMessageHandle *)configNetKeyAddWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNetKeyAdd *config = [[SigConfigNetKeyAdd alloc] initWithNetworkKeyIndex:networkKeyIndex networkKeyData:networkKeyData];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNetKeyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config NetKey Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    NetKey Index.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.33 Config NetKey Delete, seeAlso: Mesh_v1.0.pdf  (page.168)
 */
+ (SigMessageHandle *)configNetKeyDeleteWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNetKeyDelete *config = [[SigConfigNetKeyDelete alloc] initWithNetworkKeyIndex:networkKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNetKeyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config NetKey Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.35 Config NetKey Get, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configNetKeyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNetKeyGet *config = [[SigConfigNetKeyGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNetKeyListCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config NetKey Update.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    NetKey Index.
 * @param   networkKeyData    NetKey.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.32 Config NetKey Update, seeAlso: Mesh_v1.0.pdf  (page.167)
 */
+ (SigMessageHandle *)configNetKeyUpdateWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNetKeyUpdate *config = [[SigConfigNetKeyUpdate alloc] initWithNetworkKeyIndex:networkKeyIndex networkKeyData:networkKeyData];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNetKeyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Node Identity Get.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex    Index of the NetKey.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.43 Config Node Identity Get, seeAlso: Mesh_v1.0.pdf  (page.171)
 */
+ (SigMessageHandle *)configNodeIdentityGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNodeIdentityGet *config = [[SigConfigNodeIdentityGet alloc] initWithNetKeyIndex:netKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNodeIdentityStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Node Identity Set.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex    Index of the NetKey.
 * @param   identity    New Node Identity state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.44 Config Node Identity Set, seeAlso: Mesh_v1.0.pdf  (page.172)
 */
+ (SigMessageHandle *)configNodeIdentitySetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNodeIdentitySet *config = [[SigConfigNodeIdentitySet alloc] initWithNetKeyIndex:netKeyIndex identity:identity];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNodeIdentityStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Node Reset.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.53 Config Node Reset, seeAlso: Mesh_v1.0.pdf  (page.176)
 */
+ (SigMessageHandle *)resetNodeWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigNodeReset *config = [[SigConfigNodeReset alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseNodeResetStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config SIG Model App Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.49 Config SIG Model App Get, seeAlso: Mesh_v1.0.pdf  (page.174)
 */
+ (SigMessageHandle *)configSIGModelAppGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigSIGModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigSIGModelAppGet *config = [[SigConfigSIGModelAppGet alloc] initWithElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSIGModelAppListCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/**
 * @brief   Config Vendor Model App Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.51 Config Vendor Model App Get, seeAlso: Mesh_v1.0.pdf  (page.175)
 */
+ (SigMessageHandle *)configVendorModelAppGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigVendorModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigConfigVendorModelAppGet *config = [[SigConfigVendorModelAppGet alloc] initWithElementAddress:elementAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseVendorModelAppListCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}


#pragma mark - 3.2 Generic messages


#pragma mark - 3.2.1 Generic OnOff messages


/**
 * @brief   Generic OnOff Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.1.1 Generic OnOff Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.42)
 */
+ (SigMessageHandle *)genericOnOffGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericOnOffGet *message = [[SigGenericOnOffGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseOnOffStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic OnOff Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Generic OnOff state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.1.2 Generic OnOff Set and 3.2.1.3 Generic OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
 */
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigGenericOnOffSet alloc] initWithIsOn:isOn];
    } else {
        message = [[SigGenericOnOffSetUnacknowledged alloc] initWithIsOn:isOn];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseOnOffStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic OnOff Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Generic OnOff state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.1.2 Generic OnOff Set and 3.2.1.3 Generic OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
 */
+ (SigMessageHandle *)genericOnOffSetDestination:(UInt16)destination isOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseOnOffStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 3.2.2 Generic Level messages


/**
 * @brief   Generic Level Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.1 Generic Level Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
 */
+ (SigMessageHandle *)genericLevelGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericLevelGet *message = [[SigGenericLevelGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLevelStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Level Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   level    The target value of the Generic Level state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.2 Generic Level Set and 3.2.2.3 Generic Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
 */
+ (SigMessageHandle *)genericLevelSetWithDestination:(UInt16)destination level:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLevelStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Level Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   level    The target value of the Generic Level state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.2 Generic Level Set and 3.2.2.3 Generic Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
 */
+ (SigMessageHandle *)genericLevelSetWithDestination:(UInt16)destination level:(UInt16)level retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericLevelSetWithDestination:destination level:level transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Generic Delta Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   delta    The Delta change of the Generic Level state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.4 Generic Delta Set and 3.2.2.5 Generic Delta Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.46)
 */
+ (SigMessageHandle *)genericDeltaSetWithDestination:(UInt16)destination delta:(UInt32)delta transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLevelStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Delta Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   delta    The Delta change of the Generic Level state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.4 Generic Delta Set and 3.2.2.5 Generic Delta Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.46)
 */
+ (SigMessageHandle *)genericDeltaSetWithDestination:(UInt16)destination delta:(UInt32)delta retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericDeltaSetWithDestination:destination delta:delta transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Generic Move Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   deltaLevel    The Delta Level step to calculate Move speed for the Generic Level state..
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.6 Generic Move Set and 3.2.2.7 Generic Move Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.47)
 */
+ (SigMessageHandle *)genericMoveSetWithDestination:(UInt16)destination deltaLevel:(UInt16)deltaLevel transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLevelStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Move Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   deltaLevel    The Delta Level step to calculate Move speed for the Generic Level state..
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.6 Generic Move Set and 3.2.2.7 Generic Move Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.47)
 */
+ (SigMessageHandle *)genericMoveSetWithDestination:(UInt16)destination deltaLevel:(UInt16)deltaLevel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericMoveSetWithDestination:destination deltaLevel:deltaLevel transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}


#pragma mark - 3.2.3 Generic Default Transition Time messages


/**
 * @brief   Generic Default Transition Time Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.3.1 Generic Default Transition Time Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.48)
 */
+ (SigMessageHandle *)genericDefaultTransitionTimeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericDefaultTransitionTimeGet *message = [[SigGenericDefaultTransitionTimeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseDefaultTransitionTimeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Default Transition Time Set.
 * @param   destination    the unicastAddress of destination.
 * @param   defaultTransitionTime    The value of the Generic Default Transition Time state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.3.2 Generic Default Transition Time Set and 3.2.3.3 Generic Default Transition Time Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
 */
+ (SigMessageHandle *)genericDefaultTransitionTimeSetWithDestination:(UInt16)destination defaultTransitionTime:(nonnull SigTransitionTime *)defaultTransitionTime retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigGenericDefaultTransitionTimeSet alloc] initWithTransitionTime:defaultTransitionTime];
    } else {
        message = [[SigGenericDefaultTransitionTimeSetUnacknowledged alloc] initWithTransitionTime:defaultTransitionTime];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseDefaultTransitionTimeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 3.2.4 Generic OnPowerUp messages


/**
 * @brief   Generic OnPowerUp Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.4.1 Generic OnPowerUp Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
 */
+ (SigMessageHandle *)genericOnPowerUpGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericOnPowerUpGet *message = [[SigGenericOnPowerUpGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseOnPowerUpStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic OnPowerUp Set.
 * @param   destination    the unicastAddress of destination.
 * @param   onPowerUp    The value of the Generic OnPowerUp state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.4.2 Generic OnPowerUp Set and 3.2.4.3 Generic OnPowerUp Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
 */
+ (SigMessageHandle *)genericOnPowerUpSetWithDestination:(UInt16)destination onPowerUp:(SigOnPowerUp)onPowerUp retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigGenericOnPowerUpSet alloc] initWithState:onPowerUp];
    } else {
        message = [[SigGenericOnPowerUpSetUnacknowledged alloc] initWithState:onPowerUp];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseOnPowerUpStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 3.2.5 Generic Power Level messages


/**
 * @brief   Generic Power Level Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.1 Generic Power Level Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.50)
 */
+ (SigMessageHandle *)genericPowerLevelGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerLevelGet *message = [[SigGenericPowerLevelGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePowerLevelStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Power Level Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   power    The target value of the Generic Power Actual state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.2 Generic Power Level Set and 3.2.5.3 Generic Power Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.51)
 */
+ (SigMessageHandle *)genericPowerLevelSetWithDestination:(UInt16)destination power:(UInt16)power transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLevelStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Power Level Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   power    The target value of the Generic Power Actual state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.2 Generic Power Level Set and 3.2.5.3 Generic Power Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.51)
 */
+ (SigMessageHandle *)genericPowerLevelSetWithDestination:(UInt16)destination power:(UInt16)power retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self genericPowerLevelSetWithDestination:destination power:power transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Generic Power Last Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.5 Generic Power Last Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.52)
 */
+ (SigMessageHandle *)genericPowerLastGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerLastGet *message = [[SigGenericPowerLastGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePowerLastStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Power Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.7 Generic Power Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
 */
+ (SigMessageHandle *)genericPowerDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerDefaultGet *message = [[SigGenericPowerDefaultGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePowerDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Power Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.11 Generic Power Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
 */
+ (SigMessageHandle *)genericPowerRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericPowerRangeGet *message = [[SigGenericPowerRangeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePowerRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Power Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   power    The value of the Generic Power Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.8 Generic Power Default Set and 3.2.5.8 Generic Power Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
 */
+ (SigMessageHandle *)genericPowerDefaultSetWithDestination:(UInt16)destination power:(UInt16)power retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigGenericPowerDefaultSet alloc] initWithPower:power];
    } else {
        message = [[SigGenericPowerDefaultSetUnacknowledged alloc] initWithPower:power];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePowerDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Generic Power Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   rangeMin    The value of the Generic Power Min field of the Generic Power Range state.
 * @param   rangeMax    The value of the Generic Power Range Max field of the Generic Power Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.12 Generic Power Range Set and 3.2.5.13 Generic Power Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
 */
+ (SigMessageHandle *)genericPowerRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigGenericPowerRangeSet alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
    } else {
        message = [[SigGenericPowerRangeSetUnacknowledged alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePowerRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 3.2.6 Generic Battery messages


/**
 * @brief   Generic Battery Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.6.1 Generic Battery Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.55)
 */
+ (SigMessageHandle *)genericBatteryGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericBatteryStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericBatteryGet *message = [[SigGenericBatteryGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBatteryStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 4.2 Sensor messages


/**
 * @brief   Sensor Descriptor Get without propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.1 Sensor Descriptor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.114)
 */
+ (SigMessageHandle *)sensorDescriptorGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorDescriptorGet *message = [[SigSensorDescriptorGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorDescriptorStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Descriptor Get with propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor (Optional).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.1 Sensor Descriptor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.114)
 */
+ (SigMessageHandle *)sensorDescriptorGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorDescriptorGet *message = [[SigSensorDescriptorGet alloc] initWithPropertyID:propertyID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorDescriptorStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Get without propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.13 Sensor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
 */
+ (SigMessageHandle *)sensorGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorGet *message = [[SigSensorGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Get with propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor (Optional).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.13 Sensor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
 */
+ (SigMessageHandle *)sensorGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorGet *message = [[SigSensorGet alloc] initWithPropertyID:propertyID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Column Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property identifying a sensor.
 * @param   rawValueX    Raw value identifying a column.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.15 Sensor Column Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.122)
 */
+ (SigMessageHandle *)sensorColumnGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorColumnStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorColumnGet *message = [[SigSensorColumnGet alloc] initWithPropertyID:propertyID rawValueX:rawValueX];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorColumnStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Series Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property identifying a sensor.
 * @param   rawValueX1Data    Raw value identifying a starting column. (Optional).
 * @param   rawValueX2Data    Raw value identifying an ending column. (C.1).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.17 Sensor Series Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.123)
 */
+ (SigMessageHandle *)sensorSeriesGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSeriesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorSeriesGet *message = [[SigSensorSeriesGet alloc] initWithPropertyID:propertyID rawValueX1Data:rawValueX1Data rawValueX2Data:rawValueX2Data];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorSeriesStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Cadence Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor..
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.3 Sensor Cadence Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.115)
 */
+ (SigMessageHandle *)sensorCadenceGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorCadenceGet *message = [[SigSensorCadenceGet alloc] initWithPropertyID:propertyID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorCadenceStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Cadence Set.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor..
 * @param   cadenceData    Sensor Cadence config data.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.4 Sensor Cadence Set and 4.2.5 Sensor Cadence Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.115)
 */
+ (SigMessageHandle *)sensorCadenceSetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID cadenceData:(NSData *)cadenceData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorCadenceStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Settings Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a sensor.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.7 Sensor Settings Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.117)
 */
+ (SigMessageHandle *)sensorSettingsGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSettingsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorSettingsGet *message = [[SigSensorSettingsGet alloc] initWithPropertyID:propertyID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorSettingsStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Setting Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a sensor.
 * @param   settingPropertyID    Setting Property ID identifying a setting within a sensor.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.9 Sensor Setting Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
 */
+ (SigMessageHandle *)sensorSettingGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSensorSettingGet *message = [[SigSensorSettingGet alloc] initWithPropertyID:propertyID settingPropertyID:settingPropertyID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorSettingStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Sensor Setting Set.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a sensor.
 * @param   settingPropertyID    Setting Property ID identifying a setting within a sensor.
 * @param   settingRaw    Raw value for the setting.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.10 Sensor Setting Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
 */
+ (SigMessageHandle *)sensorSettingSetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigSensorSettingSet alloc] initWithPropertyID:propertyID settingPropertyID:settingPropertyID settingRaw:settingRaw];
    } else {
        message = [[SigSensorSettingSetUnacknowledged alloc] initWithPropertyID:propertyID settingPropertyID:settingPropertyID settingRaw:settingRaw];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSensorSettingStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 5.2.1 Time messages


/**
 * @brief   Time Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.1 Time Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.142)
 */
+ (SigMessageHandle *)timeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeGet *message = [[SigTimeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Time Set.
 * @param   destination    the unicastAddress of destination.
 * @param   timeModel    config time data object.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.2 Time Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
 */
+ (SigMessageHandle *)timeSetWithDestination:(UInt16)destination timeModel:(SigTimeModel *)timeModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeSet *message = [[SigTimeSet alloc] initWithTimeModel:timeModel];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Time Status.
 * @param   destination    the unicastAddress of destination.
 * @param   timeModel    config time data object.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.3 Time Status, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
 */
+ (SigMessageHandle *)timeStatusWithDestination:(UInt16)destination timeModel:(SigTimeModel *)timeModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(_Nullable responseTimeStatusMessageBlock)successCallback resultCallback:(_Nullable resultBlock)resultCallback {
    SigTimeStatus *message = [[SigTimeStatus alloc] init];
    message.timeModel = timeModel;
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Time Role Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.10 Time Role Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
 */
+ (SigMessageHandle *)timeRoleGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeRoleGet *message = [[SigTimeRoleGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeRoleStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Time Role Set.
 * @param   destination    the unicastAddress of destination.
 * @param   timeRole    The Time Role for the element.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.11 Time Role Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
 */
+ (SigMessageHandle *)timeRoleSetWithDestination:(UInt16)destination timeRole:(SigTimeRole)timeRole retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeRoleSet *message = [[SigTimeRoleSet alloc] initWithTimeRole:timeRole];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeRoleStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Time Zone Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.4 Time Zone Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
 */
+ (SigMessageHandle *)timeZoneGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeZoneGet *message = [[SigTimeZoneGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeZoneStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Time Zone Set.
 * @param   destination    the unicastAddress of destination.
 * @param   timeZoneOffsetNew    Upcoming local time zone offset.
 * @param   TAIOfZoneChange    TAI Seconds time of the upcoming Time Zone Offset change.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.5 Time Zone Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
 */
+ (SigMessageHandle *)timeZoneSetWithDestination:(UInt16)destination timeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTimeZoneSet *message = [[SigTimeZoneSet alloc] initWithTimeZoneOffsetNew:timeZoneOffsetNew TAIOfZoneChange:TAIOfZoneChange];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTimeZoneStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   TAI-UTC Delta Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.7 TAI-UTC Delta Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
 */
+ (SigMessageHandle *)TAI_UTC_DeltaGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTAI_UTC_DeltaGet *message = [[SigTAI_UTC_DeltaGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTAI_UTC_DeltaStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   TAI-UTC Delta Set.
 * @param   destination    the unicastAddress of destination.
 * @param   TAI_UTC_DeltaNew    Upcoming difference between TAI and UTC in seconds.
 * @param   padding    Always 0b0. Other values are Prohibited..
 * @param   TAIOfDeltaChange    TAI Seconds time of the upcoming TAI-UTC Delta change.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.8 TAI-UTC Delta Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
 */
+ (SigMessageHandle *)TAI_UTC_DeltaSetWithDestination:(UInt16)destination TAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigTAI_UTC_DeltaSet *message = [[SigTAI_UTC_DeltaSet alloc] initWithTAI_UTC_DeltaNew:TAI_UTC_DeltaNew padding:padding TAIOfDeltaChange:TAIOfDeltaChange];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseTAI_UTC_DeltaStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 5.2.2 Scene messages


/**
 * @brief   Scene Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.5 Scene Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSceneGet *message = [[SigSceneGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSceneStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Scene Recall with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be recalled.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.3 Scene Recall and 5.2.2.4 Scene Recall Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneRecallWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigSceneRecall alloc] initWithSceneNumber:sceneNumber transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigSceneRecallUnacknowledged alloc] initWithSceneNumber:sceneNumber transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSceneStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Scene Recall without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be recalled.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.3 Scene Recall and 5.2.2.4 Scene Recall Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneRecallWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self sceneRecallWithDestination:destination sceneNumber:sceneNumber transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Scene Register Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.7 Scene Register Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneRegisterGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSceneRegisterGet *message = [[SigSceneRegisterGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSceneRegisterStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Scene Store.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be stored.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.1 Scene Store and 5.2.2.2 Scene Store Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
 */
+ (SigMessageHandle *)sceneStoreWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigSceneStore alloc] initWithSceneNumber:sceneNumber];
    } else {
        message = [[SigSceneStoreUnacknowledged alloc] initWithSceneNumber:sceneNumber];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSceneRegisterStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Scene Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be deleted.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.9 Scene Delete and 5.2.2.10 Scene Delete Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.149)
 */
+ (SigMessageHandle *)sceneDeleteWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigSceneDelete alloc] initWithSceneNumber:sceneNumber];
    } else {
        message = [[SigSceneDeleteUnacknowledged alloc] initWithSceneNumber:sceneNumber];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSceneRegisterStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 5.2.3 Scheduler messages


/**
 * @brief   Scheduler Action Get.
 * @param   destination    the unicastAddress of destination.
 * @param   schedulerIndex    Index of the Schedule Register entry to get.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.3.3 Scheduler Action Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
 */
+ (SigMessageHandle *)schedulerActionGetWithDestination:(UInt16)destination schedulerIndex:(UInt8)schedulerIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSchedulerActionGet *message = [[SigSchedulerActionGet alloc] initWithIndex:schedulerIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSchedulerActionStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Scheduler Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.3.1 Scheduler Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
 */
+ (SigMessageHandle *)schedulerGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSchedulerGet *message = [[SigSchedulerGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSchedulerStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Scheduler Action Set.
 * @param   destination    the unicastAddress of destination.
 * @param   schedulerModel    Bit field defining an entry in the Schedule Register (see Section 5.1.4.2)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.3.4 Scheduler Action Set and 5.2.3.5 Scheduler Action Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
 */
+ (SigMessageHandle *)schedulerActionSetWithDestination:(UInt16)destination schedulerModel:(SchedulerModel *)schedulerModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigSchedulerActionSet alloc] initWithSchedulerModel:schedulerModel];
    } else {
        message = [[SigSchedulerActionSetUnacknowledged alloc] initWithSchedulerModel:schedulerModel];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseSchedulerActionStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 6.3.1 Light Lightness messages


/**
 * @brief   Light Lightness Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.1 Light Lightness Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.199)
 */
+ (SigMessageHandle *)lightLightnessGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessGet *message = [[SigLightLightnessGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Actual state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.2 Light Lightness Set and 6.3.1.3 Light Lightness Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.200)
 */
+ (SigMessageHandle *)lightLightnessSetWithDestination:(UInt16)destination lightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLightnessSet alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightLightnessSetUnacknowledged alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Actual state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.2 Light Lightness Set and 6.3.1.3 Light Lightness Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.200)
 */
+ (SigMessageHandle *)lightLightnessSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLightnessSetWithDestination:destination lightness:lightness transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light Lightness Linear Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.5 Light Lightness Linear Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
 */
+ (SigMessageHandle *)lightLightnessLinearGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessLinearGet *message = [[SigLightLightnessLinearGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessLinearStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Linear Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Linear state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.6 Light Lightness Linear Set and 6.3.1.7 Light Lightness Linear Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
 */
+ (SigMessageHandle *)lightLightnessLinearSetWithDestination:(UInt16)destination lightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLightnessLinearSet alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightLightnessLinearSetUnacknowledged alloc] initWithLightness:lightness transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessLinearStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Linear Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Linear state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.6 Light Lightness Linear Set and 6.3.1.7 Light Lightness Linear Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
 */
+ (SigMessageHandle *)lightLightnessLinearSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLightnessLinearSetWithDestination:destination lightness:lightness transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light Lightness Last Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.9 Light Lightness Last Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.203)
 */
+ (SigMessageHandle *)lightLightnessLastGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessLastGet *message = [[SigLightLightnessLastGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessLastStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.11 Light Lightness Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
 */
+ (SigMessageHandle *)lightLightnessDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessDefaultGet *message = [[SigLightLightnessDefaultGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.15 Light Lightness Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
 */
+ (SigMessageHandle *)lightLightnessRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLightnessRangeGet *message = [[SigLightLightnessRangeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The value of the Light Lightness Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.12 Light Lightness Default Set and 6.3.1.13 Light Lightness Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
 */
+ (SigMessageHandle *)lightLightnessDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLightnessDefaultSet alloc] initWithLightness:lightness];
    } else {
        message = [[SigLightLightnessDefaultSetUnacknowledged alloc] initWithLightness:lightness];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light Lightness Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   rangeMin    The value of the Lightness Range Min field of the Light Lightness Range state.
 * @param   rangeMax    The value of the Lightness Range Max field of the Light Lightness Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.16 Light Lightness Range Set and 6.3.1.17 Light Lightness Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
 */
+ (SigMessageHandle *)lightLightnessRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLightnessRangeSet alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
    } else {
        message = [[SigLightLightnessRangeSetUnacknowledged alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLightnessRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 6.3.2 Light CTL Messages


/**
 * @brief   Light CTL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.1 Light CTL Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
 */
+ (SigMessageHandle *)lightCTLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLGet *message = [[SigLightCTLGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light CTL Lightness state.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.2 Light CTL Set and 6.3.2.3 Light CTL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
 */
+ (SigMessageHandle *)lightCTLSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightCTLSet alloc] initWithCTLLightness:lightness CTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightCTLSetUnacknowledged alloc] initWithCTLLightness:lightness CTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light CTL Lightness state.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.2 Light CTL Set and 6.3.2.3 Light CTL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
 */
+ (SigMessageHandle *)lightCTLSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightCTLSetWithDestination:destination lightness:lightness temperature:temperature deltaUV:deltaUV transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light CTL Temperature Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.5 Light CTL Temperature Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
 */
+ (SigMessageHandle *)lightCTLTemperatureGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLTemperatureGet *message = [[SigLightCTLTemperatureGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLTemperatureStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Temperature Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.9 Light CTL Temperature Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
 */
+ (SigMessageHandle *)lightCTLTemperatureRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLTemperatureRangeGet *message = [[SigLightCTLTemperatureRangeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLTemperatureRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Temperature Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.6 Light CTL Temperature Set and 6.3.2.7 Light CTL Temperature Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
 */
+ (SigMessageHandle *)lightCTLTemperatureSetWithDestination:(UInt16)destination temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightCTLTemperatureSet alloc] initWithCTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightCTLTemperatureSetUnacknowledged alloc] initWithCTLTemperature:temperature CTLDeltaUV:deltaUV transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLTemperatureStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Temperature Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.6 Light CTL Temperature Set and 6.3.2.7 Light CTL Temperature Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
 */
+ (SigMessageHandle *)lightCTLTemperatureSetWithDestination:(UInt16)destination temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightCTLTemperatureSetWithDestination:destination temperature:temperature deltaUV:deltaUV transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light CTL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.13 Light CTL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
 */
+ (SigMessageHandle *)lightCTLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightCTLDefaultGet *message = [[SigLightCTLDefaultGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The value of the Light Lightness Default state.
 * @param   temperature    The value of the Light CTL Temperature Default state.
 * @param   deltaUV    The value of the Light CTL Delta UV Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.14 Light CTL Default Set and 6.3.2.15 Light CTL Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
 */
+ (SigMessageHandle *)lightCTLDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightCTLDefaultSet alloc] initWithLightness:lightness temperature:temperature deltaUV:deltaUV];
    } else {
        message = [[SigLightCTLDefaultSetUnacknowledged alloc] initWithLightness:lightness temperature:temperature deltaUV:deltaUV];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light CTL Temperature Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   rangeMin    The value of the Temperature Range Min field of the Light CTL Temperature Range state.
 * @param   rangeMax    The value of the Temperature Range Max field of the Light CTL Temperature Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.10 Light CTL Temperature Range Set and 6.3.2.11 Light CTL Temperature Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
 */
+ (SigMessageHandle *)lightCTLTemperatureRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightCTLTemperatureRangeSet alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
    } else {
        message = [[SigLightCTLTemperatureRangeSetUnacknowledged alloc] initWithRangeMin:rangeMin rangeMax:rangeMax];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightCTLTemperatureRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 6.3.3 Light HSL messages


/**
 * @brief   Light HSL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.1 Light HSL Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightHSLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLGet *message = [[SigLightHSLGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Hue Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.7 Light HSL Hue Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
 */
+ (SigMessageHandle *)lightHSLHueGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLHueGet *message = [[SigLightHSLHueGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLHueStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Hue Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   hue    The target value of the Light HSL Hue state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.8 Light HSL Hue Set and 6.3.3.9 Light HSL Hue Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
 */
+ (SigMessageHandle *)lightHSLHueSetWithDestination:(UInt16)destination hue:(UInt16)hue transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightHSLHueSet alloc] initWithHue:hue transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightHSLHueSetUnacknowledged alloc] initWithHue:hue transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLHueStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Hue Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   hue    The target value of the Light HSL Hue state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.8 Light HSL Hue Set and 6.3.3.9 Light HSL Hue Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
 */
+ (SigMessageHandle *)lightHSLHueSetWithDestination:(UInt16)destination hue:(UInt16)hue retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightHSLHueSetWithDestination:destination hue:hue transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light HSL Saturation Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.11 Light HSL Saturation Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
 */
+ (SigMessageHandle *)lightHSLSaturationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLSaturationGet *message = [[SigLightHSLSaturationGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLSaturationStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Saturation Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   saturation    The target value of the Light HSL Saturation state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.12 Light HSL Saturation Set and 6.3.3.13 Light HSL Saturation Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
 */
+ (SigMessageHandle *)lightHSLSaturationSetWithDestination:(UInt16)destination saturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightHSLSaturationSet alloc] initWithSaturation:saturation transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightHSLSaturationSetUnacknowledged alloc] initWithSaturation:saturation transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLSaturationStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Saturation Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   saturation    The target value of the Light HSL Saturation state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.12 Light HSL Saturation Set and 6.3.3.13 Light HSL Saturation Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
 */
+ (SigMessageHandle *)lightHSLSaturationSetWithDestination:(UInt16)destination saturation:(UInt16)saturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightHSLSaturationSetWithDestination:destination saturation:saturation transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light HSL Saturation Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   HSLLight    The target value of the Light HSL Lightness state.
 * @param   HSLHue    The target value of the Light HSL Hue state.
 * @param   HSLSaturation    The target value of the Light HSL Saturation state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.2 Light HSL Set and 6.3.3.3 Light HSL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightHSLSetWithDestination:(UInt16)destination HSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightHSLSet alloc] initWithHSLLightness:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightHSLSetUnacknowledged alloc] initWithHSLLightness:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Saturation Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   HSLLight    The target value of the Light HSL Lightness state.
 * @param   HSLHue    The target value of the Light HSL Hue state.
 * @param   HSLSaturation    The target value of the Light HSL Saturation state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.2 Light HSL Set and 6.3.3.3 Light HSL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightHSLSetWithDestination:(UInt16)destination HSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightHSLSetWithDestination:destination HSLLight:HSLLight HSLHue:HSLHue HSLSaturation:HSLSaturation transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light HSL Target Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.5 Light HSL Target Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.215)
 */
+ (SigMessageHandle *)lightHSLTargetGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLTargetGet *message = [[SigLightHSLTargetGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLTargetStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.15 Light HSL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
 */
+ (SigMessageHandle *)lightHSLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLDefaultGet *message = [[SigLightHSLDefaultGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.15 Light HSL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
 */
+ (SigMessageHandle *)lightHSLRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightHSLRangeGet *message = [[SigLightHSLRangeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   light    The value of the Light Lightness Default state.
 * @param   hue    The value of the Light HSL Hue Default state.
 * @param   saturation    The value of the Light HSL Saturation Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.16 Light HSL Default Set and 6.3.3.17 Light HSL Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
 */
+ (SigMessageHandle *)lightHSLDefaultSetWithDestination:(UInt16)destination light:(UInt16)light hue:(UInt16)hue saturation:(UInt16)saturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightHSLDefaultSet alloc] initWithLightness:light hue:hue saturation:saturation];
    } else {
        message = [[SigLightHSLDefaultSetUnacknowledged alloc] initWithLightness:light hue:hue saturation:saturation];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light HSL Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   hueRangeMin    The value of the Hue Range Min field of the Light HSL Hue Range state.
 * @param   hueRangeMax    The value of the Hue Range Max field of the Light HSL Hue Range state.
 * @param   saturationRangeMin    The value of the Saturation Range Min field of the Light HSL Saturation Range state.
 * @param   saturationRangeMax    The value of the Saturation Range Max field of the Light HSL Saturation Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.20 Light HSL Range Set and 6.3.3.21 Light HSL Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.221)
 */
+ (SigMessageHandle *)lightHSLRangeSetWithDestination:(UInt16)destination hueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightHSLRangeSet alloc] initWithHueRangeMin:hueRangeMin hueRangeMax:hueRangeMax saturationRangeMin:saturationRangeMin saturationRangeMax:saturationRangeMax];
    } else {
        message = [[SigLightHSLRangeSetUnacknowledged alloc] initWithHueRangeMin:hueRangeMin hueRangeMax:hueRangeMax saturationRangeMin:saturationRangeMin saturationRangeMax:saturationRangeMax];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightHSLRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 6.3.4 Light xyL messages


/**
 * @brief   Light xyL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.1 Light xyL Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.223)
 */
+ (SigMessageHandle *)lightXyLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLGet *message = [[SigLightXyLGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light xyL Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   xyLLightness    The target value of the Light xyL Lightness state.
 * @param   xyLx    The target value of the Light xyL x state.
 * @param   xyLy    The target value of the Light xyL y state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.2 Light xyL Set and 6.3.4.3 Light xyL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightXyLSetWithDestination:(UInt16)destination xyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightXyLSet alloc] initWithXyLLightness:xyLLightness xyLX:xyLx xyLY:xyLy transitionTime:transitionTime delay:delay];
    } else {
        message = [[SigLightXyLSetUnacknowledged alloc] initWithXyLLightness:xyLLightness xyLX:xyLx xyLY:xyLy transitionTime:transitionTime delay:delay];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light xyL Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   xyLLightness    The target value of the Light xyL Lightness state.
 * @param   xyLx    The target value of the Light xyL x state.
 * @param   xyLy    The target value of the Light xyL y state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.2 Light xyL Set and 6.3.4.3 Light xyL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightXyLSetWithDestination:(UInt16)destination xyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightXyLSetWithDestination:destination xyLLightness:xyLLightness xyLx:xyLx xyLy:xyLy transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}

/**
 * @brief   Light xyL Target Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.5 Light xyL Target Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.225)
 */
+ (SigMessageHandle *)lightXyLTargetGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLTargetGet *message = [[SigLightXyLTargetGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLTargetStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light xyL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.7 Light xyL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
 */
+ (SigMessageHandle *)lightXyLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLDefaultGet *message = [[SigLightXyLDefaultGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light xyL Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.11 Light xyL Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
 */
+ (SigMessageHandle *)lightXyLRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightXyLRangeGet *message = [[SigLightXyLRangeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light xyL Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light xyL Lightness state.
 * @param   xyLx    The target value of the Light xyL x state.
 * @param   xyLy    The target value of the Light xyL y state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.8 Light xyL Default Set and 6.3.4.9 Light xyL Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
 */
+ (SigMessageHandle *)lightXyLDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightXyLDefaultSet alloc] initWithLightness:lightness xyLX:xyLx xyLY:xyLy];
    } else {
        message = [[SigLightXyLDefaultSetUnacknowledged alloc] initWithLightness:lightness xyLX:xyLx xyLY:xyLy];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLDefaultStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light xyL Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   xyLxRangeMin    The value of the xyL x Range Min field of the Light xyL x Range state.
 * @param   xyLxRangeMax    The value of the xyL x Range Max field of the Light xyL x Range state.
 * @param   xyLyRangeMin    The value of the xyL y Range Min field of the Light xyL y Range state.
 * @param   xyLyRangeMax    The value of the xyL y Range Max field of the Light xyL y Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.12 Light xyL Range Set and 6.3.4.13 Light xyL Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
 */
+ (SigMessageHandle *)lightXyLRangeSetWithDestination:(UInt16)destination xyLxRangeMin:(UInt16)xyLxRangeMin xyLxRangeMax:(UInt16)xyLxRangeMax xyLyRangeMin:(UInt16)xyLyRangeMin xyLyRangeMax:(UInt16)xyLyRangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightXyLRangeSet alloc] initWithXyLXRangeMin:xyLxRangeMin xyLXRangeMax:xyLxRangeMax xyLYRangeMin:xyLyRangeMin xyLYRangeMax:xyLyRangeMax];
    } else {
        message = [[SigLightXyLRangeSetUnacknowledged alloc] initWithXyLXRangeMin:xyLxRangeMin xyLXRangeMax:xyLxRangeMax xyLYRangeMin:xyLyRangeMin xyLYRangeMax:xyLyRangeMax];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightXyLRangeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 6.3.5 Light LC messages


#pragma mark 6.3.5.1 Light LC Mode messages


/**
 * @brief   Light LC Mode Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.1.1 Light LC Mode Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
 */
+ (SigMessageHandle *)lightLCModeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCModeGet *message = [[SigLightLCModeGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCModeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light LC Mode Set.
 * @param   destination    the unicastAddress of destination.
 * @param   enable    The target value of the Light LC Mode state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.1.2 Light LC Mode Set and 6.3.5.1.3 Light LC Mode Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
 */
+ (SigMessageHandle *)lightLCModeSetWithDestination:(UInt16)destination enable:(BOOL)enable retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLCModeSet alloc] initWithMode:enable?0x01:0x00];
    } else {
        message = [[SigLightLCModeSetUnacknowledged alloc] initWithMode:enable?0x01:0x00];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCModeStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 6.3.5.2 Light LC Occupancy Mode messages


/**
 * @brief   Light LC OM Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.2.1 Light LC OM Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.230)
 */
+ (SigMessageHandle *)lightLCOMGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCOMGet *message = [[SigLightLCOMGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCOMStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light LC OM Set.
 * @param   destination    the unicastAddress of destination.
 * @param   enable    The target value of the Light LC Occupancy Mode state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.2.2 Light LC OM Set and 6.3.5.2.3 Light LC OM Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.230)
 */
+ (SigMessageHandle *)lightLCOMSetWithDestination:(UInt16)destination enable:(BOOL)enable retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLCOMSet alloc] initWithMode:enable?0x01:0x00];
    } else {
        message = [[SigLightLCOMSetUnacknowledged alloc] initWithMode:enable?0x01:0x00];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCOMStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 6.3.5.3 Light LC Light OnOff messages


/**
 * @brief   Light LC Light OnOff Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.3.1 Light LC Light OnOff Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
 */
+ (SigMessageHandle *)lightLCLightOnOffGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCLightOnOffGet *message = [[SigLightLCLightOnOffGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCLightOnOffStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light LC Light OnOff Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Light xyL Lightness state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.3.2 Light LC Light OnOff Set and 6.3.5.3.2 Light LC Light OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
 */
+ (SigMessageHandle *)lightLCLightOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
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
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:msg retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCLightOnOffStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:msg fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light LC Light OnOff Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Light xyL Lightness state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.3.2 Light LC Light OnOff Set and 6.3.5.3.2 Light LC Light OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
 */
+ (SigMessageHandle *)lightLCLightOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    return [self lightLCLightOnOffSetWithDestination:destination isOn:isOn transitionTime:nil delay:0 retryCount:retryCount responseMaxCount:responseMaxCount ack:ack successCallback:successCallback resultCallback:resultCallback];
}


#pragma mark - 6.3.6 Light LC Property Messages


/**
 * @brief   Light LC Property Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a Light LC Property.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.6.1 Light LC Property Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.232)
 */
+ (SigMessageHandle *)lightLCPropertyGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLightLCPropertyGet *message = [[SigLightLCPropertyGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCPropertyStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/**
 * @brief   Light LC Property Set.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a Light LC Property.
 * @param   propertyValue    Raw value for the Light LC Property.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.6.2 Light LC Property Set and 6.3.6.2 Light LC Property Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.233)
 */
+ (SigMessageHandle *)lightLCPropertySetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID propertyValue:(NSData *)propertyValue retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigMeshMessage *message;
    if (ack) {
        message = [[SigLightLCPropertySet alloc] initWithLightLCPropertyID:propertyID lightLCPropertyValue:propertyValue];
    } else {
        message = [[SigLightLCPropertySetUnacknowledged alloc] initWithLightLCPropertyID:propertyID lightLCPropertyValue:propertyValue];
        if (successCallback != nil || resultCallback != nil) {
            TelinkLogWarn(@"ack is NO, successCallback and failCallback need set to nil.");
            successCallback = nil;
            resultCallback = nil;
        }
        if (retryCount != 0) {
            TelinkLogWarn(@"ack is NO, retryCount need set to 0.");
            retryCount = 0;
        }
        if (responseMaxCount != 0) {
            TelinkLogWarn(@"ack is NO, responseMaxCount need set to 0.");
            responseMaxCount = 0;
        }
    }
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseLightLCPropertyStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 6.5 Proxy configuration messages


/**
 * @brief   The Set Filter Type message can be sent by a Proxy Client to change the proxy filter type and clear the proxy filter list.
 * @param   type    the unicastAddress of destination.
 * @param   successCallback    callback when node response the status message.
 * @param   failCallback    Callback when command sending fails.
 * @note    6.5.1 Set Filter Type, seeAlso: Mesh_v1.0.pdf  (page.263)
 */
+ (void)setType:(SigProxyFilerType)type successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigSetFilterType *message = [[SigSetFilterType alloc] initWithType:type];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/**
 * @brief   The Add Addresses to Filter message is sent by a Proxy Client to add destination addresses to the proxy filter list.
 * @param   addresses    List of addresses where N is the number of addresses in this message..
 * @param   successCallback    callback when node response the status message.
 * @param   failCallback    Callback when command sending fails.
 * @note    6.5.2 Add Addresses to Filter, seeAlso: Mesh_v1.0.pdf  (page.264)
 */
+ (void)addAddressesToFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigAddAddressesToFilter *message = [[SigAddAddressesToFilter alloc] initWithAddresses:addresses];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/**
 * @brief   The Remove Addresses from Filter message is sent by a Proxy Client to remove destination addresses from the proxy filter list.
 * @param   addresses    List of addresses where N is the number of addresses in this message..
 * @param   successCallback    callback when node response the status message.
 * @param   failCallback    Callback when command sending fails.
 * @note    6.5.3 Remove Addresses from Filter, seeAlso: Mesh_v1.0.pdf  (page.264)
 */
+ (void)removeAddressesFromFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback {
    SigRemoveAddressesFromFilter *message = [[SigRemoveAddressesFromFilter alloc] initWithAddresses:addresses];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseFilterStatusCallBack = successCallback;
    command.resultCallback = failCallback;
    [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

/**
 * @brief   Adds all the addresses the Provisioner is subscribed to to the Proxy Filter.
 * @param   provisioner    the provisioner that need to set filter
 * @param   successCallback    callback when node response the status message.
 * @param   failCallback    Callback when command sending fails.
 * @note    This API will auto call setFilterType+AddAddressList, seeAlso: Mesh_v1.0.pdf  (page.263)
 */
+ (void)setFilterForProvisioner:(SigProvisionerModel *)provisioner successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)failCallback {
    SigNodeModel *node = provisioner.node;
    if (!node) {
        TelinkLogError(@"provisioner.node = nil.");
        return;
    }

    //旧版本，开发者不可以自定义配置proxy filter.
    /*
    NSMutableArray *addresses = [NSMutableArray array];
    if (node.elements && node.elements.count > 0) {
        NSArray *elements = [NSArray arrayWithArray:node.elements];
        for (SigElementModel *element in elements) {
            element.parentNodeAddress = node.address;
            // Add Unicast Addresses of all Elements of the Provisioner's Node.
            [addresses addObject:@(element.unicastAddress)];
            // Add all addresses that the Node's Models are subscribed to.
            NSArray *models = [NSArray arrayWithArray:element.models];
            for (SigModelIDModel *model in models) {
                if (model.subscribe && model.subscribe.count > 0) {
                    NSArray *subscribe = [NSArray arrayWithArray:model.subscribe];
                    for (NSString *addr in subscribe) {
                        UInt16 indAddr = [LibTools uint16From16String:addr];
                        [addresses addObject:@(indAddr)];
                    }
                }
            }
        }
    } else {
        [addresses addObject:@(node.address)];
    }

    // Add All Nodes group address.
    [addresses addObject:@(kMeshAddress_allNodes)];
    // Submit.
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        if (SigMeshLib.share.meshPrivateBeacon && (SigMeshLib.share.sendBeaconType == AppSendBeaconType_auto || SigMeshLib.share.sendBeaconType == AppSendBeaconType_meshPrivateBeacon)) {
            [weakSelf sendMeshPrivateBeacon];
        } else {
            [weakSelf sendSecureNetworkBeacon];
        }
        [NSThread sleepForTimeInterval:0.1];
        TelinkLogVerbose(@"filter addresses:%@",addresses);
        [self setType:SigProxyFilerType_whitelist successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
//            TelinkLogVerbose(@"filter type:%@",responseMessage);
            //逻辑1.for循环每次只添加一个地址
            //逻辑2.一次添加多个地址
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf addAddressesToFilterWithAddresses:addresses successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
//                    TelinkLogVerbose(@"responseMessage.listSize=%d",responseMessage.listSize);
                    SigMeshLib.share.dataSource.unicastAddressOfConnected = source;
                    if (successCallback) {
                        successCallback(source,destination,responseMessage);
                    }
                } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                    TelinkLogVerbose(@"add address,isResponseAll=%d,error:%@",isResponseAll,error);
                    if (failCallback) {
                        failCallback(error==nil,error);
                    }
                }];
            });
        } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//            TelinkLogVerbose(@"filter type,isResponseAll=%d,error:%@",isResponseAll,error);
            if (error != nil) {
                if (failCallback) {
                    failCallback(NO,error);
                }
            }
        }];

    }];
     */

    //v4.1.0.0及之后版本，开发者可以自定义配置proxy filter. 从SigMeshLib.share.dataSource.filterModel取出filter的配置。
    NSMutableArray *addresses = [NSMutableArray arrayWithArray:SigMeshLib.share.dataSource.filterModel.addressList];
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        if (SigMeshLib.share.meshPrivateBeacon && (SigMeshLib.share.sendBeaconType == AppSendBeaconType_auto || SigMeshLib.share.sendBeaconType == AppSendBeaconType_meshPrivateBeacon)) {
            [weakSelf sendMeshPrivateBeacon];
        } else {
            [weakSelf sendSecureNetworkBeacon];
        }
        [NSThread sleepForTimeInterval:0.1];
        TelinkLogVerbose(@"filter addresses:%@",addresses);
        [self setType:SigMeshLib.share.dataSource.filterModel.filterType successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
            TelinkLogVerbose(@"filter type:%@",responseMessage);
            //逻辑1.for循环每次只添加一个地址
            //逻辑2.一次添加多个地址
            dispatch_async(dispatch_get_main_queue(), ^{
                [weakSelf addAddressesToFilterWithAddresses:addresses successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
//                    TelinkLogVerbose(@"responseMessage.listSize=%d",responseMessage.listSize);
                    SigMeshLib.share.dataSource.unicastAddressOfConnected = source;
                    if (successCallback) {
                        successCallback(source,destination,responseMessage);
                    }
                } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                    TelinkLogVerbose(@"add address,isResponseAll=%d,error:%@",isResponseAll,error);
                    if (failCallback) {
                        failCallback(error==nil,error);
                    }
                }];
            });
        } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//            TelinkLogVerbose(@"filter type,isResponseAll=%d,error:%@",isResponseAll,error);
            if (error != nil) {
                if (failCallback) {
                    failCallback(NO,error);
                }
            }
        }];
    }];
}


#pragma mark - API by Telink


/**
 * @brief   Get Online device status by Telink private OnlineStatusCharacteristic.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    Get the online, on/off, brightness, and color temperature status of all devices in the current mesh network (private customization, specific OnlineStatusCharacteristic is required)
 */
+ (nullable NSError *)telinkApiGetOnlineStatueFromUUIDWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigGenericOnOffGet *message = [[SigGenericOnOffGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendTelinkApiGetOnlineStatueFromUUIDWithMessage:message command:command];
}

/**
 * @brief   Read the value of OTA Charachteristic.
 * @param   timeout    the value of timeout.
 * @param   complete    callback when value of OTA Charachteristic had response or timeout.
 */
+ (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete {
    [SigBluetooth.share readOTACharachteristicWithTimeout:timeout complete:complete];
}

/**
 * @brief   Cancel the timeout action of Read the value of OTA Charachteristic.
 * @note    callback when call stopOTA or trigger otaFailed.
 */
+ (void)cancelReadOTACharachteristic {
    [SigBluetooth.share cancelReadOTACharachteristic];
}

/**
 * @brief   Send customs command.
 * @param   model    customs command object.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 */
+ (nullable SigMessageHandle *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    UInt16 op = model.opcode;
    if (model.opcode > 0xff) {
        op = ((model.opcode & 0xff) << 8) | (model.opcode >> 8);
    }
    if (model.vendorId > 0 && model.responseOpcode == 0) {
        model.retryCount = 0;
        TelinkLogWarn(@"change retryCount to 0.");
    }
    BOOL reliable = [self isReliableCommandWithOpcode:op vendorOpcodeResponse:model.responseOpcode];
    if ([SigHelper.share isUnicastAddress:model.address] && reliable && model.responseMax > 1) {
        model.responseMax = 1;
        TelinkLogWarn(@"change responseMax to 1.");
    }
    TelinkLogVerbose(@"ini data:0x%@", [LibTools convertDataToHexStr:model.commandData]);
    SigIniMeshMessage *message = [[SigIniMeshMessage alloc] initWithParameters:model.commandData];
    if (model.vendorId) {
        message.opCode = (op << 16) | ((model.vendorId & 0xff) << 8) | (model.vendorId >> 8);
        //vendor的控制指令的tid特殊处理:添加tid在commandData里面tidPosition-1的位置。
        if (model.tidPosition) {
            NSMutableData *mData = [NSMutableData dataWithData:model.commandData];
            UInt8 tid = model.tid;
            [mData replaceBytesInRange:NSMakeRange(model.tidPosition-1, 1) withBytes:&tid length:1];
            message.parameters = mData;
        } else {
            message.parameters = [NSData dataWithData:model.commandData];
        }
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
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:model.retryCount responseMaxCount:model.responseMax responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.curNetkey = model.curNetkey;
    command.curAppkey = model.curAppkey;
    command.curIvIndex = model.curIvIndex;
    command.hadRetryCount = 0;
    command.tidPosition = model.tidPosition;
    command.tid = model.tid;
    if (model.timeout) {
        command.timeout = model.timeout;
    }

    if (model.isEncryptByDeviceKey) {
        //mesh数据使用DeviceKey进行加密
        return [SigMeshLib.share sendConfigMessage:(SigConfigMessage *)message toDestination:model.address command:command];
    } else {
        SigMeshAddress *destination = [[SigMeshAddress alloc] initWithAddress:model.address];
        if (model.meshAddressModel) {
            destination = model.meshAddressModel;
        }
        //mesh数据使用AppKey进行加密
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:destination usingApplicationKey:model.curAppkey command:command];
    }
}

/**
 * @brief   Send customs command by Hex data.(Hex data like "a3ff000000000200ffffc21102c4020100".)
 * @param   iniData    customs command Hex data.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 */
+ (nullable SigMessageHandle *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    IniCommandModel *model = [[IniCommandModel alloc] initWithIniCommandData:iniData];
    if (SigMeshLib.share.dataSource.curNetkeyModel.index != model.netkeyIndex) {
        BOOL has = NO;
        for (SigNetkeyModel *netkey in SigMeshLib.share.dataSource.netKeys) {
            if (netkey.index == model.netkeyIndex) {
                has = YES;
                SigMeshLib.share.dataSource.curNetkeyModel = netkey;
                break;
            }
        }
        if (!has) {
            TelinkLogError(@"%@",kSigMeshLibCommandInvalidNetKeyIndexErrorMessage);
            return nil;
        }
    }
    if (SigMeshLib.share.dataSource.curAppkeyModel.index != model.appkeyIndex) {
        BOOL has = NO;
        for (SigAppkeyModel *appkey in SigMeshLib.share.dataSource.appKeys) {
            if (appkey.index == model.appkeyIndex) {
                has = YES;
                SigMeshLib.share.dataSource.curAppkeyModel = appkey;
                break;
            }
        }
        if (!has) {
            TelinkLogError(@"%@",kSigMeshLibCommandInvalidAppKeyIndexErrorMessage);
            return nil;
        }
    }
    return [self sendIniCommandModel:model successCallback:successCallback resultCallback:resultCallback];
}

+ (BOOL)isReliableCommandWithOpcode:(UInt16)opcode vendorOpcodeResponse:(UInt32)vendorOpcodeResponse {
    if ([SigHelper.share getOpCodeTypeWithOpcode:opcode] == SigOpCodeType_vendor3) {
        return ((vendorOpcodeResponse & 0xff) != 0);
    } else {
        int responseOpCode = [SigHelper.share getResponseOpcodeWithSendOpcode:opcode];
        return responseOpCode != 0;
    }
}

/**
 * @brief   Start Telink SigMesh SDK.
 */
+ (void)startMeshSDK {
    //初始化本地存储的mesh网络数据
    [SigMeshLib.share.dataSource configData];

    //初始化ECC算法的公钥(iphone 6s耗时0.6~1.3秒，放到背景线程调用)
    [SigECCEncryptHelper.share performSelectorInBackground:@selector(eccInit) withObject:nil];

    //初始化添加设备的参数
    [SigAddDeviceManager.share setNeedDisconnectBetweenProvisionToKeyBind:NO];

    //初始化蓝牙
    [[SigBluetooth share] bleInit:^(CBCentralManager * _Nonnull central) {
        TelinkLogInfo(@"finish init SigBluetooth.");
        [SigMeshLib share];
    }];

//    ///默认为NO，连接速度更加快。设置为YES，表示扫描到的设备必须包含MacAddress，有些客户在添加流程需要通过MacAddress获取三元组信息，需要使用YES。
//    [SigBluetooth.share setWaitScanRseponseEnable:YES];
}

/**
 * @brief   Returns whether Bluetooth has been initialized.
 * @return  YES means Bluetooth has been initialized, NO means other.
 */
+ (BOOL)isBLEInitFinish {
    return [SigBluetooth.share isBLEInitFinish];
}

/**
 * @brief   Send Secure Network Beacon.
 * @note    SDK will auto send Secure Network Beacon or Mesh Private Beacon when app receive ivBeacon from node.
 */
+ (void)sendSecureNetworkBeacon {
    if (SigMeshLib.share.secureNetworkBeacon) {
        //APP端已经接收到设备端上报的SecureNetworkBeacon,无需处理。

    } else {
        //APP端未接收到设备端上报的SecureNetworkBeacon,需要根据APP端的SequenceNumber生成SecureNetworkBeacon。
        if (SigMeshLib.share.dataSource.getSequenceNumberUInt32 >= 0xc00000) {
            SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:YES networkId:SigMeshLib.share.dataSource.curNetkeyModel.networkId ivIndex:SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index+1 usingNetworkKey:SigMeshLib.share.dataSource.curNetkeyModel];
            SigMeshLib.share.secureNetworkBeacon = beacon;
        } else {
            SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:NO networkId:SigMeshLib.share.dataSource.curNetkeyModel.networkId ivIndex:SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index usingNetworkKey:SigMeshLib.share.dataSource.curNetkeyModel];
            SigMeshLib.share.secureNetworkBeacon = beacon;
        }

    }
    SigSecureNetworkBeacon *beacon = SigMeshLib.share.secureNetworkBeacon;
    TelinkLogInfo(@"send SecureNetworkBeacon=%@",beacon);
    [SigBearer.share sendBlePdu:beacon ofType:SigPduType_meshBeacon];
}

/**
 * @brief   Send Mesh Private Beacon.
 * @note    SDK will auto send Secure Network Beacon or Mesh Private Beacon when app receive ivBeacon from node.
 */
+ (void)sendMeshPrivateBeacon {
    if (SigMeshLib.share.meshPrivateBeacon) {
        //APP端已经接收到设备端上报的meshPrivateBeacon,无需处理。

    } else {
        //APP端未接收到设备端上报的meshPrivateBeacon,需要根据APP端的SequenceNumber生成meshPrivateBeacon。
        if (SigMeshLib.share.dataSource.getSequenceNumberUInt32 >= 0xc00000) {
            SigMeshPrivateBeacon *beacon = [[SigMeshPrivateBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:YES ivIndex:SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index+1 randomData:[LibTools createRandomDataWithLength:13] usingNetworkKey:SigMeshLib.share.dataSource.curNetkeyModel];
            SigMeshLib.share.meshPrivateBeacon = beacon;
        } else {
            SigMeshPrivateBeacon *beacon = [[SigMeshPrivateBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:NO ivIndex:SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index randomData:[LibTools createRandomDataWithLength:13] usingNetworkKey:SigMeshLib.share.dataSource.curNetkeyModel];
            SigMeshLib.share.meshPrivateBeacon = beacon;
        }
    }
    SigMeshPrivateBeacon *beacon = SigMeshLib.share.meshPrivateBeacon;
    TelinkLogInfo(@"send meshPrivateBeacon=%@",beacon);
    [SigBearer.share sendBlePdu:beacon ofType:SigPduType_meshBeacon];
}

/**
 * @brief   Update ivIndex by APP.
 * @note    SDK will update ivIndex automatically, when sequenceNumber of APP is more than 0xC00000.
 */
+ (void)updateIvIndexWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey {
    SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithKeyRefreshFlag:keyRefreshFlag ivUpdateActive:ivUpdateActive networkId:networkId ivIndex:ivIndex usingNetworkKey:networkKey];
    TelinkLogInfo(@"send updateIvIndex SecureNetworkBeacon=%@",[LibTools convertDataToHexStr:beacon.pduData]);
    [SigBearer.share sendBlePdu:beacon ofType:SigPduType_meshBeacon];
}

/**
 * @brief   Update the now time to node without response.
 * @note    SDK will call this api automatically, when SDK set filter success.
 */
+ (void)statusNowTime {
    //time_auth = 1;//每次无条件接受这个时间指令。
    UInt64 seconds = (UInt64)[LibTools secondsFrom2000];
    [NSTimeZone resetSystemTimeZone]; // 重置手机系统的时区
    NSInteger offset = [NSTimeZone localTimeZone].secondsFromGMT;
    UInt8 zoneOffset = (UInt8)(offset/60/15+64);//时区=分/15+64
    SigTimeModel *timeModel = [[SigTimeModel alloc] initWithTAISeconds:seconds subSeconds:0 uncertainty:0 timeAuthority:1 TAI_UTC_Delta:0 timeZoneOffset:zoneOffset];
    [self timeStatusWithDestination:kMeshAddress_allNodes timeModel:timeModel retryCount:0 responseMaxCount:0 successCallback:nil resultCallback:nil];
}

/**
 * @brief   Add Single Device (provision+keyBind)
 * @param   configModel all config message of add device.
 * @param   capabilitiesResponse callback when capabilities response.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @note    SDK will call this api automatically, when SDK set filter success.
 * @note    parameter of SigAddConfigModel:
 *  1.normal provision + normal keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+keyBindType:KeyBindType_Normal
 *  2.normal provision + fast keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+keyBindType:KeyBindType_Fast+productID+cpsData
 *  3.static oob provision(cloud oob) + normal keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+staticOOBData+keyBindType:KeyBindType_Normal
 *  4.static oob provision(cloud oob) + fast keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+staticOOBData+keyBindType:KeyBindType_Fast+productID+cpsData
 */
+ (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    [SigAddDeviceManager.share startAddDeviceWithSigAddConfigModel:configModel capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

/**
 * @brief   Provision Method, calculate unicastAddress when capabilities response from unProvision node. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   networkKey network key.
 * @param   netkeyIndex netkey index.
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   capabilitiesResponse callback when capabilities response, app need return unicastAddress for provision process.
 * @param   provisionSuccess callback when provision success.
 * @param   fail callback when provision fail.
 * @note    1.get provision type from Capabilities response.
 * 2.Static OOB Type is NO_OOB, SDK will call no oob provision.
 * 3.Static OOB Type is Static_OOB, SDK will call static oob provision.
 * 4.if it is no static oob data when Static OOB Type is Static_OOB, SDK will try on oob provision when SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable is YES.
 */
+ (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(NSData *)staticOOBData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    [SigProvisioningManager.share provisionWithPeripheral:peripheral networkKey:networkKey netkeyIndex:netkeyIndex staticOOBData:staticOOBData capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess fail:fail];
}

/**
 * @brief   Keybind Method1. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   unicastAddress new unicastAddress of keybind node.
 * @param   appkey appkey
 * @param   appkeyIndex appkeyIndex
 * @param   netkeyIndex netkeyIndex
 * @param   keyBindType KeyBindType_Normal means add appkey and model bind, KeyBindType_Fast means just add appkey.
 * @param   productID the productID info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   cpsData the elements info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   keyBindSuccess callback when keybind success.
 * @param   fail callback when keybind fail.
 */
+ (void)startKeyBindWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex netkeyIndex:(UInt16)netkeyIndex keyBindType:(KeyBindType)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail {
    [SigBearer.share connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
        if (successful) {
            SigAppkeyModel *appkeyModel = [SigMeshLib.share.dataSource getAppkeyModelWithAppkeyIndex:appkeyIndex];
            if (!appkeyModel || ![appkeyModel.getDataKey isEqualToData:appkey] || netkeyIndex != appkeyModel.boundNetKey) {
                TelinkLogVerbose(@"appkey is error.");
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

/**
 * @brief   Keybind Method2. (SDK need to connect mesh in first )
 * @param   address new unicastAddress of keybind node.
 * @param   appkeyModel appkeyModel
 * @param   type KeyBindType_Normal means add appkey and model bind, KeyBindType_Fast means just add appkey.
 * @param   productID the productID info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   cpsData the elements info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   keyBindSuccess callback when keybind success.
 * @param   fail callback when keybind fail.
 */
+ (void)keyBind:(UInt16)address appkeyModel:(SigAppkeyModel *)appkeyModel keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail {
    [SigKeyBindManager.share keyBind:address appkeyModel:appkeyModel keyBindType:type productID:productID cpsData:cpsData keyBindSuccess:keyBindSuccess fail:fail];
}

/**
 * @brief   Get Characteristic of peripheral.
 * @param   uuid UUIDString of Characteristic.
 * @param   peripheral the CBPeripheral object.
 * @return  A CBCharacteristic object.
 */
+ (CBCharacteristic *)getCharacteristicWithUUIDString:(NSString *)uuid OfPeripheral:(CBPeripheral *)peripheral {
    return [SigBluetooth.share getCharacteristicWithUUIDString:uuid OfPeripheral:peripheral];
}

/**
 * @brief   Set handle of BluetoothCentralUpdateState.
 * @param   bluetoothCentralUpdateStateCallback callback of BluetoothCentralUpdateState.
 */
+ (void)setBluetoothCentralUpdateStateCallback:(_Nullable bleCentralUpdateStateCallback)bluetoothCentralUpdateStateCallback {
    [SigBluetooth.share setBluetoothCentralUpdateStateCallback:bluetoothCentralUpdateStateCallback];
}

/**
 * @brief   Start connecting to the single column mesh network of SigDataSource.
 * @param   complete callback of connect mesh complete.
 * @note    Internally, it will retry every 10 seconds until the connection is successful or the 'stopMeshConnectWithComplete' call is made to stop the connection:`
 */
+ (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete {
    [SigBearer.share startMeshConnectWithComplete:complete];
}

/**
 * @brief   Disconnect mesh connect.
 * @param   complete callback of connect mesh complete.
 * @note    It will use in switch mesh network.
 */
+ (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete {
    [SigBearer.share stopMeshConnectWithComplete:complete];
}


#pragma mark - Scan API


/**
 * @brief   Scan unprovisioned devices.
 * @param   result Report once when a device is scanned.
 */
+ (void)scanUnprovisionedDevicesWithResult:(bleScanPeripheralCallback)result {
    [SigBluetooth.share scanUnprovisionedDevicesWithResult:result];
}

/**
 * @brief   Scan provisioned devices.
 * @param   result Report once when a device is scanned.
 */
+ (void)scanProvisionedDevicesWithResult:(bleScanPeripheralCallback)result {
    [SigBluetooth.share scanProvisionedDevicesWithResult:result];
}

/**
 * @brief   Scan devices with ServiceUUIDs.
 * @param   UUIDs ServiceUUIDs.
 * @param   checkNetworkEnable YES means the device must belong current mesh network.
 * @param   result Report once when a device is scanned.
 */
+ (void)scanWithServiceUUIDs:(NSArray <CBUUID *>* _Nonnull)UUIDs checkNetworkEnable:(BOOL)checkNetworkEnable result:(bleScanPeripheralCallback)result {
    [SigBluetooth.share scanWithServiceUUIDs:UUIDs checkNetworkEnable:checkNetworkEnable result:result];
}

/**
 * @brief   Scan devices with specified UUID.
 * @param   peripheralUUID uuid of peripheral.
 * @param   timeout timeout of scan.
 * @param   block Report when the specified device is scanned.
 */
+ (void)scanMeshNodeWithPeripheralUUID:(NSString *)peripheralUUID timeout:(NSTimeInterval)timeout resultBlock:(bleScanSpecialPeripheralCallback)block {
    [SigBluetooth.share scanMeshNodeWithPeripheralUUID:peripheralUUID timeout:timeout resultBlock:block];
}

/**
 * @brief   Stop scan action.
 */
+ (void)stopScan {
    [SigBluetooth.share stopScan];
}


#pragma mark - deprecated API


/**
 * @brief   Deprecated Add Device Method1 (auto add)
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   uuid uuid of remote device.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   isAuto 添加完成一个设备后，是否自动扫描添加下一个设备.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @param   finish callback when add device finish.
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish {
    [SigAddDeviceManager.share startAddDeviceWithNextAddress:address networkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel uuid:uuid keyBindType:type productID:productID cpsData:cpsData isAutoAddNextDevice:isAuto provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail finish:finish];
}

/**
 * @brief   Deprecated Add Device Method2 (auto add), add new callback of startConnect and startProvision.
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   uuid uuid of remote device.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   isAuto 添加完成一个设备后，是否自动扫描添加下一个设备.
 * @param   startConnect callback when SDK start connect node.
 * @param   startProvision callback when SDK start provision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @param   finish callback when add device finish.
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish {
    [SigAddDeviceManager.share startAddDeviceWithNextAddress:address networkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel uuid:uuid keyBindType:type productID:productID cpsData:cpsData isAutoAddNextDevice:isAuto startConnect:startConnect startProvision:startProvision provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail finish:finish];
}

/**
 * @brief   Deprecated Add Device Method3 (single add)
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    [SigAddDeviceManager.share startAddDeviceWithNextAddress:address networkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel peripheral:peripheral staticOOBData:staticOOBData keyBindType:type productID:productID cpsData:cpsData provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

/**
 * @brief   Deprecated Add Device Method4 (single add), calculate unicastAddress when capabilities response from unProvision node.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   capabilitiesResponse callback when capabilities response from unProvision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
+ (void)startAddDeviceWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData capabilitiesResponse:(nullable addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    [SigAddDeviceManager.share startAddDeviceWithNetworkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel peripheral:peripheral staticOOBData:staticOOBData keyBindType:type productID:productID cpsData:cpsData capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

/**
 * @brief   Deprecated Provision Method. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   unicastAddress new unicastAddress of provision node.
 * @param   networkKey network key.
 * @param   netkeyIndex netkey index.
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   provisionSuccess callback when provision success.
 * @param   fail callback when provision fail.
 * @note    1.get provision type from Capabilities response.
 * 2.Static OOB Type is NO_OOB, SDK will call no oob provision.
 * 3.Static OOB Type is Static_OOB, SDK will call static oob provision.
 * 4.if it is no static oob data when Static OOB Type is Static_OOB, SDK will try on oob provision when SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable is YES.
 */
+ (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    [SigProvisioningManager.share provisionWithPeripheral:peripheral unicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex staticOOBData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
}

@end
