/********************************************************************************************************
 * @file     TelinkHttpTool.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/1/10
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

#import "TelinkHttpTool.h"
#import <AFNetworking/AFNetworking.h>

#define kTimeoutInterval    (5.0)
#define BaseMeshCloudUrl @"https://192.168.18.213:8081"

//#define kCOOKIE @"Set-Cookie"

@implementation TelinkHttpTool

/// 发送一个GET请求
/// - Parameters:
///   - url: 请求路径
///   - params: 请求参数
///   - success: 请求成功后的回调（请将请求成功后想做的事情写到这个block中）
///   - failure: 请求失败后的回调（请将请求失败后想做的事情写到这个block中）
+ (void)getRequestWithUrl:(NSString *)url params:( NSDictionary *)params success:(void(^)(id json))success failure:(void(^)(NSError *error))failure {
    // 1.获得请求管理者，https方式
    AFHTTPSessionManager *sessionManager = [[AFHTTPSessionManager alloc] initWithBaseURL:[NSURL URLWithString:BaseMeshCloudUrl]];
    [self configSecurityPolicyWithAFHTTPSessionManager:sessionManager];

    [sessionManager.requestSerializer setTimeoutInterval:kTimeoutInterval];
    //    [sessionManager.requestSerializer setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    //    sessionManager.requestSerializer = [AFJSONRequestSerializer serializer];
    //    //申明返回的结果是json类型
    //    sessionManager.responseSerializer = [AFJSONResponseSerializer serializer];
    // 2.发送GET请求
    [sessionManager GET:url parameters:params headers:nil progress:^(NSProgress * _Nonnull downloadProgress) {
        
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        //        NSHTTPURLResponse *response = (NSHTTPURLResponse *)task.response;
        //        NSString*cookieStr = response.allHeaderFields[kCOOKIE];
        //        if(cookieStr !=nil) {
        //            // 根据cookie中的内容字符串用“; ”来分割出cookie
        //            NSArray*result = [cookieStr componentsSeparatedByString:@"; "];
        //            NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
        //            NSLog(@"cookieStr=======>%@",result[0]);
        //            [userDefaults setObject:result[0] forKey:kCOOKIE];
        //            [userDefaults synchronize];
        //        }
        if(success) {
            success(responseObject);
        }
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        if(failure) {
            NSLog(@"error=%@",error);
            failure(error);
        }
    }];
}

/// 发送一个POST请求
/// - Parameters:
///   - url: 请求路径
///   - params: 请求参数
///   - success: 请求成功后的回调（请将请求成功后想做的事情写到这个block中）
///   - failure: 请求失败后的回调（请将请求失败后想做的事情写到这个block中）
+ (void)postRequestWithUrl:(NSString *)url params:( NSDictionary *)params success:(void(^)(id json))success failure:(void(^)(NSError *error))failure {
    // 1.获得请求管理者，https方式
    AFHTTPSessionManager *sessionManager = [[AFHTTPSessionManager alloc] initWithBaseURL:[NSURL URLWithString:BaseMeshCloudUrl]];
    [self configSecurityPolicyWithAFHTTPSessionManager:sessionManager];

    [sessionManager.requestSerializer setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [sessionManager.requestSerializer setTimeoutInterval:kTimeoutInterval];
    ////    sessionManager.requestSerializer = [AFJSONRequestSerializer serializer];
    //    //申明返回的结果是json类型
    //    sessionManager.responseSerializer = [AFJSONResponseSerializer serializer];
    // 2.发送POST请求
    [sessionManager POST:url parameters:params headers:nil progress:^(NSProgress * _Nonnull uploadProgress) {
        
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        //        NSHTTPURLResponse *response = (NSHTTPURLResponse *)task.response;
        //        NSString*cookieStr = response.allHeaderFields[kCOOKIE];
        //        if(cookieStr !=nil) {
        //            // 根据cookie中的内容字符串用“; ”来分割出cookie
        //            NSArray*result = [cookieStr componentsSeparatedByString:@"; "];
        //            NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
        //            NSLog(@"cookieStr=======>%@",result[0]);
        //            [userDefaults setObject:result[0] forKey:kCOOKIE];
        //            [userDefaults synchronize];
        //        }
        if(success) {
            success(responseObject);
        }
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        if(failure) {
            NSLog(@"error=%@",error);
            failure(error);
        }
    }];
}

+ (void)configSecurityPolicyWithAFHTTPSessionManager:(AFURLSessionManager *)sessionManager {
    //证书
    AFSecurityPolicy *securityPolicy = [AFSecurityPolicy policyWithPinningMode:AFSSLPinningModeCertificate];
    sessionManager.securityPolicy = securityPolicy;
    // 2.设置证书模式，cerData是通过MAC电脑的二进阶工具`Hex Fiend`读取到服务器的公钥证书`client_key.cer`得到的数据，需去掉空格。
    // 因为SDK是静态库，使用bundle的方式集成.cer的话，用户除了集成SDK外，还需要集成.bundle。
    NSData *cerData = [LibTools nsstringToHex:@"308203733082025BA003020102020445335A46300D06092A864886F70D01010B0500306A310B300906035504061302636E3111300F060355040813085368616E674861693111300F060355040713085368616E67486169310F300D060355040A130674656C696E6B310F300D060355040B130674656C696E6B311330110603550403130A636F6D2E74656C696E6B301E170D3233303730333038353933365A170D3333303633303038353933365A306A310B300906035504061302636E3111300F060355040813085368616E674861693111300F060355040713085368616E67486169310F300D060355040A130674656C696E6B310F300D060355040B130674656C696E6B311330110603550403130A636F6D2E74656C696E6B30820122300D06092A864886F70D01010105000382010F003082010A0282010100A36E18AD127A1DCE04CF671173DD45E4B605791AA373D572B70E9D428018F62D847A07560B1442083369640670E601604C75A3F769FCD510AC8B6F502383F3F8156981665062D6CAA4F09BAEDBB59724D39F734EAC94961EE82AE88FDF10B37E0892D62D9744F7698BC622A801D5E5C10B4E62310D36EDBEE9AEC5451C914A8D5275446EC93DF6588F7212FBF120AA1090FEC8BC53383315D13F6CC521D57F980FE643965733E523E7DF183515E6ED8DFAA764AC712CB9ED0054228A876BA81246C769173B74CB78E1833BE943BDAAEFFF934B6738E157AFEA1A328D16F48748CFF00B8E05D2415379FE3E8F47BE1C74D5A55B52ADEDF02D16DBBECB9ECC55130203010001A321301F301D0603551D0E04160414AA8205C95F2CEF0B1A71B4323A1AF9FC5FCA12D7300D06092A864886F70D01010B0500038201010023EC707051B51FBD4CECBA04654EF33AEBF780FEBAE386BC870C215C98D1121A6D377912D853682104DD2D189531210809945EEB43A7D2B04870F7536ED5CC39768E134AAF5C5FA0DE136A8C0C6E2FD5A6BE42806A433A5BD3221DCDA5C61EC5E7FD9F8400C982DE44DE49AE44B0CA9765A52FE2DF00859575A1D7827A19ACBEC7CF4B064CB07D9B5DB1129243401C8EA98DAFC328A3F5CB8A1296DC60504093712616214B2EEED439A2D7FAC5A33B84BD5701AAD1703A7A014130DCB63689FBD7684D8C41367F0AE98360A31975E5A54CBFDB7C0424931B9D2AA17B3A332E7C00F16FBBF8F6F09968A91D6B75879CC0019180E019524F652C1CF60C71CDD7AF"];
    sessionManager.securityPolicy = [AFSecurityPolicy policyWithPinningMode:AFSSLPinningModeCertificate withPinnedCertificates:[[NSSet alloc] initWithObjects:cerData, nil]];
    //客户端是否信任非法证书
    sessionManager.securityPolicy.allowInvalidCertificates = YES;
    //是否在证书域字段中验证域名
    [sessionManager.securityPolicy setValidatesDomainName:NO];
}

/// 发送一个GET请求
/// - Parameters:
///   - url: 请求路径
///   - content: 请求参数
///   - block: 请求结果的回调
+ (void)getRequestWithUrl:(NSString *)url content:( NSDictionary *)content didLoadData:(MyBlock)block {
    [self getRequestWithUrl:url params:content success:^(id  _Nonnull json) {
        if (block) {
            block(json, nil);
        }
    } failure:^(NSError * _Nonnull error) {
        if (block) {
            block(nil, error);
        }
    }];
}

/// 发送一个POST请求
/// - Parameters:
///   - url: 请求路径
///   - content: 请求参数
///   - block: 请求结果的回调
+ (void)postRequestWithUrl:(NSString *)url content:( NSDictionary *)content didLoadData:(MyBlock)block {
    [self postRequestWithUrl:url params:content success:^(id  _Nonnull json) {
        if (block) {
            block(json, nil);
        }
    } failure:^(NSError * _Nonnull error) {
        if (block) {
            block(nil, error);
        }
    }];
}

#pragma mark - 用户管理 : User Controller 相关

/// 1.checkNameAvailable 检查用户名是否可用
/// @param name user name
/// @param block result callback
+ (void)checkNameAvailableRequestWithName:(NSString *)name didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/user/checkNameAvailable?name=%@", BaseMeshCloudUrl, name] content:@{} didLoadData:block];
}

/// 2.getVerificationCode 获取验证码
/// @param mailAddress user mail
/// @param usage 用途, 1-注册 2-重置密码
/// @param block result callback
+ (void)getVerificationCodeRequestWithMailAddress:(NSString *)mailAddress usage:(TelinkUserUsage)usage didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/user/getVerificationCode?mailAddress=%@&usage=%lu", BaseMeshCloudUrl, mailAddress, (unsigned long)usage] content:@{} didLoadData:block];
}

/// 3.register 注册
/// @param mailAddress user mail
/// @param name user name
/// @param verificationCode verification Code
/// @param password password
/// @param block result callback
+ (void)registerRequestWithMailAddress:(NSString *)mailAddress name:(NSString *)name verificationCode:(NSString *)verificationCode password:(NSString *)password didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"mailAddress":mailAddress, @"name":name, @"verificationCode":verificationCode, @"password": password};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/user/register", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 4.login 登录
/// @param username user mail or user name
/// @param password password
/// @param block result callback
+ (void)loginRequestWithUsername:(NSString *)username password:(NSString *)password didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"username":username, @"password":password};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/user/login", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 5.logout 退出登录
/// @param block result callback
+ (void)logoutRequestWithDidLoadData:(MyBlock)block {
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/user/logout", BaseMeshCloudUrl] content:@{} didLoadData:block];
}

/// 6.resetPassword 通过验证码来重置密码
/// @param mailAddress user mail
/// @param verificationCode verification Code
/// @param newPassword user new password
/// @param block result callback
+ (void)resetPasswordRequestWithMailAddress:(NSString *)mailAddress verificationCode:(NSString *)verificationCode newPassword:(NSString *)newPassword didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"mailAddress":mailAddress, @"verificationCode":verificationCode, @"newPassword":newPassword};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/user/resetPassword", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 7.updateInfo 更新用户信息
/// @param nickname user nickname
/// @param sign sign
/// @param phone user phone
/// @param company user company
/// @param block result callback
+ (void)updateInfoRequestWithNickname:(NSString * _Nullable )nickname sign:(NSString * _Nullable )sign phone:(NSString * _Nullable )phone company:(NSString * _Nullable )company didLoadData:(MyBlock)block {
    NSMutableDictionary *content = [NSMutableDictionary dictionary];
    if (nickname && nickname.length > 0) {
        content[@"nickname"] = nickname;
    }
    if (sign && sign.length > 0) {
        content[@"sign"] = sign;
    }
    if (phone && phone.length > 0) {
        content[@"phone"] = phone;
    }
    if (company && company.length > 0) {
        content[@"company"] = company;
    }
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/user/updateInfo", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 8.updatePassword 通过旧密码来更新密码
/// @param oldPassword user old password
/// @param newPassword user new password
/// @param block result callback
+ (void)updatePasswordRequestWithOldPassword:(NSString *)oldPassword newPassword:(NSString *)newPassword didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"oldPassword":oldPassword, @"newPassword":newPassword};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/user/updatePassword", BaseMeshCloudUrl] content:content didLoadData:block];
}

#pragma mark - 网络管理 : Mesh Network Controller 相关

/// 1.applyProvisioner 申请provisioner
/// @param networkId 网络id
/// @param clientId 客户端设备ID
/// @param block result callback
+ (void)applyProvisionerRequestWithNetworkId:(NSInteger)networkId clientId:(NSString *)clientId didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/applyProvisioner?networkId=%lu&clientId=%@", BaseMeshCloudUrl, (unsigned long)networkId, clientId] content:@{} didLoadData:block];
}

/// 2.create 创建网络信息
/// @param name 网络名称
/// @param clientId 客户端设备ID
/// @param block result callback
+ (void)createMeshNetworkRequestWithName:(NSString *)name clientId:(NSString *)clientId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"name":name, @"clientId":clientId};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/create", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 3.createShare 创建分享链接
/// @param networkId 网络id
/// @param validPeriod 有效时间-单位分钟
/// @param maxCount 最大次数
/// @param block result callback
+ (void)createShareLinkRequestWithNetworkId:(NSInteger)networkId validPeriod:(NSInteger)validPeriod maxCount:(NSInteger)maxCount didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"networkId": @(networkId), @"validPeriod": @(validPeriod), @"maxCount": @(maxCount)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/createShareLink", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 4.deleteNetwork 删除网络信息
/// @param networkId 网络id
/// @param block result callback
+ (void)deleteNetworkRequestWithNetworkId:(NSInteger)networkId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"networkId": @(networkId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/delete", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 5.deleteAllNetwork 删除用户下所有网络信息
/// @param block result callback
+ (void)deleteAllNetworkRequestWithDidLoadData:(MyBlock)block {
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/deleteAll", BaseMeshCloudUrl] content:@{} didLoadData:block];
}

/// 6.deleteNetworkByNetworkId 删除用户下指定ID的网络信息
/// @param networkId 网络id
/// @param block result callback
+ (void)deleteNetworkByNetworkIdRequestWithNetworkId:(NSInteger)networkId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"id": @(networkId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/deleteById", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 7.deleteShareLink 删除分享链接（APP目前用不到该接口）
/// @param shareLinkId 分享链接id
/// @param block result callback
+ (void)deleteNetworkByNetworkIdRequestWithShareLinkId:(NSInteger)shareLinkId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"shareLinkId": @(shareLinkId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/deleteShareLink", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 8.getCreatedNetworkList 获取用户创建的网络列表
/// @param block result callback
+ (void)getCreatedNetworkListRequestWithDidLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/getCreatedList", BaseMeshCloudUrl] content:@{} didLoadData:block];
}

/// 9.getNetworkDetail 获取用户下指定ID的网络信息
/// @param networkId 网络id
/// @param clientId 客户端设备ID
/// @param type 类型(默认为0)： 0-创建的网络， 1-分享加入的网络
/// @param block result callback
+ (void)getNetworkDetailRequestWithNetworkId:(NSInteger)networkId clientId:(NSString *)clientId type:(NSInteger)type didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/getDetail?id=%lu&clientId=%@&type=%lu", BaseMeshCloudUrl, (unsigned long)networkId, clientId, type] content:@{} didLoadData:block];
}

/// 10.getJoinedList 获取用户创建的分享状态信息， 即用户申请加入过哪些网络
/// @param state 分享状态： 申请中， 正常， 被禁用
/// @param block result callback
+ (void)getJoinedListRequestWithState:(ResponseShareState)state didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"state": @(state)};
    if (state == ResponseShareStateAll) {
        content = @{};
    }
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/getJoinedList", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 11.getShareInfoList 获取该网络下的分享信息， 即分享给谁
/// @param networkId 网络id
/// @param state 分享状态： 申请中， 正常， 被禁用
/// @param block result callback
+ (void)getShareInfoListRequestWithNetworkId:(NSInteger)networkId state:(ResponseShareState)state didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"networkId": @(networkId), @"state": @(state)};
    if (state == ResponseShareStateAll) {
        content = @{@"networkId": @(networkId)};
    }
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/getShareInfoList", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 12.getShareLinkList 获取该网络下创建的分享链接列表（APP目前用不到该接口）
/// @param networkId 网络id
/// @param expired 是否已过期
/// @param block result callback
+ (void)getShareLinkListRequestWithNetworkId:(NSInteger)networkId expired:(BOOL)expired didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/getShareLinkList?networkId=%lu&expired=%d", BaseMeshCloudUrl, (unsigned long)networkId, expired ? 1 : 0] content:@{} didLoadData:block];
}

/// 13.joinNetworkFromLink 通过share link加入网络， 传入shareLinkId
/// @param shareLinkId 网络分享链接ID
/// @param block result callback
+ (void)joinNetworkFromLinkRequestWithShareLinkId:(NSInteger)shareLinkId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"shareLinkId": @(shareLinkId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/joinNetworkFromLink", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 14.leaveNetwork 用户主动退出之前加入的网络
/// @param shareInfoId 分享状态id
/// @param block result callback
+ (void)leaveNetworkRequestWithShareInfoId:(NSInteger)shareInfoId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"shareInfoId": @(shareInfoId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/leaveNetwork", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 15.updateIvIndex 更新网络的ivIndex信息
/// @param networkId 网络id
/// @param ivIndex 新的ivIndex
/// @param block result callback
+ (void)updateIvIndexRequestWithNetworkId:(NSInteger)networkId ivIndex:(NSInteger)ivIndex didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"networkId":@(networkId), @"ivIndex":@(ivIndex)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/updateIvIndex", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 16.updateNetworkName 更新网络名称
/// @param networkId 网络id
/// @param name 网络名称
/// @param block result callback
+ (void)updateNetworkNameRequestWithNetworkId:(NSInteger)networkId name:(NSString *)name didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"id":@(networkId), @"name":name};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/updateName", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 17.updateSequenceNumber 更新provisioner的sequence number信息
/// @param provisionerId provisioner ID
/// @param sequenceNumber 新的sequenceNumber
/// @param block result callback
+ (void)updateSequenceNumberRequestWithProvisionerId:(NSString *)provisionerId sequenceNumber:(NSInteger)sequenceNumber didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"provisionerId":provisionerId, @"sequenceNumber":@(sequenceNumber)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/updateSequenceNumber", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 18.updateShareInfoState 网络创建者更新网络分享状态, 允许网络分享(1)， 不允许(禁用)网络分享（2）, 修改权限等。。
/// @param shareInfoId 网络分享id
/// @param state 更新状态
/// @param block result callback
+ (void)updateShareInfoStateRequestWithShareInfoId:(NSInteger)shareInfoId state:(ResponseShareState)state didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"shareInfoId": @(shareInfoId), @"state": @(state)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-network/updateShareInfoState", BaseMeshCloudUrl] content:content didLoadData:block];
}

#pragma mark - mesh节点管理 : Mesh Node Controller 相关

/// 1.addGroup 添加设备分组
/// @note 在添加分组后调用
/// @param nodeId node id
/// @param groupId group id
/// @param provisionerId provisioner ID
/// @param block result callback
+ (void)addGroupRequestWithNodeId:(NSInteger)nodeId groupId:(NSInteger)groupId provisionerId:(NSInteger)provisionerId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId), @"groupId":@(groupId), @"provisionerId":@(provisionerId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/addGroup", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 2.addNode 添加设备
/// @param tcVendorId 从蓝牙广播中获取的厂商自定义ID
/// @param pid 从蓝牙广播中获取的pid, 与cps data中一致
/// @param networkId 网络id 
/// @param deviceUUID deviceUUID 
/// @param name 设备名称 
/// @param address address 
/// @param provisionerId provisionerId 
/// @param deviceKey deviceKey 
/// @param block result callback
+ (void)addNodeRequestWithTcVendorId:(NSInteger)tcVendorId pid:(NSInteger)pid networkId:(NSInteger)networkId deviceUUID:(NSString *)deviceUUID name:(NSString *)name address:(NSInteger)address provisionerId:(NSInteger)provisionerId deviceKey:(NSString *)deviceKey didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"tcVendorId":@(tcVendorId), @"pid":@(pid), @"networkId":@(networkId), @"deviceUUID":deviceUUID, @"name":name, @"address":@(address), @"provisionerId":@(provisionerId), @"deviceKey":deviceKey};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/addNode", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 3.applyNodeAddress 申请节点地址
/// @note 在扫描到设备时， 传入设备ID， 从云端申请地址
/// @param tcVendorId 从蓝牙广播中获取的厂商自定义ID
/// @param pid 从蓝牙广播中获取的pid, 与cps data中一致
/// @param networkId 网络id 
/// @param provisionerId provisionerId 
/// @param elementCount 节点元素个数， 从capability中获取 
/// @param block result callback
+ (void)applyNodeAddressRequestWithTCVendorId:(NSInteger)tcVendorId pid:(NSInteger)pid networkId:(NSInteger)networkId provisionerId:(NSInteger)provisionerId elementCount:(NSInteger)elementCount didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"tcVendorId":@(tcVendorId), @"pid":@(pid), @"networkId":@(networkId), @"provisionerId":@(provisionerId), @"elementCount":@(elementCount)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/applyNodeAddress", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 4.deleteNode 剔除节点
/// @note 在踢灯完成后调用
/// @param nodeId node id
/// @param block result callback
+ (void)deleteNodeRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/delete", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 5.deleteGroup 删除设备分组
/// @note 在删除分组后调用
/// @param nodeId node id
/// @param groupId group id
/// @param provisionerId provisioner ID
/// @param block result callback
+ (void)deleteNodeRequestWithNodeId:(NSInteger)nodeId groupId:(NSInteger)groupId provisionerId:(NSInteger)provisionerId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId), @"groupId":@(groupId), @"provisionerId":@(provisionerId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/deleteGroup", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 6.downloadBin 获取该设备下的所有分组信息
/// @param path path
/// @param block result callback
+ (void)downloadBinRequestWithPath:(NSString *)path didLoadData:(MyBlock)block {
    NSString *url = [NSString stringWithFormat:@"%@/mesh-node/downloadBin?path=%@", BaseMeshCloudUrl, path];
    url = [url stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]; //url编码（utf8）
    NSURL *URL = [NSURL URLWithString:url];
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
    //AFN3.0+基于封住URLSession的句柄
    AFURLSessionManager *manager = [[AFURLSessionManager alloc] initWithSessionConfiguration:configuration];
    [self configSecurityPolicyWithAFHTTPSessionManager:manager];
    //请求
    NSURLRequest *request = [NSURLRequest requestWithURL:URL];
    //下载Task操作
    NSURLSessionDownloadTask *downloadTask = [manager downloadTaskWithRequest:request progress:^(NSProgress * _Nonnull downloadProgress) {

    } destination:^NSURL * _Nonnull(NSURL * _Nonnull targetPath, NSURLResponse * _Nonnull response) {

        //- block的返回值, 要求返回一个URL, 返回的这个URL就是文件的位置的路径

        NSString *cachesPath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject];
        NSString *path = [cachesPath stringByAppendingPathComponent:response.suggestedFilename];
        return [NSURL fileURLWithPath:path];

    } completionHandler:^(NSURLResponse * _Nonnull response, NSURL * _Nullable filePath, NSError * _Nullable error) {
        // filePath就是你下载文件的位置，你可以解压，也可以直接拿来使用
        if (error == nil) {
            if (block) {
                block(filePath, nil);
            }
        } else {
            if (block) {
                block(nil, error);
            }
        }
    }];
    //开始下载
    [downloadTask resume];

}

/// 7.getGroups 获取该设备下的所有分组信息
/// @note 传入设备ID
/// @param nodeId node id
/// @param block result callback
+ (void)getGroupsOfNodeRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/getGroups?nodeId=%lu", BaseMeshCloudUrl, (unsigned long)nodeId] content:@{} didLoadData:block];
}

/// 8.getLatestVersionInfo 获取最新的固件信息， app需要检查vid是否更大
/// @note 传入产品ID
/// @param productId 产品ID
/// @param block result callback
+ (void)getLatestVersionInfoRequestWithProductId:(NSInteger)productId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"productId":@(productId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/getLatestVersionInfo", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 9.releaseNodeAddress 释放节点地址
/// @note 组网失败时调用， 传入设备ID， 从云端释放该地址
/// @param networkId 网络ID
/// @param provisionerId provisioner Id
/// @param elementCount elementCount
/// @param tcVendorId 从蓝牙广播中获取的厂商自定义ID
/// @param nodeAddress 已申请到的address
/// @param block result callback
+ (void)releaseNodeAddressRequestWithNetworkId:(NSInteger)networkId provisionerId:(NSInteger)provisionerId elementCount:(NSInteger)elementCount tcVendorId:(NSInteger)tcVendorId nodeAddress:(NSInteger)nodeAddress didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/releaseNodeAddress?networkId=%lu&provisionerId=%lu&elementCount=%lu&tcVendorId=%lu&nodeAddress=%lu", BaseMeshCloudUrl, (unsigned long)networkId, (unsigned long)provisionerId, (unsigned long)elementCount, (unsigned long)tcVendorId, (unsigned long)nodeAddress] content:@{} didLoadData:block];
}

/// 10.updateNodeBindState 更新设备key bind状态
/// @note 在key bind成功后调用
/// @param nodeId node id
/// @param vid versionId 从composition data中获取的vid
/// @param bindState bind State
/// @param block result callback
+ (void)updateNodeBindStateRequestWithNodeId:(NSInteger)nodeId vid:(NSInteger)vid bindState:(NSInteger)bindState didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"id":@(nodeId), @"vid":@(vid), @"bindState":@(bindState)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/updateNodeBindState", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 11.updateNodeVersion 更新设备版本信息
/// @note 在OTA完成后，或者在获取到cps后发现不匹配时调用
/// @param nodeId node id
/// @param vid versionId 从composition data中获取的vid
/// @param block result callback
+ (void)updateNodeVersionRequestWithNodeId:(NSInteger)nodeId vid:(NSInteger)vid didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId), @"vid":@(vid)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/updateNodeVersion", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 12.uploadRecord 更新设备的操作日志
/// @note 在OTA完成后，或者在获取到cps后发现不匹配时调用
/// @param nodeId 设备ID
/// @param status 状态
/// @param block result callback
+ (void)uploadRecordRequestWithNodeId:(NSInteger)nodeId status:(NSString *)status didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId), @"status":status};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/uploadRecord", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 13.getPublish 获取设备publish信息
/// @param nodeId 设备ID
/// @param block result callback
+ (void)getPublishRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/getPublish?nodeId=%lu", BaseMeshCloudUrl, (unsigned long)nodeId] content:@{} didLoadData:block];
}

/// 14.setPublish 设置设备publish信息，目前仅能配置一个，如果当前已存在，则会替换
/// @param nodeId 设备ID
/// @param elementAddress 元素地址
/// @param publishAddress publish地址
/// @param appKeyIndex appKeyIndex
/// @param period period
/// @param modelId modelId
/// @param isSigModel 1 为 true, 0 为 false， 可选 默认为1
/// @param credentialFlag 默认为0x00
/// @param ttl 可选，默认为 0xFF
/// @param retransmitCount 可选 默认为 0x05
/// @param retransmitIntervalSteps 可选 默认为 0x00
/// @param block result callback
+ (void)setPublishRequestWithNodeId:(NSInteger)nodeId elementAddress:(NSInteger)elementAddress publishAddress:(NSInteger)publishAddress appKeyIndex:(NSInteger)appKeyIndex period:(NSInteger)period modelId:(NSInteger)modelId isSigModel:(NSInteger)isSigModel credentialFlag:(NSInteger)credentialFlag ttl:(NSInteger)ttl retransmitCount:(NSInteger)retransmitCount retransmitIntervalSteps:(NSInteger)retransmitIntervalSteps didLoadData:(MyBlock)block {
#warning 可选参数待优化
    NSDictionary *content = @{@"nodeId":@(nodeId), @"elementAddress":@(elementAddress), @"publishAddress":@(publishAddress), @"appKeyIndex":@(appKeyIndex), @"period":@(period), @"modelId":@(modelId), @"isSigModel":@(isSigModel), @"credentialFlag":@(credentialFlag), @"ttl":@(ttl), @"retransmitCount":@(retransmitCount), @"retransmitIntervalSteps":@(retransmitIntervalSteps)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/setPublish", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 15.updatePublish 更新设备publish信息，除publish ID外，其它均为可选，只需传入需要修改的参数。
/// @param publishId publish Id
/// @param elementAddress 元素地址
/// @param publishAddress publish地址
/// @param appKeyIndex appKeyIndex
/// @param period period
/// @param modelId modelId
/// @param isSigModel 1 为 true, 0 为 false， 可选 默认为1
/// @param credentialFlag 默认为0x00
/// @param ttl 可选，默认为 0xFF
/// @param retransmitCount 可选 默认为 0x05
/// @param retransmitIntervalSteps 可选 默认为 0x00
/// @param block result callback
+ (void)updatePublishRequestWithPublishId:(NSInteger)publishId elementAddress:(NSInteger)elementAddress publishAddress:(NSInteger)publishAddress appKeyIndex:(NSInteger)appKeyIndex period:(NSInteger)period modelId:(NSInteger)modelId isSigModel:(NSInteger)isSigModel credentialFlag:(NSInteger)credentialFlag ttl:(NSInteger)ttl retransmitCount:(NSInteger)retransmitCount retransmitIntervalSteps:(NSInteger)retransmitIntervalSteps didLoadData:(MyBlock)block {
#warning 可选参数待优化
    NSDictionary *content = @{@"publishId":@(publishId), @"elementAddress":@(elementAddress), @"publishAddress":@(publishAddress), @"appKeyIndex":@(appKeyIndex), @"period":@(period), @"modelId":@(modelId), @"isSigModel":@(isSigModel), @"credentialFlag":@(credentialFlag), @"ttl":@(ttl), @"retransmitCount":@(retransmitCount), @"retransmitIntervalSteps":@(retransmitIntervalSteps)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/updatePublish", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 16.deletePublish 删除指定publish信息
/// @param publishId publish ID
/// @param block result callback
+ (void)deletePublishRequestWithPublishId:(NSInteger)publishId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"publishId":@(publishId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-node/deletePublish", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 17.getConfigs 返回所有的config消息，在修改设备配置前，需要先调用该接口以获取所有状态
/// @param nodeId 设备ID
/// @param block result callback
+ (void)getConfigsRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block {
    [self getRequestWithUrl:[NSString stringWithFormat:@"%@/node-config/getConfigs?nodeId=%lu", BaseMeshCloudUrl, (unsigned long)nodeId] content:@{} didLoadData:block];
}

/// 18.updateConfigs 更新所有config信息, 可用于设置多个config信息, 只需要上传需要修改的信息即可
/// @param nodeConfigs node configs
/// @param block result callback
+ (void)updateConfigsRequestWithNodeConfigs:(CloudNodeConfigsModel *)nodeConfigs didLoadData:(MyBlock)block {
    NSDictionary *content = nodeConfigs.getDictionaryOfCloudNodeConfigsModel;
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/node-config/updateConfigs", BaseMeshCloudUrl] content:content didLoadData:block];
}

#pragma mark - Mesh场景管理 : Mesh Scene Controller 相关

/// 1.addNodeToScene 将设备添加到场景
/// @note 在app发送scene store 成功后调用
/// @param nodeId node id
/// @param sceneId scene id
/// @param block result callback
+ (void)addNodeToSceneRequestWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId), @"sceneId":@(sceneId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scene/addNodeToScene", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 2.createScene 添加场景
/// @note 用户可以在app上添加场景
/// @param networkId 网络ID
/// @param name 场景名称
/// @param info 描述信息
/// @param block result callback
+ (void)createSceneRequestWithNetworkId:(NSInteger)networkId name:(NSString *)name info:(NSString * _Nullable)info didLoadData:(MyBlock)block {
    NSMutableDictionary *content = [NSMutableDictionary dictionaryWithDictionary:@{@"networkId":@(networkId), @"name":name}];
    if (info && info.length > 0) {
        content[@"info"] = info;
    }
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scene/createScene", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 3.deleteScene 删除场景
/// @note 用户可以在app上删除场景
/// @param sceneId scene id
/// @param block result callback
+ (void)deleteSceneRequestWithSceneId:(NSInteger)sceneId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"sceneId":@(sceneId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scene/deleteScene", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 4.getInnerNodes 获取场景内的所有设备信息
/// @note 在app发送scene store成功后调用
/// @param sceneId scene id
/// @param block result callback
+ (void)getInnerNodesRequestWithSceneId:(NSInteger)sceneId didLoadData:(MyBlock)block {
    /*
     {
       "code": 200,
       "message": "get inner nodes success",
       "data": [
         {
           "id": 11,
           "name": "common node",
           "deviceUuid": "110200010001008154BE38C1A4000045",
           "createUserId": 3,
           "networkId": 23,
           "address": 5,
           "state": 1,
           "deviceKey": "EC551021D020D2B59CBD4C8C2F7EC3D2",
           "bindState": 1,
           "productId": 1,
           "versionId": 1,
           "meshProvisionerId": 52,
           "createTime": 1700018623375,
           "updateTime": null,
           "description": null,
           "productInfo": null,
           "versionInfo": null,
           "subList": null,
           "productName": null,
           "networkName": null
         },
         {
           "id": 12,
           "name": "common node",
           "deviceUuid": "11020001000100644C4F38C1A40000B1",
           "createUserId": 3,
           "networkId": 23,
           "address": 7,
           "state": 1,
           "deviceKey": "8F59D9DCDC7C950283A8F1DE79B9804B",
           "bindState": 1,
           "productId": 1,
           "versionId": 1,
           "meshProvisionerId": 52,
           "createTime": 1700018667796,
           "updateTime": null,
           "description": null,
           "productInfo": null,
           "versionInfo": null,
           "subList": null,
           "productName": null,
           "networkName": null
         }
       ],
       "totalCount": null
     }
     */
    NSDictionary *content = @{@"sceneId":@(sceneId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scene/getInnerNodes", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 5.getSceneList 通过网络ID获取所有的场景信息
/// @note 进入scene 列表页面时调用
/// @param networkId 网络ID
/// @param block result callback
+ (void)getSceneListRequestWithNetworkId:(NSInteger)networkId didLoadData:(MyBlock)block {
    /*
     {
       "code": 200,
       "message": "get scene list success",
       "data": [
         {
           "id": 4,
           "name": "场景1",
           "meshNetworkId": 23,
           "sceneId": 1,
           "info": null,
           "createTime": 1700018025335
         },
         {
           "id": 6,
           "name": "场景2",
           "meshNetworkId": 23,
           "sceneId": 3,
           "info": null,
           "createTime": 1700126749847
         },
         {
           "id": 7,
           "name": "场景3",
           "meshNetworkId": 23,
           "sceneId": 4,
           "info": null,
           "createTime": 1700126755936
         },
         {
           "id": 8,
           "name": "场景4",
           "meshNetworkId": 23,
           "sceneId": 5,
           "info": null,
           "createTime": 1700126761718
         }
       ],
       "totalCount": null
     }
     */
    NSDictionary *content = @{@"networkId":@(networkId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scene/getSceneList", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 6.removeNodeFromScene 将设备移出场景
/// @note 在app发送scene delete 成功后调用
/// @param nodeId node id
/// @param sceneId scene id
/// @param block result callback
+ (void)removeNodeFromSceneRequestWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId), @"sceneId":@(sceneId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scene/removeNodeFromScene", BaseMeshCloudUrl] content:content didLoadData:block];
}

#pragma mark - Mesh闹钟管理 : Mesh Scheduler Controller 相关

/// 1.createScheduler 添加闹钟
/// @note 用户在app上添加闹钟， 一个节点最多只能16个（0-15）, index值由云端分配
/// @param nodeId 节点ID
/// @param name 闹钟名称
/// @param info 描述信息
/// @param block result callback
+ (void)createSchedulerRequestWithNodeId:(NSInteger)nodeId name:(NSString *)name info:(NSString * _Nullable)info didLoadData:(MyBlock)block {
    NSMutableDictionary *content = [NSMutableDictionary dictionaryWithDictionary:@{@"nodeId":@(nodeId), @"name":name}];
    if (info && info.length > 0) {
        content[@"info"] = info;
    }
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scheduler/createScheduler", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 2.deleteScheduler 删除闹钟
/// @note 用户在app上删除闹钟
/// @param schedulerId 闹钟ID
/// @param block result callback
+ (void)deleteSchedulerRequestWithSchedulerId:(NSInteger)schedulerId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"schedulerId":@(schedulerId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scheduler/deleteScheduler", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 3.getSchedulerList 通过设备ID获取所有的闹钟信息
/// @note 在app发送scene store成功后调用
/// @param nodeId 节点ID
/// @param block result callback
+ (void)getSchedulerListRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"nodeId":@(nodeId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scheduler/getSchedulerList", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 4.saveScheduler 保存闹钟信息
/// @note 在配置闹钟完成后调用, 如果设置的action是scene, 则需要传入sceneId
/// @param schedulerId 闹钟ID
/// @param params scheduler参数信息(hex string)
/// @param meshSceneId 场景ID
/// @param block result callback
+ (void)saveSchedulerRequestWithSchedulerId:(NSInteger)schedulerId params:(NSString *)params meshSceneId:(NSInteger)meshSceneId didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"schedulerId":@(schedulerId), @"params":params, @"meshSceneId":@(meshSceneId)};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scheduler/saveScheduler", BaseMeshCloudUrl] content:content didLoadData:block];
}

/// 5.updateScheduler 更新闹钟的名称和描述信息
/// @note 在用户修改闹钟的名称和描述信息时调用, 参数可以为空， 但不能同时都为空
/// @param schedulerId 闹钟ID
/// @param name 闹钟名称
/// @param info 描述信息
/// @param block result callback
+ (void)updateSchedulerRequestWithSchedulerId:(NSInteger)schedulerId name:(NSString *)name info:(NSString *)info didLoadData:(MyBlock)block {
    NSDictionary *content = @{@"schedulerId":@(schedulerId), @"name":name, @"info":info};
    [self postRequestWithUrl:[NSString stringWithFormat:@"%@/mesh-scheduler/updateScheduler", BaseMeshCloudUrl] content:content didLoadData:block];
}

@end
