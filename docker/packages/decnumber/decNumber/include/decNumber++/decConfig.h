/* ------------------------------------------------------------------ */
/* decConfig.h header                                                 */
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

#ifndef _DECCONFIG_H_
#define _DECCONFIG_H_

#include <climits>

/*
   The decNumber++ library recognizes a number of configuration macros.
   Set these as required for your compiler and architecture.

   Byte order settings (#define exactly one of these):
   ---------------------------------------------------
   _DEC_BIG_ENDIAN        -- #define this macro if you are targeting a
                             big endian architecture.
   _DEC_LITTLE_ENDIAN     -- #define this macro if you are targeting a
                             little endian architecture.

   Long double format settings (#define exactly one of these):
   -----------------------------------------------------------
   _DEC_LONGDOUBLE64      -- #define this macro if your compiler uses the
                             64-bit IEEE binary floating-point format to
                             represent a long double.
   _DEC_LONGDOUBLE80      -- #define this macro if your compiler uses the
                             Intel 80-bit extended-precision floating-point
                             format to represent a long double
                             (valid only when _DEC_LITTLE_ENDIAN is #defined).
   _DEC_LONGDOUBLE128IEEE -- #define this macro if your compiler uses the
                             128-bit IEEE binary floating-point format to
                             represent a long double.
   _DEC_LONGDOUBLE128PPC  -- #define this macro if your compiler uses the
                             PowerPC 128-bit binary floating-point format to
                             represent a long double.

   C++ compiler language support settings:
   ---------------------------------------
   _DEC_HAS_C99_MATH      -- #define this macro if your environment supports
                             the C99 math library.
   _DEC_HAS_ISINF         -- #define this macro if your environment supports
                             the C99 isinf() macro.
   _DEC_HAS_ISNAN         -- #define this macro if your environment supports
                             the C99 isnan() macro.
   _DEC_HAS_POWL          -- #define this macro if your environment supports
                             the powl() function.
   _DEC_HAS_TR1_MATH      -- #define this macro if your C++ environment
                             supports the C++ TR1 math library facilities.
                             In particular, if the header file <cmath>
                             contains the template std::tr1::fpclassify<>,
                             you can safely define this macro. 
   _DEC_HAS_TYPE_TRAITS   -- This library uses some C++ TR1 type_traits
                             internally.  You should #define this macro if
			     your C++ environment includes support for the
			     C++ TR1 header <type_traits>.
                             If this macro is not #defined, decNumber++ will
			     use the alternative type_traits provided in this
                             file.
   _DEC_HAS_SFINAE        -- #define this macro if your compiler supports
                             SFINAE.
   _DEC_STATIC_ASSERT     -- #define this macro to expand to the C++ keyword
                             "static_assert" if your C++ compiler supports
                             C++0x static_assert declarations.
                             Some compilers may support static_assert
                             functionality under an alternative
                             (non-standard) keyword, such as __static_assert.
                             To use an alternative form, #define this macro
                             to expand to the supported keyword
                             (eg. #define _DEC_STATIC_ASSERT __static_assert).
                             If your compiler supports neither form, leave
                             this macro undefined.
*/

/*************************************************************************
  Configuration macros:
  You may wish to add configuration #definitions here for your C++ compiler
  and environment.
*************************************************************************/

//**** Configuration for GNU g++ on AIX and LinuxPPC:

#if defined(__GNUC__) && defined(__powerpc__) && defined(__BIG_ENDIAN__)

#  if defined(__LONGDOUBLE128)
#    define _DEC_LONGDOUBLE128PPC 1
#  else
#    define _DEC_LONGDOUBLE64 1
#  endif /* defined(__LONGDOUBLE128) */

#  define _DEC_BIG_ENDIAN 1 
 
#  if !defined(LLONG_MIN)
#    define LLONG_MIN (-__LONG_LONG_MAX__ - 1)
#  endif /* !defined(LLONG_MIN) */
 
#  if !defined(LLONG_MAX)
#    define LLONG_MAX (__LONG_LONG_MAX__)
#  endif /* !defined(LLONG_MAX) */

#  define _DEC_HAS_ISINF  1
#  define _DEC_HAS_ISNAN  1
#  define _DEC_HAS_SFINAE 1

