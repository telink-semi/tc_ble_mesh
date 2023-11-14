/********************************************************************************************************
 * @file     SDKLibCommand+certificate.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2021/3/3
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SDKLibCommand+certificate.h"

@implementation SDKLibCommand (certificate)

/**
 * @brief   Add Single Device (provision+keyBind), the flag of Certificate Based in unProvision adv data is 1, SDK will get and verify the Certificate of node.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   startConnect callback when SDK start connect node.
 * @param   startProvision callback when SDK start provision node.
 * @param   capabilitiesResponse callback when capabilities response from unProvision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
+ (void)startCertificateBasedWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    [SigAddDeviceManager.share startCertificateBasedWithNetworkKey:networkKey netkeyIndex:netkeyIndex appkeyModel:appkeyModel peripheral:peripheral staticOOBData:staticOOBData keyBindType:type productID:productID cpsData:cpsData startConnect:startConnect startProvision:startProvision capabilitiesResponse:capabilitiesResponse provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

@end
