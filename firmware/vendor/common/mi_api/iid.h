/*
 * iid.h
 *
 *  Created on: 2020Äê12ÔÂ11ÈÕ
 *      Author: mi
 */

#ifndef HANDLER_IID_H_
#define HANDLER_IID_H_
#include "vendor/common/user_config.h"
#include "ble_spec/mi_spec_type.h"

#define IID_2_Switch                                                          2
#define IID_2_1_On                                                            1
#define IID_2_2_Mode                                                          2
#define IID_2_3_val                                                           3

#define IID_2_1_Toggle                                                        1

#define IID_3_Switch                                                          3
#define IID_3_1_On                                                            1
#define IID_3_2_Mode                                                          2
#define IID_2_1_Toggle                                                        1

#define IID_4_Power_Manage                                                    4
#define IID_4_1_Anti_Flk                                                      1

#define IID_5_Environment                                                     5
#define IID_5_1_Relative_Humidity                                             1
#define IID_5_2_Temperature                                                   2

#define IID_6_Switch_Sensort                                                  6
#define IID_6_1_Status                                                        1
#define IID_6_2_Fault                                                         2
#define IID_6_1_Click                                                         1
#define IID_6_2_Long_Press                                                    2

#define IID_128_Server_Info                                                   128
#define IID_128_1_GMT_Offset                                                  1
#define IID_128_2_Weather                                                     2
#define IID_128_3_UTC_Time                                                    3

void P_2_1_On_doChange(bool newValue);
void P_2_1_Mode_doChange(uint8_t newValue);
void P_3_1_On_doChange(bool newValue);
void P_3_2_Mode_doChange(uint8_t newValue);
void P_4_1_Anti_Flk_doChange(bool newValue);
void P_5_1_Relative_Humidity_doChange(float newValue);
void P_5_2_Temperature_doChange(float newValue);
void P_6_1_Status_doChange(uint8_t newValue);
void P_6_2_Fault_doChange(int64_t newValue);

void P_128_1_GMT_Offset_doRequest(void);
void P_128_2_Weather_doRequest(void);
void P_128_3_UTC_Time_doRequest(void);

void E_6_1_Click_doEvent(void);
void E_6_2_Long_Press_doEvent(uint32_t arg1, int64_t arg2);

void on_property_set(property_operation_t *o);
void on_property_get(property_operation_t *o);
void on_action_invoke(action_operation_t *o);

typedef struct {
    bool        left_on;
    uint8_t     left_mode;
	uint16_t 	ctl;
    bool        right_on;
    uint8_t     right_mode;
    bool        anti_flk;
    float       humidity;
    float       temperature;
    int32_t     gmt_offset;
    int32_t     weather;
    uint32_t    utc_time;
	uint8_t     tid;
}switch_demo_t;

extern switch_demo_t two_key_switch;
void property_para_init();

#endif /* HANDLER_IID_H_ */
