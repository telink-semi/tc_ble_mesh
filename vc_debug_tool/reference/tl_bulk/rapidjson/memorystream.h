/********************************************************************************************************
 * @file     memorystream.h 
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

#ifndef RAPIDJSON_MEMORYSTREAM_H_
#define RAPIDJSON_MEMORYSTREAM_H_

#include "stream.h"

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(unreachable-code)
RAPIDJSON_DIAG_OFF(missing-noreturn)
#endif

RAPIDJSON_NAMESPACE_BEGIN

//! Represents an in-memory input byte stream.
/*!
    This class is mainly for being wrapped by EncodedInputStream or AutoUTFInputStream.

    It is similar to FileReadBuffer but the source is an in-memory buffer instead of a file.

    Differences between MemoryStream and StringStream:
    1. StringStream has encoding but MemoryStream is a byte stream.
    2. MemoryStream needs size of the source buffer and the buffer don't need to be null terminated. StringStream assume null-terminated string as source.
    3. MemoryStream supports Peek4() for encoding detection. StringStream is specified with an encoding so it should not have Peek4().
    \note implements Stream concept
*/
struct MemoryStream {
    typedef char Ch; // byte

    MemoryStream(const Ch *src, size_t size) : src_(src), begin_(src), end_(src + size), size_(size) {}

    Ch Peek() const { return RAPIDJSON_UNLIKELY(src_ == end_) ? '\0' : *src_; }
    Ch Take() { return RAPIDJSON_UNLIKELY(src_ == end_) ? '\0' : *src_++; }
    size_t Tell() const { return static_cast<size_t>(src_ - begin_); }

    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

    // For encoding detection only.
    const Ch* Peek4() const {
        return Tell() + 4 <= size_ ? src_ : 0;
    }

    const Ch* src_;     //!< Current read position.
    const Ch* begin_;   //!< Original head of the string.
    const Ch* end_;     //!< End of stream.
    size_t size_;       //!< Size of the stream.
};

RAPIDJSON_NAMESPACE_END

#ifdef __clang__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_MEMORYBUFFER_H_
