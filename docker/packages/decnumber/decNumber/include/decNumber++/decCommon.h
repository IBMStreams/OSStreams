/* ------------------------------------------------------------------ */
/* decCommon.h header                                                 */
/* ------------------------------------------------------------------ */
/* Copyright (c) IBM Corporation, 2006.  All rights reserved.         */
/*                                                                    */
/* This software is made available under the terms of the IBM         */
/* alphaWorks License Agreement (distributed with this software as    */
/* alphaWorks-License.txt).  Your use of this software indicates      */
/* your acceptance of the terms and conditions of that Agreement.     */
/*                                                                    */
/* Please send comments, suggestions, and corrections to the author:  */
/*   mfc@uk.ibm.com                                                   */
/*   Mike Cowlishaw, IBM Fellow                                       */
/*   IBM UK, PO Box 31, Birmingham Road, Warwick CV34 5JL, UK         */
/* ------------------------------------------------------------------ */

#ifndef _DECCOMMON_H
#define _DECCOMMON_H

// configuration macros for the decNumber++ library:
#include "decConfig.h"

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cmath>
#include <ios>
#include <iosfwd>
#include <stdint.h>

#ifdef _DEC_HAS_C99_MATH
#include <fenv.h>
#endif /* def _DEC_HAS_C99_MATH */

#define DECNUMDIGITS 45 // IMPORTANT: must appear first!
extern "C" {
#include "decContext.h"
#include "decNumber.h"
#include "decPacked.h"
}

/* from <fenv.h> */

#define FE_DEC_ALWAYSFROMZERO    DEC_ROUND_UP
#define FE_DEC_DOWNWARD          DEC_ROUND_FLOOR
#define FE_DEC_TONEAREST         DEC_ROUND_HALF_EVEN
#define FE_DEC_TONEARESTFROMZERO DEC_ROUND_HALF_UP
#define FE_DEC_TOWARDZERO        DEC_ROUND_HALF_DOWN
#define FE_DEC_TRUNCATE          DEC_ROUND_DOWN
#define FE_DEC_UPWARD            DEC_ROUND_CEILING

namespace ibm {
namespace decimal {

template <class _Fmt> struct _FmtTraits;
template <class _Tr> struct _DecBase;

class decimal32;
class decimal64;
class decimal128;

template <uint32_t _Digits, uint32_t _Scale>
class _PackedDecimal;

// global floating-point context for decimal:
struct _ContextBase : ::decContext {

        friend int fe_dec_getround();
        friend int fe_dec_setround(int _Round);
        friend uint32_t _GetEnvFlags();
        friend void _ClearEnvFlags();

        void _DomainError() const
                {
#ifdef _DEC_HAS_C99_MATH
                if (math_errhandling & MATH_ERRNO)
                        { errno = EDOM; }
                if (math_errhandling & MATH_ERREXCEPT)
                        { feraiseexcept(FE_INVALID); }
#else
                errno = EDOM;
#endif /* _DEC_HAS_C99_MATH */
                }

        void _RangeError() const
                {
#ifdef _DEC_HAS_C99_MATH
                if (math_errhandling & MATH_ERRNO)
                        { errno = ERANGE; }
                if (math_errhandling & MATH_ERREXCEPT)
                        { feraiseexcept(FE_OVERFLOW); }
#else
                errno = ERANGE;
#endif /* _DEC_HAS_C99_MATH */
                }

        void _CheckForRangeError() const
                {
                if (status & (DEC_Overflow  |
                              DEC_Underflow |
                              DEC_Division_impossible))
                        { _RangeError(); }
                }

protected:

        _ContextBase(bool _FPExcp = true) :
                _TrapsFlag(_FPExcp)
                { }

        virtual ~_ContextBase()
                {
                if (_TrapsFlag)
                        { _RaiseTraps(); }
                _SetEnvFlags();
                }

        void _GetRoundingMode()
                { round = _RoundingMode; }

        const bool _TrapsFlag;

private:

