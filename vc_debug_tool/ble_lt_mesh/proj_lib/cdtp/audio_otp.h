/********************************************************************************************************
 * @file	audio_otp.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#ifndef _AUDIO_OTP_H_
#define _AUDIO_OTP_H_

#include "proj/common/bit.h"

#define OTS_FOR_CDTP_EN			1

typedef enum{
	OACP_CreateOpCodeSupported					= BIT(0),
	OACP_DeleteOpCodeSupported					= BIT(1),
	OACP_CalculateChecksumOpCodeSupported		= BIT(2),
	OACP_ExecuteOpCodeSupported					= BIT(3),
	OACP_ReadOpCodeSupported					= BIT(4),
	OACP_WriteOpCodeSupported					= BIT(5),
	AppendingAdditionalDatatoObjectsSupported	= BIT(6),
	TruncationofObjectsSupported				= BIT(7),
	PatchingofObjectsSupported					= BIT(8),
	OACP_AbortOpCodeSupported					= BIT(9),
}ots_oacp_features_field_e; // refer to op code of BLC_AUDIO_OTP_OACP_OPCODE_ENUM.

typedef enum{
	OLCP_GoToOpCode								= BIT(0),
	OLCP_OrderOpCodeSupported					= BIT(1),
	OLCP_RequestNumberofObjectsOpCodeSupported	= BIT(2),
	OLCP_ClearMarkingOpCodeSupported			= BIT(3),
}otp_olcp_features_field_e; // refer to op code of BLC_AUDIO_OTP_OLCP_OPCODE_ENUM.


// OTS   Opcode for Action Control Point
/* Types of Object Action Control Point Procedures. */
#if OTS_FOR_CDTP_EN
typedef enum{
	BLC_AUDIO_OTP_OACP_OPCODE_CREATE        = 0x01, //
	OTS_OACP_OPCODE_CALC_CHECKSUM 			= 0x03, // Calculate Checksum
	OTS_OACP_OPCODE_READ          			= 0x05, // 
	OTS_OACP_OPCODE_WRITE         			= 0x06, // 
	OTS_OACP_OPCODE_USER_DATA_RX         	= 0x50, // user define, rx data from phone App.
	OTS_OACP_OPCODE_USER_DATA_TX         	= 0x51, // user define, tx data from vs to gateway, then to phone App.
	OTS_OACP_OPCODE_USER_CHECKSUM_TX     	= 0x52, // user define, tx checksum from vs to gateway
	OTS_OACP_OPCODE_USER_TX_OBJECT_SIZE     = 0x53, // user define, tx object size from vs to gateway, then to phone App.
	OTS_OACP_OPCODE_USER_SMP_TK_DISPLAY    	= 0x54, // user define
	OTS_OACP_OPCODE_USER_ADV_ONOFF    		= 0x55, // user define, start/stop GATT ADV with OTS service UUID.
	OTS_OACP_OPCODE_RSPCODE       			= 0x60, // Response Code
}ots_oacp_opcode_e;
#else
typedef enum{
	BLC_AUDIO_OTP_OACP_OPCODE_CREATE        = 0x01, //
	BLC_AUDIO_OTP_OACP_OPCODE_DELETE        = 0x02, // -- unuse for CDTP
	BLC_AUDIO_OTP_OACP_OPCODE_CALC_CHECKSUM = 0x03, // Calculate Checksum
	BLC_AUDIO_OTP_OACP_OPCODE_EXECUTE       = 0x04, // -- unuse for CDTP
	BLC_AUDIO_OTP_OACP_OPCODE_READ          = 0x05, // 
	BLC_AUDIO_OTP_OACP_OPCODE_WRITE         = 0x06, // 
	BLC_AUDIO_OTP_OACP_OPCODE_ABORT         = 0x07, // -- unuse for CDTP
	BLC_AUDIO_OTP_OACP_OPCODE_RSPCODE       = 0x60, // Response Code
}BLC_AUDIO_OTP_OACP_OPCODE_ENUM;

