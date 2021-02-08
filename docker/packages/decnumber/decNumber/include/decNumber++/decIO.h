/* ------------------------------------------------------------------ */
/* decIO.h header                                                     */
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

#ifndef _DECIO_H
#define _DECIO_H

#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <string>

namespace ibm {
namespace decimal {

// extended numeric facets
template <class _CharT,
          class _Iter = std::istreambuf_iterator<_CharT,
                                                 std::char_traits<_CharT> > >
class extended_num_get : public std::locale::facet {
public:
        typedef _CharT char_type;
        typedef _Iter iter_type;
        typedef std::num_get<_CharT, _Iter> _MyNumGet;

        explicit extended_num_get(size_t _Refs = 0)
                : facet(_Refs) {}
        extended_num_get(std::locale & _Loc, size_t _Refs = 0)
                : facet(_Refs),
                  _Baseloc(_Loc) {}

        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, decimal32 & _V) const
                { return do_get(_In, _End, _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, decimal64 & _V) const
                { return do_get(_In, _End, _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, decimal128 & _V) const
                { return do_get(_In, _End, _B, _Err, _V); }

        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, bool & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, long & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, unsigned short & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, unsigned int & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, unsigned long & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, float & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, double & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, long double & _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }
        _Iter get(_Iter _In, _Iter _End, std::ios_base & _B,
                  std::ios_base::iostate & _Err, void *& _V) const
                { return std::use_facet<_MyNumGet>(_Baseloc).get(_In, _End,
                                                            _B, _Err, _V); }

        static std::locale::id id;

protected:

        virtual _Iter do_get(_Iter _In, _Iter _End, std::ios_base & _B,
                             std::ios_base::iostate & _Err,
                             decimal32 & _V) const
                {
                _DecNumber _D(_V);
                return _D._DoGet(&_V, _In, _End, _B, _Err);
                }
        virtual _Iter do_get(_Iter _In, _Iter _End, std::ios_base & _B,
                             std::ios_base::iostate & _Err,
                             decimal64 & _V) const
                {
                _DecNumber _D(_V);
                return _D._DoGet(&_V, _In, _End, _B, _Err);
                }
        virtual _Iter do_get(_Iter _In, _Iter _End, std::ios_base & _B,
                             std::ios_base::iostate & _Err,
                             decimal128 & _V) const
                {
                _DecNumber _D(_V);
                return _D._DoGet(&_V, _In, _End, _B, _Err);
                }

        virtual ~extended_num_get() {}

        std::locale & _Baseloc;
};

template <class _CharT, class _Iter>
std::locale::id extended_num_get<_CharT, _Iter>::id;

template <class _CharT,
          class _Iter = std::ostreambuf_iterator<_CharT,
                                                 std::char_traits<_CharT> > >
class extended_num_put : public std::locale::facet {
public:
        typedef _CharT char_type;
        typedef _Iter iter_type;
        typedef std::num_put<_CharT, _Iter> _MyNumPut;

