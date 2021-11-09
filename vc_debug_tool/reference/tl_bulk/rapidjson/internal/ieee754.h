/********************************************************************************************************
 * @file     ieee754.h 
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

#ifndef RAPIDJSON_IEEE754_
#define RAPIDJSON_IEEE754_

#include "../rapidjson.h"

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

class Double {
public:
    Double() {}
    Double(double d) : d_(d) {}
    Double(uint64_t u) : u_(u) {}

    double Value() const { return d_; }
    uint64_t Uint64Value() const { return u_; }

    double NextPositiveDouble() const {
        RAPIDJSON_ASSERT(!Sign());
        return Double(u_ + 1).Value();
    }

    bool Sign() const { return (u_ & kSignMask) != 0; }
    uint64_t Significand() const { return u_ & kSignificandMask; }
    int Exponent() const { return static_cast<int>(((u_ & kExponentMask) >> kSignificandSize) - kExponentBias); }

    bool IsNan() const { return (u_ & kExponentMask) == kExponentMask && Significand() != 0; }
    bool IsInf() const { return (u_ & kExponentMask) == kExponentMask && Significand() == 0; }
    bool IsNanOrInf() const { return (u_ & kExponentMask) == kExponentMask; }
    bool IsNormal() const { return (u_ & kExponentMask) != 0 || Significand() == 0; }
    bool IsZero() const { return (u_ & (kExponentMask | kSignificandMask)) == 0; }

    uint64_t IntegerSignificand() const { return IsNormal() ? Significand() | kHiddenBit : Significand(); }
    int IntegerExponent() const { return (IsNormal() ? Exponent() : kDenormalExponent) - kSignificandSize; }
    uint64_t ToBias() const { return (u_ & kSignMask) ? ~u_ + 1 : u_ | kSignMask; }

    static int EffectiveSignificandSize(int order) {
        if (order >= -1021)
            return 53;
        else if (order <= -1074)
            return 0;
        else
            return order + 1074;
    }

private:
    static const int kSignificandSize = 52;
    static const int kExponentBias = 0x3FF;
    static const int kDenormalExponent = 1 - kExponentBias;
    static const uint64_t kSignMask = RAPIDJSON_UINT64_C2(0x80000000, 0x00000000);
    static const uint64_t kExponentMask = RAPIDJSON_UINT64_C2(0x7FF00000, 0x00000000);
    static const uint64_t kSignificandMask = RAPIDJSON_UINT64_C2(0x000FFFFF, 0xFFFFFFFF);
    static const uint64_t kHiddenBit = RAPIDJSON_UINT64_C2(0x00100000, 0x00000000);

    union {
        double d_;
        uint64_t u_;
    };
};

} // namespace internal
RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_IEEE754_
