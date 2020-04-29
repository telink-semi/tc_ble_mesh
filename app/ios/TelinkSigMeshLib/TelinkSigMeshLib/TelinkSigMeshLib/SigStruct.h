/********************************************************************************************************
* @file     SigStruct.h
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
//  SigStruct.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#ifndef SigStruct_h
#define SigStruct_h

struct Flags {
    union{
        UInt8 value;
        struct{
            /// Static OOB Information is available.
            UInt8 keyRefreshFinalizing  :1;//value的低1个bit
            UInt8 ivUpdateActive  :1;//val的低位第2个bit
        };
    };
};

typedef struct {
    union{
        UInt16 value;
        struct{
            UInt8 SigFeatures_relay      :1;//value的低1个bit
            UInt8 SigFeatures_proxy :1;//val的低位第2个bit
            UInt8 SigFeatures_friend :1;//val的低位第3个bit
            UInt8 SigFeatures_lowPower :1;//val的低位第4个bit
        };
    };
}SigFeatures;

/// The Additional Information bitfield
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.28)
typedef struct {
    union{
        UInt8 value;
        struct{
            /// When set to 1 the device be in unprovisioned state after the new FW is applied (possibly due to new models added).
            UInt8 ProvisioningNeeded  :1;//value的低1个bit
        };
    };
}SigAdditionalInformation;

#endif /* SigStruct_h */
