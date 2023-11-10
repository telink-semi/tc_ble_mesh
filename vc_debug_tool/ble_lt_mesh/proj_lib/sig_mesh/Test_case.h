/********************************************************************************************************
 * @file	Test_case.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
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
#include "tl_common.h"
#include "vendor/common/mesh_node.h"

#if (TESTCASE_FLAG_ENABLE)
#include "Test_case_lib.h"
#endif


enum {
	GW_INI_PTS_TC_CMD_BLOB_TRANS_FLOW = 1,		// TC: TEST CASE
};

#ifndef SET_TC_FIFO
#define SET_TC_FIFO(cmd, pfifo, cmd_len)	
#endif
#ifndef SET_RESULT_TESTCASE
#define SET_RESULT_TESTCASE(reason, para)	
#endif

