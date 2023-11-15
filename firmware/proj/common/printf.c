/********************************************************************************************************
 * @file	printf.c
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
/*
 putchar is the only external dependency for this file,
 if you have a working putchar, leave it commented out.
 If not, uncomment the define below and
 replace outbyte(c) by your own function call.

 #define putchar(c) outbyte(c)
 */
#if(!__PROJECT_8263_BLE_MODULE__)
#ifndef WIN32

#include <stdarg.h>

#include "types.h"
#include "proj_lib/ble/hci/hci.h"
#include "../mcu/putchar.h"
#include "vendor/common/myprintf.h"
#include "vendor/common/user_config.h"


#define MAX_PRINT_STRING_CNT 60

_PRINT_FUN_RAMCODE_ static void printchar(char **str, int c) {
	if (str) {
		if(PP_GET_PRINT_BUF_LEN_FLAG != str){
			**str = c;
			++(*str);
		}
	} else {
		#if (LLSYNC_ENABLE && LLSYNC_LOG_EN && HCI_LOG_FW_EN) // for printing log of llsync SDK
		uart_simu_send_bytes((u8 *)&c, 1);
		#else
		//(void) putchar(c);	// NULL // modify by qifa to decrease code.
		#endif
	}
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

_PRINT_FUN_RAMCODE_ static int prints(char **out, const char *string, int width, int pad) {
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr)
			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for (; width > 0; --width) {
			printchar(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		printchar(out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		printchar(out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

_PRINT_FUN_RAMCODE_ static int printi(char **out, int i, int b, int sg, int width, int pad,
		int letbase) {
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	while (u) {
		t = u % b;
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if (width && (pad & PAD_ZERO)) {
			printchar(out, '-');
			++pc;
			--width;
		} else {
			*--s = '-';
		}
	}

	return pc + prints(out, s, width, pad);
}

#define FLOAT_PRINT_EN		0

#if FLOAT_PRINT_EN
static inline void set_buff_with_check(char s[], int index, int len_max, char value)
{
	if(index < len_max){
		s[index] = value;
	}
}

#if 1
void itoa_one_char(int n, char s[], int radix)
{
	if(n < 0){
		n = -n;	// use abs value.
	}

	s[0] = n % radix + '0';
}
#else
void itoa (int n, char s[], int radix)
{
	int i,j,sign;
	sign = n;
	if(sign < 0){
		n = -n;	// use abs value.
	}

	i = 0;
	do{
		s[i++] = n%radix + '0';
		n /= radix;
	}while(n > 0);
	
	if(sign<0)
		s[i++]='-';
	
	swapX( , , i);	// swap to big endianness
}
#endif

//
// Tim Hirzel
// tim@growdown.com
// March 2008
// float to string
// @param  places: Number of decimal points
// @retval len of outsrt
// If you don't save this as a .h, you will want to remove the default arguments
//     uncomment this first line, and swap it for the next.  I don't think keyword arguments compile in .pde files
//char * floatToString(char * outstr, float value, int places, int minwidth=0, bool rightjustify=false) {
int floatToString_ll(char * outstr, int len_max, float value, int places, int minwidth, bool rightjustify) 
{
    // this is used to write a float value to string, outstr.  oustr is also the return value.
    int digit;
    float tens = (float)0.1;
    int tenscount = 0;
    int i;
    float tempfloat = value;
    int c = 0;
    int charcount = 1;
    int extra = 0;
    // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
    // if this rounding step isn't here, the value  54.321 prints as 54.3209

    // calculate rounding term d:   0.5/pow(10,places)
    float d = 0.5;
    if (value < 0)
        d *= -1.0;
    // divide by ten for each decimal place
    for (i = 0; i < places; i++)
        d/= 10.0;
    // this small addition, combined with truncation will round our values properly
    tempfloat +=  d;

    // first get value tens to be the large power of ten less than value
    if (value < 0)
        tempfloat *= -1.0;
    while ((tens * 10.0) <= tempfloat) {
        tens *= 10.0;
        tenscount += 1;
    }

    if (tenscount > 0)
        charcount += tenscount;
    else
        charcount += 1;

    if (value < 0)
        charcount += 1;
    charcount += 1 + places;

    minwidth += 1; // both count the null final character
    if (minwidth > charcount){
        extra = minwidth - charcount;
        charcount = minwidth;
    }

    if (extra > 0 && rightjustify) {
        for (int i = 0; i< extra; i++) {
        	set_buff_with_check(outstr, c++, len_max, ' ');
        }
    }

    // write out the negative if needed
    if (value < 0){
    	set_buff_with_check(outstr, c++, len_max, '-');
    }

    if (tenscount == 0){
		set_buff_with_check(outstr, c++, len_max, '0');
    }

    for (i=0; i< tenscount; i++) {
        digit = (int) (tempfloat/tens);
        if(c < len_max){
        	itoa_one_char(digit, &outstr[c++], 10);
        }else{
        	return c;
        }
        tempfloat = tempfloat - ((float)digit * tens);
        tens /= 10.0;
    }

    // if no places after decimal, stop now and return

    // otherwise, write the point and continue on
    if (places > 0){
    	set_buff_with_check(outstr, c++, len_max, '.');
    }


    // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
    for (i = 0; i < places; i++) {
        tempfloat *= 10.0;
        digit = (int) tempfloat;
        if(c < len_max){
        	itoa_one_char(digit, &outstr[c++], 10);
        }else{
        	return c;
        }
        // once written, subtract off that digit
        tempfloat = tempfloat - (float) digit;
    }
    if (extra > 0 && !rightjustify) {
        for (int i = 0; i< extra; i++) {
			set_buff_with_check(outstr, c++, len_max, ' ');
        }
    }

	set_buff_with_check(outstr, c++, len_max, '\0');
    
    return c;
}

// @param  places: Number of decimal points
// @retval len of outsrt
int floatToString(char * outstr, int len_max, float value) 
{
	#define DEFAULT_VALID_NUMBER	(0)
	int places = DEFAULT_VALID_NUMBER;
	float value_temp = value;
	if(value_temp < 0){
		value_temp = -value_temp;
	}
	
	for(unsigned int i = 0; i < (45 - DEFAULT_VALID_NUMBER); ++i){	// max 45 valid number
		if(value_temp > 1000000){ // only keep 7 valid number is enough
			break;
		}
		value_temp *= 10;
		places++;
	}
	
	int count = floatToString_ll(outstr, len_max, value, places, 0, 0);	// 6.5 valid number.
	if(count >= len_max){
		if(len_max > 0){
			outstr[len_max - 1] = '\0';
			if(len_max > 1){
				outstr[len_max - 2] = 'E';	// error flag
			}
		}
	}
	return count;
}

#if 0
void float_test()
{
	char buff[1024] = {0};

	foreach(i, 2){
		float init_test = (float)9.876543210987654321;
		if(1 == i){
			init_test = -init_test;
		}
		
		float value_test = init_test;
		foreach(i, 38){
			floatToString(buff, sizeof(buff), value_test);
			LOG_MSG_ERR(TL_LOG_MESH, 0, 0 ,"float print test: %s",buff);
			value_test *= 10;
		}
		
		value_test = init_test;
		foreach(i, 45){
			floatToString(buff, sizeof(buff), value_test);
			LOG_MSG_ERR(TL_LOG_MESH, 0, 0 ,"float print test: %s",buff);
			value_test /= 10;
		}
	}
}
#endif
#endif


_PRINT_FUN_RAMCODE_ int print(char **out, const char *format, va_list args) {
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if(pc> 512-2){
			return pc;
		}
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if (*format == 's') {
				register char *s = (char *) va_arg( args, int );
				pc += prints(out, s ? s : "(null)", width, pad);
				continue;
			}
			if (*format == 'd') {
				pc += printi(out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			
			#if FLOAT_PRINT_EN
			if (*format == 'f') {
				char str[48];// = {0}; // max 45 valid number
				floatToString(str, sizeof(str), va_arg( args, double ));
				str[sizeof(str) - 1] = 0;	// make sure end of string
				pc += prints(out, str, 0, 0); // max length of string is sizeof(str).
				continue;
			}
			#endif
			
			if (*format == 'x') {
				pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'X') {
				pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if (*format == 'u') {
				pc += printi(out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char) va_arg( args, int );
				scr[1] = '\0';
				pc += prints(out, scr, width, pad);
				continue;
			}
		} else {
			out: printchar(out, *format);
			++pc;
		}
	}
	if (out){
		if((PP_GET_PRINT_BUF_LEN_FLAG == out)){
			++pc;
		}else{
			**out = '\0';
		}
	}
	va_end( args );
	return pc;
	
}
const char printf_arrb2t[] = "0123456789abcdef";

_PRINT_FUN_RAMCODE_ u32 get_len_Bin2Text(u32 buf_len)
{
    return (buf_len*3 + (((buf_len+15)/16)*(1+2))); // 1: space, 2: /r/n
}

_PRINT_FUN_RAMCODE_ int printf_Bin2Text (char *lpD, int lpD_len_max, char *lpS, int n)
{
    int i = 0;
	int m = n;
	int d = 0;

    #define LINE_MAX_LOG        (5)
	if(m > BIT(LINE_MAX_LOG)){
	    if(lpD_len_max > 2){
    		lpD[d++] = '\r';
    		lpD[d++] = '\n';
    		lpD_len_max -= 2;
		}
	}
	
	for (i=0; i<m; i++) {
	    if((d + 6 + 2) > lpD_len_max){
	        break;
	    }
	    
        if((0 == (i & BIT_MASK_LEN(LINE_MAX_LOG))) && i){
            lpD[d++] = '\r';
            lpD[d++] = '\n';
        }

		lpD[d++] = printf_arrb2t [(lpS[i]>>4) & 15];
		lpD[d++] = printf_arrb2t [lpS[i] & 15];

		if((i & BIT_MASK_LEN(LINE_MAX_LOG)) != BIT_MASK_LEN(LINE_MAX_LOG)){	// end of line
    		lpD[d++] = ' ';

            if(m > BIT(LINE_MAX_LOG)){
    		    if ((i&7)==7){
    		        lpD[d++] = ' ';
    		    }
    		}
		}
	}

	if(lpD_len_max >= d+2){
    	lpD[d++] = '\r';    // lpS is always true here. so can't distinguish whether there is buffer or not.
    	lpD[d++] = '\n';
        // lpD[d++] = '\0';        // can't add 0, because some UART Tool will show error.
    }
    
	return d;
}

extern int my_fifo_push_hci_tx_fifo (unsigned char *p, unsigned short n, unsigned char *head, unsigned char head_len);
#if HCI_LOG_FW_EN
char hex_dump_buf[MAX_PRINT_STRING_CNT];
unsigned char printf_uart[512];
#endif
extern unsigned char  mi_ota_is_busy();

int my_printf_uart_hexdump(unsigned char *p_buf,int len )
{
    #if HCI_LOG_FW_EN
        #if MI_API_ENABLE
	if(mi_ota_is_busy()){
		return 1;
	}
	    #endif
	int dump_len ;
	dump_len = printf_Bin2Text(hex_dump_buf, sizeof(hex_dump_buf), (char*)p_buf,len);
    uart_simu_send_bytes((unsigned char *)hex_dump_buf,dump_len);
    #endif
	return 1;
	#if 0
	u8 head[2] = {HCI_LOG};
	head[1]= dump_len+2;
	return	my_fifo_push_hci_tx_fifo((unsigned char *)hex_dump_buf,dump_len,head,2);
	#endif
}

int my_printf_uart(const char *format,va_list args)
{
    #if HCI_LOG_FW_EN
        #if MI_API_ENABLE
	if(mi_ota_is_busy()){
		return 1;
	}
	    #endif
	unsigned char **pp_buf;
	unsigned char *p_buf;
	unsigned int len =0;
	p_buf = printf_uart;
	pp_buf = &(p_buf);
	//va_start( args, format );
	len = print((char **)pp_buf, format, args);
	uart_simu_send_bytes(printf_uart,len);
	#endif
	return 1;
	
}

int my_printf(const char *format, ...) {
	va_list args;
	va_start( args, format );
	return print(0, format, args);
}

int my_sprintf(char *out, const char *format, ...) {
	va_list args;
	va_start( args, format );
	return print(&out, format, args);
}

#endif
#endif
