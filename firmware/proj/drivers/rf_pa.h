/********************************************************************************************************
 * @file	rf_pa.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef BLT_PA_H_
#define BLT_PA_H_

#include "../tl_common.h"



// for not 826x,PA pin use integrated RF synthesizer, refer to RF_LNARxPinDef and RF_PATxPinDef for pa pin!!!
#ifndef PA_TXEN_PIN
#define PA_TXEN_PIN                         GPIO_PC7
#endif

#ifndef PA_RXEN_PIN
#define PA_RXEN_PIN                         GPIO_PC6
#endif



#define PA_TYPE_OFF							0
#define PA_TYPE_TX_ON						1
#define PA_TYPE_RX_ON						2


typedef void (*rf_pa_callback_t)(int type);
extern rf_pa_callback_t  blc_rf_pa_cb;



void rf_pa_init(void);


#endif /* BLT_PA_H_ */
