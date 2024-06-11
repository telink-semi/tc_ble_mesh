/********************************************************************************************************
 * @file     TelinkCloudModel.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/12/4
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

#import "TelinkCloudModel.h"

@implementation TelinkCloudModel

@end


/// user info of cloud.
@implementation UserInfoModel

/// get dictionary from UserInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfUserInfoModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_company) {
        dict[@"company"] = _company;
    }
    if (_email) {
        dict[@"email"] = _email;
    }
    if (_lastLoginTime) {
        dict[@"lastLoginTime"] = _lastLoginTime;
    }
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_nickname) {
        dict[@"nickname"] = _nickname;
    }
    if (_password) {
        dict[@"password"] = _password;
    }
    if (_phone) {
        dict[@"phone"] = _phone;
    }
    if (_sign) {
        dict[@"sign"] = _sign;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"id"] = [NSNumber numberWithInteger:_userId];
    return dict;
}

/// Set dictionary to UserInfoModel object.
/// @param dictionary UserInfoModel dictionary object.
- (void)setDictionaryToUserInfoModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"company"] && ![dictionary[@"company"] isMemberOfClass:[NSNull class]]) {
        _company = dictionary[@"company"];
    }
    if ([allKeys containsObject:@"email"] && ![dictionary[@"email"] isMemberOfClass:[NSNull class]]) {
        _email = dictionary[@"email"];
    }
    if ([allKeys containsObject:@"lastLoginTime"] && ![dictionary[@"lastLoginTime"] isMemberOfClass:[NSNull class]]) {
        _lastLoginTime = dictionary[@"lastLoginTime"];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"nickname"] && ![dictionary[@"nickname"] isMemberOfClass:[NSNull class]]) {
        _nickname = dictionary[@"nickname"];
    }
    if ([allKeys containsObject:@"password"] && ![dictionary[@"password"] isMemberOfClass:[NSNull class]]) {
        _password = dictionary[@"password"];
    }
    if ([allKeys containsObject:@"phone"] && ![dictionary[@"phone"] isMemberOfClass:[NSNull class]]) {
        _phone = dictionary[@"phone"];
    }
    if ([allKeys containsObject:@"sign"] && ![dictionary[@"sign"] isMemberOfClass:[NSNull class]]) {
        _sign = dictionary[@"sign"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _userId = [dictionary[@"id"] intValue];
    }
}

@end


/// Mesh info of created network list in the cloud.
@implementation AppMeshNetworkModel

/// get dictionary from AppMeshNetworkModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfAppMeshNetworkModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"createUserId"] = [NSNumber numberWithInteger:_createUserId];
    dict[@"id"] = [NSNumber numberWithInteger:_networkId];
    dict[@"ivIndex"] = [NSNumber numberWithInteger:_ivIndex];
    dict[@"nodeAddressIndex"] = [NSNumber numberWithInteger:_nodeAddressIndex];
    dict[@"provisionerAddressIndex"] = [NSNumber numberWithInteger:_provisionerAddressIndex];
    dict[@"sharePermission"] = [NSNumber numberWithInteger:_sharePermission];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_networkSchema) {
        dict[@"networkSchema"] = _networkSchema;
    }
    if (_uuid) {
        dict[@"uuid"] = _uuid;
    }
    if (_version) {
        dict[@"version"] = _version;
    }
    if (_createUserName) {
        dict[@"createUserName"] = _createUserName;
    }
    return dict;
}

/// Set dictionary to AppMeshNetworkModel object.
/// @param dictionary AppMeshNetworkModel dictionary object.
- (void)setDictionaryToAppMeshNetworkModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"createUserId"] && ![dictionary[@"createUserId"] isMemberOfClass:[NSNull class]]) {
        _createUserId = [dictionary[@"createUserId"] intValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"ivIndex"] && ![dictionary[@"ivIndex"] isMemberOfClass:[NSNull class]]) {
        _ivIndex = [dictionary[@"ivIndex"] intValue];
    }
    if ([allKeys containsObject:@"nodeAddressIndex"] && ![dictionary[@"nodeAddressIndex"] isMemberOfClass:[NSNull class]]) {
        _nodeAddressIndex = [dictionary[@"nodeAddressIndex"] intValue];
    }
    if ([allKeys containsObject:@"provisionerAddressIndex"] && ![dictionary[@"provisionerAddressIndex"] isMemberOfClass:[NSNull class]]) {
        _provisionerAddressIndex = [dictionary[@"provisionerAddressIndex"] intValue];
    }
    if ([allKeys containsObject:@"sharePermission"] && ![dictionary[@"sharePermission"] isMemberOfClass:[NSNull class]]) {
        _sharePermission = [dictionary[@"sharePermission"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"networkSchema"] && ![dictionary[@"networkSchema"] isMemberOfClass:[NSNull class]]) {
        _networkSchema = dictionary[@"networkSchema"];
    }
    if ([allKeys containsObject:@"uuid"] && ![dictionary[@"uuid"] isMemberOfClass:[NSNull class]]) {
        _uuid = dictionary[@"uuid"];
    }
    if ([allKeys containsObject:@"version"] && ![dictionary[@"version"] isMemberOfClass:[NSNull class]]) {
        _version = dictionary[@"version"];
    }
    if ([allKeys containsObject:@"createUserName"] && ![dictionary[@"createUserName"] isMemberOfClass:[NSNull class]]) {
        _createUserName = dictionary[@"createUserName"];
    }
}

@end


@implementation AppJoinedNetworkModel

- (NSDictionary *)getDictionaryOfAppJoinedNetworkModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"updateTime"] = [NSNumber numberWithLong:_updateTime];
    dict[@"applyUserId"] = [NSNumber numberWithInteger:_applyUserId];
    dict[@"joinNetworkId"] = [NSNumber numberWithInteger:_joinNetworkId];
    dict[@"networkId"] = [NSNumber numberWithInteger:_networkId];
    dict[@"state"] = [NSNumber numberWithInteger:_state];
    if (_applyUserName) {
        dict[@"applyUserName"] = _applyUserName;
    }
    if (_networkCreator) {
        dict[@"networkCreator"] = _networkCreator;
    }
    if (_networkName) {
        dict[@"networkName"] = _networkName;
    }
    return dict;
}

- (void)setDictionaryToAppJoinedNetworkModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = [dictionary[@"updateTime"] intValue];
    }
    if ([allKeys containsObject:@"applyUserId"] && ![dictionary[@"applyUserId"] isMemberOfClass:[NSNull class]]) {
        _applyUserId = [dictionary[@"applyUserId"] intValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _joinNetworkId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"networkId"] && ![dictionary[@"networkId"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"networkId"] intValue];
    }
    if ([allKeys containsObject:@"state"] && ![dictionary[@"state"] isMemberOfClass:[NSNull class]]) {
        _state = [dictionary[@"state"] intValue];
    }
    if ([allKeys containsObject:@"applyUserName"] && ![dictionary[@"applyUserName"] isMemberOfClass:[NSNull class]]) {
        _applyUserName = dictionary[@"applyUserName"];
    }
    if ([allKeys containsObject:@"networkCreator"] && ![dictionary[@"networkCreator"] isMemberOfClass:[NSNull class]]) {
        _networkCreator = dictionary[@"networkCreator"];
    }
    if ([allKeys containsObject:@"networkName"] && ![dictionary[@"networkName"] isMemberOfClass:[NSNull class]]) {
        _networkName = dictionary[@"networkName"];
    }
}

@end


/// Application key info  in the cloud.
@implementation CloudApplicationKeyModel

/// get dictionary from CloudApplicationKeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudApplicationKeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"id"] = [NSNumber numberWithInteger:_applicationKeyId];
    dict[@"networkId"] = [NSNumber numberWithInteger:_networkId];
    dict[@"networkKeyId"] = [NSNumber numberWithInteger:_networkKeyId];
    dict[@"index"] = [NSNumber numberWithInteger:_index];
    if (_modelDescription) {
        dict[@"description"] = _modelDescription;
    }
    if (_key) {
        dict[@"key"] = _key;
    }
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_oldKey) {
        dict[@"oldKey"] = _oldKey;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    return dict;
}

/// Set dictionary to CloudApplicationKeyModel object.
/// @param dictionary CloudApplicationKeyModel dictionary object.
- (void)setDictionaryToCloudApplicationKeyModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _applicationKeyId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"networkId"] && ![dictionary[@"networkId"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"networkId"] intValue];
    }
    if ([allKeys containsObject:@"networkKeyId"] && ![dictionary[@"networkKeyId"] isMemberOfClass:[NSNull class]]) {
        _networkKeyId = [dictionary[@"networkKeyId"] intValue];
    }
    if ([allKeys containsObject:@"index"] && ![dictionary[@"index"] isMemberOfClass:[NSNull class]]) {
        _index = [dictionary[@"index"] intValue];
    }
    if ([allKeys containsObject:@"description"] && ![dictionary[@"description"] isMemberOfClass:[NSNull class]]) {
        _modelDescription = dictionary[@"description"];
    }
    if ([allKeys containsObject:@"key"] && ![dictionary[@"key"] isMemberOfClass:[NSNull class]]) {
        _key = dictionary[@"key"];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"oldKey"] && ![dictionary[@"oldKey"] isMemberOfClass:[NSNull class]]) {
        _oldKey = dictionary[@"oldKey"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
}

@end


/// Group info  in the cloud.
@implementation CloudGroupModel

/// get dictionary from CloudGroupModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudGroupModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"address"] = [NSNumber numberWithInteger:_address];
    dict[@"createProvisionerId"] = [NSNumber numberWithInteger:_createProvisionerId];
    dict[@"createUserId"] = [NSNumber numberWithInteger:_createUserId];
    dict[@"id"] = [NSNumber numberWithInteger:_groupId];
    dict[@"networkId"] = [NSNumber numberWithInteger:_networkId];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    return dict;
}

/// Set dictionary to CloudGroupModel object.
/// @param dictionary CloudGroupModel dictionary object.
- (void)setDictionaryToCloudGroupModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"address"] && ![dictionary[@"address"] isMemberOfClass:[NSNull class]]) {
        _address = [dictionary[@"address"] intValue];
    }
    if ([allKeys containsObject:@"createProvisionerId"] && ![dictionary[@"createProvisionerId"] isMemberOfClass:[NSNull class]]) {
        _createProvisionerId = [dictionary[@"createProvisionerId"] intValue];
    }
    if ([allKeys containsObject:@"createUserId"] && ![dictionary[@"createUserId"] isMemberOfClass:[NSNull class]]) {
        _createUserId = [dictionary[@"createUserId"] intValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _groupId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"networkId"] && ![dictionary[@"networkId"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"networkId"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
}

@end


/// Network key info  in the cloud.
@implementation CloudNetworkKeyModel

/// get dictionary from CloudNetworkKeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNetworkKeyModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"phase"] = [NSNumber numberWithInteger:_phase];
    dict[@"networkId"] = [NSNumber numberWithInteger:_networkId];
    dict[@"id"] = [NSNumber numberWithInteger:_networkKeyId];
    dict[@"index"] = [NSNumber numberWithInteger:_index];
    if (_modelDescription) {
        dict[@"description"] = _modelDescription;
    }
    if (_key) {
        dict[@"key"] = _key;
    }
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_oldKey) {
        dict[@"oldKey"] = _oldKey;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    return dict;
}

/// Set dictionary to CloudNetworkKeyModel object.
/// @param dictionary CloudNetworkKeyModel dictionary object.
- (void)setDictionaryToCloudNetworkKeyModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _networkKeyId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"networkId"] && ![dictionary[@"networkId"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"networkId"] intValue];
    }
    if ([allKeys containsObject:@"networkKeyId"] && ![dictionary[@"networkKeyId"] isMemberOfClass:[NSNull class]]) {
        _networkKeyId = [dictionary[@"networkKeyId"] intValue];
    }
    if ([allKeys containsObject:@"index"] && ![dictionary[@"index"] isMemberOfClass:[NSNull class]]) {
        _index = [dictionary[@"index"] intValue];
    }
    if ([allKeys containsObject:@"description"] && ![dictionary[@"description"] isMemberOfClass:[NSNull class]]) {
        _modelDescription = dictionary[@"description"];
    }
    if ([allKeys containsObject:@"key"] && ![dictionary[@"key"] isMemberOfClass:[NSNull class]]) {
        _key = dictionary[@"key"];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"oldKey"] && ![dictionary[@"oldKey"] isMemberOfClass:[NSNull class]]) {
        _oldKey = dictionary[@"oldKey"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
}

@end


/// Product info  in the cloud.
@implementation CloudProductInfoModel

/// get dictionary from CloudProductInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudProductInfoModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"id"] = [NSNumber numberWithInteger:_productInfoId];
    dict[@"manufactureId"] = [NSNumber numberWithInteger:_manufactureId];
    dict[@"pid"] = [NSNumber numberWithInteger:_pid];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    if (_info) {
        dict[@"info"] = _info;
    }
    if (_manufactureName) {
        dict[@"manufactureName"] = _manufactureName;
    }
    return dict;
}

/// Set dictionary to CloudProductInfoModel object.
/// @param dictionary CloudProductInfoModel dictionary object.
- (void)setDictionaryToCloudProductInfoModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _productInfoId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"manufactureId"] && ![dictionary[@"manufactureId"] isMemberOfClass:[NSNull class]]) {
        _manufactureId = [dictionary[@"manufactureId"] intValue];
    }
    if ([allKeys containsObject:@"pid"] && ![dictionary[@"pid"] isMemberOfClass:[NSNull class]]) {
        _pid = [dictionary[@"pid"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
    if ([allKeys containsObject:@"info"] && ![dictionary[@"info"] isMemberOfClass:[NSNull class]]) {
        _info = dictionary[@"info"];
    }
    if ([allKeys containsObject:@"manufactureName"] && ![dictionary[@"manufactureName"] isMemberOfClass:[NSNull class]]) {
        _manufactureName = dictionary[@"manufactureName"];
    }
}

@end


/// Publish info  in the cloud.
@implementation CloudPublishInfoModel

/// get dictionary from CloudPublishInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudPublishInfoModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"appKeyIndex"] = [NSNumber numberWithInteger:_appKeyIndex];
    dict[@"credentialFlag"] = [NSNumber numberWithInteger:_credentialFlag];
    dict[@"elementAddress"] = [NSNumber numberWithInteger:_elementAddress];
    dict[@"id"] = [NSNumber numberWithInteger:_publishId];
    dict[@"isSigModel"] = [NSNumber numberWithInteger:_isSigModel];
    dict[@"modelId"] = [NSNumber numberWithInteger:_modelId];
    dict[@"nodeId"] = [NSNumber numberWithInteger:_nodeId];
    dict[@"period"] = [NSNumber numberWithInteger:_period];
    dict[@"publishAddress"] = [NSNumber numberWithInteger:_publishAddress];
    dict[@"retransmitCount"] = [NSNumber numberWithInteger:_retransmitCount];
    dict[@"retransmitIntervalSteps"] = [NSNumber numberWithInteger:_retransmitIntervalSteps];
    dict[@"ttl"] = [NSNumber numberWithInteger:_ttl];
    return dict;
}

/// Set dictionary to CloudPublishInfoModel object.
/// @param dictionary CloudPublishInfoModel dictionary object.
- (void)setDictionaryToCloudPublishInfoModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _publishId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"appKeyIndex"] && ![dictionary[@"appKeyIndex"] isMemberOfClass:[NSNull class]]) {
        _appKeyIndex = [dictionary[@"appKeyIndex"] intValue];
    }
    if ([allKeys containsObject:@"credentialFlag"] && ![dictionary[@"credentialFlag"] isMemberOfClass:[NSNull class]]) {
        _credentialFlag = [dictionary[@"credentialFlag"] intValue];
    }
    if ([allKeys containsObject:@"elementAddress"] && ![dictionary[@"elementAddress"] isMemberOfClass:[NSNull class]]) {
        _elementAddress = [dictionary[@"elementAddress"] intValue];
    }
    if ([allKeys containsObject:@"isSigModel"] && ![dictionary[@"isSigModel"] isMemberOfClass:[NSNull class]]) {
        _isSigModel = [dictionary[@"isSigModel"] intValue];
    }
    if ([allKeys containsObject:@"modelId"] && ![dictionary[@"modelId"] isMemberOfClass:[NSNull class]]) {
        _modelId = [dictionary[@"modelId"] intValue];
    }
    if ([allKeys containsObject:@"nodeId"] && ![dictionary[@"nodeId"] isMemberOfClass:[NSNull class]]) {
        _nodeId = [dictionary[@"nodeId"] intValue];
    }
    if ([allKeys containsObject:@"period"] && ![dictionary[@"period"] isMemberOfClass:[NSNull class]]) {
        _period = [dictionary[@"period"] intValue];
    }
    if ([allKeys containsObject:@"publishAddress"] && ![dictionary[@"publishAddress"] isMemberOfClass:[NSNull class]]) {
        _publishAddress = [dictionary[@"publishAddress"] intValue];
    }
    if ([allKeys containsObject:@"retransmitCount"] && ![dictionary[@"retransmitCount"] isMemberOfClass:[NSNull class]]) {
        _retransmitCount = [dictionary[@"retransmitCount"] intValue];
    }
    if ([allKeys containsObject:@"retransmitIntervalSteps"] && ![dictionary[@"retransmitIntervalSteps"] isMemberOfClass:[NSNull class]]) {
        _retransmitIntervalSteps = [dictionary[@"retransmitIntervalSteps"] intValue];
    }
    if ([allKeys containsObject:@"ttl"] && ![dictionary[@"ttl"] isMemberOfClass:[NSNull class]]) {
        _ttl = [dictionary[@"ttl"] intValue];
    }
}

@end


/// Node configs in the cloud.
@implementation CloudNodeConfigsModel

/// get dictionary from CloudNodeConfigsModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNodeConfigsModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"configId"] = [NSNumber numberWithInteger:_configId];
    if (_defaultTtl ) {
        dict[@"ttl"] = _defaultTtl;
    }
    if (_relay) {
        dict[@"relay"] = _relay;
    }
    if (_relayRetransmit) {
        dict[@"relayRetransmit"] = _relayRetransmit;
    }
    if (_secureNetworkBeacon) {
        dict[@"secureNetworkBeacon"] = _secureNetworkBeacon;
    }
    if (_gattProxy) {
        dict[@"gattProxy"] = _gattProxy;
    }
    if (_privateBeacon) {
        dict[@"privateBeacon"] = _privateBeacon;
    }
    if (_privateGattProxy) {
        dict[@"privateGattProxy"] = _privateGattProxy;
    }
    if (_friendConfig) {
        dict[@"friend"] = _friendConfig;
    }
    if (_networkRetransmit) {
        dict[@"networkRetransmit"] = _networkRetransmit;
    }
    if (_onDemandPrivateGattProxy) {
        dict[@"onDemandPrivateGattProxy"] = _onDemandPrivateGattProxy;
    }
    if (_directForwarding) {
        dict[@"directForwarding"] = _directForwarding;
    }
    if (_directRelay) {
        dict[@"directRelay"] = _directRelay;
    }
    if (_directProxy) {
        dict[@"directProxy"] = _directProxy;
    }
    if (_directFriend) {
        dict[@"directFriend"] = _directFriend;
    }
    return dict;
}

/// Set dictionary to CloudNodeConfigsModel object.
/// @param dictionary CloudNodeConfigsModel dictionary object.
- (void)setDictionaryToCloudNodeConfigsModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _configId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"defaultTtl"] && ![dictionary[@"defaultTtl"] isMemberOfClass:[NSNull class]]) {
        _defaultTtl = dictionary[@"defaultTtl"];
    }
    if ([allKeys containsObject:@"relay"] && ![dictionary[@"relay"] isMemberOfClass:[NSNull class]]) {
        _relay = dictionary[@"relay"];
    }
    if ([allKeys containsObject:@"relayRetransmit"] && ![dictionary[@"relayRetransmit"] isMemberOfClass:[NSNull class]]) {
        _relayRetransmit = dictionary[@"relayRetransmit"];
    }
    if ([allKeys containsObject:@"secureNetworkBeacon"] && ![dictionary[@"secureNetworkBeacon"] isMemberOfClass:[NSNull class]]) {
        _secureNetworkBeacon = dictionary[@"secureNetworkBeacon"];
    }
    if ([allKeys containsObject:@"gattProxy"] && ![dictionary[@"gattProxy"] isMemberOfClass:[NSNull class]]) {
        _gattProxy = dictionary[@"gattProxy"];
    }
    if ([allKeys containsObject:@"privateBeacon"] && ![dictionary[@"privateBeacon"] isMemberOfClass:[NSNull class]]) {
        _privateBeacon = dictionary[@"privateBeacon"];
    }
    if ([allKeys containsObject:@"privateGattProxy"] && ![dictionary[@"privateGattProxy"] isMemberOfClass:[NSNull class]]) {
        _privateGattProxy = dictionary[@"privateGattProxy"];
    }
    if ([allKeys containsObject:@"friend"] && ![dictionary[@"friend"] isMemberOfClass:[NSNull class]]) {
        _friendConfig = dictionary[@"friend"];
    }
    if ([allKeys containsObject:@"networkRetransmit"] && ![dictionary[@"networkRetransmit"] isMemberOfClass:[NSNull class]]) {
        _networkRetransmit = dictionary[@"networkRetransmit"];
    }
    if ([allKeys containsObject:@"onDemandPrivateGattProxy"] && ![dictionary[@"onDemandPrivateGattProxy"] isMemberOfClass:[NSNull class]]) {
        _onDemandPrivateGattProxy = dictionary[@"onDemandPrivateGattProxy"];
    }
    if ([allKeys containsObject:@"directForwarding"] && ![dictionary[@"directForwarding"] isMemberOfClass:[NSNull class]]) {
        _directForwarding = dictionary[@"directForwarding"];
    }
    if ([allKeys containsObject:@"directRelay"] && ![dictionary[@"directRelay"] isMemberOfClass:[NSNull class]]) {
        _directRelay = dictionary[@"directRelay"];
    }
    if ([allKeys containsObject:@"directProxy"] && ![dictionary[@"directProxy"] isMemberOfClass:[NSNull class]]) {
        _directProxy = dictionary[@"directProxy"];
    }
    if ([allKeys containsObject:@"directFriend"] && ![dictionary[@"directFriend"] isMemberOfClass:[NSNull class]]) {
        _directFriend = dictionary[@"directFriend"];
    }
}

@end


/// Version info  in the cloud.
@implementation CloudVersionInfoModel

/// get dictionary from CloudVersionInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudVersionInfoModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"elementCount"] = [NSNumber numberWithInteger:_elementCount];
    dict[@"versionInfoId"] = [NSNumber numberWithInteger:_versionInfoId];
    dict[@"productId"] = [NSNumber numberWithInteger:_productId];
    dict[@"state"] = [NSNumber numberWithInteger:_state];
    dict[@"vid"] = [NSNumber numberWithInteger:_vid];
    if (_compositionData) {
        dict[@"compositionData"] = _compositionData;
    }
    if (_binFilePath) {
        dict[@"binFilePath"] = _binFilePath;
    }
    if (_info) {
        dict[@"info"] = _info;
    }
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    return dict;
}

/// Set dictionary to CloudVersionInfoModel object.
/// @param dictionary CloudVersionInfoModel dictionary object.
- (void)setDictionaryToCloudVersionInfoModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"elementCount"] && ![dictionary[@"elementCount"] isMemberOfClass:[NSNull class]]) {
        _elementCount = [dictionary[@"elementCount"] intValue];
    }
    if ([allKeys containsObject:@"versionInfoId"] && ![dictionary[@"versionInfoId"] isMemberOfClass:[NSNull class]]) {
        _versionInfoId = [dictionary[@"versionInfoId"] intValue];
    }
    if ([allKeys containsObject:@"productId"] && ![dictionary[@"productId"] isMemberOfClass:[NSNull class]]) {
        _productId = [dictionary[@"productId"] intValue];
    }
    if ([allKeys containsObject:@"state"] && ![dictionary[@"state"] isMemberOfClass:[NSNull class]]) {
        _state = [dictionary[@"state"] intValue];
    }
    if ([allKeys containsObject:@"vid"] && ![dictionary[@"vid"] isMemberOfClass:[NSNull class]]) {
        _vid = [dictionary[@"vid"] intValue];
    }
    if ([allKeys containsObject:@"compositionData"] && ![dictionary[@"compositionData"] isMemberOfClass:[NSNull class]]) {
        _compositionData = dictionary[@"compositionData"];
    }
    if ([allKeys containsObject:@"binFilePath"] && ![dictionary[@"binFilePath"] isMemberOfClass:[NSNull class]]) {
        _binFilePath = dictionary[@"binFilePath"];
    }
    if ([allKeys containsObject:@"info"] && ![dictionary[@"info"] isMemberOfClass:[NSNull class]]) {
        _info = dictionary[@"info"];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
}

@end


/// Node info  in the cloud.
@implementation CloudNodeModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _subList = [NSMutableArray array];
        _schedulerList = [NSMutableArray array];
    }
    return self;
}

/// get dictionary from CloudNodeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNodeModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"address"] = [NSNumber numberWithInteger:_address];
    dict[@"bindState"] = [NSNumber numberWithInteger:_bindState];
    dict[@"createUserId"] = [NSNumber numberWithInteger:_createUserId];
    dict[@"id"] = [NSNumber numberWithInteger:_nodeId];
    dict[@"meshProvisionerId"] = [NSNumber numberWithInteger:_meshProvisionerId];
    dict[@"networkId"] = [NSNumber numberWithInteger:_networkId];
    dict[@"productId"] = [NSNumber numberWithInteger:_productId];
    dict[@"state"] = [NSNumber numberWithInteger:_state];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_nodeDescription) {
        dict[@"description"] = _nodeDescription;
    }
    if (_deviceKey) {
        dict[@"deviceKey"] = _deviceKey;
    }
    if (_deviceUuid) {
        dict[@"deviceUuid"] = _deviceUuid;
    }
    if (_updateTime) {
        dict[@"updateTime"] = _updateTime;
    }
    if (_versionId) {
        dict[@"versionId"] = _versionId;
    }
    if (_subList && _subList.count > 0) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *groups = [NSArray arrayWithArray:_subList];
        for (CloudGroupModel *model in groups) {
            NSDictionary *groupDict = [model getDictionaryOfCloudGroupModel];
            [array addObject:groupDict];
        }
        dict[@"subList"] = array;
    }
    if (_productInfo) {
        dict[@"productInfo"] = [_productInfo getDictionaryOfCloudProductInfoModel];
    }
    if (_versionInfo) {
        dict[@"versionInfo"] = [_versionInfo getDictionaryOfCloudVersionInfoModel];
    }
    if (_publishInfo) {
        dict[@"publishInfo"] = [_publishInfo getDictionaryOfCloudPublishInfoModel];
    }
    return dict;
}

/// Set dictionary to CloudNodeModel object.
/// @param dictionary CloudNodeModel dictionary object.
- (void)setDictionaryToCloudNodeModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"address"] && ![dictionary[@"address"] isMemberOfClass:[NSNull class]]) {
        _address = [dictionary[@"address"] intValue];
    }
    if ([allKeys containsObject:@"bindState"] && ![dictionary[@"bindState"] isMemberOfClass:[NSNull class]]) {
        _bindState = [dictionary[@"bindState"] intValue];
    }
    if ([allKeys containsObject:@"createUserId"] && ![dictionary[@"createUserId"] isMemberOfClass:[NSNull class]]) {
        _createUserId = [dictionary[@"createUserId"] intValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _nodeId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"meshProvisionerId"] && ![dictionary[@"meshProvisionerId"] isMemberOfClass:[NSNull class]]) {
        _meshProvisionerId = [dictionary[@"meshProvisionerId"] intValue];
    }
    if ([allKeys containsObject:@"networkId"] && ![dictionary[@"networkId"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"networkId"] intValue];
    }
    if ([allKeys containsObject:@"productId"] && ![dictionary[@"productId"] isMemberOfClass:[NSNull class]]) {
        _productId = [dictionary[@"productId"] intValue];
    }
    if ([allKeys containsObject:@"state"] && ![dictionary[@"state"] isMemberOfClass:[NSNull class]]) {
        _state = [dictionary[@"state"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"description"] && ![dictionary[@"description"] isMemberOfClass:[NSNull class]]) {
        _nodeDescription = dictionary[@"description"];
    }
    if ([allKeys containsObject:@"deviceKey"] && ![dictionary[@"deviceKey"] isMemberOfClass:[NSNull class]]) {
        _deviceKey = dictionary[@"deviceKey"];
    }
    if ([allKeys containsObject:@"deviceUuid"] && ![dictionary[@"deviceUuid"] isMemberOfClass:[NSNull class]]) {
        _deviceUuid = dictionary[@"deviceUuid"];
    }
    if ([allKeys containsObject:@"updateTime"] && ![dictionary[@"updateTime"] isMemberOfClass:[NSNull class]]) {
        _updateTime = dictionary[@"updateTime"];
    }
    if ([allKeys containsObject:@"versionId"] && ![dictionary[@"versionId"] isMemberOfClass:[NSNull class]]) {
        _versionId = dictionary[@"versionId"];
    }
    if ([allKeys containsObject:@"subList"] && ![dictionary[@"subList"] isMemberOfClass:[NSNull class]]) {
        NSMutableArray *groups = [NSMutableArray array];
        NSArray *array = dictionary[@"subList"];
        for (NSDictionary *groupDict in array) {
            CloudGroupModel *model = [[CloudGroupModel alloc] init];
            [model setDictionaryToCloudGroupModel:groupDict];
            [groups addObject:model];
        }
        _subList = groups;
    }
    if ([allKeys containsObject:@"productInfo"] && ![dictionary[@"productInfo"] isMemberOfClass:[NSNull class]]) {
        CloudProductInfoModel *info = [[CloudProductInfoModel alloc] init];
        [info setDictionaryToCloudProductInfoModel:dictionary[@"productInfo"]];
        _productInfo = info;
    }
    if ([allKeys containsObject:@"versionInfo"] && ![dictionary[@"versionInfo"] isMemberOfClass:[NSNull class]]) {
        CloudVersionInfoModel *info = [[CloudVersionInfoModel alloc] init];
        [info setDictionaryToCloudVersionInfoModel:dictionary[@"versionInfo"]];
        _versionInfo = info;
    }
    if ([allKeys containsObject:@"publishInfo"] && ![dictionary[@"publishInfo"] isMemberOfClass:[NSNull class]]) {
        CloudPublishInfoModel *info = [[CloudPublishInfoModel alloc] init];
        [info setDictionaryToCloudPublishInfoModel:dictionary[@"publishInfo"]];
        _publishInfo = info;
    }
}

/// delete scheduler with schedulerId.
/// @param schedulerId cloud schedulerId.
- (void)deleteCloudSchedulerModelWithSchedulerId:(NSInteger)schedulerId {
    NSArray *array = [NSArray arrayWithArray:_schedulerList];
    for (CloudSchedulerModel *model in array) {
        if (model.schedulerId == schedulerId) {
            [_schedulerList removeObject:model];
            break;
        }
    }
}

/// Determine if the data of two CloudNodeModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[CloudNodeModel class]]) {
        //nodeId is the unique identifier of CloudNodeModel.
        return self.nodeId == ((CloudNodeModel *)object).nodeId;
    } else {
        //Two CloudNodeModel object is different.
        return NO;
    }
}

@end


/// Provisioner info  in the cloud.
@implementation CloudProvisionerModel

/// get dictionary from CloudProvisionerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudProvisionerModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"address"] = [NSNumber numberWithInteger:_address];
    dict[@"id"] = [NSNumber numberWithInteger:_provisionerId];
    dict[@"networkId"] = [NSNumber numberWithInteger:_networkId];
    dict[@"sequenceNumber"] = [NSNumber numberWithInteger:_sequenceNumber];
    dict[@"userId"] = [NSNumber numberWithInteger:_userId];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_clientId) {
        dict[@"clientId"] = _clientId;
    }
    return dict;
}

/// Set dictionary to CloudProvisionerModel object.
/// @param dictionary CloudProvisionerModel dictionary object.
- (void)setDictionaryToCloudProvisionerModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"address"] && ![dictionary[@"address"] isMemberOfClass:[NSNull class]]) {
        _address = [dictionary[@"address"] intValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _provisionerId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"networkId"] && ![dictionary[@"networkId"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"networkId"] intValue];
    }
    if ([allKeys containsObject:@"sequenceNumber"] && ![dictionary[@"sequenceNumber"] isMemberOfClass:[NSNull class]]) {
        _sequenceNumber = [dictionary[@"sequenceNumber"] intValue];
    }
    if ([allKeys containsObject:@"userId"] && ![dictionary[@"userId"] isMemberOfClass:[NSNull class]]) {
        _userId = [dictionary[@"userId"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"clientId"] && ![dictionary[@"clientId"] isMemberOfClass:[NSNull class]]) {
        _clientId = dictionary[@"clientId"];
    }
}

@end


/// Network detail info  in the cloud.
@implementation CloudNetworkDetailModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _networkKeyList = [NSMutableArray array];
        _applicationKeyList = [NSMutableArray array];
        _groupList = [NSMutableArray array];
        _nodeList = [NSMutableArray array];
        _sceneList = [NSMutableArray array];
    }
    return self;
}

/// get dictionary from CloudNetworkDetailModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNetworkDetailModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"id"] = [NSNumber numberWithInteger:_networkId];
    dict[@"ivIndex"] = [NSNumber numberWithInteger:_ivIndex];
    dict[@"role"] = [NSNumber numberWithInteger:_role];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_createUser) {
        dict[@"createUser"] = _createUser;
    }
    if (_provisioner) {
        dict[@"provisioner"] = [_provisioner getDictionaryOfCloudProvisionerModel];
    }
    if (_networkKeyList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *netKeys = [NSArray arrayWithArray:_networkKeyList];
        for (CloudNetworkKeyModel *model in netKeys) {
            NSDictionary *netkeyDict = [model getDictionaryOfCloudNetworkKeyModel];
            [array addObject:netkeyDict];
        }
        dict[@"networkKeyList"] = array;
    }
    if (_applicationKeyList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *appKeys = [NSArray arrayWithArray:_applicationKeyList];
        for (CloudApplicationKeyModel *model in appKeys) {
            NSDictionary *appkeyDict = [model getDictionaryOfCloudApplicationKeyModel];
            [array addObject:appkeyDict];
        }
        dict[@"applicationKeyList"] = array;
    }
    if (_groupList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *groups = [NSArray arrayWithArray:_groupList];
        for (CloudGroupModel *model in groups) {
            NSDictionary *groupDict = [model getDictionaryOfCloudGroupModel];
            [array addObject:groupDict];
        }
        dict[@"groupList"] = array;
    }
    if (_nodeList) {
        NSMutableArray *array = [NSMutableArray array];
        NSArray *nodeKeys = [NSArray arrayWithArray:_nodeList];
        for (CloudNodeModel *model in nodeKeys) {
            NSDictionary *nodeDict = [model getDictionaryOfCloudNodeModel];
            [array addObject:nodeDict];
        }
        dict[@"nodeList"] = array;
    }
    return dict;
}

/// Set dictionary to CloudNetworkDetailModel object.
/// @param dictionary CloudNetworkDetailModel dictionary object.
- (void)setDictionaryToCloudNetworkDetailModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _networkId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"ivIndex"] && ![dictionary[@"ivIndex"] isMemberOfClass:[NSNull class]]) {
        _ivIndex = [dictionary[@"ivIndex"] intValue];
    }
    if ([allKeys containsObject:@"role"] && ![dictionary[@"role"] isMemberOfClass:[NSNull class]]) {
        _role = [dictionary[@"role"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"createUser"] && ![dictionary[@"createUser"] isMemberOfClass:[NSNull class]]) {
        _createUser = dictionary[@"createUser"];
    }
    if ([allKeys containsObject:@"provisioner"] && ![dictionary[@"provisioner"] isMemberOfClass:[NSNull class]]) {
        CloudProvisionerModel *model = [[CloudProvisionerModel alloc] init];
        [model setDictionaryToCloudProvisionerModel:dictionary[@"provisioner"]];
        _provisioner = model;
    }

    if ([allKeys containsObject:@"networkKeyList"]) {
        NSMutableArray *netKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"networkKeyList"];
        for (NSDictionary *netkeyDict in array) {
            CloudNetworkKeyModel *model = [[CloudNetworkKeyModel alloc] init];
            [model setDictionaryToCloudNetworkKeyModel:netkeyDict];
            [netKeys addObject:model];
        }
        _networkKeyList = netKeys;
    }
    if ([allKeys containsObject:@"applicationKeyList"]) {
        NSMutableArray *appKeys = [NSMutableArray array];
        NSArray *array = dictionary[@"applicationKeyList"];
        for (NSDictionary *appkeyDict in array) {
            CloudApplicationKeyModel *model = [[CloudApplicationKeyModel alloc] init];
            [model setDictionaryToCloudApplicationKeyModel:appkeyDict];
            [appKeys addObject:model];
        }
        _applicationKeyList = appKeys;
    }
    if ([allKeys containsObject:@"groupList"]) {
        NSMutableArray *groups = [NSMutableArray array];
        NSArray *array = dictionary[@"groupList"];
        for (NSDictionary *groupDict in array) {
            CloudGroupModel *model = [[CloudGroupModel alloc] init];
            [model setDictionaryToCloudGroupModel:groupDict];
            [groups addObject:model];
        }
        _groupList = groups;
    }
    if ([allKeys containsObject:@"nodeList"]) {
        NSMutableArray *nodes = [NSMutableArray array];
        NSArray *array = dictionary[@"nodeList"];
        for (NSDictionary *nodeDict in array) {
            CloudNodeModel *model = [[CloudNodeModel alloc] init];
            [model setDictionaryToCloudNodeModel:nodeDict];
            //将subList映射为当前Mesh的groupList里面的元素
            NSMutableArray *mArray = [NSMutableArray array];
            for (CloudGroupModel *group in model.subList) {
                CloudGroupModel *g = [self getCloudGroupModelWithGroupAddress:group.address fromGroupList:_groupList];
                [mArray addObject:g];
            }
            model.subList = mArray;
            [nodes addObject:model];
        }
        _nodeList = nodes;
    }
}

/// Get CloudGroupModel from group list with groupAddress.
/// @param groupAddress group address
/// @param groupList group list
- (CloudGroupModel *)getCloudGroupModelWithGroupAddress:(UInt16)groupAddress fromGroupList:(NSArray <CloudGroupModel *>*)groupList {
    CloudGroupModel *group = nil;
    for (CloudGroupModel *tem in groupList) {
        if (tem.address == groupAddress) {
            group = tem;
            break;
        }
    }
    return group;
}

@end


/// Scheduler info  in the cloud.
@implementation CloudSchedulerModel

/// get dictionary from CloudSchedulerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudSchedulerModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"id"] = [NSNumber numberWithInteger:_schedulerId];
    dict[@"meshNodeId"] = [NSNumber numberWithInteger:_meshNodeId];
    dict[@"meshSceneId"] = [NSNumber numberWithInteger:_meshSceneId];
    dict[@"schedulerIndex"] = [NSNumber numberWithInteger:_schedulerIndex];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_params) {
        dict[@"params"] = _params;
    }
    if (_info) {
        dict[@"info"] = _info;
    }
    return dict;
}

/// Set dictionary to CloudSchedulerModel object.
/// @param dictionary CloudSchedulerModel dictionary object.
- (void)setDictionaryToCloudSchedulerModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _schedulerId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"meshSceneId"] && ![dictionary[@"meshSceneId"] isMemberOfClass:[NSNull class]]) {
        _meshSceneId = [dictionary[@"meshSceneId"] intValue];
    }
    if ([allKeys containsObject:@"schedulerIndex"] && ![dictionary[@"schedulerIndex"] isMemberOfClass:[NSNull class]]) {
        _schedulerIndex = [dictionary[@"schedulerIndex"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"params"] && ![dictionary[@"params"] isMemberOfClass:[NSNull class]]) {
        _params = dictionary[@"params"];
    }
    if ([allKeys containsObject:@"info"] && ![dictionary[@"info"] isMemberOfClass:[NSNull class]]) {
        _info = dictionary[@"info"];
    }
}

@end


/// Scene info  in the cloud.
@implementation CloudSceneModel

/// get dictionary from CloudSceneModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudSceneModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"createTime"] = [NSNumber numberWithLong:_createTime];
    dict[@"id"] = [NSNumber numberWithInteger:_cloudSceneId];
    dict[@"meshNetworkId"] = [NSNumber numberWithInteger:_meshNetworkId];
    dict[@"sceneId"] = [NSNumber numberWithInteger:_sceneId];
    if (_name) {
        dict[@"name"] = _name;
    }
    if (_info) {
        dict[@"info"] = _info;
    }
    return dict;
}

/// Set dictionary to CloudSceneModel object.
/// @param dictionary CloudSceneModel dictionary object.
- (void)setDictionaryToCloudSceneModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"createTime"] && ![dictionary[@"createTime"] isMemberOfClass:[NSNull class]]) {
        _createTime = [dictionary[@"createTime"] longValue];
    }
    if ([allKeys containsObject:@"id"] && ![dictionary[@"id"] isMemberOfClass:[NSNull class]]) {
        _cloudSceneId = [dictionary[@"id"] intValue];
    }
    if ([allKeys containsObject:@"meshNetworkId"] && ![dictionary[@"meshNetworkId"] isMemberOfClass:[NSNull class]]) {
        _meshNetworkId = [dictionary[@"meshNetworkId"] intValue];
    }
    if ([allKeys containsObject:@"sceneId"] && ![dictionary[@"sceneId"] isMemberOfClass:[NSNull class]]) {
        _sceneId = [dictionary[@"sceneId"] intValue];
    }
    if ([allKeys containsObject:@"name"] && ![dictionary[@"name"] isMemberOfClass:[NSNull class]]) {
        _name = dictionary[@"name"];
    }
    if ([allKeys containsObject:@"info"] && ![dictionary[@"info"] isMemberOfClass:[NSNull class]]) {
        _info = dictionary[@"info"];
    }
}

@end
