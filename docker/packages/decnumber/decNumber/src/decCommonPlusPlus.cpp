/* ------------------------------------------------------------------ */
/* decCommon.cpp source file                                          */
/* ------------------------------------------------------------------ */
/* Copyright (c) IBM Corporation, 2006.  All rights reserved.         */
/*                                                                    */
/* This software is made available under the terms of the IBM         */
/* alphaWorks License Agreement (distributed with this software as    */
/* alphaWorks-License.txt).  Your use of this software indicates      */
/* your acceptance of the terms and conditions of that Agreement.     */
/*                                                                    */
/* Please send comments, suggestions, and corrections to the author:  */
/*   klarer@ca.ibm.com                                                */
/*   Robert Klarer                                                    */
/* ------------------------------------------------------------------ */

#include <cmath>
#include <limits>
#include <cstring>

#define DECNUMDIGITS 45 // IMPORTANT: must appear first!
extern "C" {
#include "decNumber.h"
#include "decNumberLocal.h"
#include "decimal128.h"
#include "decimal64.h"
#include "decimal32.h"
#include "decPacked.h"
}
#include "decimal"

namespace ibm {
namespace decimal {

static const size_t lsu_extent = DECNUMUNITS;

rounding _ContextBase::_RoundingMode = DEC_ROUND_HALF_UP;
uint32_t _ContextBase::_EnvFlags = 0;

const char   _DecNumber::_InfStr[] = "infinity";
const size_t _DecNumber::_InfStrLen = sizeof(_InfStr);
const char   _DecNumber::_INFStr[] = "INFINITY";
const size_t _DecNumber::_INFStrLen = sizeof(_INFStr);
const char   _DecNumber::_NanStr[] = "nan";
const size_t _DecNumber::_NanStrLen = sizeof(_InfStr);
const char   _DecNumber::_NANStr[] = "NAN";
const size_t _DecNumber::_NANStrLen = sizeof(_INFStr);

// class to aid conversions involving objects of long double type
struct _LongDouble {

        _LongDouble()
                { as_ldbl = +0.0L; } 
        _LongDouble(long double ld)
                { as_ldbl = ld; }
        _LongDouble(const _DecNumber & dec)
                { as_ldbl = decimalToBinary(dec); }

        operator long double() const
                { return as_ldbl; }
        operator _DecNumber() const
                { return binaryToDecimal(as_ldbl); }

        uint16_t _GetBiasedExponent() const;
        int16_t  _GetExponent() const;
        void     _SetExponent(int16_t);
        void     _SetSignFlag(bool b);

        bool _IsSubnormal() const
                { return ((_GetBiasedExponent() == 0) && (as_ldbl != 0)); }
        bool _IsNeg() const
                { return as_ldbl < 0; }

        bool _IsNAN() const
#if defined(_DEC_HAS_ISNAN)
                { return isnan(as_ldbl); }
#else
                {
                if (_GetBiasedExponent() != exp_mask) { return false; }
                _ConstRepIter iter(bytes);

                // If the first bit of the significand is set, we have a qNaN.
                if (iter._GetBit())
                        { return true; }
                iter._NextBit();

                // Otherwise, if any significand bit is set, we have a sNaN.
                for(; !iter._IsDone() && !iter._GetBit(); iter._NextBit()) {}
                return !iter._IsDone();
                }
#endif /* defined(_DEC_HAS_ISNAN) */

        bool _IsINF() const
#if defined(_DEC_HAS_ISINF)
                { return isinf(as_ldbl); }
#else
                {
                if (_GetBiasedExponent() != exp_mask) { return false; }
                _ConstRepIter iter(bytes);

                // If no significand bit is set, we have a +ve or -ve infinity.
                for(; !iter._IsDone() && !iter._GetBit(); iter._NextBit()) {}
                return iter._IsDone();
                }
#endif /* defined(_DEC_HAS_ISINF) */

        static long double decimalToBinary(const _DecNumber &);
        _DecNumber decimalToBinaryAUX(const _DecNumber &);
        static _DecNumber binaryToDecimal(long double);
        _DecNumber binaryToDecimalAUX() const;
 
