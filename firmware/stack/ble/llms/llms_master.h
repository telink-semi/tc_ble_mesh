/********************************************************************************************************
 * @file     llms_master.h 
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

#ifndef LLMS_MASTER_H_
#define LLMS_MASTER_H_



/***************************************************************************
 *   17		  19	  23	 29		  31	 37		 39       41
 * 21.25	23.75	28.75	36.25	38.75	46.25	48.75    51.25
 *					  X				  				 X
 *					  		 V				 V				  V
 *
 *					 23		 29		 31		 37			  	  41
 **************************************************************************/
#define 		MASTER_CONN_INTER_23							23
#define 		MASTER_CONN_INTER_29							29
#define 		MASTER_CONN_INTER_31							31
#define 		MASTER_CONN_INTER_37							37
#define 		MASTER_CONN_INTER_41							41







typedef struct {
	u8	conn_policy;
	u8	conn_advType;
	u8 	conn_num;
	u8 	cur_connInterval;


	u8	conn_mac[6];

} st_llm_conn_t;

extern	st_llm_conn_t bltcm;


/************************************ User Interface  ******************************************************/




/*********************************** Stack Interface, user can not use!!! **********************************/
int 	blms_m_connect (rf_packet_connect_t * pInit);
int 	blms_btx_start (void);
int 	blms_btx_post (void);



#endif /* LLMS_MASTER_H_ */
