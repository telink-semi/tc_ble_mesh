/********************************************************************************************************
 * @file     ble_UUID.h
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#ifndef _BLE_UUID_H_
#define _BLE_UUID_H_


/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _BLE_UUID_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


/* ------------------------------------
    Type Definitions
   ------------------------------------ */
typedef struct{
  u16 intervalMin;  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 latency;      /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 timeout;      /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
}gap_periConnectParams_t;


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
//EXTERN gap_periConnectParams_t my_periConnParameters;

EXTERN const u16 my_gapServiceUUID;     // 0x1800

//EXTERN const u16 primaryServiceUUID;    // 0x2800   //!< Primary Service
//EXTERN const u16 characterUUID;         // 0x2803   //!< Characteristic

EXTERN const u16 CEPUUID;               // 0x2900   //!< Characteristic Extended Properties
EXTERN const u16 userdesc_UUID;         // 0x2901   //!< Characteristic User Description
EXTERN const u16 ClientCharConfig_UUID; // 0x2902   //!< Client Characteristic Configuration
EXTERN const u16 CFGUUID;               // 0x2904   //!< Characteristic Present Format
EXTERN const u16 ValidRangeUUID;        // 0x2906   //!< Valid Range

EXTERN const u16 my_devNameUUID;        // 0x2a00   //!< Report Reference
EXTERN const u16 my_appearanceUUID;     // 0x2a01   //!< Report Reference
EXTERN const u16 my_periConnParamUUID;  // 0x2a04
EXTERN const u16 my_ServiceChangedUUID; // 0x2a05   //!< Report Reference

EXTERN u16 my_appearance;

EXTERN const u8 myServiceUUID[16];
EXTERN const u8 TelinkSppDataPairUUID[16];
EXTERN const u8 my_OtaUUID[16];

EXTERN const u8 CFG_BOOL_UNITLESS[];
EXTERN const u8 CFG_UINT8[];
EXTERN const u8 CFG_UINT8_PERCENTAGE[];
EXTERN const u8 CFG_UINT32[];
EXTERN const u8 CFG_UINT32_SECONDS[];
EXTERN const u8 CFG_UINT16[];
EXTERN const u8 CFG_INT32[];
EXTERN const u8 CFG_INT32_PERCENTAGE[];
EXTERN const u8 CFG_INT32_CELSIUS[];
EXTERN const u8 CFG_INT32_ARCDEGREE[];
//--EXTERN const u8 CFG_UTF8S[];
EXTERN const u8 CFG_STRING[];
//--EXTERN const u8 CFG_OPAQUE[];
EXTERN const u8 CFG_TLV8[];
EXTERN const u8 CFG_DATA[];
EXTERN const u8 CFG_FLOAT32[];
EXTERN const u8 CFG_FLOAT32_ARCDEGREE[];
EXTERN const u8 CFG_FLOAT32_PERCENTAGE[];
EXTERN const u8 CFG_FLOAT32_CELSIUS[];
EXTERN const u8 CFG_FLOAT32_LUX[];

//-EXTERN const u16 CCCnoneProp;               // 0x0000
EXTERN const u16 CCCnotifyOnlyProp;         // 0x0001   //!< permit broadcasts of the Characteristic Value
EXTERN const u16 CCCindicationOnlyProp;     // 0x0002   //!< permit reads of the Characteristic Value

EXTERN const u8 writeOnlyProp;
EXTERN const u8 writeNotifyProp;
EXTERN const u8 readOnlyProp;
EXTERN const u8 readIndicateProp;
EXTERN const u8 readWriteProp;
EXTERN const u8 notifyOnlyProp;
EXTERN const u8 readWriteIndicateProp;
EXTERN const u8 readWriteNotifyProp;
EXTERN const u8 ServiceChangedProp;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */


#endif /* _BLE_UUID_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
