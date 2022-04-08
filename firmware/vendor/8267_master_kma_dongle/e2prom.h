/********************************************************************************************************
 * @file	e2prom.h
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

/***************************
 * @brief	EEPROM initiate, call this function to initiate the whole storage space
 *
 * @param	none
 *
 * @return	none
 */
void e2prom_init();

/***************************
 * @brief	write variable length data to the e2prom
 *
 * @param	adr:	destination e2prom address
 * 			p:		stored data space address
 * 			len:	data length need to be stored
 *
 * @return	none
 */
void e2prom_write (int adr, u8 *p, int len);

/***************************
 * @brief	read variable length data to from e2prom
 *
 * @param	adr:	destination address
 * 			p:		address used to store read bytes
 * 			len:	read byte length
 *
 * @return	none
 */
void e2prom_read (int adr, u8 *p, int len);
