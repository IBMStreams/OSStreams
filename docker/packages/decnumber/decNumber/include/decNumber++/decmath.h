/* ------------------------------------------------------------------ */
/* decmath.h header                                                   */
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

#ifndef _DECMATH_H_
#define _DECMATH_H_

#include "cdecmath"

using std::decimal::decimal32;
using std::decimal::decimal64;
using std::decimal::decimal128;

using std::decimal::abs;
using std::decimal::fabsd32;
using std::decimal::fabsd64;
using std::decimal::fabsd128;

using std::decimal::ceil;
using std::decimal::ceild32;
using std::decimal::ceild64;
using std::decimal::ceild128;

using std::decimal::exp;
using std::decimal::expd32;
using std::decimal::expd64;
using std::decimal::expd128;

using std::decimal::floor;
using std::decimal::floord32;
using std::decimal::floord64;
using std::decimal::floord128;

using std::decimal::llrint;
using std::decimal::llrintd32;
using std::decimal::llrintd64;
using std::decimal::llrintd128;

using std::decimal::llround;
using std::decimal::llroundd32;
using std::decimal::llroundd64;
using std::decimal::llroundd128;

using std::decimal::log;
using std::decimal::logd32;
using std::decimal::logd64;
using std::decimal::logd128;

using std::decimal::log10;
using std::decimal::log10d32;
using std::decimal::log10d64;
using std::decimal::log10d128;

#if !defined(_DEC_TARGET_OS390)
using std::decimal::logb;
#endif /* !defined(_DEC_TARGET_OS390) */
using std::decimal::logbd32;
using std::decimal::logbd64;
using std::decimal::logbd128;

using std::decimal::lrint;
using std::decimal::lrintd32;
using std::decimal::lrintd64;
using std::decimal::lrintd128;

using std::decimal::lround;
using std::decimal::lroundd32;
using std::decimal::lroundd64;
using std::decimal::lroundd128;

using std::decimal::nearbyint;
using std::decimal::nearbyintd32;
using std::decimal::nearbyintd64;
using std::decimal::nearbyintd128;

#if !defined(_DEC_TARGET_OS390)
using std::decimal::rint;
#endif /* !defined(_DEC_TARGET_OS390) */
using std::decimal::rintd32;
using std::decimal::rintd64;
using std::decimal::rintd128;

using std::decimal::round;
using std::decimal::roundd32;
using std::decimal::roundd64;
using std::decimal::roundd128;

using std::decimal::sqrt;
using std::decimal::sqrtd32;
using std::decimal::sqrtd64;
using std::decimal::sqrtd128;

using std::decimal::trunc;
using std::decimal::truncd32;
using std::decimal::truncd64;
using std::decimal::truncd128;

using std::decimal::fmod;
using std::decimal::fmodd32;
using std::decimal::fmodd64;
using std::decimal::fmodd128;

using std::decimal::fmax;
using std::decimal::fmaxd32;
using std::decimal::fmaxd64;
using std::decimal::fmaxd128;

using std::decimal::fmin;
using std::decimal::fmind32;
using std::decimal::fmind64;
using std::decimal::fmind128;

using std::decimal::pow;
using std::decimal::powd32;
using std::decimal::powd64;
using std::decimal::powd128;

using std::decimal::quantize;
using std::decimal::quantized32;
using std::decimal::quantized64;
using std::decimal::quantized128;

#if !defined(_DEC_TARGET_OS390)
using std::decimal::remainder;
#endif /* !defined(_DEC_TARGET_OS390) */
using std::decimal::remainderd32;
using std::decimal::remainderd64;
using std::decimal::remainderd128;

using std::decimal::samequantum;
using std::decimal::samequantumd32;
using std::decimal::samequantumd64;
using std::decimal::samequantumd128;

using std::decimal::strtod32;
using std::decimal::strtod64;
using std::decimal::strtod128;

using std::decimal::wcstod32;
using std::decimal::wcstod64;
using std::decimal::wcstod128;

#ifdef _DEC_HAS_TR1_MATH
using std::tr1::fpclassify;
using std::tr1::isfinite;
using std::tr1::isinf;
using std::tr1::isnan;
using std::tr1::isnormal;
using std::tr1::signbit;

using std::tr1::isgreater;
using std::tr1::isgreaterequal;
using std::tr1::isless;
using std::tr1::islessequal;
using std::tr1::islessgreater;
using std::tr1::isunordered;
#endif /* _DEC_HAS_TR1_MATH */

#endif /* ndef _DECMATH_H_ */
