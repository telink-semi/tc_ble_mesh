/********************************************************************************************************
 * @file	printf.h
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
#pragma once
#if(!__PROJECT_8263_BLE_MODULE__)
#ifdef WIN32
#include <stdio.h>
#else
#include <stdarg.h>

int my_printf(const char *fmt, ...);
int my_sprintf(char* s, const char *fmt, ...);

int my_printf_uart(const char *format,va_list args);

int my_printf_uart_hexdump(unsigned char *p_buf,int len );
int print(char **out, const char *format, va_list args) ;
int printf_Bin2Text (char *lpD, int lpD_len_max, char *lpS, int n);
u32 get_len_Bin2Text(u32 buf_len);


#define printf	my_printf
#define sprintf	my_sprintf

#define PP_GET_PRINT_BUF_LEN_FALG		((char **)1)	// a valid pointer should never be 1. // length by getting is 1 more than actually print because the end of "\0".
//extern u8 get_print_buf_len_flag;

#endif
#endif

