/* ------------------------------------------------------------------ */
/* decfloat.h header -- dfp additions to standard header <float.h>    */
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

#ifndef _DECFLOAT_H_
#define _DECFLOAT_H_

#include "cdecfloat"

typedef std::decimal::decimal32  _Decimal32;
typedef std::decimal::decimal64  _Decimal64;
typedef std::decimal::decimal128 _Decimal128;

using std::decimal::decimal32;
using std::decimal::decimal64;
using std::decimal::decimal128;

using std::decimal::fe_dec_getround;
using std::decimal::fe_dec_setround;

#endif /* ndef _DECFLOAT_H_ */
