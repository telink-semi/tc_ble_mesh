/********************************************************************************************************
 * @file	debug.h
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
#ifndef _DEBUG_H
#define _DEBUG_H


#if 0
#include <stdio.h>


/****************
 *  Definitions
 ****************/
/* Set values of the following to 1 to turn on debug 
 * printing for the desired code areas.
 */
#define DEBUG_APP     1     /* application debug */
#define DEBUG_CCM     0     /* ccm debug */


#if (DEBUG_APP)
#define dbgp_app(format, a...)     printf(format, ## a)
#else
#define dbgp_app(format, a...)  
#endif

#if (DEBUG_CCM)
#define dbgp_ccm(format, a...)     printf(format, ## a)
#else
#define dbgp_ccm(format, a...)  
#endif
#endif

#endif /* _DEBUG_H */