        explicit extended_num_put(size_t _Refs = 0)
                : facet(_Refs) {}
        extended_num_put(std::locale & _Loc, size_t _Refs = 0)
                : facet(_Refs),
                  _Baseloc(_Loc) {}

        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  decimal32 _V) const
                { return do_put(_S, _F, _Fill, _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  decimal64 _V) const
                { return do_put(_S, _F, _Fill, _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  decimal128 _V) const
                { return do_put(_S, _F, _Fill, _V); }

        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  bool _V) const
                { return std::use_facet<_MyNumPut>(_Baseloc).do_put(_S, _F,
                                                                    _Fill,
                                                                    _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  long _V) const
                { return std::use_facet<_MyNumPut>(_Baseloc).do_put(_S, _F,
                                                                    _Fill,
                                                                    _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  unsigned long _V) const
                { return std::use_facet<_MyNumPut>(_Baseloc).do_put(_S, _F,
                                                                    _Fill,
                                                                    _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  double _V) const
                { return std::use_facet<_MyNumPut>(_Baseloc).do_put(_S, _F,
                                                                    _Fill,
                                                                    _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  long double _V) const
                { return std::use_facet<_MyNumPut>(_Baseloc).do_put(_S, _F,
                                                                    _Fill,
                                                                    _V); }
        _Iter put(_Iter _S, std::ios_base & _F, char_type _Fill,
                  const void * _V) const
                { return std::use_facet<_MyNumPut>(_Baseloc).do_put(_S, _F,
                                                                    _Fill,
                                                                    _V); }

        static std::locale::id id;

protected:

        virtual _Iter do_put(_Iter _S, std::ios_base & _F, char_type _Fill,
                             decimal32 _V) const
                {
                _DecNumber _D(_V);
                return _D._DoPut(_S, _F, _Fill);
                }
        virtual _Iter do_put(_Iter _S, std::ios_base & _F, char_type _Fill,
                             decimal64 _V) const
                {
                _DecNumber _D(_V);
                return _D._DoPut(_S, _F, _Fill);
                }
        virtual _Iter do_put(_Iter _S, std::ios_base & _F, char_type _Fill,
                             decimal128 _V) const
                {
                _DecNumber _D(_V);
                return _D._DoPut(_S, _F, _Fill);
                }

        virtual ~extended_num_put() {}

        std::locale & _Baseloc;
};

template <class _CharT, class _Iter>
std::locale::id extended_num_put<_CharT, _Iter>::id;

// helpers for iostreams insertion, extraction
template <class _CharType, class _CharTraits, class _FmtTraits> inline
std::basic_istream<_CharType, _CharTraits> &
_Extractor(std::basic_istream<_CharType, _CharTraits> & _Is,
           typename _FmtTraits::_Fmt & _V) 
        {
        typedef std::basic_istream<_CharType, _CharTraits> _MyStream_t;
        typedef std::istreambuf_iterator<_CharType, _CharTraits> _II;
        typedef extended_num_get<_CharType, _II> _MyFac_t;

        typename _MyStream_t::sentry _Sentry(_Is, false);
        if (_Sentry)
                {
                std::ios_base::iostate _Err = std::ios_base::goodbit;
                if (std::has_facet<_MyFac_t>(_Is.getloc()))
                        {
                        try 
                                {
                                const _MyFac_t & _MyFacet =
                                        std::use_facet<_MyFac_t>(_Is.getloc());

                                _MyFacet.get(_II(_Is.rdbuf()), _II(0), _Is,
                                             _Err, _V);
                                _Is.setstate(_Err);
                                }
                        catch (...)
                                {
                                _Is.setstate(std::ios_base::badbit);
                                } 
                        }
                else
                        {
                        // If the imbued locale doesn't have the right facet,
                        // then cheat.
                        _DecNumber _D(_V);
                        _D._DoGet(&_V, _II(_Is.rdbuf()), _II(0), _Is, _Err);
                        _Is.setstate(_Err);
                        }
                }

        return _Is;
        }

template <class _CharType, class _CharTraits, class _DecType> inline
std::basic_ostream<_CharType, _CharTraits> & _Inserter(
                std::basic_ostream<_CharType, _CharTraits> & _Os,
                _DecType _V)
        {
        typedef std::basic_ostream<_CharType, _CharTraits> _MyStream_t;
        typedef std::ostreambuf_iterator<_CharType, _CharTraits> _OI;
        typedef extended_num_put<_CharType, _OI> _MyFac_t;

        typename _MyStream_t::sentry _Sentry(_Os);
        if (_Sentry)
                {
                if (std::has_facet<_MyFac_t>(_Os.getloc()))
                        {
                        try 
                                {
                                const _MyFac_t & _MyFacet =
                                        std::use_facet<_MyFac_t>(_Os.getloc());

                                if (_MyFacet.put(_OI(_Os), _Os, _Os.fill(), _V)
                                            .failed())
                                        {
                                        _Os.setstate(std::ios_base::failbit);
                                        }
                                }
                        catch (...)
                                {
                                _Os.setstate(std::ios_base::badbit);
                                } 
                        }
                else
                        {
                        // If the imbued locale doesn't have the right facet,
                        // then cheat.
                        _DecNumber _D(_V);
                        if (_D._DoPut(_OI(_Os), _Os, _Os.fill()).failed())
                                { _Os.setstate(std::ios_base::failbit); }
                        }
                }

        return _Os;
        } 

// iostream inserters and extractors:

template <class _CharType, class _CharTraits> inline
std::basic_istream<_CharType, _CharTraits> & operator>>(
                std::basic_istream<_CharType, _CharTraits> & _Is,
                decimal32 & _V)
        {
        return _Extractor<_CharType, _CharTraits,
                          _FmtTraits<decimal32> >(_Is, _V);
        }

template <class _CharType, class _CharTraits> inline
std::basic_ostream<_CharType, _CharTraits> & operator<<(
                std::basic_ostream<_CharType, _CharTraits> & _Os,
                decimal32 _V)
        {
        return _Inserter(_Os, _V);
        }

template <class _CharType, class _CharTraits> inline
std::basic_istream<_CharType, _CharTraits> & operator>>(
                std::basic_istream<_CharType, _CharTraits> & _Is,
                decimal64 & _V)
        {
        return _Extractor<_CharType, _CharTraits,
                          _FmtTraits<decimal64> >(_Is, _V);
        }

template <class _CharType, class _CharTraits> inline
std::basic_ostream<_CharType, _CharTraits> & operator<<(
                std::basic_ostream<_CharType, _CharTraits> & _Os,
                decimal64 _V)
        {
        return _Inserter(_Os, _V);
        }

template <class _CharType, class _CharTraits> inline
std::basic_istream<_CharType, _CharTraits> & operator>>(
                std::basic_istream<_CharType, _CharTraits> & _Is,
                decimal128 & _V)
        {
        return _Extractor<_CharType, _CharTraits,
                          _FmtTraits<decimal128> >(_Is, _V);
        }

template <class _CharType, class _CharTraits> inline
std::basic_ostream<_CharType, _CharTraits> & operator<<(
                std::basic_ostream<_CharType, _CharTraits> & _Os,
                decimal128 _V)
        {
        return _Inserter(_Os, _V);
        }

template <class _CharType, class _CharTraits,
          uint32_t _Digits, uint32_t _Scale> inline
std::basic_istream<_CharType, _CharTraits> & operator>>(
                std::basic_istream<_CharType, _CharTraits> & _Is,
                _PackedDecimal<_Digits, _Scale> & _V)
        {
        typedef std::basic_istream<_CharType, _CharTraits> _MyStream_t;
        typedef std::istreambuf_iterator<_CharType, _CharTraits> _II;
        typedef extended_num_get<_CharType, _II> _MyFac_t;

        typename _MyStream_t::sentry _Sentry(_Is, false);
        if (_Sentry)
                {
                std::ios_base::iostate _Err = std::ios_base::goodbit;
                _DecNumber _D(_V);
                _D._DoGet(&_V, _II(_Is.rdbuf()), _II(0), _Is, _Err);
                _Is.setstate(_Err);
                }

        return _Is;
        }

template <class _CharType, class _CharTraits,
          uint32_t _Digits, uint32_t _Scale> inline
std::basic_ostream<_CharType, _CharTraits> & operator<<(
                std::basic_ostream<_CharType, _CharTraits> & _Os,
                _PackedDecimal<_Digits, _Scale> _V)
        {
        typedef std::basic_ostream<_CharType, _CharTraits> _MyStream_t;
        typedef std::ostreambuf_iterator<_CharType, _CharTraits> _OI;
        typedef extended_num_put<_CharType, _OI> _MyFac_t;

        typename _MyStream_t::sentry _Sentry(_Os);
        if (_Sentry)
                {
                _DecNumber _D(_V);
                if (_D._DoPut(_OI(_Os), _Os, _Os.fill()).failed())
                        { _Os.setstate(std::ios_base::failbit); }
                }

        return _Os;
        }

// helper functions for formatted output
template <class _CharType> inline
_CharType _WidenChar(const std::locale & _Loc, const char _C)
        { return std::use_facet<std::ctype<_CharType> >(_Loc).widen(_C); }

template <> inline
char _WidenChar<char>(const std::locale & _Loc, const char _C)
        { return _C; }

template <class _CharType> inline
std::basic_string<_CharType> _WidenStr(const std::locale & _Loc,
                                       const char * _Str, 
                                       const size_t _StrLen)
        {
        _CharType * _Tmp = new _CharType[_StrLen + 1];
        std::use_facet<std::ctype<_CharType> >(_Loc).widen(_Str,
                                                           _Str + _StrLen,
                                                           _Tmp);
        _Tmp[_StrLen] = _WidenChar<_CharType>(_Loc, '\0');
        std::basic_string<_CharType> _Ret(_Tmp);
        delete [] _Tmp;
        return _Ret;
        }

template <> inline
std::string _WidenStr<char>(const std::locale & _Loc,
                            const char * _Str, 
                            const size_t _StrLen)
        { return _Str; }

template <class _CharType>
std::basic_string<_CharType> _DecNumber::_DoPut(const std::ios_base & _F,
                                                _CharType _Fill) const
        {
        typedef std::basic_string<_CharType> _MyStrT;

        // output string
        _MyStrT _Num;

        // exponent of number to be output
        int32_t _Exp = exponent;

        // number of leading zeros in formatted string
        std::streamsize _ZeroCnt;

        // position of the decimal point, counting from left (i.e. the
        // number of significant digits to the left of the decimal point)
        std::streamsize _DecPtPos;

        // is true if we need to output an exponent field
        bool _ShowExponent = false;

        // decimal point character
        typedef std::numpunct<_CharType> _NPunctT;
        _CharType _DecPt =
                std::use_facet<_NPunctT>(_F.getloc()).decimal_point();
        if (_DecPt == 0)
                { _DecPt = _WidenChar<_CharType>(_F.getloc(), '.'); }

        // zero character
        const _CharType _Zero = _WidenChar<_CharType>(_F.getloc(), '0');

        // handle NAN
        if (bits & DECNAN)
                {
                if (_F.flags() & std::ios_base::uppercase) 
                        { _Num = _WidenStr<_CharType>(_F.getloc(),
                                                      _NANStr, _NANStrLen); }
                else
                        { _Num = _WidenStr<_CharType>(_F.getloc(),
                                                      _NanStr, _NanStrLen); }

                return _DoPadding(_F, _Num, _PutSign(_F, _Num), _Fill);
                }

        // handle INF
        if (bits & DECINF)
                {
                if (_F.flags() & std::ios_base::uppercase) 
                        { _Num = _WidenStr<_CharType>(_F.getloc(),
                                                      _INFStr, _INFStrLen); }
                else
                        { _Num = _WidenStr<_CharType>(_F.getloc(),
                                                      _InfStr, _InfStrLen); }

                return _DoPadding(_F, _Num, _PutSign(_F, _Num), _Fill);
                }

        // possibly normalize number
        switch(_F.flags() & std::ios_base::floatfield)
                {
                case std::ios_base::fixed:
                        _DoFixedNotation(_Exp, _ZeroCnt, _DecPtPos);
                        break;
                case std::ios_base::scientific:
                        _DoSciNotation(_Exp, _ZeroCnt, _DecPtPos);
                        _ShowExponent = true;
                        break;
                default:
                        _DoSciNotation(_Exp, _ZeroCnt, _DecPtPos);
                        if ((_Exp > -4) && (_Exp < _F.precision()))
                                {
                                _Exp -= digits - 1;
                                _DoFixedNotation(_Exp, _ZeroCnt, _DecPtPos);
                                }
                        else
                                { _ShowExponent = true; }
                        break;
                }

        // do mantissa
        _PutMantissa(_F, _Num, _Exp, _ShowExponent, _DecPtPos, _ZeroCnt,
                    _Zero, _DecPt);

        // do exponent
        if (_ShowExponent)
                { _PutExponent(_F, _Num, _Exp); }

        // do sign, and then do padding for width
        return _DoPadding(_F, _Num, _PutSign(_F, _Num), _Fill);
        }

inline
void _DecNumber::_DoFixedNotation(const int32_t _Exp,
                                  std::streamsize & _ZeroCnt,
                                  std::streamsize & _DecPtPos) const
        {
        // number of leading zeros
        _ZeroCnt = (_Exp < 0) && (digits <= -_Exp) ?
                           -(digits + _Exp) + 1 : 0;
        // position of decimal point, counting from left
        _DecPtPos = _ZeroCnt != 0 ? 0 : _Exp < 0 ? digits + _Exp : digits;
        }

inline
void _DecNumber::_DoSciNotation(int32_t & _Exp,
                                std::streamsize & _ZeroCnt,
                                std::streamsize & _DecPtPos) const
        {
        _Exp += digits - 1;
        _ZeroCnt = 0;
        _DecPtPos = 1;
        }

static const decNumberUnit _PowersOfTen[] = { 1, 10, 100,
#if DECDPUN > 2
                                              1000, 10000,
#endif
#if DECDPUN > 4
                                              100000, 1000000,
                                              10000000, 100000000
#endif
                                            };

template <class _CharType> inline
_CharType _DecodeDigit(const unsigned char _Dig,
                       decNumberUnit & _Unit,
                       const _CharType _Zero)
        {
        _CharType _Ret = _Zero; 

        decNumberUnit _Pow = _PowersOfTen[_Dig] * 8;
        for (unsigned char _Bit = 8; _Bit != 0; _Bit /= 2, _Pow /= 2)
                { if (_Unit >= _Pow) { _Unit -= _Pow; _Ret += _Bit; } }

        return _Ret;
        }

template <class _CharType> inline
void _DecNumber::_PutExponent(const std::ios_base & _F,
                              std::basic_string<_CharType> & _Num,
                              int32_t _Exp) const
        {
        _Num.push_back(_F.flags() & std::ios_base::uppercase ?
                       _WidenChar<_CharType>(_F.getloc(), 'E') :
                       _WidenChar<_CharType>(_F.getloc(), 'e'));

        if (_Exp >= 0)
                { _Num.push_back(_WidenChar<_CharType>(_F.getloc(), '+')); }
        else 
                {
                _Num.push_back(_WidenChar<_CharType>(_F.getloc(), '-'));
                _Exp *= -1;
                }

        // the exponent has to be at least two digits wide, so use a leading
        // zero if necessary
        if (_Exp < 10)
                { _Num.push_back(_WidenChar<_CharType>(_F.getloc(), '0')); }

        std::basic_ostringstream<_CharType> _Oss;
        _Oss << _Exp;
        _Num.append(_Oss.str());
        }

template <class _CharType>
void _DecNumber::_RoundUp(const std::ios_base & _F,
                          std::basic_string<_CharType> & _Num,
                          int32_t & _Exp,
                          const bool _ShowExponent,
                          const _CharType _Zero,
                          const _CharType _DecPt) const
        {
        typedef std::basic_string<_CharType> _MyStrT;
        bool _NeedNewDigit = true;
        typename _MyStrT::reverse_iterator _Rit = _Num.rbegin();

        // Start from the least significant digit.  If we need to carry,
        // move on to the next-least significant digit, etc.
        for (; _Rit != _Num.rend(); ++_Rit)
                {
                if (*_Rit != _DecPt)
                        {
                        if (*_Rit == _Zero + 9)
                                { *_Rit = _Zero; }
                        else
                                {
                                *_Rit += 1;
                                // We don't have to carry, so stop now.
                                _NeedNewDigit = false;
                                break;
                                }
                        }
                }

        // We may need to add a new digit.  If this is the case, then the
        // number after rounding is certain to be a power of ten.
        if (_NeedNewDigit)
                {
                // Add the new digit.
                typename _MyStrT::iterator _It =
                        _Num.insert(_Rit.base(), _Zero + 1);

                // If the number is to be output in scientific notation,
                // we must normalize it once more.
                if (_ShowExponent)
                        {
                        // Adjust the exponent.
                        _Exp += 1;

                        // Move the decimal point.
                        _It = _Num.insert(++_It, _DecPt);
                        _It += 2;
                        _Num.erase(_It);

                        // Delete a zero from the fractional part of the
                        // number, so that the number of fractional digits
                        // is still equal to the precision.
                        _Num.erase(_It);
                        }
                else if((_F.flags() & std::ios_base::fixed) &&
                        (_F.flags() & std::ios_base::scientific))
                        {
                        // Delete a zero from the fractional part of the
                        // number, so that the total number of digits
                        // is still equal to the precision.
                        size_t _Pos = _Num.find(_DecPt);
                        _Num.erase(_Pos + 1, 1);

                        // If there are no remaining fractional digits,
                        // we may need to erase the decimal point.
                        if ((*_Num.rbegin() == _DecPt) &&
                            (!(_F.flags() & std::ios_base::showpoint) ||
                             (_F.flags() & std::ios_base::showpos)))
                                { _Num.erase(_Pos, 1); }
                        }
                }
        }

inline 
bool _IsOdd(unsigned char _Val)
        {
        return _Val & 0x1 == 0x1;
        }

template <class _CharType>
void _DecNumber::_PutMantissa(const std::ios_base & _F,
                              std::basic_string<_CharType> & _Num,
                              int32_t & _Exp,
                              const bool _ShowExponent,
                              std::streamsize _DecPtPos,
                              std::streamsize _ZeroCnt,
                              const _CharType _Zero,
                              const _CharType _DecPt) const
        {
        // precision
        std::streamsize _Prec = _F.precision();

        // is true if output is to use "general" (similar to "%G") notation
        const bool _IsGeneralMode =
                static_cast<bool>(_F.flags() & std::ios_base::fixed) ==
                static_cast<bool>(_F.flags() & std::ios_base::scientific);

        // the number of leading zeros in the fraction part
        const size_t _FracZeros = _ZeroCnt == 0 ? 0 : _ZeroCnt - 1;

        // the number of fractional digits, excluding leading zeros
        const size_t _FracDigits = digits - _DecPtPos;

        // the number of significant digits allowed by the selected precision
        size_t _PrecDigits = _IsGeneralMode ? _Prec - _FracZeros :
                                              _DecPtPos + _Prec - _FracZeros;

        // Do we have excess digits that will not be representable within the
        // selected precision?
        const bool _ExcessDigits =
                _IsGeneralMode ? digits > (_Prec - _FracZeros) :
                                 _FracDigits > (_Prec - _FracZeros);

        // do leading zeros
        if (_ZeroCnt != 0)
                {
                _Num.push_back(_Zero);
                if (_FracZeros != 0)
                        {
                        _Num.push_back(_DecPt);
                        _Num.append(_FracZeros, _Zero);
                        } 
                }

        // handle significant digits
        const size_t _MSUDigits = digits % DECDPUN;
        int _UnitCnt = digits / DECDPUN + (_MSUDigits == 0 ? -1 : 0);
        signed char _Dig = (_MSUDigits == 0 ? DECDPUN : _MSUDigits) - 1;
        for (; _UnitCnt >= 0 && _PrecDigits != 0; --_UnitCnt)
                {
                decNumberUnit _Unit = lsu[_UnitCnt]; 
                for (; _Dig >= 0; --_Dig, --_PrecDigits)
                        {
                        // output the decimal point
                        if (_DecPtPos-- == 0 && _FracZeros == 0)
                                { _Num.push_back(_DecPt); }

                        // Output the current digit.
                        _Num.push_back(_DecodeDigit(_Dig, _Unit, _Zero));

                        // If we've already output all of the digits
                        // required for the selected precision, and
                        // there are still digits left over, then we'll
                        // need to round the number.
                        if ((_PrecDigits == 1) && _ExcessDigits)
                                {
                                // Get an extra digit so that we know which
                                // way to round.
                                if (_Dig == 0)
                                        {
                                        _Dig = DECDPUN - 1;
                                        _Unit = lsu[--_UnitCnt];
                                        }
                                else
                                        { --_Dig; }
                                const unsigned char _Extra =
                                        _DecodeDigit(_Dig, _Unit, _Zero)
                                                - _Zero;
                                // do rounding
                                _DoRounding(_F, _Num, _Exp, _ShowExponent,
                                            _Extra, _Zero, _DecPt);

                                _PrecDigits = 0;
                                break;
                                }
                        }

                _Dig = DECDPUN - 1;
                }

        // Append extra zeros if we are using fixed-mode notation and we
        // need more non-fractional digits (eg. if the number to be output
        // is 1.23E4, we've only output "123" so far, and we need to output
        // zeros so that we get "12300").
        const size_t _RemainingZeros =
               (exponent > _DecPtPos) && !_ShowExponent ?
                        exponent - _DecPtPos : 0;
        _Num.append(_RemainingZeros, _Zero);

        // If the number to be output is an integral (i.e. no digits to
        // the right of the decimal point), it may be necessary to append
        // a trailing decimal point.
        if (_FracDigits == 0)
                {
                if ((_F.flags() & std::ios_base::showpoint) &&
                    !(_IsGeneralMode && _F.flags() & std::ios_base::showpos))
                        { _Num.push_back(_DecPt); }
                else
                        { _Prec = 0; }
                }

        // Append trailing zeros if the required precision exceeds the number
        // of fractional digits we've already produced.
        if (!_IsGeneralMode)
                {
                const size_t _TrailingZeros =
                        _Prec > _FracZeros + _FracDigits ?
                        _Prec - _FracZeros - _FracDigits : 0;
                _Num.append(_TrailingZeros, _Zero);
                }
        else
                {
                if ((_F.flags() & std::ios_base::showpoint) &&
                    !(_F.flags() & std::ios_base::showpos))
                        {
                        const size_t _TrailingZeros =
                                _Prec > _FracZeros + _RemainingZeros + digits ?
                                _Prec - _FracZeros - _RemainingZeros - digits :
                                0;
                        _Num.append(_TrailingZeros, _Zero);
                        }
                }
        }

template <class _CharType>
void _DecNumber::_DoRounding(const std::ios_base & _F,
                             std::basic_string<_CharType> & _Num,
                             int32_t & _Exp,
                             const bool _ShowExponent,
                             const unsigned char _Extra,
                             const _CharType _Zero,
                             const _CharType _DecPt) const
        {
        switch(_ContextDefault().round)
                {
                case DEC_ROUND_DOWN:
                case DEC_ROUND_FLOOR:
                        // truncate
                        break;
                case DEC_ROUND_HALF_DOWN:
                        if (_Extra > 5)
                                { _RoundUp(_F, _Num, _Exp, _ShowExponent,
                                           _Zero, _DecPt); }
                        break;
                case DEC_ROUND_HALF_UP:
                        if (_Extra >= 5)
                                { _RoundUp(_F, _Num, _Exp, _ShowExponent,
                                           _Zero, _DecPt); }
                        break;
                case DEC_ROUND_HALF_EVEN:
                        if ((_Extra > 5) ||
                            (_Extra == 5 && _IsOdd(*_Num.rbegin() - _Zero)))
                                { _RoundUp(_F, _Num, _Exp, _ShowExponent,
                                           _Zero, _DecPt); }
                        break;
                case DEC_ROUND_CEILING:
                case DEC_ROUND_UP:
                default:
                        _RoundUp(_F, _Num, _Exp, _ShowExponent, _Zero, _DecPt);
                        break;
                }
        }

template <class _CharType> inline
bool _DecNumber::_PutSign(const std::ios_base & _F,
                          std::basic_string<_CharType> & _Num) const
        {
        bool _HasSign = true;
        if (bits & DECNEG)
                { _Num.insert(0, 1, _WidenChar<_CharType>(_F.getloc(), '-')); }
        else if (_F.flags() & std::ios_base::showpos)
                { _Num.insert(0, 1, _WidenChar<_CharType>(_F.getloc(), '+')); }
        else
                { _HasSign = false; }

        return _HasSign;
        }

template <class _CharType>
std::basic_string<_CharType>
_DecNumber::_DoPadding(const std::ios_base & _F,
                       std::basic_string<_CharType> & _Num,
                       const bool _HasSign,
                       _CharType _Fill) const
        {
        std::ios_base::fmtflags _AdjField =
                _F.flags() & std::ios_base::adjustfield;

        const long _PadCnt = _F.width() - _Num.length();
        if (_PadCnt <= 0)
                { return _Num; }

        switch(_AdjField)
                {
                case std::ios_base::internal:
                        // pad before the digits, but after the sign
                        _Num.insert(_HasSign ? 1 : 0, _PadCnt, _Fill);
                        break;
                case std::ios_base::left:
                        // pad after
                        _Num.append(_PadCnt, _Fill);
                        break;
                case std::ios_base::right:
                default:
                        // pad before
                        _Num.insert(0, _PadCnt, _Fill);
                        break;
                }

        return _Num;
        }

// helper functions for formatted input
template <class _Iter, class _Fmt> inline
_Iter _DecNumber::_DoGet(_Fmt * _P,
                         _Iter _Beg,
                         _Iter _End,
                         std::ios_base & _F,
                         std::ios_base::iostate & _Err)
        {
        typedef _FmtTraits<_Fmt> _Tr;
        _Iter _Ret =
		_DoGetAux<typename _Iter::value_type>(_Beg, _End, _F, _Err);
        if (_Err == std::ios_base::goodbit)
                { _Tr::_FromNumber(_P, *this); }
        return _Ret;
        }

template <class _CharType, class _Iter>
_Iter _DecNumber::_DoGetAux(_Iter _Beg,
                            _Iter _End,
                            std::ios_base & _F,
                            std::ios_base::iostate & _Err)
        {
        _Iter _It = _GetWhiteSpace<_CharType>(_Beg, _End, _F);
        _It = _GetSign<_CharType>(_It, _End, _F);
        _It = _GetWhiteSpace<_CharType>(_It, _End, _F);
        _It = _GetSpecialNumber<_CharType>(_It, _End, _F, _Err);
        if (_IsNAN() || _IsINF())
                {
                exponent =  0;
                _Err = std::ios_base::goodbit;
                }
        else if ((_Err & std::ios_base::failbit) == 0)
                {
                _It = _GetMantissa<_CharType>(_It, _End, _F, _Err);
                if ((_Err & std::ios_base::failbit) == 0)
                        {
                        _GetExponent<_CharType>(_It, _End, _F, _Err);
                        if ((_Err & std::ios_base::failbit) == 0)
                                { _Err = std::ios_base::goodbit; }
                        }
                }
        return _Beg;
        }

template <class _CharType, class _Iter> inline
_Iter _DecNumber::_GetWhiteSpace(_Iter _Beg,
                                 _Iter _End,
                                 const std::ios_base & _F) const
        {
        const std::ctype<_CharType> & _CType =
                std::use_facet<std::ctype<_CharType> >(_F.getloc());
        for(; _Beg != _End && _CType.is(std::ctype_base::space, *_Beg); _Beg++)
                { }

        return _Beg;
        }

template <class _CharType, class _Iter> inline
_Iter _DecNumber::_GetSign(_Iter _Beg,
                           _Iter _End,
                           const std::ios_base & _F)
        {
        if (*_Beg == _WidenChar<_CharType>(_F.getloc(), '+'))
                  { ++_Beg; }
        else if (*_Beg == _WidenChar<_CharType>(_F.getloc(), '-'))
                  {
                  ++_Beg;
                  bits |= DECNEG;
                  }

        return _Beg;
        }

template <class _CharType, class _Iter> inline
_Iter _DecNumber::_GetSpecialNumberAux(_Iter & _Beg,
                                       _Iter _End,
                                       std::ios_base & _F,
                                       std::ios_base::iostate & _Err,
                                       const char * _Str,
                                       const size_t _StrLen) const
        {
        ++_Beg;
        for (size_t _Idx = 1; _Idx < _StrLen - 1; ++_Beg, ++_Idx)
                {
                if (*_Beg != _WidenChar<_CharType>(_F.getloc(), _Str[_Idx]))
                        {
                        _Err = std::ios_base::failbit;
                        break;
                        }
                }

        return _Beg;
        }

template <class _CharType, class _Iter>
_Iter _DecNumber::_GetSpecialNumber(_Iter & _Beg,
                                    _Iter _End,
                                    std::ios_base & _F,
                                    std::ios_base::iostate & _Err)
        {
        // Handle NANs and infinities.
        if (*_Beg == _WidenChar<_CharType>(_F.getloc(), _NanStr[0]))
                {
                _Beg = _GetSpecialNumberAux<_CharType>(_Beg, _End, _F, _Err,
                                                       _NanStr, _NanStrLen);
                if (_Err != std::ios_base::failbit)
                        { bits |= DECNAN; }
                }
        else if (*_Beg == _WidenChar<_CharType>(_F.getloc(), _NANStr[0]))
                {
                _Beg = _GetSpecialNumberAux<_CharType>(_Beg, _End, _F, _Err,
                                                       _NANStr, _NANStrLen);
                if (_Err != std::ios_base::failbit)
                        { bits |= DECNAN; }
                }
        else if (*_Beg == _WidenChar<_CharType>(_F.getloc(), _InfStr[0]))
                {
                _Beg = _GetSpecialNumberAux<_CharType>(_Beg, _End, _F, _Err,
                                                       _InfStr, _InfStrLen);
                if (_Err != std::ios_base::failbit)
                        { bits |= DECINF; }
                }
        else if (*_Beg == _WidenChar<_CharType>(_F.getloc(), _INFStr[0]))
                {
                _Beg = _GetSpecialNumberAux<_CharType>(_Beg, _End, _F, _Err,
                                                       _INFStr, _INFStrLen);
                if (_Err != std::ios_base::failbit)
                        { bits |= DECINF; }
                }

        return _Beg;
        }

template <class _CharType, class _Iter>
_Iter _DecNumber::_GetMantissa(_Iter _Beg,
                               _Iter _End,
                               std::ios_base & _F,
                               std::ios_base::iostate & _Err)
        {
        typedef std::basic_string<_CharType> _MyStrT;

        const std::ctype<_CharType> & _CType =
                std::use_facet<std::ctype<_CharType> >(_F.getloc());
        const _CharType _Zero = _WidenChar<_CharType>(_F.getloc(), '0');
        const _CharType _DecPt = _WidenChar<_CharType>(_F.getloc(), '.');

        // if there is no mantissa, fail
        if (_Beg == _End)
                {
                _Err = std::ios_base::eofbit | std::ios_base::failbit;
                return _Beg;
                }
        bool _FoundLeadingZeros = *_Beg == _Zero;

        // discard leading zeros
        std::streamsize _DecPtPos = 0;
        std::streamsize _FractionalLeadingZeros = 0; 
        bool _FoundDecPt = false;
        for (; _Beg != _End && *_Beg == _Zero; ++_Beg)
                { }
        if (*_Beg == _DecPt)
                {
                _FoundDecPt = true;
                ++_Beg;
                }
        // count and discard leading zeros on the right side of the radix pt.
        // i.e. 0.00001
        //        ^^^^---- these.
        for (; _Beg != _End && *_Beg == _Zero; ++_Beg)
                { ++_FractionalLeadingZeros; }

        // extract digits from input range, and locate the decimal point
        std::basic_string<_CharType> _Acc;
        for (; _Beg != _End; ++_Beg)
                {
                if (_CType.is(std::ctype_base::digit, *_Beg))
                        {
                        _Acc.push_back(*_Beg);
                        if (!_FoundDecPt)
                                { ++_DecPtPos; }
                        }
                else if (*_Beg == _DecPt)
                        {
                        if (_FoundDecPt)
                                {
                                // We've already found a decimal point, so
                                // this decimal point is not a part of the
                                // number.
                                break;
                                }
                        else
                                { _FoundDecPt = true; }
                        }
                else
                        { break; }
                }

        if (_Acc.length() == 0)
                {
                if (_FoundLeadingZeros)
                        {
                        _Acc.push_back(_Zero);
                        digits = 1;
                        exponent = 0;
                        }
                else
                        {
                        _Err = std::ios_base::failbit;
                        return _Beg;
                        }
                }
        else
                {
                digits = _Acc.length();
                exponent = _DecPtPos - digits - _FractionalLeadingZeros;
                }

        // encode the mantissa
        const size_t _MSUDigits = _Acc.length() % DECDPUN;
        int _UnitCnt = _Acc.length() / DECDPUN + (_MSUDigits == 0 ? -1 : 0);
        unsigned char _Dig = (_MSUDigits == 0 ? DECDPUN : _MSUDigits);
        typename _MyStrT::iterator _SIter = _Acc.begin();
        for (; _UnitCnt >= 0; --_UnitCnt)
                {
                decNumberUnit _Unit = 0;
                for (; _Dig != 0; --_Dig)
                        {
                        _Unit *= 10;
                        _Unit += *_SIter++ - _Zero;
                        }
                lsu[_UnitCnt] = _Unit;
                _Dig = DECDPUN;
                }

        return _Beg;
        }

template <class _CharType, class _Iter>
_Iter _DecNumber::_GetExponent(_Iter _Beg,
                               _Iter _End,
                               std::ios_base & _F,
                               std::ios_base::iostate & _Err)
        {
        const std::ctype<_CharType> & _CType =
                std::use_facet<std::ctype<_CharType> >(_F.getloc());
        const _CharType _Zero = _WidenChar<_CharType>(_F.getloc(), '0');
        bool _NegExponent = false;

        int32_t _Exp = 0;
        if (*_Beg == _WidenChar<_CharType>(_F.getloc(), 'e') ||
            *_Beg == _WidenChar<_CharType>(_F.getloc(), 'E'))
                {
                if (++_Beg == _End)
                        {
                        _Err = std::ios_base::eofbit | std::ios_base::failbit;
                        return _Beg;
                        }

                // Get the sign.
                if (*_Beg == _WidenChar<_CharType>(_F.getloc(), '+'))
                        { ++_Beg; }
                else if (*_Beg == _WidenChar<_CharType>(_F.getloc(), '-'))
                        {
                        _NegExponent = true;
                        ++_Beg;
                        } 

                // Look for a end of string, or a non-digit character,
                // and report failure if we've found either.
                if (_Beg == _End || !_CType.is(std::ctype_base::digit, *_Beg))
                        {
                        _Err = std::ios_base::eofbit | std::ios_base::failbit;
                        return _Beg;
                        }

                // Get the digits.
                for (; _Beg != _End && _CType.is(std::ctype_base::digit, *_Beg)
                     ; ++_Beg)
                        {
                        _Exp *= 10;
                        _Exp += *_Beg - _Zero;
                        }

                if (_NegExponent)
                        { _Exp *= -1; }
                }

                exponent += _Exp;

        return _Beg;
        }

template <class _Tr> inline
void _DecBase<_Tr>::_FromString(const std::string & _Str)
        {
        _Derived _Tmp;
        std::stringstream _Iss(_Str);
        _Iss >> _Tmp;
        *this = _Iss.good() ? _Tmp : _GetNAN();
        }

template <class _Tr> inline
void _DecBase<_Tr>::_FromString(const char * _Str)
        {
        std::string _Str2(_Str);
        _FromString(_Str2);
        }

} // namespace decimal
} // namespace std

#endif /* ndef _DECIO_H */