        void _RaiseTraps() const
                {
#ifdef _DEC_HAS_C99_MATH
                if (status & DEC_Division_by_zero)
                        { feraiseexcept(FE_DIVBYZERO); }
                if (status & DEC_Invalid_operation)
                        { feraiseexcept(FE_INVALID); }
                if (status & DEC_Overflow)
                        { feraiseexcept(FE_OVERFLOW); }
                if (status & DEC_Underflow)
                        { feraiseexcept(FE_UNDERFLOW); }
                // inexact should be signalled last
                if (status & DEC_Inexact)
                        { feraiseexcept(FE_INEXACT); }
#endif /* _DEC_HAS_C99_MATH */
                }

        void _SetEnvFlags() const
                { _EnvFlags |= status; }

        static rounding _RoundingMode;
        static uint32_t _EnvFlags;
};

struct _ContextDefault : _ContextBase {
        _ContextDefault(bool _FPExcp = true) :
                _ContextBase(_FPExcp)
                {
                decContextDefault(this, DEC_INIT_BASE);
                _GetRoundingMode();
                }
};

struct _Context32 : _ContextBase {
        _Context32(bool _FPExcp = true) :
                _ContextBase(_FPExcp)
                {
                decContextDefault(this, DEC_INIT_DECIMAL32);
                _GetRoundingMode();
                }
};

struct _Context64 : _ContextBase {
        _Context64(bool _FPExcp = true) :
                _ContextBase(_FPExcp)
                {
                decContextDefault(this, DEC_INIT_DECIMAL64);
                _GetRoundingMode();
                }
};

struct _Context128 : _ContextBase {
        _Context128(bool _FPExcp = true) :
                _ContextBase(_FPExcp)
                {
                decContextDefault(this, DEC_INIT_DECIMAL128);
                _GetRoundingMode();
                }
};

inline int fe_dec_getround()
        {
        return (_ContextBase::_RoundingMode < DEC_ROUND_MAX) ?
                _ContextBase::_RoundingMode : -1;
        }

inline int fe_dec_setround(int _Mode)
        {
        return (_Mode < DEC_ROUND_MAX) ?
                _ContextBase::_RoundingMode =
                        static_cast<rounding>(_Mode), 0 : 1;
        }

inline uint32_t _GetEnvFlags() { return _ContextBase::_EnvFlags; }
inline void _ClearEnvFlags()   { _ContextBase::_EnvFlags = 0; }

// general decimal class
struct _DecNumber : public decNumber {

        // constructors
        _DecNumber();
        _DecNumber(_DecBase<_FmtTraits<decimal32> >);
        _DecNumber(_DecBase<_FmtTraits<decimal64> >);
        _DecNumber(_DecBase<_FmtTraits<decimal128> >);

        template<uint32_t _Digits, uint32_t _Scale>
        _DecNumber(_DecBase<_FmtTraits<_PackedDecimal<_Digits, _Scale> > >);

        _DecNumber(int _Src)
                { _FromSignedIntegral(_Src); }

        _DecNumber(unsigned int _Src)
                { _FromUnsignedIntegral(_Src); }

        _DecNumber(long _Src)
                { _FromSignedIntegral(_Src); }

        _DecNumber(unsigned long _Src)
                { _FromUnsignedIntegral(_Src); }

        _DecNumber(long long _Src)
                { _FromSignedIntegral(_Src); }

        _DecNumber(unsigned long long _Src)
                { _FromUnsignedIntegral(_Src); }

        _DecNumber(unsigned long long _Coeff, int32_t _Exp, bool _Sign)
                {
                bits = _Sign ? DECNEG : 0;
                _FromUnsignedIntegral(_Coeff);
                exponent += _Exp;
                }

        explicit _DecNumber(long double);
        _DecNumber(const _DecNumber &);
        _DecNumber & operator=(const _DecNumber &);