        static const std::size_t bytes_extent =
                sizeof(long double)/sizeof(uint8_t);

#if defined(_DEC_LONGDOUBLE80) || defined(_DEC_LONGDOUBLE128IEEE)
        static const uint32_t exp_bias  = 16383;
        static const uint32_t exp_mask = 0x7FFF;
#elif defined(_DEC_LONGDOUBLE64) || defined(_DEC_LONGDOUBLE128PPC)
        static const uint32_t exp_bias  = 1023;
        static const uint32_t exp_mask = 0x7FF;
#else
#  error "Invalid long double format setting.  Please update decConfig.h."
#endif /* defined(_DEC_LONGDOUBLExx) */

        union {
                uint8_t bytes[bytes_extent];
                long double as_ldbl; 
        };

        struct _RepIterBase {
                _RepIterBase();
                void _NextBit()
                        {
                        if (mask == 0x01)
                                {
                                mask = 0x80; 
                                _NextIndex();
                                }
                        else
                                { mask >>= 1; }
                        }
                void _PrevBit()
                        {
                        if (mask == 0x80)
                                {
                                mask = 0x01; 
                                _PrevIndex();
                                }
                        else
                                { mask <<= 1; }
                        }
                bool _IsDone() const;
        private:
                void _NextIndex();
                void _PrevIndex();
        protected:
                int    index;
                uint8_t   mask;
        };

        struct _RepIter : _RepIterBase {
                _RepIter(uint8_t * p) : rep(p) {}
                void _SetBit()
                        { rep[index] |= mask; }
        private:
                uint8_t * rep;
        };

