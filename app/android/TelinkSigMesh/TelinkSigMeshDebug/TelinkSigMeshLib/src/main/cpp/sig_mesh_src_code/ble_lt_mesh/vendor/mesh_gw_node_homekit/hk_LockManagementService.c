/********************************************************************************************************
 * @file     hk_LockManagementService.c
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
#define _HK_LOCK_MANAGEMENT_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_LockManagementService.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"

extern u32 session_timeout_tick;
extern u8 EventNotifyCNT;
extern store_char_change_value_t* char_save_restore;


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
// Types of Lock Control Point
#define LCP_CMD_ReadLogsFromTime    0x00    // format length : 4B(int)
                                            // Read the logs starting at the value, which is in seconds since epoch time. A zero-length value will read all logs since the last wipe.
#define LCP_CMD_ClearLogs           0x02    // format length : 4B(int), clear the lock's logs
#define LCP_CMD_SetCurrentTime      0x03    // format length : 4B(int), Set the Lock's current time (seconds)

// Types of Logs Characteristic Response
#define LCR_RSP_ACCESSOR        0x01    // format length : string name
#define LCR_RSP_TIME            0x02    // format length : 4B(int)
#define LCR_RSP_ACTION          0x03    // format length : 4B(int)
                                        // Bit 1: Lock Mechanism State read
                                        // Bit 2: Lock Mechanism Action occurred
                                        // Bit 3: All Log cleared
                                        // Bit 6-8: Reserved
#define LCR_RSP_VENDORSPECIFIC  0x04    // format length : data, limited to 255 bytes
#define LCR_RSP_SEPARATOR       0x05    // format length : none


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
unsigned char Lock_Control_Point[24] = {LCP_CMD_ReadLogsFromTime, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const unsigned char Lock_Management_Version[4] = "1.0";
unsigned char LockManagementLogs[24] = {LCR_RSP_ACCESSOR, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//////////////  the test initial value for test usage only ////////////////////////
//--unsigned char LockManagementLogs[24] = {LCR_RSP_TIME, 0x04, 0x58, 0xC9, 0x2C, 0xC0};          // TLV8, initial testing
// Using epoch time format and the reference links are as below :
// https://en.wikipedia.org/wiki/Unix_time
// https://www.epochconverter.com/
/////////////////////////////////////////////
unsigned char AudioFeedback = 0;
unsigned int AutoSecurityTimeout = 0;
unsigned char AdminOnlyAccess = 0;
unsigned char LockLastKnownAction = 0;
unsigned char LMCurrentDoorState = 0;
unsigned char LMMotionDetected = 0;

unsigned char VAR_LOCKMANAGEMENT_UNKOWN=0;

unsigned short ccc_LockManagementLogs = 0;
unsigned short ccc_AudioFeedback = 0;
unsigned short ccc_AutoSecurityTimeout = 0;
unsigned short ccc_AdminOnlyAccess = 0;
unsigned short ccc_LockLastKnownAction = 0;
unsigned short ccc_LMCurrentDoorState = 0;
unsigned short ccc_LMMotionDetected = 0;

#if LOCKMANAGEMENT_AUDIO_FEEDBACK_BRC_ENABLE
    unsigned char bc_para_AudioFeedback = 1;
#endif

#if LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_BRC_ENABLE
    unsigned char bc_para_AutoSecurityTimeout = 1;
#endif

#if LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_BRC_ENABLE
    unsigned char bc_para_AdminOnlyAccess = 1;
#endif

#if LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_BRC_ENABLE
    unsigned char bc_para_LockLastKnownAction = 1;
#endif

#if LOCKMANAGEMENT_CURRENT_DOOR_STATE_BRC_ENABLE
    unsigned char bc_para_LMCurrentDoorState = 1;
#endif

#if LOCKMANAGEMENT_MOTION_DETECTED_BRC_ENABLE
    unsigned char bc_para_LMMotionDetected = 1;
#endif

const u8 my_lockmanagementServiceName[23] = "Lock Management Service";

const u8 LAST_ACTION_VALID_RANGE[2] = { 0x00, 0x08};
const u8 CURRENT_DOOR_STATE_VALID_RANGE[2] = { 0x00, 0x04};
const u8 AUTO_SECURITY_TIMEOUT_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x80, 0x51, 0x01, 0x00};  // 0 to 86400 seconds

const u8 LAST_ACTION_VALID_STEP = 0x01;
const u8 CURRENT_DOOR_STATE_VALID_STEP = 0x01;
const u8 AUTO_SECURITY_TIMEOUT_VALID_STEP[4] = { 0x01, 0x00, 0x00, 0x00};

const hap_characteristic_desc_t hap_desc_char_LOGS_R_S = {hap_PRead_NOTIFY_NO_BRC, 0, CFG_TLV8, 0, 0, 0, 0, 0, 0, 0, 0};
const hap_characteristic_desc_t hap_desc_char_AUDIO_FEEDBACK_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_LOCKMANAGEMENT_AUDIO_FEEDBACK};
const hap_characteristic_desc_t hap_desc_char_AUTO_SECURITY_TIMEOUT_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT32_SECONDS, 0, 8, 4, 0, 0, AUTO_SECURITY_TIMEOUT_VALID_RANGE, AUTO_SECURITY_TIMEOUT_VALID_STEP, BC_PARA_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT};
const hap_characteristic_desc_t hap_desc_char_ADMIN_ONLY_ACCESS_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS};
const hap_characteristic_desc_t hap_desc_char_LOCK_LAST_KNOWN_ACTION_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, LAST_ACTION_VALID_RANGE, &LAST_ACTION_VALID_STEP, BC_PARA_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION};
const hap_characteristic_desc_t hap_desc_char_CURRENT_DOOR_STATE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, CURRENT_DOOR_STATE_VALID_RANGE, &CURRENT_DOOR_STATE_VALID_STEP, BC_PARA_LOCKMANAGEMENT_CURRENT_DOOR_STATE};
const hap_characteristic_desc_t hap_desc_char_MOTION_DETECTED_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_LOCKMANAGEMENT_MOTION_DETECTED};

const hap_characteristic_desc_t hap_desc_char_lockManagement_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if LOCKMANAGEMENT_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hLockManagement_linked_svcs[] = {};
#endif

#if(LOCKMANAGEMENT_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_lockManagement_A5_R_S = {LOCKMANAGEMENT_INST_NO+1, (0x02 + LOCKMANAGEMENT_AS_PRIMARY_SERVICE), LOCKMANAGEMENT_LINKED_SVCS_LENGTH, LOCKMANAGEMENT_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_lockManagement_A5_R_S = {LOCKMANAGEMENT_INST_NO+1, (0x00 + LOCKMANAGEMENT_AS_PRIMARY_SERVICE), LOCKMANAGEMENT_LINKED_SVCS_LENGTH, LOCKMANAGEMENT_LINKED_SVCS};
#endif


#if 0  // Demo Only
typedef struct {
    unsigned char type01;
    unsigned char length01;
    unsigned short log_time;
    unsigned char type02;
    unsigned char length02;
    unsigned short log_action;
    unsigned char type03;
    unsigned char length03;
    unsigned short log_vendorSpecific;
    unsigned char type04;
    unsigned char length04;
    u8* log_accessor;
} LM_log_dat_t;

// example :    sample log#10
LM_log_dat_t lm_log10 = {
    LCR_RSP_TIME,           4, 1489572010,         // LCR_RSP_TIME : recorded epoch time, 2017/3/15 PM 6:00:10 GMT+8:00
    LCR_RSP_ACTION,         1, 0x01,               // LCR_RSP_ACTION Bit 1: Lock Mechanism State read
    LCR_RSP_VENDORSPECIFIC, 1, 10,                 // LCR_RSP_VENDORSPECIFIC: index of log
    LCR_RSP_ACCESSOR,       7, "Telink1",          // LCR_RSP_ACCESSOR
};

// example :    sample log#11
LM_log_dat_t lm_log11 = {
    LCR_RSP_TIME,           4, 1489579825,         // LCR_RSP_TIME : recorded epoch time, 2017/3/15 PM 8:10:25 GMT+8:00
    LCR_RSP_ACTION,         1, 0x02,               // LCR_RSP_ACTION Bit 2: Lock Mechanism Action occurred
    LCR_RSP_VENDORSPECIFIC, 1, 11,                 // LCR_RSP_VENDORSPECIFIC: index of log
    LCR_RSP_ACCESSOR,       7, "Telink2",          // LCR_RSP_ACCESSOR
};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int LockControlPointWriteCallback(void *pp)     //
{
    //blt_adv_set_global_state ();	//this char do not support Notify, so no need update the GSN.

#if 0
        u8 LCP_type = Lock_Control_Point[0];
        u8 LCP_len = Lock_Control_Point[1];

        if(LCP_type == LCP_CMD_ReadLogsFromTime) {
        }
        else if (LCP_type == LCP_CMD_ClearLogs) {
        }
        else if (LCP_type == LCP_CMD_SetCurrentTime) {
        }

        printf("TYPE : %2X\n", LCP_type);
        printf("LENGTH : %2X\n",LCP_len);

        printf("VALUE : ");
        for(int i=0; i<LCP_len; i++)
            printf("%2X ", Lock_Control_Point[2+i]);
        printf("\n");
#endif

    memcpy(LockManagementLogs, Lock_Control_Point, sizeof(Lock_Control_Point));

    /////////////////////////////////////////////////////////////////////////
    //////  check and wait LCP ACTION ARE finished                      /////
    //////  then invoke notify/indicate LOCKMANAGEMENT_HANDLE_NUM_LOGS  /////
    //////  to controller                                               /////
    /////////////////////////////////////////////////////////////////////////
    // log notification
    extern int indicate_handle_no_send_once_after_checkit(unsigned char handle_no);
    if(EventNotifyCNT)
        if(indicate_handle_no_send_once_after_checkit(LOCKMANAGEMENT_HANDLE_NUM_LOGS) == 1)
            sleep_us(250);
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LockManagementVersionReadCallback(void *pp) //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LockManagementLogsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AudioFeedbackWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if LOCKMANAGEMENT_SERVICE_ENABLE
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AudioFeedbackReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AutoSecurityTimeoutWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~86400
        assign_current_flash_char_value_addr();
    #if LOCKMANAGEMENT_SERVICE_ENABLE
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AutoSecurityTimeoutReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AdminOnlyAccessWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if LOCKMANAGEMENT_SERVICE_ENABLE
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AdminOnlyAccessReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LockLastKnownActionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LMCurrentDoorStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LMMotionDetectedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_LockManagementLogsWriteCB(void *pp)
{
    static u16 ccc_LockManagementLogs_pre = 0;

    if(ccc_LockManagementLogs_pre != ccc_LockManagementLogs)
    {
        if(ccc_LockManagementLogs == 0)
        {
            ccc_LockManagementLogs_pre = ccc_LockManagementLogs;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_LOGS);
        }
        else
        {
            ccc_LockManagementLogs_pre = ccc_LockManagementLogs;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_LOGS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_AudioFeedbackWriteCB(void *pp)
{
    static u16 ccc_AudioFeedback_pre = 0;

    if(ccc_AudioFeedback_pre != ccc_AudioFeedback)
    {
        if(ccc_AudioFeedback == 0)
        {
            ccc_AudioFeedback_pre = ccc_AudioFeedback;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_AUDIO_FEEDBACK);
        }
        else
        {
            ccc_AudioFeedback_pre = ccc_AudioFeedback;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_AUDIO_FEEDBACK);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_AutoSecurityTimeoutWriteCB(void *pp)
{
    static u16 ccc_AutoSecurityTimeout_pre = 0;

    if(ccc_AutoSecurityTimeout_pre != ccc_AutoSecurityTimeout)
    {
        if(ccc_AutoSecurityTimeout == 0)
        {
            ccc_AutoSecurityTimeout_pre = ccc_AutoSecurityTimeout;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_AUTO_SECURITY_TIMEOUT);
        }
        else
        {
            ccc_AutoSecurityTimeout_pre = ccc_AutoSecurityTimeout;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_AUTO_SECURITY_TIMEOUT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_AdminOnlyAccessWriteCB(void *pp)
{
    static u16 ccc_AdminOnlyAccess_pre = 0;

    if(ccc_AdminOnlyAccess_pre != ccc_AdminOnlyAccess)
    {
        if(ccc_AdminOnlyAccess == 0)
        {
            ccc_AdminOnlyAccess_pre = ccc_AdminOnlyAccess;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_ADMIN_ONLY_ACCESS);
        }
        else
        {
            ccc_AdminOnlyAccess_pre = ccc_AdminOnlyAccess;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_ADMIN_ONLY_ACCESS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_LockLastKnownActionWriteCB(void *pp)
{
    static u16 ccc_LockLastKnownAction_pre = 0;

    if(ccc_LockLastKnownAction_pre != ccc_LockLastKnownAction)
    {
        if(ccc_LockLastKnownAction == 0)
        {
            ccc_LockLastKnownAction_pre = ccc_LockLastKnownAction;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_LOCK_LAST_KNOWN_ACTION);
        }
        else
        {
            ccc_LockLastKnownAction_pre = ccc_LockLastKnownAction;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_LOCK_LAST_KNOWN_ACTION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_LMCurrentDoorStateWriteCB(void *pp)
{
    static u16 ccc_LMCurrentDoorState_pre = 0;

    if(ccc_LMCurrentDoorState_pre != ccc_LMCurrentDoorState)
    {
        if(ccc_LMCurrentDoorState == 0)
        {
            ccc_LMCurrentDoorState_pre = ccc_LMCurrentDoorState;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_CURRENT_DOOR_STATE);
        }
        else
        {
            ccc_LMCurrentDoorState_pre = ccc_LMCurrentDoorState;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_CURRENT_DOOR_STATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_LMMotionDetectedWriteCB(void *pp)
{
    static u16 ccc_LMMotionDetected_pre = 0;

    if(ccc_LMMotionDetected_pre != ccc_LMMotionDetected)
    {
        if(ccc_LMMotionDetected == 0)
        {
            ccc_LMMotionDetected_pre = ccc_LMMotionDetected;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMANAGEMENT_HANDLE_NUM_MOTION_DETECTED);
        }
        else
        {
            ccc_LMMotionDetected_pre = ccc_LMMotionDetected;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMANAGEMENT_HANDLE_NUM_MOTION_DETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