// OTS	 Opcode for Action Control Point
typedef enum{
	BLC_AUDIO_OTP_OLCP_OPCODE_FIRST         = 0x01,
	BLC_AUDIO_OTP_OLCP_OPCODE_LAST          = 0x02,
	BLC_AUDIO_OTP_OLCP_OPCODE_PREVIOUS      = 0x03,
	BLC_AUDIO_OTP_OLCP_OPCODE_NEXT          = 0x04,
	BLC_AUDIO_OTP_OLCP_OPCODE_GOTO          = 0x05,
	BLC_AUDIO_OTP_OLCP_OPCODE_ORDER         = 0x06,
	BLC_AUDIO_OTP_OLCP_OPCODE_REQ_OBJNUMB   = 0x07,
	BLC_AUDIO_OTP_OLCP_OPCODE_CLEAR_MARKING = 0x08,
	BLC_AUDIO_OTP_OLCP_OPCODE_RSPCODE       = 0x70,
}BLC_AUDIO_OTP_OLCP_OPCODE_ENUM;
#endif

/* Object Action Control Point return codes. */
typedef enum{
	/** Success.*/
	OTS_OACP_RES_SUCCESS        = 0x01,
	/** Not supported*/
	OTS_OACP_RES_OPCODE_NOT_SUP = 0x02,
	/** Invalid parameter*/
	OTS_OACP_RES_INV_PARAM      = 0x03,
	/** Insufficient resources.*/
	OTS_OACP_RES_INSUFF_RES     = 0x04,
	/** Invalid object.*/
	OTS_OACP_RES_INV_OBJ        = 0x05,
	/** Channel unavailable.*/
	OTS_OACP_RES_CHAN_UNAVAIL   = 0x06,
	/** Unsupported procedure.*/
	OTS_OACP_RES_UNSUP_TYPE     = 0x07,
	/** Procedure not permitted.*/
	OTS_OACP_RES_NOT_PERMITTED  = 0x08,
	/** Object locked.*/
	OTS_OACP_RES_OBJ_LOCKED     = 0x09,
	/** Operation Failed.*/
	OTS_OACP_RES_OPER_FAILED    = 0x0A
}ots_oacp_res_code;


#if 0
typedef struct{
	u32 size;
	struct bt_ots_obj_type type;
}ots_oacp_create_params_t;
#endif

typedef struct{
	u32 offset;
	u32 len;
}ots_oacp_cs_calc_params_t;

typedef struct{
	u32 offset;
	u32 len;
}ots_oacp_read_params_t;

typedef struct{
	u32 offset;
	u32 len;
	u8 mode;
}ots_oacp_write_params_t;

/** @brief Descriptor for OTS Object Size parameter. */
typedef struct{
	u32 cur;	/** @brief Current Size */
	u32 alloc;	/** @brief Allocated Size */
}ots_oacp_ots_obj_size_t;


#if (BLC_AUDIO_OTP_ENABLE)


typedef enum{
	BLT_AUDIO_OTP_FLAG_NONE                     = 0x0000,

	BLT_AUDIO_OTP_FLAG_OACP_IND                 = 0x0001,
	BLT_AUDIO_OTP_FLAG_OLCP_IND                 = 0x0002,
	BLT_AUDIO_OTP_FLAG_OBJECT_CHANGED           = 0x0004,
	BLT_AUDIO_OTP_FLAG_OBJECT_VALID             = 0x0010,
	BLT_AUDIO_OTP_FLAG_OACP_PARAMETER_IND       = 0x0020,
	BLT_AUDIO_OTP_FLAG_OLCP_NUM_IND             = 0X0040,
}BLT_AUDIO_OTP_FLAGS_ENUM;

typedef enum{
	BLT_AUDIO_OTP_FLAG_OBJECT_NONE           = 0x00,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_1        = 0X01,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_2        = 0X02,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_3        = 0X03,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_4        = 0X04,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_5        = 0X05,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_6        = 0X06,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_7        = 0X07,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_8        = 0X08,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_9        = 0X09,
	BLT_AUDIO_OTP_FLAG_OBJECT_INDEX_10       = 0X0a,

}BLT_AUDIO_OTP_OBJECT_INDEX_ENUM;

typedef enum{
	BLT_AUDIO_OTP_ERRCODE_WRITE_REJECTED = 0x80, //Write Request Rejected: An attempt was made to write a value that is invalid or not supported by this Server for a reason other than the attribute permissions.
	BLT_AUDIO_OTP_ERRCODE_OBJECT_NOTSEL  = 0x81, //Object Not Selected: An attempt was made to read or write to an Object Metadata characteristic while the Current Object was an Invalid Object
	BLT_AUDIO_OTP_ERRCODE_LIMIT_EXCEEDED = 0x82, //Concurrency Limit Exceeded: The Server is unable to service the Read Request or Write Request because it exceeds the concurrency limit of the service.
	BLT_AUDIO_OTP_ERRCODE_NAME_EXISTS    = 0x83, //Object Name Already Exists: The requested object name was rejected because the name was already in use by an existing object on the Server.
}BLT_AUDIO_OTP_ERRCODE_ENUM;

