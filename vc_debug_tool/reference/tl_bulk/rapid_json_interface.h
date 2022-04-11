/********************************************************************************************************
 * @file	rapid_json_interface.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2017
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