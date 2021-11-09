/********************************************************************************************************
 * @file     stream.h 
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

#include "rapidjson.h"

#ifndef RAPIDJSON_STREAM_H_
#define RAPIDJSON_STREAM_H_

#include "encodings.h"

RAPIDJSON_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  Stream

/*! \class rapidjson::Stream
    \brief Concept for reading and writing characters.

    For read-only stream, no need to implement PutBegin(), Put(), Flush() and PutEnd().

    For write-only stream, only need to implement Put() and Flush().

\code
concept Stream {
    typename Ch;    //!< Character type of the stream.

    //! Read the current character from stream without moving the read cursor.
    Ch Peek() const;

    //! Read the current character from stream and moving the read cursor to next character.
    Ch Take();

    //! Get the current read cursor.
    //! \return Number of characters read from start.
    size_t Tell();

    //! Begin writing operation at the current read pointer.
    //! \return The begin writer pointer.
    Ch* PutBegin();

    //! Write a character.
    void Put(Ch c);

    //! Flush the buffer.
    void Flush();

    //! End the writing operation.
    //! \param begin The begin write pointer returned by PutBegin().
    //! \return Number of characters written.
    size_t PutEnd(Ch* begin);
}
\endcode
*/

//! Provides additional information for stream.
/*!
    By using traits pattern, this type provides a default configuration for stream.
    For custom stream, this type can be specialized for other configuration.
    See TEST(Reader, CustomStringStream) in readertest.cpp for example.
*/
template<typename Stream>
struct StreamTraits {
    //! Whether to make local copy of stream for optimization during parsing.
    /*!
        By default, for safety, streams do not use local copy optimization.
        Stream that can be copied fast should specialize this, like StreamTraits<StringStream>.
    */
    enum { copyOptimization = 0 };
};

//! Reserve n characters for writing to a stream.
template<typename Stream>
inline void PutReserve(Stream& stream, size_t count) {
    (void)stream;
    (void)count;
}

//! Write character to a stream, presuming buffer is reserved.
template<typename Stream>
inline void PutUnsafe(Stream& stream, typename Stream::Ch c) {
    stream.Put(c);
}

//! Put N copies of a character to a stream.
template<typename Stream, typename Ch>
inline void PutN(Stream& stream, Ch c, size_t n) {
    PutReserve(stream, n);
    for (size_t i = 0; i < n; i++)
        PutUnsafe(stream, c);
}

///////////////////////////////////////////////////////////////////////////////
// StringStream

//! Read-only string stream.
/*! \note implements Stream concept
*/
template <typename Encoding>
struct GenericStringStream {
    typedef typename Encoding::Ch Ch;

    GenericStringStream(const Ch *src) : src_(src), head_(src) {}

    Ch Peek() const { return *src_; }
    Ch Take() { return *src_++; }
    size_t Tell() const { return static_cast<size_t>(src_ - head_); }

    Ch* PutBegin() { RAPIDJSON_ASSERT(false); return 0; }
    void Put(Ch) { RAPIDJSON_ASSERT(false); }
    void Flush() { RAPIDJSON_ASSERT(false); }
    size_t PutEnd(Ch*) { RAPIDJSON_ASSERT(false); return 0; }

    const Ch* src_;     //!< Current read position.
    const Ch* head_;    //!< Original head of the string.
};

template <typename Encoding>
struct StreamTraits<GenericStringStream<Encoding> > {
    enum { copyOptimization = 1 };
};

//! String stream with UTF8 encoding.
typedef GenericStringStream<UTF8<> > StringStream;

///////////////////////////////////////////////////////////////////////////////
// InsituStringStream

//! A read-write string stream.
/*! This string stream is particularly designed for in-situ parsing.
    \note implements Stream concept
*/
template <typename Encoding>
struct GenericInsituStringStream {
    typedef typename Encoding::Ch Ch;

    GenericInsituStringStream(Ch *src) : src_(src), dst_(0), head_(src) {}

    // Read
    Ch Peek() { return *src_; }
    Ch Take() { return *src_++; }
    size_t Tell() { return static_cast<size_t>(src_ - head_); }

    // Write
    void Put(Ch c) { RAPIDJSON_ASSERT(dst_ != 0); *dst_++ = c; }

    Ch* PutBegin() { return dst_ = src_; }
    size_t PutEnd(Ch* begin) { return static_cast<size_t>(dst_ - begin); }
    void Flush() {}

    Ch* Push(size_t count) { Ch* begin = dst_; dst_ += count; return begin; }
    void Pop(size_t count) { dst_ -= count; }

    Ch* src_;
    Ch* dst_;
    Ch* head_;
};

template <typename Encoding>
struct StreamTraits<GenericInsituStringStream<Encoding> > {
    enum { copyOptimization = 1 };
};

//! Insitu string stream with UTF8 encoding.
typedef GenericInsituStringStream<UTF8<> > InsituStringStream;

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_STREAM_H_
