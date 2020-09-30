/********************************************************************************************************
 * @file     llms_scan.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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

#ifndef LLMS_SCAN_H_
#define LLMS_SCAN_H_



/************************************ User Interface  ******************************************************/
void 	blc_llms_initScanning_module(void);



/*********************************** Stack Interface, user can not use!!! **********************************/

void 	blt_llms_switchScanChannel (int set_chn);
int  	blt_llms_procScanPkt(u8 *raw_pkt, u8 *new_pkt, u32 tick_now);
int 	blt_llms_procScanData(u8 *raw_pkt);

#endif /* LLMS_SCAN_H_ */
