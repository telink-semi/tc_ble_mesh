/********************************************************************************************************
 * @file	qdec.h
 *
 * @brief	This is the head file for TLSR8258
 *
 * @author	Driver Group
 * @date	December 5, 2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
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
 *
 *******************************************************************************************************/
#pragma once

#include "clock.h"
#include "pm.h"
#include "analog.h"
#include "register.h"

/**
 * qedc input channel type of A
 */
typedef enum
{
	PA2A,
	PA3A,
	PB6A,
	PB7A,
	PC2A,
	PC3A,
	PD6A,
	PD7A,

}QDEC_InputAchTypeDef;

/**
 * qedc input channel type of A
 */
typedef enum
{
	PA2B,
	PA3B,
	PB6B,
	PB7B,
	PC2B,
	PC3B,
	PD6B,
	PD7B,
}QDEC_InputBchTypeDef;


#define reg_qdec_set       0xd1
#define reg_qdec_channel_a 0xd2
#define reg_qdec_channel_b 0xd3

#define reg_qdec_mode 0xd7

/**
 * qedc mode
 */
typedef enum
{
	COMMON_MODE,
	DOUBLE_ACCURACY_MODE,
}QDEC_ModeTypeDef;

#define rge_qdec_load 	0xd8
#define reg_qdec_count 	0xd0
#define reg_qdec_reset 0xd6

/**
 * @brief      This function servers to set input port.
 * @param[in]  QDEC_InputAchTypeDef - input types of A channel.
 * @param[in]  QDEC_InputBchTypeDef - input types of A channel.
 * @return     none.
 */
void qdec_set_pin(QDEC_InputAchTypeDef channelA,QDEC_InputBchTypeDef channelB);

/**
 * @brief      This function servers to set qdec's mode.
 * @param[in]  QDEC_ModeTypeDef - mode type to select.
 * @return     none.
 */
void qdec_set_mode(QDEC_ModeTypeDef mode);

/**
 * @brief      This function servers to initials qedc source clock.
 * @param[in]  none.
 * @return     none.
 */
void qdec_clk_en(void);

/**
 * @brief      This function servers to read hardware counting value.
 * @param[in]  none.
 * @return     hardware counting value.
 */
unsigned char qdec_get_count_value(void);

/**
 * @brief      This function servers to reset the counter.
 * @param[in]  none.
 * @return     none.
 */
void qdec_clear_conuter(void);

/**
 * @brief      This function servers to set hardware debouncing.
 * @param[in]  thrsh - lower the value of thrsh,will be regard as jitter.
 * @return     none.
 */
void qdec_set_debouncing(char thrsh);
