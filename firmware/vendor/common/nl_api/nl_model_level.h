/********************************************************************************************************
 * @file	nl_model_level.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#ifndef WIN32
#include <stdbool.h>
#include <stdint.h>
#endif
#include "../user_config.h"

#if NL_API_ENABLE
/******************************
 ***** LEVEL SERVER MODEL *****
 ******************************/

/**
 * Callback function
 * @fn void level_state_changed(uint8_t element_index, int16_t old_level, int16_t new_level)
 * @param[in] element_idx is an index to the element in this device on which the level changed
 * @param[in] old_level is the previous value of the generic level state
 * @param[in] new_level is the new value of the generic level state
 *
 * Registering function
 * @brief     Registers callback that gets called whenever there is a level change in the model
 * @param[in] level_state_changed ptr to function callback
 *           
 */
//void nl_register_level_server_state_changed_callback(void (*level_state_changed)(uint8_t element_idx, int16_t present_level, int16_t target_level));
typedef void (*nl_level_state_changed_t)(uint8_t element_idx, int16_t present_level, int16_t target_level);
extern nl_level_state_changed_t	p_nl_level_state_changed;
void nl_register_level_server_state_changed_callback(void *p);

void nl_level_server_state_changed(uint8_t element_idx, int16_t pre_level, int16_t tar_level);


/**
 * gets the current state from the level server
 * @param[in] element_idx index to the element in this device on which the level changed. 0-based index
 * @return the current level state in the level server
 */
int16_t nl_get_current_level_in_level_server(uint16_t element_idx);

#define CB_NL_LEVEL_STATE_CHANGE(p, element_idx, present_level, target_level)	
#else
#define nl_level_server_state_changed(element_idx, present_level, target_level) 
#endif
