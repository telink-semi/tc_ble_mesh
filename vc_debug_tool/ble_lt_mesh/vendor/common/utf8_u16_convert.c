/********************************************************************************************************
 * @file     utf8_u16_convert.c 
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
#include "proj/tl_common.h"
#include "mesh_common.h"


u32 U16ToUTF8(u16 UniChar, u8 *OutUTFString, int OutUTFLenMax)
{
    u32 UTF8CharLength = 0;

    if(UniChar < 0x80){
        if(OutUTFLenMax >= 1){
            OutUTFString[UTF8CharLength++] = (char)UniChar;
        }else{
            UTF8CharLength = 1;
        }
    }else if(UniChar < 0x800){
        if(OutUTFLenMax >= 2){
            OutUTFString[UTF8CharLength++] = 0xc0 | ( UniChar >> 6 );
            OutUTFString[UTF8CharLength++] = 0x80 | ( UniChar & 0x3f );
        }else{
            UTF8CharLength = 2;
        }
    }else if(UniChar < 0x10000){ // will be optimized to "else", because UniChar is u16.
        if(OutUTFLenMax >= 3){
            OutUTFString[UTF8CharLength++] = 0xe0 | ( UniChar >> 12 );
            OutUTFString[UTF8CharLength++] = 0x80 | ( (UniChar >> 6) & 0x3f );
            OutUTFString[UTF8CharLength++] = 0x80 | ( UniChar & 0x3f );
        }else{
            UTF8CharLength = 3;
        }
    }

    return UTF8CharLength;
}


/**
 * @brief  
 * @param  : .
 * @return val : count of u16.
 */
u32 UTF8StrToU16( const u8 * UTF8String, int UTF8StringLength, u8* OutUnicodeString, int UnicodeStringBufferSize, u16 *OutUTF8LenCost )
{
    int UTF8Index = 0;
    int UniIndex = 0;
    int size_u16 = sizeof(u16_struct);

    while ( UTF8Index < UTF8StringLength ){
        unsigned char UTF8Char = UTF8String[UTF8Index];
        u16_struct *p_u16out = (u16_struct *)(OutUnicodeString + UniIndex);

        if ( UniIndex + size_u16 > UnicodeStringBufferSize ){
            break;
        }
        
        int cUTF8CharRequire = 1;
        if ((UTF8Char & 0x80) == 0){
            cUTF8CharRequire = 1;
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                break;

            if ( OutUnicodeString ){
                p_u16out->val = UTF8Char;
            }
        }else if((UTF8Char & 0xE0) == 0xC0){  ///< 110x-xxxx 10xx-xxxx
            cUTF8CharRequire = 2;
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength ){
                break;
            }

            if ( OutUnicodeString )
            {
                u16 WideChar  = (UTF8String[UTF8Index + 0] & 0x3F) << 6;
                WideChar |= (UTF8String[UTF8Index + 1] & 0x3F);
                p_u16out->val = WideChar;
            }
        }else if((UTF8Char & 0xF0) == 0xE0){  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
            cUTF8CharRequire = 3;
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength ){
                break;
            }

            if ( OutUnicodeString ){
                u16 WideChar  = (UTF8String[UTF8Index + 0] & 0x1F) << 12;
                WideChar |= (UTF8String[UTF8Index + 1] & 0x3F) << 6;
                WideChar |= (UTF8String[UTF8Index + 2] & 0x3F);
                p_u16out->val = WideChar;
            }
        }else{
            break ; // invalid
        }
        
        UTF8Index += cUTF8CharRequire;
        UniIndex += size_u16;
    }

    *OutUTF8LenCost = (u16)UTF8Index;
    return UniIndex / size_u16;
}                                                                                                                                                                                        

/**
 * @brief  
 * @param  : .
 * @return OutUTF8LenCost : .
 */
u16 UTF8StrGetOneU16( const u8 * UTF8String, int UTF8StringLength, u16 *OutU16 )
{
    u16 OutUTF8LenCost = 0;
    UTF8StrToU16(UTF8String, UTF8StringLength, (u8 *)OutU16, 2, &OutUTF8LenCost);
    return OutUTF8LenCost;
}

#if 0
u16 g_1_unicode[] = {0x00,0x79,0x80,0x7ff,0x800,0xffff,0x100};
u8 g_2_utf8[100];
u32 g_2_utf8_cnt = 0;
u8 g_3_unicode[sizeof(g_1_unicode)+100];
u32 g_3_unicode_cnt;

void utf8_test()
{
    int offset = 1; // not align test
    foreach_arr(i,g_1_unicode){
        u32 cnt = U16ToUTF8(g_1_unicode[i], g_2_utf8+offset+g_2_utf8_cnt, sizeof(g_2_utf8)-g_2_utf8_cnt-offset);
        g_2_utf8_cnt += cnt;
    }
    
    u16 UTF8LenCost = 0;
    g_3_unicode_cnt = UTF8StrToU16(g_2_utf8+offset, g_2_utf8_cnt, g_3_unicode+offset, sizeof(g_3_unicode)-offset, &UTF8LenCost);
}
#endif
