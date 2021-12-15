/********************************************************************************************************
 * @file     memorybuffer.h 
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

#ifndef RAPIDJSON_MEMORYBUFFER_H_
#define RAPIDJSON_MEMORYBUFFER_H_

#include "stream.h"
#include "internal/stack.h"

RAPIDJSON_NAMESPACE_BEGIN

//! Represents an in-memory output byte stream.
/*!
    This class is mainly for being wrapped by EncodedOutputStream or AutoUTFOutputStream.

    It is similar to FileWriteBuffer but the destination is an in-memory buffer instead of a file.

    Differences between MemoryBuffer and StringBuffer:
    1. StringBuffer has Encoding but MemoryBuffer is only a byte buffer. 
    2. StringBuffer::GetString() returns a null-terminated string. MemoryBuffer::GetBuffer() returns a buffer without terminator.

    \tparam Allocator type for allocating memory buffer.
    \note implements Stream concept
*/
template <typename Allocator = CrtAllocator>
struct GenericMemoryBuffer {
    typedef char Ch; // byte

    GenericMemoryBuffer(Allocator* allocator = 0, size_t capacity = kDefaultCapacity) : stack_(allocator, capacity) {}

    void Put(Ch c) { *stack_.template Push<Ch>() = c; }
    void Flush() {}

    void Clear() { stack_.Clear(); }
    void ShrinkToFit() { stack_.ShrinkToFit(); }
    Ch* Push(size_t count) { return stack_.template Push<Ch>(count); }
    void Pop(size_t count) { stack_.template Pop<Ch>(count); }

    const Ch* GetBuffer() const {
        return stack_.template Bottom<Ch>();
    }

    size_t GetSize() const { return stack_.GetSize(); }

    static const size_t kDefaultCapacity = 256;
    mutable internal::Stack<Allocator> stack_;
};

typedef GenericMemoryBuffer<> MemoryBuffer;

//! Implement specialized version of PutN() with memset() for better performance.
template<>
inline void PutN(MemoryBuffer& memoryBuffer, char c, size_t n) {
    std::memset(memoryBuffer.stack_.Push<char>(n), c, n * sizeof(c));
}

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_MEMORYBUFFER_H_
