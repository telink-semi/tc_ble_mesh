/********************************************************************************************************
 * @file	compatibility.c
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
#include "../tl_common.h"

int LoadTblCmdSet (	const TBLCMDSET * pt, int size){
	int l=0;

	while (l<size) {
		unsigned int  cadr = ((u32)0x800000) | pt[l].adr;
		unsigned char cdat = pt[l].dat;
		unsigned char ccmd = pt[l].cmd;
		unsigned char cvld =(ccmd & TCMD_UNDER_WR);
		ccmd &= TCMD_MASK;
		if (cvld) {
			if (ccmd == TCMD_WRITE) {
				write_reg8 (cadr, cdat);
			}
			else if (ccmd == TCMD_WAREG) {
				WriteAnalogReg (cadr, cdat);
			}
			else if (ccmd == TCMD_WAIT) {
				WaitUs (pt[l].adr*256 + cdat);
			}
		}
		l++;
	}
	return size;
	
}