#  if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
#  define _DEC_HAS_POWL   1
#  endif /* (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) */

#endif /* defined(__GNUC__) && defined(__powerpc__) &&defined(__BIG_ENDIAN__) */

//**** Configuration for GNU g++ on x86
#if defined(__GNUC__) && (defined(__i386) || defined(__x86_64))

#  define _DEC_LONGDOUBLE80  1
#  define _DEC_LITTLE_ENDIAN 1
#  define _DEC_HAS_ISINF     1
#  define _DEC_HAS_ISNAN     1
#  define _DEC_HAS_SFINAE    1

#  if !defined(LLONG_MIN)
#    define LLONG_MIN (-__LONG_LONG_MAX__ - 1)
#  endif /* !defined(LLONG_MIN) */
 
#  if !defined(LLONG_MAX)
#    define LLONG_MAX (__LONG_LONG_MAX__)
#  endif /* !defined(LLONG_MAX) */

#if defined(__MINGW32__)
#  define _DEC_HAS_POWL      1
#else
#  if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1))
#  define _DEC_HAS_POWL   1
#  endif /* (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) */
#endif /* defined(__MINGW__) */

#endif /* defined(__GNUC__) && defined(__i386) */

//**** Configuration for IBM xlC++ on AIX and Z/OS:

#if defined(__IBMCPP__)

#  if defined(_AIX)
#    define _DEC_HAS_C99_MATH 1
#  endif /* defined(_AIX) */

#  if (__IBMCPP__ >= 800) && !defined(OS390) && !defined(_DEC_HIDE_TR1)
#    define _DEC_HAS_TYPE_TRAITS 1
#    if !(defined __IBMCPP_TR1__)
#      define __IBMCPP_TR1__ 1
#    endif /* !(defined __IBMCPP_TR1__) */
#  endif /* (__IBMCPP__ >= 800) && !defined(OS390) */

#  if (__IBMCPP__ >= 700)
#    define _DEC_STATIC_ASSERT __static_assert
#  endif /* (__IBMCPP__ >= 700) */

#  define _DEC_BIG_ENDIAN 1

#  if defined(__LONGDOUBLE128)
#    define _DEC_LONGDOUBLE128PPC 1
#  else
#    define _DEC_LONGDOUBLE64 1
#  endif /* defined(__LONGDOUBLE128) */

#  if !defined(_LONG_LONG)
#    error "Please enable support for the long long int data type."
#  endif /* defined(_LONG_LONG) */

#  if !defined(LLONG_MIN)
#    define LLONG_MIN (LONGLONG_MIN)
#  endif /* !defined(LLONG_MIN) */
 
#  if !defined(LLONG_MAX)
#    define LLONG_MAX (LONGLONG_MAX)
#  endif /* !defined(LLONG_MAX) */

#  if defined(OS390) && defined(_XOPEN_SOURCE_EXTENDED)
#    define _DEC_TARGET_OS390 1
#  endif /* defined(OS390) */

#  if defined(_AIX)
#    if !defined(__LONGDOUBLE128)
#      define _DEC_HAS_ISINF 1
#      define _DEC_HAS_ISNAN 1
#    else
#      define _DEC_HAS_POWL  1
#    endif /* !defined(__LONGDOUBLE128) */
#  endif /* _AIX */

#  define _DEC_HAS_SFINAE 1

#endif /* defined(__IBMCPP__) */

//**** Configuration for Microsoft Visual C++ 2005:

#if defined(_MSC_VER)
#  define _DEC_LITTLE_ENDIAN 1
#  define _DEC_LONGDOUBLE64  1
#  define _DEC_HAS_POWL      1
#endif /* defined(_MSC_VER) */


/*************************************************************************
  Configuration code:
  Do not modify the code below this line.
*************************************************************************/

// Handle _DEC_STATIC_ASSERT
#if defined(_DEC_STATIC_ASSERT)
#  define _DEC_NO_MIXED_RADIX \
        _DEC_STATIC_ASSERT(false, "Mixed-radix arithmetic is not permitted.");