typedef enum{
	BLT_AUDIO_OTP_OACP_SUCCESS                         = 0x01, //Response for successful operation.
	BLT_AUDIO_OTP_OACP_ERRCODE_OPCODE_NOT_SUPPORTED    = 0x02, //Response if unsupported Op Code is received
	BLT_AUDIO_OTP_OACP_ERRCODE_INVALID_PARAMETER       = 0x03, //Response if Parameter received does not meet the requirements of the service.
	BLT_AUDIO_OTP_OACP_ERRCODE_INSUFFICIENT_RESOURCES  = 0x04, //
	BLT_AUDIO_OTP_OACP_ERRCODE_INVALID_OBJECT          = 0x05, //Response if the requested OACP procedure cannot be performed because the Current Object is an Invalid Object.
	BLT_AUDIO_OTP_OACP_ERRCODE_CHANNEL_UNAVAILABLE     = 0x06, //Response if the requested procedure could not be performed because an Object Transfer Channel was not available for use.
	BLT_AUDIO_OTP_OACP_ERRCODE_UNSUPPORTED_TYPE        = 0x07, //Response if the object type specified in the OACP procedure Type parameter is not supported by the Server.
	BLT_AUDIO_OTP_OACP_ERRCODE_PROCEDURE_NOT_PERMITTED = 0x08, //Response if the requested procedure is not permitted according to the properties of the Current Object 
	BLT_AUDIO_OTP_OACP_ERRCODE_OBJECT_LOCKED           = 0x09, //Response if the Current Object is temporarily locked by the Server.
	BLT_AUDIO_OTP_OACP_ERRCODE_OPERATION_FAILED        = 0x0A, //Response if the requested procedure failed for any reason other than those enumerated in this table.
}BLT_AUDIO_OTP_OACP_ERRCODE_ENUM;
typedef enum{
	BLT_AUDIO_OTP_OLCP_SUCCESS                      = 0x01, //Response for successful operation.
	BLT_AUDIO_OTP_OLCP_ERRCODE_OPCODE_NOT_SUPPORTED = 0x02, //Response if unsupported Op Code is received.
	BLT_AUDIO_OTP_OLCP_ERRCODE_INVALID_PARAMETER    = 0x03, //Response if Parameter received does not meet the requirements of the service.
	BLT_AUDIO_OTP_OLCP_ERRCODE_OPERATION_FAILED     = 0x04, //Response if the requested procedure failed for a reason other than those enumerated below.
	BLT_AUDIO_OTP_OLCP_ERRCODE_OUT_OF_BOUNDS        = 0x05, //Response if the requested procedure attempted to select an object beyond the first object or beyond the last object in the current list.
	BLT_AUDIO_OTP_OLCP_ERRCODE_TOO_MANY_OBJECTS     = 0x06, //Response if the requested procedure failed due to too many objects in the current list.
	BLT_AUDIO_OTP_OLCP_ERRCODE_NO_OBJECT            = 0x07, //Response if the requested procedure failed due to there being zero objects in the current list.
	BLT_AUDIO_OTP_OLCP_ERRCODE_OBJID_NOT_FOUND      = 0x08, //Response if the requested procedure failed due to there being no object with the requested Object ID.
}BLT_AUDIO_OTP_OLCP_ERRCODE_ENUM;


int blc_audio_otpAttRead(u16 connHandle, void* p);
int blc_audio_otpAttWrite(u16 connHandle, void* p);

int blc_audio_otpSetObjectName(u16 connHandle, u8 *pName, u16 nameLen);
int blc_audio_otpSendActionCtrl(u16 connHandle, u8 opcode, u8 *pData, u8 dataLen);
int blc_audio_otpSendListCtrl(u16 connHandle, u8 opcode, u8 *pData, u8 dataLen);



#endif //#if (BLC_AUDIO_OTP_ENABLE)

#endif //_AUDIO_OTP_H_