        // conversions
        float       _ToFloat() const;
        double      _ToDouble() const;
        long double _ToLongDouble() const;
        long long   _ToSignedIntegral() const;
        void        _ToDecimal32 (_DecBase<_FmtTraits<decimal32> >  *,
                                  _Context32  *) const;
        void        _ToDecimal64 (_DecBase<_FmtTraits<decimal64> >  *,
                                  _Context64  *) const;
        void        _ToDecimal128(_DecBase<_FmtTraits<decimal128> > *,
                                  _Context128 *) const;

        template <uint32_t _Digits, uint32_t _Scale>
        void        _ToPackedDecimal(
               _DecBase<_FmtTraits<_PackedDecimal<_Digits, _Scale> > > *) const;

        void _FromIntegral(unsigned long long);

        void _FromSignedIntegral(signed long long _Src)
                {
                if (_Src >= 0)
                        { bits = 0; }
                else
                        {
                        bits = DECNEG;
                        _Src = -_Src;
                        }
                _FromIntegral(_Src);
                }

        void _FromUnsignedIntegral(unsigned long long _Src)
                {
                bits = 0;
                _FromIntegral(_Src);
                }

        // predicates:
        bool _IsNAN() const
                { return _IsQNAN() || _IsSNAN(); }
        bool _IsQNAN()  const
                { return bits & DECNAN; }
        bool _IsSNAN() const
                { return bits & DECSNAN; }
        bool _IsINF()  const
                { return bits & DECINF; }
        bool _IsNeg()  const
                { return bits & DECNEG; }
        bool _IsZero() const
                { return decNumberIsZero(this); }
        bool _IsFinite() const
                { return !_IsNAN() && !_IsSNAN() && !_IsINF(); }
        bool _IsInteger() const
                {
                _DecNumber _Tmp(*this);
                decNumberTrim(&_Tmp); 
                return _Tmp.exponent + 1 > _Tmp.digits;
                }

        // sign:
        void _ClearSign()
                { bits &= ~DECNEG; }
        void _SetSign()
                { bits |= DECNEG; }

        // utility function for unary operations
        template <class _Op, class _Tr>
        static typename _Tr::_Fmt
                _UnaryOp(const _DecNumber & _Rhs);

        // unary arithmetic operations
        struct _Plus {
                static _DecNumber _Operator(const _DecNumber &_Rhs, 
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result;
                        decNumberPlus(&_Result, &_Rhs, _Context);
                        return _Result;
                        }
                };
        struct _Minus {
                static _DecNumber _Operator(const _DecNumber &_Rhs,
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result(_Rhs);
                        _Result._IsNeg() ? _Result._ClearSign() :
                                           _Result._SetSign();
                        return _Result;
                        }
                };
        struct _Abs {
                static _DecNumber _Operator(const _DecNumber &_Rhs,
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result;
                        decNumberAbs(&_Result, &_Rhs, _Context);
                        return _Result;
                        }
                };

        // utilities for binary operations
        template <class _Op, class _Tr>
        static typename _Tr::_Fmt _BinaryOp(const _DecNumber & _Lhs,
                                            const _DecNumber & _Rhs);

        template <class _Op, class _Tr>
        static void _BinaryOpInPlace(const _DecNumber & _Lhs,
                                     const _DecNumber & _Rhs,
                                     typename _Tr::_BasePtr _ResultPtr);

        // binary arithmetic operations
        struct _Add {
                static _DecNumber _Operator(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result;
                        decNumberAdd(&_Result, &_Lhs, &_Rhs, _Context);
                        return _Result;
                        }
                };
        struct _Sub {
                static _DecNumber _Operator(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result;
                        decNumberSubtract(&_Result, &_Lhs, &_Rhs, _Context);
                        return _Result;
                        }
                };
        struct _Mul {
                static _DecNumber _Operator(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result;
                        decNumberMultiply(&_Result, &_Lhs, &_Rhs, _Context);
                        return _Result;
                        }
                };
        struct _Div {
                static _DecNumber _Operator(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            ::decContext * _Context)
                        {
                        _DecNumber _Result;
                        decNumberDivide(&_Result, &_Lhs, &_Rhs, _Context);
                        return _Result;
                        }
                };

