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
