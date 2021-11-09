/********************************************************************************************************
 * @file     rapid_json_interface.h 
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
#ifndef __RAPID_JSON_INTERFACE_H
#define __RAPID_JSON_INTERFACE_H

#include "rapidjson/document.h"

using namespace RAPIDJSON_NAMESPACE;

void json_write();
void json_read() ;
int  init_json(char *p_json,unsigned char mode);
void read_json_file_init();
void read_json_file_doc();
void write_json_file_doc(char *p_file);
int json_db_set_mesh_data(Document& doc);
void write_json_file_doc_static(char *p_file);
int json_db_set_mesh_data_static(Document& doc);
#define FILE_MESH_DATA_BASE 	"mesh_database.json"	
#define MODEL_SIG_FLAG  1
#define MODEL_VENDOR_FLAG 2
int json_file_exist(Document& doc, char *p_json);
int json_db_get_mesh_data_check(char *p_file);
#endif 