        // utilities for one-argument math functions
        template <class _Op, class _Tr>
        static typename _Tr::_Fmt _UnaryMathFn(const _DecNumber &_Rhs);

        // unary math functions
        struct _Ceil {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        _Context.round = DEC_ROUND_CEILING;
                        decNumberToIntegralValue(&_Result, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Exp {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        decNumberExp(&_Result, &_Rhs, &_Context);
                        _Context._CheckForRangeError();
                        return _Result;
                        }
                };
        struct _Floor {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        _Context.round = DEC_ROUND_FLOOR;
                        decNumberToIntegralValue(&_Result, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Log {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsNeg())
                                { _Context._DomainError(); }
                        else
                                { decNumberLn(&_Result, &_Rhs, &_Context); }
                        return _Result;
                        }
                };
        struct _Log10 {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsNeg())
                                { _Context._DomainError(); }
                        else
                                { decNumberLog10(&_Result, &_Rhs, &_Context); }
                        return _Result;
                        }
                };
        struct _Logb {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsZero())
                                { _Context._RangeError(); }
                        else
                                {
                                decNumberNormalize(&_Result, &_Rhs, &_Context);
                                _Result = _DecNumber(_Result.exponent);
                                }
                        return _Result;
                        }
                };
        struct _Nearbyint {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        decNumberToIntegralValue(&_Result, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Rint {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        decNumberToIntegralValue(&_Result, &_Rhs, &_Context);
                        if ((_Rhs.exponent < 0) &&
                            _Compare<_Eq>(_Result, _Rhs))
                                { _Context.status |= DEC_Inexact; }
                        return _Result;
                        }
                };
        struct _Round {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        _Context.round = DEC_ROUND_HALF_UP;
                        decNumberToIntegralValue(&_Result, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Sqrt {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsNeg())
                                { _Context._DomainError(); }
                        else
                                { decNumberSquareRoot(&_Result, &_Rhs,
                                                      &_Context); }
                        return _Result;
                        }
                };
        struct _Trunc {
                static _DecNumber _Function(const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        _Context.round = DEC_ROUND_DOWN;
                        decNumberToIntegralValue(&_Result, &_Rhs, &_Context);
                        return _Result;
                        }
                };

        // utilities for two-argument math functions
        template <class _Op, class _Tr>
        static typename _Tr::_Fmt _BinaryMathFn(const _DecNumber &_Lhs,
                                                const _DecNumber &_Rhs);

        // binary math functions
        struct _Fmod {
                static _DecNumber _Function(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsZero())
                                { _Context._DomainError(); }
                        else
                                {
                                decNumberRemainder(&_Result, &_Lhs, &_Rhs,
                                                   &_Context);
                                _Context._CheckForRangeError();
                                }
                        return _Result;
                        }
                };
        struct _Max {
                static _DecNumber _Function(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        decNumberMax(&_Result, &_Lhs, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Min {
                static _DecNumber _Function(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        decNumberMin(&_Result, &_Lhs, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Pow {
                static _DecNumber _Function(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsNeg() && _Rhs._IsFinite() &&
                            !_Lhs._IsInteger())
                                { _Context._DomainError(); }
                        else if (_Lhs._IsZero())
                                {
                                if (_Rhs._IsZero())
                                        { _Context._DomainError(); }
                                else if (_Rhs._IsNeg())
                                        { _Context._RangeError(); }
                                }
                        else
                                {
                                decNumberPower(&_Result, &_Lhs, &_Rhs,
                                               &_Context);
                                _Context._CheckForRangeError();
                                }
                        return _Result;
                        }
                };
        struct _Quantize {
                static _DecNumber _Function(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        decNumberQuantize(&_Result, &_Lhs, &_Rhs, &_Context);
                        return _Result;
                        }
                };
        struct _Remainder {
                static _DecNumber _Function(const _DecNumber &_Lhs,
                                            const _DecNumber &_Rhs,
                                            _ContextBase &_Context)
                        {
                        _DecNumber _Result;
                        if (_Rhs._IsZero())
                                { _Context._DomainError(); }
                        else
                                {
                                decNumberRemainderNear(&_Result, &_Lhs, &_Rhs,
                                                       &_Context);
                                _Context._CheckForRangeError();
                                }
                        return _Result;
                        }
                };

        // utilities for relational operations
        template <class _CompareOp>
        static bool _Compare(const _DecNumber &_Lhs,
                             const _DecNumber &_Rhs);

        template <class _CompareOp>
        static bool _CompareNoInvalid(const _DecNumber &_Lhs,
                                      const _DecNumber &_Rhs);

        // relational operations
        struct _Eq {
                static bool _Operator(const _DecNumber &_R)
                        { return _R._IsZero(); }
                };
        struct _Ne {
                static bool _Operator(const _DecNumber &_R)
                        { return !_R._IsZero(); }
                };
        struct _Lt {
                static bool _Operator(const _DecNumber &_R)
                        { return _R._IsNeg(); }
                };
        struct _Le {
                static bool _Operator(const _DecNumber &_R)
                        { return _R._IsNeg() || _R._IsZero(); }
                };
        struct _Gt {
                static bool _Operator(const _DecNumber &_R)
                        { return !(_R._IsNeg() || _R._IsZero()); }
                };
        struct _Ge {
                static bool _Operator(const _DecNumber &_R)
                        { return !_R._IsNeg(); }
                };

        // samequantum function
        static bool _SameQuantum(const _DecNumber &_Lhs,
                                 const _DecNumber &_Rhs)
                {
                _DecNumber _Result;
                decNumberSameQuantum(&_Result, &_Lhs, &_Rhs);
                return _Result._IsZero();
                }

#ifdef _DEC_HAS_C99_MATH
        // ilogb function
        static int _Ilogb(const _DecNumber &_Lhs)
                {
                _ContextDefault _Context(false);
                if (_Lhs._IsZero())
                        {
                        _Context._DomainError();
                        return FP_ILOGB0;
                        }
                if (_Lhs._IsINF())
                        {
                        _Context._DomainError();
                        return INT_MAX;
                        }
                if (_Lhs._IsNAN())
                        {
                        _Context._DomainError();
                        return FP_ILOGBNAN;
                        }
                return _Logb::_Function(_Lhs, _Context)._ToSignedIntegral();
                }
#endif /* def _DEC_HAS_C99_MATH */

        // formatted output
        template <class _CharType, class _Iter>
        _Iter _DoPut(_Iter _S,
                     const std::ios_base & _F,
                     _CharType _Fill) const
                {
                std::basic_string<_CharType> _Str = _DoPut(_F, _Fill);
                std::copy(_Str.begin(), _Str.end(), _S);
                return _S;
                } 

        // formatted input
        template <class _Iter, class _Fmt>
        _Iter _DoGet(_Fmt * _P,
                     _Iter _Beg,
                     _Iter _End,
                     std::ios_base & _F,
                     std::ios_base::iostate & _Err);

private:

        // constants for formatted input/output of NAN and INF
        static const char _InfStr[];
        static const size_t _InfStrLen;
        static const char _INFStr[];
        static const size_t _INFStrLen;
        static const char _NanStr[];
        static const size_t _NanStrLen;
        static const char _NANStr[];
        static const size_t _NANStrLen;

        // helper function for constructors
        void _ZeroSignificand();

        // helper functions for formatted output
        template <class _CharType>
        std::basic_string<_CharType> _DoPut(const std::ios_base & _F,
                                            _CharType _Fill) const;

        template <class _CharType>
        void _PutMantissa(const std::ios_base & _F,
                          std::basic_string<_CharType> & _Num,
                          int32_t & _Exp,
                          const bool _ShowExponent,
                          std::streamsize _DecPtPos,
                          std::streamsize _ZeroCnt,
                          const _CharType _Zero,
                          const _CharType _DecPt) const;

        void _DoFixedNotation(const int32_t _Exp,
                              std::streamsize & _ZeroCnt,
                              std::streamsize & _DecPtPos) const;

        void _DoSciNotation(int32_t & _Exp,
                            std::streamsize & _ZeroCnt,
                            std::streamsize & _DecPtPos) const;

        template <class _CharType>
        void _PutExponent(const std::ios_base & _F,
                          std::basic_string<_CharType> & _Num,
                          int32_t _Exp) const;

        template <class _CharType>
        void _DoRounding(const std::ios_base & _F,
                         std::basic_string<_CharType> & _Num,
                         int32_t & _Exp,
                         const bool _ShowExponent,
                         const unsigned char _Extra,
                         const _CharType _Zero,
                         const _CharType _DecPt) const;

        template <class _CharType>
        void _RoundUp(const std::ios_base & _F,
                      std::basic_string<_CharType> & _Num,
                      int32_t & _Exp,
                      const bool _ShowExponent,
                      const _CharType _Zero,
                      const _CharType _DecPt) const;

        template <class _CharType>
        bool _PutSign(const std::ios_base & _F,
                      std::basic_string<_CharType> & _Num) const;

        template <class _CharType>
        std::basic_string<_CharType>
        _DoPadding(const std::ios_base & _F,
                   std::basic_string<_CharType> & _Num,
                   const bool _HasSign,
                   _CharType _Fill) const;

        // helper functions for formatted input
        template <class _CharType, class _Iter>
        _Iter _DoGetAux(_Iter _Beg,
                        _Iter _End,
                        std::ios_base & _F,
                        std::ios_base::iostate & _Err);

        template <class _CharType, class _Iter>
        _Iter _GetWhiteSpace(_Iter _Beg,
                             _Iter _End,
                             const std::ios_base & _F) const;

        template <class _CharType, class _Iter>
        _Iter _GetSign(_Iter _Beg,
                       _Iter _End,
                       const std::ios_base & _F);

        template <class _CharType, class _Iter>
        _Iter _GetSpecialNumberAux(_Iter & _Beg,
                                   _Iter _End,
                                   std::ios_base & _F,
                                   std::ios_base::iostate & _Err,
                                   const char * _Str,
                                   const size_t _StrLen) const;

        template <class _CharType, class _Iter>
        _Iter _GetSpecialNumber(_Iter & _Beg,
                                _Iter _End,
                                std::ios_base & _F,
                                std::ios_base::iostate & _Err);

        template <class _CharType, class _Iter>
        _Iter _GetMantissa(_Iter _Beg,
                           _Iter _End,
                           std::ios_base & _F,
                           std::ios_base::iostate & _Err);

        template <class _CharType, class _Iter>
        _Iter _GetExponent(_Iter _Beg,
                           _Iter _End,
                           std::ios_base & _F,
                           std::ios_base::iostate & _Err);
};

// _DecNumber member templates
template <uint32_t _Digits, uint32_t _Scale>
_DecNumber::_DecNumber(
        _DecBase<_FmtTraits<_PackedDecimal<_Digits, _Scale> > > source)
        {
        typedef _FmtTraits<_PackedDecimal<_Digits, _Scale> > _Tr;
        const int _Sc = _Scale;
        _ZeroSignificand();
        decPackedToNumber(reinterpret_cast<uint8_t *>(&source),
                          _Tr::_NumBytes, & _Sc, this);
        }

template <uint32_t _Digits, uint32_t _Scale>
void _DecNumber::_ToPackedDecimal(
        _DecBase<_FmtTraits<_PackedDecimal<_Digits, _Scale> > > * _Tgt) const
        {
        typedef _FmtTraits<_PackedDecimal<_Digits, _Scale> > _Tr;
        int _Sc = _Scale;
        _ContextDefault _Ctxt;
        _Ctxt.digits = _Digits;
        _DecNumber _ScaleAsDecNumber(_Scale);
        _DecNumber _Tmp(*this);
        decNumberRescale(&_Tmp, this, &_ScaleAsDecNumber, &_Ctxt);
        decPackedFromNumber(reinterpret_cast<uint8_t *>(_Tgt),
                            _Tr::_NumBytes, & _Sc, &_Tmp);
        }

// utilites for unary operations
template <class _Op, class _Tr> inline
typename _Tr::_Fmt
_DecNumber::_UnaryOp(const _DecNumber & _Rhs)
        {
        typename _Tr::_Context _Ctxt;
        typename _Tr::_Fmt _Result;
        _DecNumber _Tmp = _Op::_Operator(_Rhs, &_Ctxt);
        _Tr::_FromNumber(&_Result, _Tmp, &_Ctxt);
        return _Result;
        }

// utilites for binary operations
template <class _Op, class _Tr> inline
typename _Tr::_Fmt
_DecNumber::_BinaryOp(const _DecNumber &_Lhs, const _DecNumber &_Rhs)
        {
        typename _Tr::_Context _Ctxt;
        typename _Tr::_Fmt _Result;
        _DecNumber _Tmp = _Op::_Operator(_Lhs, _Rhs, &_Ctxt);
        _Tr::_FromNumber(&_Result, _Tmp, &_Ctxt);
        return _Result;
        }

template <class _Op, class _Tr> inline
void _DecNumber::_BinaryOpInPlace(const _DecNumber &_Lhs,
                                  const _DecNumber &_Rhs,
                                  typename _Tr::_BasePtr _ResultPtr)
        {
        typename _Tr::_Context _Ctxt;
        _DecNumber _Tmp = _Op::_Operator(_Lhs, _Rhs, &_Ctxt);
        _Tr::_FromNumber(_ResultPtr, _Tmp, &_Ctxt);
        }

// utilities for one-argument math functions
template <class _Op, class _Tr> inline
typename _Tr::_Fmt _DecNumber::_UnaryMathFn(const _DecNumber & _Rhs)
        {
        typename _Tr::_Context _Ctxt(false);
        typename _Tr::_Fmt _Result;
        _DecNumber _Tmp = _Op::_Function(_Rhs, _Ctxt);
        _Tr::_FromNumber(&_Result, _Tmp, &_Ctxt);
        return _Result;
        }

// utilites for two-argument operations
template <class _Op, class _Tr> inline
typename _Tr::_Fmt
_DecNumber::_BinaryMathFn(const _DecNumber &_Lhs, const _DecNumber &_Rhs)
        {
        typename _Tr::_Context _Ctxt(false);
        typename _Tr::_Fmt _Result;
        _DecNumber _Tmp = _Op::_Function(_Lhs, _Rhs, _Ctxt);
        _Tr::_FromNumber(&_Result, _Tmp, &_Ctxt);
        return _Result;
        }

// utilities for relational operations
template <class _CompareOp> inline
bool _DecNumber::_Compare(const _DecNumber &_Lhs,
                          const _DecNumber &_Rhs)
        {
        _ContextDefault _Ctxt;
        if (_Lhs._IsNAN() || _Rhs._IsNAN())
                {
                _Ctxt.status |= DEC_Invalid_operation;
                return false;
                }
        else
                {
                _DecNumber _Result;
                // Assumption: this decNumberCompare never returns -0
                decNumberCompare(&_Result, &_Lhs, &_Rhs, &_Ctxt);
                return _CompareOp::_Operator(_Result);
                }
        }

template <class _CompareOp> inline
bool _DecNumber::_CompareNoInvalid(const _DecNumber &_Lhs,
                                   const _DecNumber &_Rhs)
        {
        _ContextDefault _Ctxt;
        if (_Lhs._IsNAN() || _Rhs._IsNAN())
                { return false; }
        else
                {
                _DecNumber _Result;
                // Assumption: this decNumberCompare never returns -0
                decNumberCompare(&_Result, &_Lhs, &_Rhs, &_Ctxt);
                return _CompareOp::_Operator(_Result);
                }
        }

} // namespace decimal
} // namespace ibm


#endif /* ndef _DECCOMMON_H */
