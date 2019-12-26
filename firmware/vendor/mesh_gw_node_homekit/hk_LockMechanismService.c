/********************************************************************************************************
 * @file     hk_LockMechanismService.c
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
#define _HK_LOCK_MECHANISM_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_LockMechanismService.h"
#include "ble_UUID.h"
#include "../common/led_cfg.h"
#include "hk_TopUUID.h"

extern u32 session_timeout_tick;
extern u8 EventNotifyCNT;
extern unsigned int  store_char_change_value_timeout;
extern store_char_change_value_t* char_save_restore;


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#if !DOORLOCK_LED_DEMO_ENABLE
#if DOORLOCK_TYPE == TYPE_DEADBOLT
#define PinState(pin)    (((gpio_read(pin)) && ((pin) & (0xFF))) ? (1) : (0))
#define SetFlag(flag)    ((flag) = (1))
#define ClrFlag(flag)    ((flag) = (0))

enum sigLevel   {LOW, HIGH};
enum doorState  {UNLOCKED, LOCKED, JAMMED, UNKNOWN};
enum ability    {DISABLE, ENABLE};
enum dir        {MOTOR_CW, MOTOR_CCW};

#define PWMStart(id) BM_SET(reg_pwm_enable, BIT(id))
#define PWMStop (id) BM_CLR(reg_pwm_enable, BIT(id))

#define SW1         BUTTON_ON   //SW1
#define SW2         BUTTON_OFF  //SW2

#define SW_1A_PIN   GPIO_PD3    //G5
#define SW_1B_PIN   GPIO_PD0    //G2
#define SW_2A_PIN   GPIO_PD1    //G3
#define SW_2B_PIN   GPIO_PC6    //G0
#define AIN1_PIN    GPIO_PD4    //G6
#define AIN2_PIN    GPIO_PB5    //P4N
#define PWMA_PIN    GPIO_PB1    //P2N

#define LOCK_PWMID      PWMID_B       //pwm2
#define LOCK_INIT_LED   PWM_B

#define MY_PD2_INPUT_ENABLE ENABLE
#define MY_PC7_INPUT_ENABLE ENABLE
#define MY_PD3_INPUT_ENABLE ENABLE
#define MY_PD0_INPUT_ENABLE ENABLE
#define MY_PD1_INPUT_ENABLE ENABLE
#define MY_PC6_INPUT_ENABLE ENABLE
#define MY_PD4_INPUT_ENABLE ENABLE
#define MY_PB5_INPUT_ENABLE ENABLE

#define MY_PD2_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PC7_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PD3_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PD0_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PD1_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PC6_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PD4_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)
#define MY_PB5_OUTPUT_ENABLE ((ENABLE==1) ? 0 : 1)

#define MY_PD2_DATA_OUT 1
#define MY_PC7_DATA_OUT 1
#define MY_PD3_DATA_OUT 1
#define MY_PD0_DATA_OUT 1
#define MY_PD1_DATA_OUT 1
#define MY_PC6_DATA_OUT 1
#define MY_PD4_DATA_OUT 1
#define MY_PB5_DATA_OUT 1

#define MY_PD2_DATA_STRENGTH 1
#define MY_PC7_DATA_STRENGTH 1
#define MY_PD3_DATA_STRENGTH 1
#define MY_PD0_DATA_STRENGTH 1
#define MY_PD1_DATA_STRENGTH 1
#define MY_PC6_DATA_STRENGTH 1
#define MY_PD4_DATA_STRENGTH 1
#define MY_PB5_DATA_STRENGTH 1

#define MY_PD2_FUNC 0
#define MY_PC7_FUNC 0
#define MY_PD3_FUNC 0
#define MY_PD0_FUNC 0
#define MY_PD1_FUNC 0
#define MY_PC6_FUNC 0
#define MY_PD4_FUNC 0
#define MY_PB5_FUNC 0

#define MY_PULL_WAKEUP_SRC_PD2 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PC7 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PD3 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PD0 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PD1 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PC6 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PD4 GPIO_PULL_UP_1M
#define MY_PULL_WAKEUP_SRC_PB5 GPIO_PULL_UP_1M

#endif  //end of DOORLOCK_TYPE
#endif  //end of DOORLOCK_LED_DEMO_ENABLE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
unsigned char Lock_Current_State = 0;
unsigned char Lock_Target_State = 0;
unsigned char VAR_LOCKMECHANISM_UNKOWN=0;

unsigned short ccc_hLockMechanism_Lock_Current_State = 0;
unsigned short ccc_hLockMechanism_Lock_Target_State = 0;

#if LOCKMECHANISM_LOCKCURRENTSTATE_BRC_ENABLE
    unsigned char bc_para_hLockMechanism_Lock_Current_State = 1;
#endif

#if LOCKMECHANISM_LOCKTARGETSTATE_BRC_ENABLE
    unsigned char bc_para_hLockMechanism_Lock_Target_State = 1;
#endif

unsigned char Lock_Target_State_Pre = 0;
unsigned char Doorlock_Proc_Flg = 0;

const u8 my_lockmechanismServiceName[27] = "Telink Semi Smart Door Lock";

const u8 LockCurrentState_VALID_RANGE[2]    = {0x00, 0x03};
const u8 LockTargetState_VALID_RANGE[2]     = {0x00, 0x01};

const u8 LockCurrentState_VALID_STEP        = 0x01;
const u8 LockTargetState_VALID_STEP         = 0x01;

const hap_characteristic_desc_t hap_desc_char_Lock_Current_State_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, LockCurrentState_VALID_RANGE, &LockCurrentState_VALID_STEP, BC_PARA_LOCKMECHANISM_LOCKCURRENTSTATE};
const hap_characteristic_desc_t hap_desc_char_Lock_Target_State_R_S = {hap_PR_PW_TW_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, LockTargetState_VALID_RANGE, &LockTargetState_VALID_STEP, BC_PARA_LOCKMECHANISM_LOCKTARGETSTATE};

const hap_characteristic_desc_t hap_desc_char_lockMechanism_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if LOCKMECHANISM_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hLockMechanism_linked_svcs[] = {};
#endif

#if(LOCKMECHANISM_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_lockMechanism_A5_R_S = {LOCKMECHANISM_INST_NO+1, (0x02 + LOCKMECHANISM_AS_PRIMARY_SERVICE), LOCKMECHANISM_LINKED_SVCS_LENGTH, LOCKMECHANISM_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_lockMechanism_A5_R_S = {LOCKMECHANISM_INST_NO+1, (0x00 + LOCKMECHANISM_AS_PRIMARY_SERVICE), LOCKMECHANISM_LINKED_SVCS_LENGTH, LOCKMECHANISM_LINKED_SVCS};
#endif


#if !DOORLOCK_LED_DEMO_ENABLE
#if DOORLOCK_TYPE == TYPE_DEADBOLT
unsigned int  LockRunningTime = 0;
const u16 MotorSpeed = 3500;    //default = 0x800, Max = 4095;

//Lock and unlock process flag
static u8 LockPosFlag = 0;
static u8 LockNegFlag = 0;
static u8 UnlockPosFlag = 0;
static u8 UnlockNegFlag = 0;

//Debug use only
#if DOORLOCK_DEBUG_ENABLE
static u8 sw_1a;
static u8 sw_1b;
static u8 sw_2a;
static u8 sw_2b;

static u32 Lock_Test01 = 0;
static u32 Lock_Test02 = 0;
static u32 Lock_Test03 = 0;

static u32 Unlock_Test01 = 0;
static u32 Unlock_Test02 = 0;
static u32 Unlock_Test03 = 0;

#endif  //end of DOORLOCK_DEBUG_ENABLE
#endif  //end of DOORLOCK_TYPE
#endif  //end of DOORLOCK_LED_DEMO_ENABLE


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
#if DOORLOCK_LED_DEMO_ENABLE
void lock_led_w_enable(u8 en)
{
    if(en) {
        pwm_start(PWMID_W);
        pwm_set_cycle_and_duty(PWMID_W, PMW_MAX_TICK, PMW_MAX_TICK);
    } else {
        pwm_stop(PWMID_W);
        pwm_set_cycle_and_duty(PWMID_W, PMW_MAX_TICK, 0);
    }
    Lock_Target_State_Pre = en;
}

#else
#if DOORLOCK_TYPE == TYPE_DEADBOLT
void motor_move(u16 spd, u8 dir) //-inline void
{
    gpio_write(AIN1_PIN, dir);
    gpio_write(AIN2_PIN, !dir);

    pwm_set_cycle_and_duty(LOCK_PWMID, 0x0FFF, spd);
    pwm_start(LOCK_PWMID);
}

void motor_stop(void) //-inline void
{
    pwm_stop(LOCK_PWMID);
}

u8 lock_state_chk(void)
{
    u8 lock_state = 0;

#if DOORLOCK_DEBUG_ENABLE
    sw_2a = PinState(SW_2A_PIN);
    sw_2b = PinState(SW_2B_PIN);
#else
    // Switch state variables
    u8 sw_1a;
    u8 sw_1b;
#endif

    sw_1a = PinState(SW_1A_PIN);
    sw_1b = PinState(SW_1B_PIN);

#if LOCKING_DIRECTION
    if     ( (sw_1a == 0) && (sw_1b == 1) ) lock_state = LOCKED;
    else if( (sw_1a == 1) && (sw_1b == 0) ) lock_state = UNLOCKED;

#else
    if     ( (sw_1a == 0) && (sw_1b == 1) ) lock_state = UNLOCKED;
    else if( (sw_1a == 1) && (sw_1b == 0) ) lock_state = LOCKED;

#endif
    else if( (sw_1a == 0) && (sw_1b == 0) ) lock_state = JAMMED;
    else if( (sw_1a == 1) && (sw_1b == 1) ) lock_state = UNKNOWN;

    return lock_state;
}

void lock(void)
{
    if((UnlockPosFlag == 1) || (UnlockNegFlag == 1)) {
        ClrFlag(UnlockPosFlag);
        ClrFlag(UnlockNegFlag);
    }

    //Positive locking cycle
    if(LockNegFlag == 0) {
        if(LockPosFlag == 0)
        {
            SetFlag(LockPosFlag);

            #if LOCKING_DIRECTION
            motor_move(MotorSpeed, MOTOR_CW);
            #else
            motor_move(MotorSpeed, MOTOR_CCW);
            #endif
        }
        #if LOCKING_DIRECTION
        else if((PinState(SW_1A_PIN) == 0) && (PinState(SW_1B_PIN) == 1) && (PinState(SW_2A_PIN) == 0) && (PinState(SW_2B_PIN) == 1))
        #else
        else if((PinState(SW_1A_PIN) == 1) && (PinState(SW_1B_PIN) == 0) && (PinState(SW_2A_PIN) == 1) && (PinState(SW_2B_PIN) == 0))
        #endif
        {
            motor_stop();
            ClrFlag(LockPosFlag);
        }
    }

    //Negative locking cycle
    if(LockPosFlag == 0) {
        if(LockNegFlag == 0)
        {
            SetFlag(LockNegFlag);

            #if LOCKING_DIRECTION
            motor_move(MotorSpeed, MOTOR_CCW);
            #else
            motor_move(MotorSpeed, MOTOR_CW);
            #endif
        }
        else if((PinState(SW_2A_PIN) == 1) && (PinState(SW_2B_PIN) == 1))
        {
            motor_stop();
            ClrFlag(LockNegFlag);
            Doorlock_Proc_Flg = DISABLE;
        }
    }
}

void unlock(void)
{
    if((LockPosFlag == 1) || (LockNegFlag == 1)) {
        ClrFlag(LockPosFlag);
        ClrFlag(LockNegFlag);
    }

    //Positive unlocking cycle
    if(UnlockNegFlag == 0) {
        if(UnlockPosFlag == 0)
        {
            SetFlag(UnlockPosFlag);

            #if LOCKING_DIRECTION
            motor_move(MotorSpeed, MOTOR_CCW);
            #else
            motor_move(MotorSpeed, MOTOR_CW);
            #endif
        }
        #if LOCKING_DIRECTION
        else if((PinState(SW_1A_PIN) == 1) && (PinState(SW_1B_PIN) == 0) && (PinState(SW_2A_PIN) == 1) && (PinState(SW_2B_PIN) == 0))
        #else
        else if((PinState(SW_1A_PIN) == 0) && (PinState(SW_1B_PIN) == 1) && (PinState(SW_2A_PIN) == 0) && (PinState(SW_2B_PIN) == 1))
        #endif
        {
            motor_stop();
            ClrFlag(UnlockPosFlag);
        }
    }

    //Negative unlocking cycle
    if(UnlockPosFlag == 0) {
        if(UnlockNegFlag == 0)
        {
            SetFlag(UnlockNegFlag);

            #if LOCKING_DIRECTION
            motor_move(MotorSpeed, MOTOR_CW);
            #else
            motor_move(MotorSpeed, MOTOR_CCW);
            #endif
        }
        else if((PinState(SW_2A_PIN) == 1) && (PinState(SW_2B_PIN) == 1))
        {
            motor_stop();
            ClrFlag(UnlockNegFlag);
            Doorlock_Proc_Flg = DISABLE;
        }
    }
}

void lock_on_notify_led(unsigned char value)
{
    if(value) {
        gpio_set_func(LOCK_INIT_LED, AS_GPIO);
        gpio_set_output_en(LOCK_INIT_LED, 1);
        gpio_set_data_strength(LOCK_INIT_LED, 0);
    }
    gpio_write(LOCK_INIT_LED, value);
}

void lock_gpio_init(void)
{
    reg_gpio_pb_setting1 |=    //0x588
            (MY_PB5_INPUT_ENABLE<<13)|(MY_PB5_OUTPUT_ENABLE<<21)|(MY_PB5_DATA_OUT<<29)|(MY_PB5_FUNC==AS_GPIO ? BIT(21):0);

    reg_gpio_pc_setting1 |=    //0x590
            (MY_PC6_INPUT_ENABLE<<14)|(MY_PC6_OUTPUT_ENABLE<<22)|(MY_PC6_DATA_OUT<<30)|(MY_PC6_FUNC==AS_GPIO ? BIT(22):0)|
            (MY_PC7_INPUT_ENABLE<<15)|(MY_PC7_OUTPUT_ENABLE<<23)|(MY_PC7_DATA_OUT<<31)|(MY_PC7_FUNC==AS_GPIO ? BIT(23):0);

    reg_gpio_pd_setting1 |=    //0x598
            (MY_PD0_INPUT_ENABLE<< 8)|(MY_PD0_OUTPUT_ENABLE<<16)|(MY_PD0_DATA_OUT<<24)|(MY_PD0_FUNC==AS_GPIO ? BIT(16):0)|
            (MY_PD1_INPUT_ENABLE<< 9)|(MY_PD1_OUTPUT_ENABLE<<17)|(MY_PD1_DATA_OUT<<25)|(MY_PD1_FUNC==AS_GPIO ? BIT(17):0)|
            (MY_PD2_INPUT_ENABLE<<10)|(MY_PD2_OUTPUT_ENABLE<<18)|(MY_PD2_DATA_OUT<<26)|(MY_PD2_FUNC==AS_GPIO ? BIT(18):0)|
            (MY_PD3_INPUT_ENABLE<<11)|(MY_PD3_OUTPUT_ENABLE<<19)|(MY_PD3_DATA_OUT<<27)|(MY_PD3_FUNC==AS_GPIO ? BIT(19):0)|
            (MY_PD4_INPUT_ENABLE<<12)|(MY_PD4_OUTPUT_ENABLE<<20)|(MY_PD4_DATA_OUT<<28)|(MY_PD4_FUNC==AS_GPIO ? BIT(20):0);

    analog_write(0x0d, (MY_PULL_WAKEUP_SRC_PB5<<6));
    analog_write(0x10, (MY_PULL_WAKEUP_SRC_PC7<<2)|(MY_PULL_WAKEUP_SRC_PD0<<4)|(MY_PULL_WAKEUP_SRC_PD1<<6)|(MY_PULL_WAKEUP_SRC_PC6<<0));
    analog_write(0x11, (MY_PULL_WAKEUP_SRC_PD2<<0)|(MY_PULL_WAKEUP_SRC_PD3<<2)|(MY_PULL_WAKEUP_SRC_PD4<<4));
}

void lock_pwm_init(void)
{
    gpio_set_func(PWMA_PIN, AS_PWM);             //Set the pin as PWM function
    gpio_set_output_en(PWMA_PIN, 1);             //Enable the output function

    reg_rst1 &= 0xEF;
    BM_SET(reg_clk_en1, 0x10);
    BM_SET(reg_pwm_n_invert, BIT(2));            //Invert PWM_INV output, PWM_CC1
    reg_pwm_clk = 0x0F;                          //Set PWM Clock

    pwm_set_cycle_and_duty(LOCK_PWMID, 0x0FFF, 0x0800);         //Set PWM_TMAX and PWM_TMAX
}

void lock_motor_reset(void)
{
    motor_move(MotorSpeed, MOTOR_CCW);
    LockRunningTime = clock_time();
    lock_on_notify_led(1);

    do
    {
        if((PinState(SW1) == 0) && (PinState(SW2) == 0)) { //Force stop while press both SW1 and SW2
            break;
        } else if(LockRunningTime && clock_time_exceed(LockRunningTime, 15000*1000)) {  //Force stop after 15 sec
            LockRunningTime = 0;
            break;
        }
    } while (!((PinState(SW_2A_PIN) == 1) && (PinState(SW_2B_PIN) == 1)));

    lock_on_notify_led(0);
    motor_stop();
}

void lock_init(void)
{
    #if (DOORLOCK_TYPE == TYPE_DEADBOLT)
    lock_gpio_init();
    lock_pwm_init();
    lock_motor_reset();

    //Other types
    #elif (DOORLOCK_TYPE == TYPE_JIMMYPROOF)
    static u8 JIMMYPROOF_LOCK = 0;
    JIMMYPROOF_LOCK++;

    #endif  //end of DOORLOCK_TYPE
}

#endif //end of DOORLOCK_TYPE
#endif //end of DDOORLOCK_LED_DEMO_ENABLE


void doorlock_proc(void)
{
#if DOORLOCK_LED_DEMO_ENABLE
    if(Doorlock_Proc_Flg) {
        Doorlock_Proc_Flg = 0;
        lock_led_w_enable(Lock_Target_State);

        Lock_Current_State = Lock_Target_State;

		extern st_ll_conn_slave_t		bltc;
		if(EventNotifyCNT)
			if(bltc.conn_terminate_pending == 0)
            {
                if(indicate_handle_no_send_once_after_checkit(LOCKMECHANISM_HANDLE_NUM_LOCKCURRENTSTATE) == 1)
                    sleep_us(250);
                if(indicate_handle_no_send_once_after_checkit(LOCKMECHANISM_HANDLE_NUM_LOCKTARGETSTATE) == 1)
                    sleep_us(250);
            }
        blt_adv_set_global_state ();
    }

#else
    if(Doorlock_Proc_Flg) {
        u8 lockstate = lock_state_chk();
        if( (Lock_Target_State == LOCKED) && ((lockstate != LOCKED) || LockPosFlag || LockNegFlag) ) {
            lock();
        } else if( (Lock_Target_State == UNLOCKED) && ((lockstate != UNLOCKED) || UnlockPosFlag || UnlockNegFlag) ) {
            unlock();
        }

        //Force stop after 15 sec
        if(LockRunningTime && clock_time_exceed(LockRunningTime, 15000*1000)) {
            LockRunningTime = 0;
            motor_stop();
            Doorlock_Proc_Flg = DISABLE;
        }

        //Force stop while press both SW1 and SW2
        if((PinState(SW1) == 0) && ((PinState(SW2) == 0))) {
            motor_stop();
            Doorlock_Proc_Flg = DISABLE;
        }
    } else {
        Lock_Current_State = lock_state_chk();
    }
#endif
}


int LockCurrentStateReadCallback(void *pp)      //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LockTargetStateReadCallback(void *pp)       //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int LockTargetStateWriteCallback(void *pp)      //
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if LOCKMECHANISM_SERVICE_ENABLE
        LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE_RESTORE
    #endif
    } else {
        if(Lock_Target_State != Lock_Target_State_Pre) {
            store_char_value_to_flash();
            Lock_Target_State_Pre = Lock_Target_State;
        }
    }

#if !DOORLOCK_LED_DEMO_ENABLE
#if DOORLOCK_TYPE == TYPE_DEADBOLT
    LockRunningTime = clock_time();
#endif
#endif

    Doorlock_Proc_Flg = 1;
    //-doorlock_proc();

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hLockMechanism_Lock_Current_StateWriteCB(void *pp)
{
    static u16 ccc_hLockMechanism_Lock_Current_State_pre = 0;

    if(ccc_hLockMechanism_Lock_Current_State_pre != ccc_hLockMechanism_Lock_Current_State)
    {
        if(ccc_hLockMechanism_Lock_Current_State == 0)
        {
            ccc_hLockMechanism_Lock_Current_State_pre = ccc_hLockMechanism_Lock_Current_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMECHANISM_HANDLE_NUM_LOCKCURRENTSTATE);
        }
        else
        {
            ccc_hLockMechanism_Lock_Current_State_pre = ccc_hLockMechanism_Lock_Current_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMECHANISM_HANDLE_NUM_LOCKCURRENTSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hLockMechanism_Lock_Target_StateWriteCB(void *pp)
{
    static u16 ccc_hLockMechanism_Lock_Target_State_pre = 0;

    if(ccc_hLockMechanism_Lock_Target_State_pre != ccc_hLockMechanism_Lock_Target_State)
    {
        if(ccc_hLockMechanism_Lock_Target_State == 0)
        {
            ccc_hLockMechanism_Lock_Target_State_pre = ccc_hLockMechanism_Lock_Target_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(LOCKMECHANISM_HANDLE_NUM_LOCKTARGETSTATE);
        }
        else
        {
            ccc_hLockMechanism_Lock_Target_State_pre = ccc_hLockMechanism_Lock_Target_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(LOCKMECHANISM_HANDLE_NUM_LOCKTARGETSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