#else
void _ERROR_DUE_TO_MIXED_RADIX_ARITHMETIC();
#  define _DEC_NO_MIXED_RADIX \
        _ERROR_DUE_TO_MIXED_RADIX_ARITHMETIC();
#endif /* defined(_DEC_STATIC_ASSERT) */

#if defined(_DEC_HAS_POWL)
#  define _DEC_POWL(_X, _Y) powl(_X, _Y)
#else
#  define _DEC_POWL(_X, _Y) pow(_X, _Y)
#endif /* _DEC_HAS_POWL */

// Handle _DEC_HAS_TYPE_TRAITS:

#ifdef _DEC_HAS_TYPE_TRAITS
#  include <tr1/type_traits>
#else

namespace std {
namespace tr1 {

template <class _T>
struct _Is_floating_point_aux {
        static const bool value = false;
};

template <>
struct _Is_floating_point_aux<float> {
        static const bool value = true;
};

template <>
struct _Is_floating_point_aux<double> {
        static const bool value = true;
};

template <>
struct _Is_floating_point_aux<long double> {
        static const bool value = true;
};

template <class _T>
struct _Is_signed_aux {
        static const bool value = false;
};

template <>
struct _Is_signed_aux<signed char> {
        static const bool value = true;
};

template <>
struct _Is_signed_aux<short> {
        static const bool value = true;
};

template <>
struct _Is_signed_aux<int> {
        static const bool value = true;
};

template <>
struct _Is_signed_aux<long> {
        static const bool value = true;
};

template <>
struct _Is_signed_aux<long long> {
        static const bool value = true;
};

template <class _T>
struct _Is_unsigned_aux {
        static const bool value = false;
};

template <>
struct _Is_unsigned_aux<unsigned char> {
        static const bool value = true;
};

template <>
struct _Is_unsigned_aux<unsigned short> {
        static const bool value = true;
};

template <>
struct _Is_unsigned_aux<unsigned int> {
        static const bool value = true;
};

template <>
struct _Is_unsigned_aux<unsigned long> {
        static const bool value = true;
};

template <>
struct _Is_unsigned_aux<unsigned long long> {
        static const bool value = true;
};

template <class _T, class _U>
struct _Is_same_aux {
        static const bool value = false;
};

template <class _T>
struct _Is_same_aux<_T, _T> {
        static const bool value = true;
};

template <class _T>
struct _Is_integral_aux {
        static const bool value =
                _Is_signed_aux<_T>::value ||
                _Is_unsigned_aux<_T>::value ||
                _Is_same_aux<_T, bool>::value ||
                _Is_same_aux<_T, char>::value ||
                _Is_same_aux<_T, wchar_t>::value;
};

template <class _T,
          _T _V>
struct integral_constant {
        static const _T value = _V;
        typedef _T value_type;
        typedef integral_constant<_T, _V> type;
};

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

// transformation trait remove_const
//
template <class _T>
struct remove_const {
        typedef _T type;
};

template <class _T>
struct remove_const<_T const> {
        typedef _T type;
};

// transformation trait remove_volatile
//
template <class _T>
struct remove_volatile {
        typedef _T type;
};

template <class _T>
struct remove_volatile<_T volatile> {
        typedef _T type;
};

// transformation trait remove_cv
//
template <class _T>
struct remove_cv {
        typedef typename
                remove_const<typename remove_volatile<_T>::type>::type
                type;
};

// type trait is_integral 
//
template <class _T>
struct is_integral
        : integral_constant<bool,
          _Is_integral_aux<typename remove_cv<_T>::type>::value > {
        typedef bool value_type;
        typedef integral_constant<value_type, 
                _Is_integral_aux<typename remove_cv<_T>::type>::value> type;
        operator type() const { return type(); }
};

// type trait is_floating_point
//
template <class _T>
struct is_floating_point
        : integral_constant<bool,
          _Is_floating_point_aux<typename remove_cv<_T>::type>::value >{
        typedef bool value_type;
        typedef integral_constant<value_type,
                _Is_floating_point_aux<typename remove_cv<_T>::type>::value>
                type;
        operator type() const { return type(); }
};

} // namespace tr1
} // namespace std

#endif /* ndef _DEC_HAS_TYPE_TRAITS */
#endif /* ndef _DECCONFIG_H_ */
