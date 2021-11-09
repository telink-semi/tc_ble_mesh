/********************************************************************************************************
 * @file     en.h 
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

#ifndef RAPIDJSON_ERROR_EN_H_
#define RAPIDJSON_ERROR_EN_H_

#include "error.h"

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(switch-enum)
RAPIDJSON_DIAG_OFF(covered-switch-default)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Maps error code of parsing into error message.
/*!
    \ingroup RAPIDJSON_ERRORS
    \param parseErrorCode Error code obtained in parsing.
    \return the error message.
    \note User can make a copy of this function for localization.
        Using switch-case is safer for future modification of error codes.
*/
inline const RAPIDJSON_ERROR_CHARTYPE* GetParseError_En(ParseErrorCode parseErrorCode) {
    switch (parseErrorCode) {
        case kParseErrorNone:                           return RAPIDJSON_ERROR_STRING("No error.");

        case kParseErrorDocumentEmpty:                  return RAPIDJSON_ERROR_STRING("The document is empty.");
        case kParseErrorDocumentRootNotSingular:        return RAPIDJSON_ERROR_STRING("The document root must not be followed by other values.");
    
        case kParseErrorValueInvalid:                   return RAPIDJSON_ERROR_STRING("Invalid value.");
    
        case kParseErrorObjectMissName:                 return RAPIDJSON_ERROR_STRING("Missing a name for object member.");
        case kParseErrorObjectMissColon:                return RAPIDJSON_ERROR_STRING("Missing a colon after a name of object member.");
        case kParseErrorObjectMissCommaOrCurlyBracket:  return RAPIDJSON_ERROR_STRING("Missing a comma or '}' after an object member.");
    
        case kParseErrorArrayMissCommaOrSquareBracket:  return RAPIDJSON_ERROR_STRING("Missing a comma or ']' after an array element.");

        case kParseErrorStringUnicodeEscapeInvalidHex:  return RAPIDJSON_ERROR_STRING("Incorrect hex digit after \\u escape in string.");
        case kParseErrorStringUnicodeSurrogateInvalid:  return RAPIDJSON_ERROR_STRING("The surrogate pair in string is invalid.");
        case kParseErrorStringEscapeInvalid:            return RAPIDJSON_ERROR_STRING("Invalid escape character in string.");
        case kParseErrorStringMissQuotationMark:        return RAPIDJSON_ERROR_STRING("Missing a closing quotation mark in string.");
        case kParseErrorStringInvalidEncoding:          return RAPIDJSON_ERROR_STRING("Invalid encoding in string.");

        case kParseErrorNumberTooBig:                   return RAPIDJSON_ERROR_STRING("Number too big to be stored in double.");
        case kParseErrorNumberMissFraction:             return RAPIDJSON_ERROR_STRING("Miss fraction part in number.");
        case kParseErrorNumberMissExponent:             return RAPIDJSON_ERROR_STRING("Miss exponent in number.");

        case kParseErrorTermination:                    return RAPIDJSON_ERROR_STRING("Terminate parsing due to Handler error.");
        case kParseErrorUnspecificSyntaxError:          return RAPIDJSON_ERROR_STRING("Unspecific syntax error.");

        default:                                        return RAPIDJSON_ERROR_STRING("Unknown error.");
    }
}

RAPIDJSON_NAMESPACE_END

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_ERROR_EN_H_