        struct _ConstRepIter : _RepIterBase {
                _ConstRepIter(const uint8_t * p) : rep(p) {}
                bool _GetBit() const
                        { return rep[index] & mask; }
        private:
                uint8_t const * rep;
        };
};

// This macro indicates that the binary encoding of a long double
// represents the most significant bit of the significand implicitly,
// since it is always set.
// This macro will be #undefined below if the selected long double
// encoding is not packed.
//
#define _DEC_HAS_IMPLICIT_BIT 1

#if defined(_DEC_BIG_ENDIAN)

#if defined(_DEC_LONGDOUBLE128IEEE)
inline bool _LongDouble::_RepIterBase::_IsDone() const
        { return index == _LongDouble::bytes_extent; }
#else
inline bool _LongDouble::_RepIterBase::_IsDone() const
        { return index == sizeof(double); }
#endif /* defined(_DEC_LONGDOUBLE128IEEE) */

inline void _LongDouble::_RepIterBase::_NextIndex()
        { ++index; }
inline void _LongDouble::_RepIterBase::_PrevIndex()
        { --index; }

#if defined(_DEC_LONGDOUBLE64) || defined(_DEC_LONGDOUBLE128PPC)

inline _LongDouble::_RepIterBase::_RepIterBase() :
        index(1), mask(0x08) {}

inline uint16_t _LongDouble::_GetBiasedExponent() const
        { return ((bytes[0] & 0x7F) << 4) + ((bytes[1] & 0xF0) >> 4); }

inline void _LongDouble::_SetExponent(int16_t _Exp)
        {
        uint16_t exponent = _Exp + exp_bias;
        bytes[1] &= 0x0F;
        bytes[1] |= (exponent & 0x000F) << 4;
        bytes[0] &= 0x80;
        bytes[0] |= (exponent & 0x07FF) >> 4;
        }

inline void _LongDouble::_SetSignFlag(bool b)
        { b ? bytes[0] |= 0x80 : bytes[0] &= 0x7F; }

#elif defined(_DEC_LONGDOUBLE128IEEE)

inline _LongDouble::_RepIterBase::_RepIterBase() :
        index(2), mask(0x80) {}

inline uint16_t _LongDouble::_GetBiasedExponent() const
        { return ((bytes[0] & 0x7F) << 8) + bytes[1]; }

inline void _LongDouble::_SetExponent(int16_t _Exp)
        {
        uint16_t exponent = _Exp + exp_bias;
        bytes[1] = exponent & 0x00FF;
        bytes[0] &= 0x80;
        bytes[0] |= (exponent & 0x7F00) >> 8;
        }

inline void _LongDouble::_SetSignFlag(bool b)
        { b ? bytes[0] |= 0x80 : bytes[0] &= 0x7F; }

#else
#  error "Invalid long double format setting.  Please update decConfig.h."
#endif /* defined(_DEC_LONGDOUBLExx) */

#elif defined(_DEC_LITTLE_ENDIAN)

inline bool _LongDouble::_RepIterBase::_IsDone() const
        { return index < 0; }

inline void _LongDouble::_RepIterBase::_NextIndex()
        { --index; }
inline void _LongDouble::_RepIterBase::_PrevIndex()
        { ++index; }

#  if defined(_DEC_LONGDOUBLE64)

inline _LongDouble::_RepIterBase::_RepIterBase() :
        index(_LongDouble::bytes_extent - 2), mask(0x08) {}

inline uint16_t _LongDouble::_GetBiasedExponent() const
        {
        return ((bytes[bytes_extent - 1] & 0x7F) << 4) +
               ((bytes[bytes_extent - 2] & 0xF0) >> 4);
        }

inline void _LongDouble::_SetExponent(int16_t _Exp)
        {
        uint16_t exponent = _Exp + exp_bias;
        bytes[bytes_extent - 2] &= 0x0F;
        bytes[bytes_extent - 2] |= (exponent & 0x000F) << 4;
        bytes[bytes_extent - 1] &= 0x80;
        bytes[bytes_extent - 1] |= (exponent & 0x07FF) >> 4;
        }

inline void _LongDouble::_SetSignFlag(bool b)
        { b ? bytes[bytes_extent - 1] |= 0x80 : bytes[bytes_extent - 1] &= 0x7F; }

#  elif defined(_DEC_LONGDOUBLE80)

#  undef _DEC_HAS_IMPLICIT_BIT

inline _LongDouble::_RepIterBase::_RepIterBase() :
        index(7), mask(0x80) {}

inline uint16_t _LongDouble::_GetBiasedExponent() const
        {
        return ((bytes[9] & 0x7F) << 8) + bytes[8];
        }

inline void _LongDouble::_SetExponent(int16_t _Exp)
        {
        uint16_t exponent = _Exp + exp_bias;
        bytes[8] = (exponent & 0x00FF);
        bytes[9] &= 0x80;
        bytes[9] |= (exponent & 0x7F00) >> 8;
        }

inline void _LongDouble::_SetSignFlag(bool b)
        { b ? bytes[9] |= 0x80 : bytes[9] &= 0x7F; }

#  else
#    error "Invalid long double format setting.  Please update decConfig.h."
#  endif /* defined(_DEC_LONGDOUBLExx) */

#else
#  error "Invalid byte order format setting.  Please update decConfig.h."
#endif /* defined(_DEC_xxx_ENDIAN) */

int16_t _LongDouble::_GetExponent() const
        { return _GetBiasedExponent() - exp_bias; }

inline long double _LongDouble::decimalToBinary(const _DecNumber & num)
        {
        // Handle special numbers.
        if (num._IsNAN())
	  { return std::numeric_limits<long double>::quiet_NaN(); }
        if (num._IsINF())
                {
		  return std::numeric_limits<long double>::infinity() *
                        (num._IsNeg() ? -1 : 1);
                }
        if (num._IsZero() & num._IsNeg())
                { return -0.0L; }

        _LongDouble ldbl_hi;
        _DecNumber extra_digits = ldbl_hi.decimalToBinaryAUX(num);

#if defined(_DEC_LONGDOUBLE128PPC)
        _LongDouble ldbl_lo;
        if (!extra_digits._IsZero())
                {
                ldbl_lo.decimalToBinaryAUX(extra_digits);
                ldbl_hi.as_ldbl += ldbl_lo.as_ldbl;
                }
#endif /* defined(_DEC_LONGDOUBLE128PPC) */

        return ldbl_hi.as_ldbl;
        }

_DecNumber _LongDouble::decimalToBinaryAUX(const _DecNumber & num)
/*
   returns: if the precision of the argument exceeds what can be represented
            by a long double, the value returned will represent the remaining,
            unconverted decimal digits
*/
        {
        typedef _DecNumber::_Le _Le;

        static const _DecNumber five(5);
        _DecNumber fracPartBase10 = num;
        _DecNumber powerOfHalf(1);
        int32_t fracPartExpBase2 = -1;
        bool    foundImplicitBit = false;
        as_ldbl = +0.0L;

        // Break the number into an integral part and a fractional part. 
        _ContextDefault ctx;
        ctx.digits = DECNUMDIGITS;
        ctx.round  = DEC_ROUND_DOWN;
        _DecNumber intPartBase10;
        decNumberToIntegralValue(&intPartBase10, &fracPartBase10, &ctx);

        // Handle the integer part of the number.
        long double intPartBase2 = +0.0L;
        ctx.round  = DEC_ROUND_HALF_UP;
        if (!intPartBase10._IsZero())
                {
                for (size_t i = DECNUMUNITS; i != 0; --i)
                        {
                        intPartBase2 *= DECDPUNMAX + 1;
                        intPartBase2 += intPartBase10.lsu[i - 1];
                        }
                if (intPartBase10._IsNeg())
                        { intPartBase2 *= -1.0L; }
                decNumberSubtract(&fracPartBase10, &fracPartBase10,
                                  &intPartBase10, &ctx);
                }

        // Handle the sign.
        _SetSignFlag(fracPartBase10._IsNeg());
        fracPartBase10._ClearSign();

        // Handle the fractional part of the number.
        _RepIter iter(bytes);

        while (!iter._IsDone() && !fracPartBase10._IsZero() &&
               (fracPartExpBase2 >= -exp_bias))
                {
                // Divide powerOfHalf by 2.
                // We accomplish this by calculating (powerOfHalf * 5)/10.
                decNumberMultiply(&powerOfHalf, &powerOfHalf, &five, &ctx);
                --powerOfHalf.exponent; // divide by 10

                if (_DecNumber::_Compare<_Le>(powerOfHalf, fracPartBase10))
                        {
                        // This bit is one.
                        decNumberSubtract(&fracPartBase10, &fracPartBase10,
                                          &powerOfHalf, &ctx); 

                        if (!foundImplicitBit)
                                {
                                foundImplicitBit = true;
#if defined(_DEC_HAS_IMPLICIT_BIT)
                                iter._PrevBit();
#else
                                iter._SetBit();
#endif /* defined(_DEC_HAS_IMPLICIT_BIT) */
                                }
                        else
                                { iter._SetBit(); }
                        }

                if (foundImplicitBit)
                        { iter._NextBit(); }
                else
                        { --fracPartExpBase2; }
                }

        if (!fracPartBase10._IsZero())
                { decContextSetStatus(&ctx, DEC_Inexact); }

        if (as_ldbl != 0.0L || foundImplicitBit)
                { _SetExponent(fracPartExpBase2); }

        // Add the integer part of the number to the fractional part.
        as_ldbl += intPartBase2 * _DEC_POWL(10.0L, intPartBase10.exponent);

        return fracPartBase10;
        }

#if defined(_DEC_LONGDOUBLE128PPC)
inline double _Hi(long double source)
        { return (double)source; }

inline double _Lo(long double source)
        {
        const size_t bytes_extent = sizeof(long double);
        union {
                long double as_ldbl;
                uint8_t bytes[bytes_extent];
        } _Ldbl;
        double _Tmp;
        _Ldbl.as_ldbl = source;
        memcpy((void *)&_Tmp,
               &_Ldbl.bytes[bytes_extent / 2],
               bytes_extent / 2);
        return _Tmp;
        }
#else
inline long double _Hi(long double source)
        { return source; }
#endif /* defined(_DEC_LONGDOUBLE128PPC) */

inline _DecNumber _LongDouble::binaryToDecimal(long double source)
        {
        _LongDouble ldbl_hi(_Hi(source));

        // Handle special numbers.
        if (ldbl_hi._IsNAN())
                {
                _DecNumber ret;
                ret.bits = DECNAN;
                return ret;
                }
        if (ldbl_hi._IsINF())
                {
                _DecNumber ret;
                ret.bits = ldbl_hi._IsNeg() ? -DECINF : DECINF;
                return ret;
                }

#if defined(_DEC_LONGDOUBLE128PPC)
        _LongDouble ldbl_lo(_Lo(source));
        _DecNumber  dec_hi(ldbl_hi.binaryToDecimalAUX());
        _DecNumber  dec_lo(ldbl_lo.binaryToDecimalAUX());
        _ContextDefault ctx;
        ctx.digits = DECNUMDIGITS;
        decNumberAdd(&dec_hi, &dec_hi, &dec_lo, &ctx);
        return dec_hi;
#else
        return ldbl_hi.binaryToDecimalAUX();
#endif /* defined(_DEC_LONGDOUBLE128PPC) */
        }

_DecNumber _LongDouble::binaryToDecimalAUX() const
        {
        static const _DecNumber five(5);
        _DecNumber acc(0);
        _DecNumber powerOfHalf(1);
        bool is_exact = false;
        int32_t exp = -1;

        _ContextDefault ctx;
        ctx.digits = DECNUMDIGITS;

        _ConstRepIter iter(bytes);

        // Handle "hidden" bit.
        if (!_IsSubnormal() && (as_ldbl != 0.0L))
                { decNumberAdd(&acc, &acc, &powerOfHalf, &ctx); }
#if !defined(_DEC_HAS_IMPLICIT_BIT)
        iter._NextBit();
#endif /* defined(_DEC_HAS_IMPLICIT_BIT) */

        while (!iter._IsDone())
                {
                // Divide power_of_half by 2.
                // We accomplish this by calculating (powerOfHalf * 5)/10.
                decNumberMultiply(&powerOfHalf, &powerOfHalf, &five, &ctx);
                --powerOfHalf.exponent; // divide by 10

                if (iter._GetBit())
                        { decNumberAdd(&acc, &acc, &powerOfHalf, &ctx); }
                iter._NextBit();
                }

        // do exponent
        static const _DecNumber two(2);
        _DecNumber exp2(_GetExponent());
        _DecNumber scale10; 
        decNumberPower(&scale10, &two, &exp2, &ctx);
        decNumberMultiply(&acc, &acc, &scale10, &ctx);
        decNumberNormalize(&acc, &acc, &ctx);

        // do sign
        if (_IsNeg())
                { acc.bits |= DECNEG; }

        // Check for subnormals.
        if (acc.exponent == ctx.emin)
                { decContextSetStatus(&ctx, DEC_Underflow); }

        return acc;
        }

inline void _DecNumber::_ZeroSignificand()
        {
        for (size_t i = 0; i != lsu_extent; ++i)
                { lsu[i] = 0; }
        }

_DecNumber::_DecNumber()
        {
        // These members cannot be initialized in a ctor-initializer
        // list because they are actually members of the base class.
        digits = 1;
        exponent = 0;
        bits = 0;
        _ZeroSignificand();
        }

_DecNumber::_DecNumber(_DecBase<_FmtTraits<decimal32> > source)
        {
        _ZeroSignificand();
        decimal32ToNumber(reinterpret_cast< ::decimal32 *>(&source), this);
        }

_DecNumber::_DecNumber(_DecBase<_FmtTraits<decimal64> > source)
        {
        _ZeroSignificand();
        decimal64ToNumber(reinterpret_cast< ::decimal64 *>(&source), this);
        }

_DecNumber::_DecNumber(_DecBase<_FmtTraits<decimal128> > source)
        {
        _ZeroSignificand();
        decimal128ToNumber(reinterpret_cast< ::decimal128 *>(&source), this);
        }

_DecNumber::_DecNumber(long double source)
        { *this = _LongDouble::binaryToDecimal(source); }

_DecNumber::_DecNumber(const _DecNumber & source)
        {
        // These members cannot be initialized in a ctor-initializer
        // list because they are actually members of the base class.
        digits = source.digits;
        exponent = source.exponent;
        bits = source.bits;
        for (size_t i = 0; i != lsu_extent; ++i)
                { lsu[i] = source.lsu[i]; }
        }

_DecNumber & _DecNumber::operator=(const _DecNumber & source)
        {
        digits = source.digits;
        exponent = source.exponent;
        bits = source.bits;
        for (size_t i = 0; i != lsu_extent; ++i)
                { lsu[i] = source.lsu[i]; }
        return *this;
        }

long double _DecNumber::_ToLongDouble() const
        {
        _LongDouble ldbl(*this);
        return ldbl.as_ldbl;
        }

void _DecNumber::_FromIntegral(unsigned long long source)
        {
        exponent = 0;

        if (source == 0)
                { digits = 1; }
        else
                { digits = (int32_t)log10(double(source)) + 1; }

        lsu[0] = source % (DECDPUNMAX + 1);
        for (size_t i = 1; i != lsu_extent; ++i)
                {
                source /= (DECDPUNMAX + 1);
                lsu[i] = source % (DECDPUNMAX + 1);
                }
        }

long long _DecNumber::_ToSignedIntegral() const
        {
        static const int32_t maxDigits =
                std::numeric_limits<long long>::digits10 + 1;

        long long target = 0L;

        if (bits & DECINF)
                { target = bits & DECNEG ? LLONG_MIN : LLONG_MAX; }
        else if (bits & DECSPECIAL)
                { target = LLONG_MIN; }
        else
                {
                _DecNumber tmp;
                _ContextDefault ctx;
                ctx.digits = maxDigits + 1;
                ctx.round = DEC_ROUND_DOWN;
                decNumberToIntegralValue(&tmp, this, &ctx);

                if (tmp.exponent > 0)
                        {
                        if (tmp.digits + tmp.exponent <= maxDigits)
                                {
                                _DecNumber zero(0);
                                decNumberQuantize(&tmp, &tmp, &zero, &ctx);
                                }
                        else
                                {
                                decContextSetStatus(&ctx, DEC_Inexact);
                                return _IsNeg() ? LLONG_MIN : LLONG_MAX;
                                }
                        }

                for (size_t i = DECNUMUNITS; i != 0; --i)
                        {
                        target *= DECDPUNMAX + 1;
                        target += tmp.lsu[i - 1];
                        }
                if (_IsNeg())
                        { target = -target; }
                if (_Compare<_Ne>(*this, tmp)) 
                        { decContextSetStatus(&ctx, DEC_Inexact); }
                }
        return target;
        }

void _DecNumber::_ToDecimal32(_DecBase<_FmtTraits<decimal32> > * _Tgt,
                              _Context32 * _Ctxt) const
        {
        decimal32FromNumber(reinterpret_cast< ::decimal32 *>(_Tgt),
                            this, _Ctxt);
        _Ctxt->_CheckForRangeError();
        if (digits > DECIMAL32_Pmax)
                { decContextSetStatus(_Ctxt, DEC_Inexact); }
        }

void _DecNumber::_ToDecimal64(_DecBase<_FmtTraits<decimal64> > * _Tgt,
                              _Context64 * _Ctxt) const
        {
        decimal64FromNumber(reinterpret_cast< ::decimal64 *>(_Tgt),
                            this, _Ctxt);
        _Ctxt->_CheckForRangeError();
        if (digits > DECIMAL64_Pmax)
                { decContextSetStatus(_Ctxt, DEC_Inexact); }
        }

void _DecNumber::_ToDecimal128(_DecBase<_FmtTraits<decimal128> > * _Tgt,
                               _Context128 * _Ctxt) const
        {
        decimal128FromNumber(reinterpret_cast< ::decimal128 *>(_Tgt),
                             this, _Ctxt);
        _Ctxt->_CheckForRangeError();
        if (digits > DECIMAL128_Pmax)
                { decContextSetStatus(_Ctxt, DEC_Inexact); }
        }

} // namespace decimal
} // namespace std
