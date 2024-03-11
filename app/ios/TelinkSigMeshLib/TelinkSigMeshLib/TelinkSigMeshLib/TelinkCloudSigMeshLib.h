//
/********************************************************************************************************
 * @file     TelinkCloudSigMeshLib.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/12/14
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

//! Project version number for TelinkCloudSigMeshLib.
FOUNDATION_EXPORT double TelinkCloudSigMeshLibVersionNumber;

//! Project version string for TelinkCloudSigMeshLib.
FOUNDATION_EXPORT const unsigned char TelinkCloudSigMeshLibVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <TelinkCloudSigMeshLib/PublicHeader.h>

#import <CoreBluetooth/CoreBluetooth.h>

/*注意：
 1.需要新增宏定义‘#define kIsTelinkCloudSigMeshLib’，生成包含云端相关代码的库TelinkCloudSigMeshLib.framework，demo需要导入头文件‘#import "TelinkCloudSigMeshLib.h"’
 2.default release TelinkSigMeshLib.framework.
 3.SDK通过以下代码来添加云端SDK特有的代码逻辑：
  #ifdef kIsTelinkCloudSigMeshLib
  <* some code *>
  #endif
 注意：是通过在Build Settings里面定义宏kIsTelinkCloudSigMeshLib=1实现SDK内部的宏定义的。
*/

#define kIsTelinkCloudSigMeshLib

#import <TelinkCloudSigMeshLib/SigConst.h>
#import <TelinkCloudSigMeshLib/SigEnumeration.h>
#import <TelinkCloudSigMeshLib/SigStruct.h>
#import <TelinkCloudSigMeshLib/SigLogger.h>
#import <TelinkCloudSigMeshLib/SigModel.h>
#import <TelinkCloudSigMeshLib/BackgroundTimer.h>
#import <TelinkCloudSigMeshLib/SigBearer.h>
#import <TelinkCloudSigMeshLib/SigDataSource.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand.h>
#import <TelinkCloudSigMeshLib/SigConfigMessage.h>
#import <TelinkCloudSigMeshLib/SigMeshMessage.h>
#import <TelinkCloudSigMeshLib/SigMeshLib.h>
#import <TelinkCloudSigMeshLib/SigHelper.h>
#import <TelinkCloudSigMeshLib/SigMessageHandle.h>
#import <TelinkCloudSigMeshLib/SigProxyConfigurationMessage.h>
#import <TelinkCloudSigMeshLib/LibTools.h>
#import <TelinkCloudSigMeshLib/SigGenericMessage.h>
#import <TelinkCloudSigMeshLib/SigHeartbeatMessage.h>
#import <TelinkCloudSigMeshLib/OTAManager.h>
#import <TelinkCloudSigMeshLib/SigPublishManager.h>
#import <TelinkCloudSigMeshLib/TelinkHttpManager.h>
#import <TelinkCloudSigMeshLib/SigFastProvisionAddManager.h>
#import <TelinkCloudSigMeshLib/MeshOTAManager.h>
#import <TelinkCloudSigMeshLib/SigRemoteAddManager.h>
#import <TelinkCloudSigMeshLib/SigBluetooth.h>
#import <TelinkCloudSigMeshLib/TPeripheralManager.h>
#import <TelinkCloudSigMeshLib/SigAddDeviceManager.h>
#import <TelinkCloudSigMeshLib/SigPdu.h>
#import <TelinkCloudSigMeshLib/ConnectTools.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+subnetBridge.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+certificate.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+opcodesAggregatorSequence.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+privateBeacon.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+firmwareUpdate.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+remoteProvision.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+directForwarding.h>
#import <TelinkCloudSigMeshLib/OTSCommand.h>
#import <TelinkCloudSigMeshLib/OTSBaseModel.h>
#import <TelinkCloudSigMeshLib/OTSHandle.h>
#import <TelinkCloudSigMeshLib/NSData+Compression.h>
#import <TelinkCloudSigMeshLib/SDKLibCommand+minor_ENH.h>
#import <TelinkCloudSigMeshLib/CDTPServiceModel.h>
#import <TelinkCloudSigMeshLib/CDTPClientModel.h>
#import <TelinkCloudSigMeshLib/TelinkCloudModel.h>
#import <TelinkCloudSigMeshLib/AppDataSource.h>
#import <TelinkCloudSigMeshLib/TelinkHttpTool.h>
