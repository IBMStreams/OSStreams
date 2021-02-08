/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_MATH_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_MATH_FUNCTIONS_H

/*!
 * \file MathFunctions.h \brief Definitions of C++ counterparts of %SPL native functions for
 * math operations.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#endif /* DOXYGEN_SKIP_FOR_USERS */

#include <SPL/Runtime/Type/SPLType.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <cmath>
// We need to pre-declare these to avoid ambiguous call defintions in C++11. Since
// these pre-declarations are valid in non-C++11 code, it's simpler to not wrap this
// with C++11-only macros.
namespace ibm {
namespace decimal {
class decimal32;
class decimal64;
class decimal128;

decimal32 rint(decimal32 _Rhs);
decimal64 rint(decimal64 _Rhs);
decimal128 rint(decimal128 _Rhs);

decimal32 round(decimal32 _Rhs);
decimal64 round(decimal64 _Rhs);
decimal128 round(decimal128 _Rhs);
}
}
#include <cdecmath>
#endif /* DOXYGEN_SKIP_FOR_USERS */

/*
 * Builtin Math functions
 */
/// @splcppns SPL::Functions::Math
/// @spldir math
/// @splname SPL-Math-Functions

/*!
 * \namespace SPL::Functions::Math \brief C++ counterparts of %SPL native
 * functions that deal with math operations
 */

namespace SPL {
namespace Functions {
namespace Math {

/*
 * Builtin functions to generate NaNs
 */

/// Return a float32 NaN (Not A Number).
/// @return NaN value as float32.
/// @splnative public float32 nanw ()
/// @spleval $name = "nanw"; $ret = 'float32'; $numArgs = 0;
inline SPL::float32 nanw()
{
    return NAN;
}

/// Return a float64 NaN (Not A Number).
/// @return NaN value as float64.
/// @splnative public float64 nanl ()
/// @spleval $name = "nanl"; $ret = 'float64'; $numArgs = 0;
inline SPL::float64 nanl()
{
    return NAN;
}

/*
 * Builtin functions to generate infinities
 */

/// Return a float32 infinity.
/// @return Infinity value as float32.
/// @splnative public float32 infw ()
/// @spleval $name = "infw"; $ret = 'float32'; $numArgs = 0;
inline SPL::float32 infw()
{
    return INFINITY;
}

/// Return a float64 infinity.
/// @return Infinity value as float64.
/// @splnative public float64 infl ()
/// @spleval $name = "infl"; $ret = 'float64'; $numArgs = 0;
inline SPL::float64 infl()
{
    return INFINITY;
}

/*
 * Access functions for complex numbers
 */

/// Return the 'real' part of a complex number.
/// @param c Complex number input.
/// @return 'Real' part of complex number.
/// @splnative public float32 real (complex32 c)
/// @spleval $name = "real"; $ret = 'float32'; $numArgs = 1; $arg[0] = 'complex32'
/// @splnative public float64 real (complex64 c)
/// @spleval $name = "real"; $ret = 'float64'; $numArgs = 1; $arg[0] = 'complex64'
inline SPL::float32 real(const SPL::complex32& c)
{
    return c.real();
}

/// Return the 'real' part of a complex number.
/// @param c Complex number input.
/// @return 'Real' part of complex number.
inline SPL::float64 real(const SPL::complex64& c)
{
    return c.real();
}

/// Return the 'imaginary' part of a complex number.
/// @param c Complex number input.
/// @return 'Imaginary' part of complex number.
/// @splnative public float32 imag (complex32 c)
/// @spleval $name = "imag"; $ret = 'float32'; $numArgs = 1; $arg[0] = 'complex32'
/// @splnative public float64 imag (complex64 c)
/// @spleval $name = "imag"; $ret = 'float64'; $numArgs = 1; $arg[0] = 'complex64'
inline SPL::float32 imag(const SPL::complex32& c)
{
    return c.imag();
}

/// Return the 'imaginary' part of a complex number.
/// @param c Complex number input.
/// @return 'Imaginary' part of complex number.
inline SPL::float64 imag(const SPL::complex64& c)
{
    return c.imag();
}

/// Return whether the floating point value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if the value is a NaN, 'false' otherwise.
/// @splnative <floatingpoint T> public boolean isNaN (T f)
/// @spleval $name = "isNaN"; $ret = 'boolean'; $numArgs = 1; $arg[0] = '@floatingpoint@'
inline SPL::boolean isNaN(const SPL::float32 f)
{
    return ::isnan(f);
}

/// Return whether the floating point value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if the value is a NaN, 'false' otherwise.
inline SPL::boolean isNaN(const SPL::float64 f)
{
    return ::isnan(f);
}

/// Return whether the floating point value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if the value is a NaN, 'false' otherwise.
inline SPL::boolean isNaN(const SPL::decimal32 f)
{
    return ibm::decimal::_DecNumber(f)._IsNAN();
}

/// Return whether the floating point value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if the value is a NaN, 'false' otherwise.
inline SPL::boolean isNaN(const SPL::decimal64 f)
{
    return ibm::decimal::_DecNumber(f)._IsNAN();
}

/// Return whether the floating point value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if the value is a NaN, 'false' otherwise.
inline SPL::boolean isNaN(const SPL::decimal128 f)
{
    return ibm::decimal::_DecNumber(f)._IsNAN();
}

/// Return whether the complex value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if either the real or imaginary component is a NaN, 'false' otherwise.
/// @splnative <complex T> public boolean isNaN (T f)
/// @spleval $name = "isNaN"; $ret = 'boolean'; $numArgs = 1; $arg[0] = '@complex@'
inline SPL::boolean isNaN(const SPL::complex32 f)
{
    return ::isnan(f.real()) || ::isnan(f.imag());
}

/// Return whether the complex value is a NaN (Not A Number).
/// @param f Input value.
/// @return The result 'true' if either the real or imaginary component is a NaN, 'false' otherwise.
inline SPL::boolean isNaN(const SPL::complex64 f)
{
    return ::isnan(f.real()) || ::isnan(f.imag());
}

/// Return whether the floating point value is an infinity.
/// @param f Input value.
/// @return The result 'true' if the value is an infinity, 'false' otherwise.
/// @splnative <floatingpoint T> public boolean isInf (T f)
/// @spleval $name = "isInf"; $ret = 'boolean'; $numArgs = 1; $arg[0] = '@floatingpoint@'
inline SPL::boolean isInf(const SPL::float32 f)
{
    return ::isinf(f);
}

/// Return whether the floating point value is an infinity.
/// @param f Input value.
/// @return The result 'true' if the value is an infinity, 'false' otherwise.
inline SPL::boolean isInf(const SPL::float64 f)
{
    return ::isinf(f);
}

/// Return whether the floating point value is an infinity.
/// @param f Input value.
/// @return The result 'true' if the value is an infinity, 'false' otherwise.
inline SPL::boolean isInf(const SPL::decimal32 f)
{
    return ibm::decimal::_DecNumber(f)._IsINF();
}

/// Return whether the floating point value is an infinity.
/// @param f Input value.
/// @return The result 'true' if the value is an infinity, 'false' otherwise.
inline SPL::boolean isInf(const SPL::decimal64 f)
{
    return ibm::decimal::_DecNumber(f)._IsINF();
}

/// Return whether the floating point value is an infinity.
/// @param f Input value.
/// @return The result 'true' if the value is an infinity, 'false' otherwise.
inline SPL::boolean isInf(const SPL::decimal128 f)
{
    return ibm::decimal::_DecNumber(f)._IsINF();
}

/// Return whether the complex value is an infinity.
/// @param f Input value.
/// @return The result 'true' if either the real or imaginary component is an infinite value, 'false' otherwise.
/// @splnative <complex T> public boolean isInf (T f)
/// @spleval $name = "isInf"; $ret = 'boolean'; $numArgs = 1; $arg[0] = '@complex@'
inline SPL::boolean isInf(const SPL::complex32 f)
{
    return ::isinf(f.real()) || ::isinf(f.imag());
}

/// Return whether the complex value is an infinity.
/// @param f Input value.
/// @return The result 'true' if either the real or imaginary component is an infinite value, 'false' otherwise.
inline SPL::boolean isInf(const SPL::complex64 f)
{
    return ::isinf(f.real()) || ::isinf(f.imag());
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
/// @splnative <numeric T> public T sqrt (T f)
/// @spleval $name = "sqrt"; $ret = '@numeric@'; $numArgs = 1; $arg[0] = '@numeric@'
inline SPL::uint8 sqrt(const SPL::uint8 f)
{
    return static_cast<uint8_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::uint16 sqrt(const SPL::uint16 f)
{
    return static_cast<uint16_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::uint32 sqrt(const SPL::uint32 f)
{
    return static_cast<uint32_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::uint64 sqrt(const SPL::uint64 f)
{
    return static_cast<uint64_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::int8 sqrt(const SPL::int8 f)
{
    return static_cast<int8_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::int16 sqrt(const SPL::int16 f)
{
    return static_cast<int16_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::int32 sqrt(const SPL::int32 f)
{
    return static_cast<int32_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::int64 sqrt(const SPL::int64 f)
{
    return static_cast<int64_t>(::sqrt(static_cast<double>(f)));
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::float32 sqrt(const SPL::float32 f)
{
    return ::sqrtf(f);
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::float64 sqrt(const SPL::float64 f)
{
    return ::sqrt(f);
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::decimal32 sqrt(const SPL::decimal32 f)
{
    return ibm::decimal::sqrt(f);
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::decimal64 sqrt(const SPL::decimal64 f)
{
    return ibm::decimal::sqrt(f);
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::decimal128 sqrt(const SPL::decimal128 f)
{
    return ibm::decimal::sqrt(f);
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::complex32 sqrt(const SPL::complex32 f)
{
    return std::sqrt(f);
}

/// Compute the square root.
/// @param f Input value.
/// @return Square root of the input value.
inline SPL::complex64 sqrt(const SPL::complex64 f)
{
    return std::sqrt(f);
}

/// Compute the sine.
/// @param f Input value.
/// @return Sine of the input value.
/// @splnative <float T> public T sin (T f)
/// @spleval $name = "sin"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
inline SPL::float32 sin(const SPL::float32 f)
{
    return ::sinf(f);
}

/// Compute the sine.
/// @param f Input value.
/// @return Sine of the input value.
inline SPL::float64 sin(const SPL::float64 f)
{
    return ::sin(f);
}

/// Compute the cosine.
/// @param f Input value.
/// @return Cosine of the input value.
/// @splnative <float T> public T cos (T f)
/// @spleval $name = "cos"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
inline SPL::float32 cos(const SPL::float32 f)
{
    return ::cosf(f);
}

/// Compute the cosine.
/// @param f Input value.
/// @return Cosine of the input value.
inline SPL::float64 cos(const SPL::float64 f)
{
    return ::cos(f);
}

/// Compute the tangent.
/// @param f Input value.
/// @return Tangent of the input value.
/// @splnative <float T> public T tan (T f)
/// @spleval $name = "tan"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
inline SPL::float32 tan(const SPL::float32 f)
{
    return ::tanf(f);
}

/// Compute the tangent.
/// @param f Input value.
/// @return Tangent of the input value.
inline SPL::float64 tan(const SPL::float64 f)
{
    return ::tan(f);
}

/// Compute the arcsine.
/// @param f Input value.
/// @return Arcsine of the input value.
/// @splnative <float T> public T asin (T f)
/// @spleval $name = "asin"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
inline SPL::float32 asin(const SPL::float32 f)
{
    return ::asinf(f);
}

/// Compute the arcsine.
/// @param f Input value.
/// @return Arcsine of the input value.
inline SPL::float64 asin(const SPL::float64 f)
{
    return ::asin(f);
}

/// Compute the arc-cosine.
/// @param f Input value.
/// @return Arccosine of the input value.
/// @splnative <float T> public T acos (T f)
/// @spleval $name = "acos"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
inline SPL::float32 acos(const SPL::float32 f)
{
    return ::acosf(f);
}

/// Compute the arc-cosine.
/// @param f Input value.
/// @return Arccosine of the input value.
inline SPL::float64 acos(const SPL::float64 f)
{
    return ::acos(f);
}

/// Compute the arc-tangent.
/// @param f Input value.
/// @return Arctangent of the input value.
/// @splnative <float T> public T atan (T f)
/// @spleval $name = "atan"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
inline SPL::float32 atan(const SPL::float32 f)
{
    return ::atanf(f);
}

/// Compute the arc-tangent.
/// @param f Input value.
/// @return Arctangent of the input value.
inline SPL::float64 atan(const SPL::float64 f)
{
    return ::atan(f);
}

/// Value of PI.
/// @return PI value.
/// @splnative public float64 PI()
/// @spleval $name = "PI"; $ret = "float64"; $numArgs = 0;
inline SPL::float64 PI()
{
    return M_PI;
}

/// Seed the random number generator.
/// @param seed Value to be set for random number seed.
/// @return 0 on success.
/// @splnative public stateful int32 srand (int32 seed)
inline SPL::int32 srand(const SPL::int32 seed)
{
    ::srand(seed);
    return 0;
}

/// Return a random number.
/// @return A random value in the range 0 to 1.
/// @splnative public stateful float64 random()
inline SPL::float64 random()
{
    return rand() / (RAND_MAX + 1.0);
}

/// Return a list of random numbers.
/// @param cnt Number of random values to return.
/// @return A list of random values each in the range 0 to 1.
/// @splnative public stateful list<float64> random(int32 cnt)
inline SPL::list<SPL::float64> random(const SPL::int32 cnt)
{
    SPL::list<SPL::float64> res;
    if (cnt > 0) {
        res.reserve(cnt);
        for (int i = 0; i < cnt; i++) {
            res.push_back(random());
        }
    }
    return res;
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
/// @splnative <floatingpoint T> public T pow (T val, T pwr)
/// @spleval $name = "pow"; $ret = '@floatingpoint@'; $numArgs = 2; $arg[0] = '@floatingpoint@'; $arg[1] = '@floatingpoint@';
/// @splnative <integral T> public T pow (T val, T pwr)
/// @spleval $name = "pow"; $ret = '@integer@'; $numArgs = 2; $arg[0] = '@integer@'; $arg[1] = '@integer@';
/// @splnative <complex T> public T pow (T val, T pwr)
/// @spleval $name = "pow"; $ret = '@complex@'; $numArgs = 2; $arg[0] = '@complex@'; $arg[1] = '@complex@';
inline SPL::int8 pow(const SPL::int8 val, const SPL::int8 pwr)
{
    return static_cast<int8_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::int16 pow(const SPL::int16 val, const SPL::int16 pwr)
{
    return static_cast<int16_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::int32 pow(const SPL::int32 val, const SPL::int32 pwr)
{
    return static_cast<int32_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::int64 pow(const SPL::int64 val, const SPL::int64 pwr)
{
    return static_cast<int64_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::uint8 pow(const SPL::uint8 val, const SPL::uint8 pwr)
{
    return static_cast<uint8_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::uint16 pow(const SPL::uint16 val, const SPL::uint16 pwr)
{
    return static_cast<uint16_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::uint32 pow(const SPL::uint32 val, const SPL::uint32 pwr)
{
    return static_cast<uint32_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::uint64 pow(const SPL::uint64 val, const SPL::uint64 pwr)
{
    return static_cast<uint32_t>(::pow(static_cast<double>(val), static_cast<double>(pwr)));
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::float32 pow(const SPL::float32 val, const SPL::float32 pwr)
{
    return ::powf(val, pwr);
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::float64 pow(const SPL::float64 val, const SPL::float64 pwr)
{
    return ::pow(val, pwr);
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::decimal32 pow(const SPL::decimal32 val, const SPL::decimal32 pwr)
{
    return ibm::decimal::pow(val, pwr);
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::decimal64 pow(const SPL::decimal64 val, const SPL::decimal64 pwr)
{
    return ibm::decimal::pow(val, pwr);
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::decimal128 pow(const SPL::decimal128 val, const SPL::decimal128 pwr)
{
    return ibm::decimal::pow(val, pwr);
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::complex32 pow(const SPL::complex32 val, const SPL::complex32 pwr)
{
    return std::pow(val, pwr);
}

/// Perform exponentiation.
/// @param val Input value.
/// @param pwr Exponent value.
/// @return The input value raised to the power of the exponent.
inline SPL::complex64 pow(const SPL::complex64 val, const SPL::complex64 pwr)
{
    return std::pow(val, pwr);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
/// @splnative <floatingpoint T> public T exp (T val)
/// @spleval $name = "exp"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@';
/// @splnative <complex T> public T exp (T val)
/// @spleval $name = "exp"; $ret = '@complex@'; $numArgs = 1; $arg[0] = '@complex@';
inline SPL::float32 exp(const SPL::float32 val)
{
    return ::expf(val);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
inline SPL::float64 exp(const SPL::float64 val)
{
    return ::exp(val);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
inline SPL::decimal32 exp(const SPL::decimal32 val)
{
    return ibm::decimal::exp(val);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
inline SPL::decimal64 exp(const SPL::decimal64 val)
{
    return ibm::decimal::exp(val);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
inline SPL::decimal128 exp(const SPL::decimal128 val)
{
    return ibm::decimal::exp(val);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
inline SPL::complex32 exp(const SPL::complex32 val)
{
    return std::exp(val);
}

/// Compute e to a power.
/// @param val Input value.
/// @return The base of natural logarithms, e, raised to the power given by the input value.
inline SPL::complex64 exp(const SPL::complex64 val)
{
    return std::exp(val);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
/// @splnative <floatingpoint T> public T log (T f)
/// @spleval $name = "log"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
/// @splnative <complex T> public T log (T f)
/// @spleval $name = "log"; $ret = '@complex@'; $numArgs = 1; $arg[0] = '@complex@'
/// @splcppfcn log
/// @splnative <floatingpoint T> public T ln (T f)
/// @spleval $name = "log"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
/// @splcppfcn log
/// @splnative <complex T> public T ln (T f)
/// @spleval $name = "log"; $ret = '@complex@'; $numArgs = 1; $arg[0] = '@complex@'
inline SPL::float32 log(const SPL::float32 f)
{
    return ::logf(f);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
inline SPL::float64 log(const SPL::float64 f)
{
    return ::log(f);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
inline SPL::decimal32 log(const SPL::decimal32 f)
{
    return ibm::decimal::log(f);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
inline SPL::decimal64 log(const SPL::decimal64 f)
{
    return ibm::decimal::log(f);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
inline SPL::decimal128 log(const SPL::decimal128 f)
{
    return ibm::decimal::log(f);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
inline SPL::complex32 log(const SPL::complex32 f)
{
    return std::log(f);
}

/// Compute log base e.
/// @param f Input value.
/// @return Natural logarithm of the input value.
inline SPL::complex64 log(const SPL::complex64 f)
{
    return std::log(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
/// @splnative <floatingpoint T> public T log10 (T f)
/// @spleval $name = "log10"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
/// @splnative <complex T> public T log10 (T f)
/// @spleval $name = "log10"; $ret = '@complex@'; $numArgs = 1; $arg[0] = '@complex@'
/// @splcppfcn log10
/// @splnative <floatingpoint T> public T lg (T f)
/// @spleval $name = "log10"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
/// @splcppfcn log10
/// @splnative <complex T> public T lg (T f)
/// @spleval $name = "log10"; $ret = '@complex@'; $numArgs = 1; $arg[0] = '@complex@'
inline SPL::float32 log10(const SPL::float32 f)
{
    return ::log10f(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
inline SPL::float64 log10(const SPL::float64 f)
{
    return ::log10(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
inline SPL::decimal32 log10(const SPL::decimal32 f)
{
    return ibm::decimal::log10(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
inline SPL::decimal64 log10(const SPL::decimal64 f)
{
    return ibm::decimal::log10(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
inline SPL::decimal128 log10(const SPL::decimal128 f)
{
    return ibm::decimal::log10(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
inline SPL::complex32 log10(const SPL::complex32 f)
{
    return std::log10(f);
}

/// Compute log base 10.
/// @param f Input value.
/// @return Base 10 logarithm of the input value.
inline SPL::complex64 log10(const SPL::complex64 f)
{
    return std::log10(f);
}

/// Compute the smallest integer value equal to or greater than a value.
/// @param f Input value.
/// @return Smallest integer >= f.
/// @splnative <float T> public T ceil (T f)
/// @spleval $name = "ceil"; $ret = '@float@'; $numArgs = 1; $arg[0] = '@float@'
/// @splnative public decimal32 ceil (decimal32 f)
/// @spleval $name = "ceil"; $ret = 'decimal32'; $numArgs = 1; $arg[0] = 'decimal32'
/// @splnative public decimal64 ceil (decimal64 f)
/// @spleval $name = "ceil"; $ret = 'decimal64'; $numArgs = 1; $arg[0] = 'decimal64'
inline SPL::float32 ceil(const SPL::float32 f)
{
    return ::ceilf(f);
}

/// Compute the smallest integer value equal to or greater than a value.
/// @param f Input value.
/// @return Smallest integer >= f.
inline SPL::float64 ceil(const SPL::float64 f)
{
    return ::ceil(f);
}

/// Compute the smallest integer value equal to or greater than a value.
/// @param f Input value.
/// @return Smallest integer >= f.
inline SPL::decimal32 ceil(const SPL::decimal32 f)
{
    return ibm::decimal::ceil(f);
}

/// Compute the smallest integer value equal to or greater than a value.
/// @param f Input value.
/// @return Smallest integer >= f.
inline SPL::decimal64 ceil(const SPL::decimal64 f)
{
    return ibm::decimal::ceil(f);
}

/// Compute the smallest integer value equal to or greater than a value.
/// @param f Input value.
/// @return Smallest integer >= f.
inline SPL::decimal128 ceil(const SPL::decimal128 f)
{
    return ibm::decimal::ceil(f);
}

/// Compute the largest integer value equal to or less than a value.
/// @param f Input value.
/// @return Largest integer <= f.
/// @splnative <floatingpoint T> public T floor (T f)
/// @spleval $name = "floor"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
inline SPL::float32 floor(const SPL::float32 f)
{
    return ::floorf(f);
}

/// Compute the largest integer value equal to or less than a value.
/// @param f Input value.
/// @return Largest integer <= f.
inline SPL::float64 floor(const SPL::float64 f)
{
    return ::floor(f);
}

/// Compute the largest integer value equal to or less than a value.
/// @param f Input value.
/// @return Largest integer <= f.
inline SPL::decimal32 floor(const SPL::decimal32 f)
{
    return ibm::decimal::floor(f);
}

/// Compute the largest integer value equal to or less than a value.
/// @param f Input value.
/// @return Largest integer <= f.
inline SPL::decimal64 floor(const SPL::decimal64 f)
{
    return ibm::decimal::floor(f);
}

/// Compute the largest integer value equal to or less than a value.
/// @param f Input value.
/// @return Largest integer <= f.
inline SPL::decimal128 floor(const SPL::decimal128 f)
{
    return ibm::decimal::floor(f);
}

/// Round a floating point value to the nearest integer.
/// @param f Input value.
/// @return The input value rounded to the nearest integer.
/// @splnative <floatingpoint T> public T round (T f)
/// @spleval $name = "round"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
inline SPL::float32 round(const SPL::float32 f)
{
    return ::roundf(f);
}

/// Round a floating point value to the nearest integer.
/// @param f Input value.
/// @return The input value rounded to the nearest integer.
inline SPL::float64 round(const SPL::float64 f)
{
    return ::round(f);
}

/// Round a floating point value to the nearest integer.
/// @param f Input value.
/// @return The input value rounded to the nearest integer.
inline SPL::decimal32 round(const SPL::decimal32 f)
{
    return ibm::decimal::round(f);
}

/// Round a floating point value to the nearest integer.
/// @param f Input value.
/// @return The input value rounded to the nearest integer.
inline SPL::decimal64 round(const SPL::decimal64 f)
{
    return ibm::decimal::round(f);
}

/// Round a floating point value to the nearest integer.
/// @param f Input value.
/// @return The input value rounded to the nearest integer.
inline SPL::decimal128 round(const SPL::decimal128 f)
{
    return ibm::decimal::round(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
/// @splnative <floatingpoint T> public T abs (T f)
/// @spleval $name = "abs"; $ret = '@floatingpoint@'; $numArgs = 1; $arg[0] = '@floatingpoint@'
/// @splnative <integral T> public T abs (T f)
/// @spleval $name = "abs"; $ret = '@integer@'; $numArgs = 1; $arg[0] = '@integer@'
/// @splnative public float32 abs (complex32 f)
/// @spleval $name = "abs"; $ret = 'float32'; $numArgs = 1; $arg[0] = 'complex32'
/// @splnative public float64 abs (complex64 f)
/// @spleval $name = "abs"; $ret = 'float64'; $numArgs = 1; $arg[0] = 'complex64'
inline SPL::uint8 abs(const SPL::uint8 f)
{
    return f;
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::uint16 abs(const SPL::uint16 f)
{
    return f;
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::uint32 abs(const SPL::uint32 f)
{
    return f;
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::uint64 abs(const SPL::uint64 f)
{
    return f;
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::int8 abs(const SPL::int8 f)
{
    return ::abs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::int16 abs(const SPL::int16 f)
{
    return ::abs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::int32 abs(const SPL::int32 f)
{
    return ::labs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::int64 abs(const SPL::int64 f)
{
    return ::llabs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::float32 abs(const SPL::float32 f)
{
    return ::fabsf(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::float64 abs(const SPL::float64 f)
{
    return ::fabs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::decimal32 abs(const SPL::decimal32 f)
{
    return ibm::decimal::abs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::decimal64 abs(const SPL::decimal64 f)
{
    return ibm::decimal::abs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::decimal128 abs(const SPL::decimal128 f)
{
    return ibm::decimal::abs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::float32 abs(const SPL::complex32 f)
{
    return std::abs(f);
}

/// Compute the absolute value.
/// @param f Input value.
/// @return Absolute value of f.
inline SPL::float64 abs(const SPL::complex64 f)
{
    return std::abs(f);
}

/// Compute the absolute value of a list.
/// @param f List of input values.
/// @return List of the absolute values of the elements of f.
/// @splnative <numeric T> public list<T> abs (list<T> f)
template<class T>
SPL::list<T> abs(const SPL::list<T>& f)
{
    SPL::list<T> result;
    result.reserve(f.size());
    for (size_t i = 0, iu = f.size(); i < iu; ++i) {
        result.push_back(static_cast<T>(abs(f[i])));
    }
    return result;
}

/// Compute the absolute value of a list.
/// @param f List of input values.
/// @return List of the absolute values of the elements of f.
/// @splnative <numeric T>[N] public list<T>[N] abs (list<T>[N] f)
template<class T, int32_t N>
SPL::blist<T, N> abs(const SPL::blist<T, N>& f)
{
    SPL::blist<T, N> result;
    for (size_t i = 0, iu = f.size(); i < iu; ++i) {
        result.push_back(static_cast<T>(abs(f[i])));
    }
    return result;
}

/// Compute the maximimum of two values.
/// @param v1 First value.
/// @param v2 Second value.
/// @return Maximum of v1 and v2.
/// @splnative <ordered T> public T max (T v1, T v2)
/// @spleval $name = "max"; $ret = '@ordered@'; $numArgs = 2; $arg[0] = '@ordered@'; $arg[1] = '@ordered@'
template<class T>
inline T max(const T& v1, const T& v2)
{
    return v1 >= v2 ? v1 : v2;
}

/// Find the maximum value in a list.
/// @param vals List of input values.
/// @return Maximum of the input values.
/// @splnative <ordered T> public T max (list<T> vals)
/// @spleval $name = "max"; $ret = '@ordered@'; $numArgs = 1; $arg[0] = 'list<@ordered@>'
template<class T>
inline T max(const SPL::list<T>& vals)
{
    if (vals.empty()) {
        return T();
    }
    T mval = vals[0];
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        if (vals[i] > mval) {
            mval = vals[i];
        }
    }
    return mval;
}

/// Find the maximum value in a list.
/// @param vals List of input values.
/// @return Maximum of the input values.
/// @splnative <ordered T>[N] public T max (list<T>[N] vals)
template<class T, int32_t N>
inline T max(const SPL::blist<T, N>& vals)
{
    if (vals.empty()) {
        return T();
    }
    T mval = vals[0];
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        if (vals[i] > mval) {
            mval = vals[i];
        }
    }
    return mval;
}

/// Compute the minimum of two values.
/// @param v1 First value.
/// @param v2 Second value.
/// @return Minimum of v1 and v2.
/// @splnative <ordered T> public T min (T v1, T v2)
/// @spleval $name = "min"; $ret = '@ordered@'; $numArgs = 2; $arg[0] = '@ordered@'; $arg[1] = '@ordered@'
template<class T>
inline T min(const T& v1, const T& v2)
{
    return v1 <= v2 ? v1 : v2;
}

/// Find the minimum value in a list.
/// @param vals List of input values.
/// @return Minimum of the input values.
/// @splnative <ordered T> public T min (list<T> vals)
/// @spleval $name = "min"; $ret = '@ordered@'; $numArgs = 1; $arg[0] = 'list<@ordered@>'
template<class T>
inline T min(const SPL::list<T>& vals)
{
    if (vals.empty()) {
        return T();
    }
    T mval = vals[0];
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        if (vals[i] < mval) {
            mval = vals[i];
        }
    }
    return mval;
}

/// Find the minimum value in a list.
/// @param vals List of input values.
/// @return Minimum of the input values.
/// @splnative <ordered T>[N] public T min (list<T>[N] vals)
template<class T, int32_t N>
inline T min(const SPL::blist<T, N>& vals)
{
    if (vals.empty()) {
        return T();
    }
    T mval = vals[0];
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        if (vals[i] < mval) {
            mval = vals[i];
        }
    }
    return mval;
}

/// Compute the sum of a list.
/// @param vals List of input values.
/// @return Sum of the input values.
/// @splnative <numeric T> public T sum (list<T> vals)
/// @spleval $name = "sum"; $ret = '@primitive@'; $numArgs = 1; $arg[0] = 'list<@primitive@>'
template<class T>
inline T sum(const SPL::list<T>& vals)
{
    if (vals.empty()) {
        return T();
    }
    T mval = vals[0];
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        mval += vals[i];
    }
    return mval;
}

/// Compute the sum of a list.
/// @param vals List of input values.
/// @return Sum of the input values.
/// @splnative <numeric T>[N] public T sum (list<T>[N] vals)
template<class T, int32_t N>
inline T sum(const SPL::blist<T, N>& vals)
{
    if (vals.empty()) {
        return T();
    }
    T mval = vals[0];
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        mval += vals[i];
    }
    return mval;
}

/// Compute the average of a list.
/// @param vals List of input values.
/// @return Average of the input values.
/// @splnative <numeric T> public T avg (list<T> vals)
/// @spleval $name = "avg"; $ret = '@numeric@'; $numArgs = 1; $arg[0] = 'list<@numeric@>'
template<class T>
inline T avg(const SPL::list<T>& vals)
{
    if (vals.empty()) {
        return T();
    }
    return sum(vals) / static_cast<T>(vals.size());
}

/// Compute the average of a list.
/// @param vals List of input values.
/// @return Average of the input values.
/// @splnative <numeric T>[N] public T avg (list<T>[N] vals)
template<class T, int32_t N>
inline T avg(const SPL::blist<T, N>& vals)
{
    if (vals.empty()) {
        return T();
    }
    return sum(vals) / static_cast<T>(vals.size());
}

/// Compute the kth element in ascending order.
/// @param vals List of input values.
/// @param k The desired element (indexing starts from 0).
/// @return The kth element in ascending order.
/// @throws SPLRuntimeInvalidIndexException If the index is out of bounds.
/// @splnative <ordered T> public T kthelem (list<T> vals, uint32 k)
/// @spleval $name = "kthelem"; $ret = '@ordered@'; $numArgs = 2; $arg[0] = 'list<@ordered@>'; $arg[1] = "uint32"
template<class T>
T kthelem(const SPL::list<T>& vals, const SPL::uint32 k)
{
    if (k >= vals.size()) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_KTHELEM_INDEX_INVALID(k, vals.size()),
                            SPL_FUNC_DBG);
    }
    SPL::list<T> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + k, cpy.end());
    return cpy[k];
}

/// Compute the kth element in ascending order.
/// @param vals List of input values.
/// @param k The desired element (indexing starts from 0).
/// @return The kth element in ascending order.
/// @throws SPLRuntimeInvalidIndexException If the index is out of bounds.
/// @splnative <ordered T>[N] public T kthelem (list<T>[N] vals, uint32 k)
template<class T, int32_t N>
T kthelem(const SPL::blist<T, N>& vals, const SPL::uint32 k)
{
    if (k >= vals.size()) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_KTHELEM_INDEX_INVALID(k, vals.size()),
                            SPL_FUNC_DBG);
    }
    SPL::list<T> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + k, cpy.end());
    return cpy[k];
}

/// Compute the median of a list for numeric types that have overloaded the
/// arithmetic operators. For a list with an odd number of elements, it returns
/// the median value. For a list with an even number of elements, it returns the
/// average of the two middlemost elements;
/// @param vals List of input values.
/// @return Median of the input values.
/// @splnative <ordered T> public T median (list<T> vals)
/// @spleval $name = "median"; $ret = '@ordered@'; $numArgs = 1; $arg[0] = 'list<@ordered@>'
template<class T>
inline T median(const SPL::list<T>& vals)
{
    if (vals.empty()) {
        return T();
    }
    int32_t size = vals.size();
    if (size == 1) {
        return vals[0];
    }
    SPL::list<T> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + (size / 2), cpy.end());
    T median = cpy[size / 2];
    if (size % 2 == 0) {
        std::nth_element(cpy.begin(), cpy.begin() + (size / 2) - 1, cpy.end());
        median += cpy[(size / 2) - 1];
        median /= static_cast<T>(2);
    }
    return median;
}

/// Compute the median of a list for numeric types that have overloaded the
/// arithmetic operators. For a list with an odd number of elements, it returns
/// the median value. For a list with an even number of elements, it returns the
/// average of the two middlemost elements;
/// @param vals List of input values.
/// @return Median of the input values.
/// @splnative <ordered T>[N] public T median (list<T>[N] vals)
template<class T, int32_t N>
inline T median(const SPL::blist<T, N>& vals)
{
    if (vals.empty()) {
        return T();
    }
    int32_t size = vals.size();
    if (size == 1) {
        return vals[0];
    }
    SPL::blist<T, N> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + (size / 2), cpy.end());
    T median = cpy[size / 2];
    if (size % 2 == 0) {
        std::nth_element(cpy.begin(), cpy.begin() + (size / 2) - 1, cpy.end());
        median += cpy[(size / 2) - 1];
        median /= static_cast<T>(2);
    }
    return median;
}

/// @brief Compute the median of a list for ustring type.
///
/// If the list has an even size N, then this function returns
/// the N/2th element of the list sorted in ascending order.
/// @param vals List of input values.
/// @return Median of the input values.
template<>
inline SPL::ustring median<SPL::ustring>(const SPL::list<SPL::ustring>& vals)
{
    if (vals.empty()) {
        return SPL::ustring();
    }
    int32_t size = vals.size();
    if (size == 1) {
        return vals[0];
    }
    SPL::list<SPL::ustring> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + (size / 2), cpy.end());
    return cpy[size / 2];
    ;
}

/// @brief Compute the median of a list for the timestamp type.
///
/// If the list has an even size N, then this function returns
/// the N/2th element of the list sorted in ascending order.
/// @param vals List of input values.
/// @return Median of the input values.
template<>
inline SPL::timestamp median<SPL::timestamp>(const SPL::list<SPL::timestamp>& vals)
{
    if (vals.empty()) {
        return SPL::timestamp();
    }
    int32_t size = vals.size();
    if (size == 1) {
        return vals[0];
    }
    SPL::list<SPL::timestamp> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + (size / 2), cpy.end());
    return cpy[size / 2];
    ;
}

/// @brief Compute the median of a list for rstring type.
///
/// If the list has an even size N, then this function returns
/// the N/2th element of the list sorted in ascending order.
/// @param vals List of input values.
/// @return Median of the input values.
template<>
inline SPL::rstring median<SPL::rstring>(const SPL::list<SPL::rstring>& vals)
{
    if (vals.empty()) {
        return SPL::rstring();
    }
    int32_t size = vals.size();
    if (size == 1) {
        return vals[0];
    }
    SPL::list<SPL::rstring> cpy = vals;
    std::nth_element(cpy.begin(), cpy.begin() + (size / 2), cpy.end());
    return cpy[size / 2];
    ;
}

/// Compute the standard deviation of a list.
/// @param vals List of input values.
/// @param sample Whether we want sample standard deviation, or population standard deviation ('true' means sample, 'false' means population).
/// @return Standard deviation of the input values.
/// @splnative <numeric T> public T stddev (list<T> vals, boolean sample)
/// @spleval $name = "stddev"; $ret = '@numeric@'; $numArgs = 2; $arg[0] = 'list<@numeric@>'; $arg[1] = "boolean"
/// @splnative <numeric T>[N] public T stddev (list<T>[N] vals, boolean sample)

/// @spldesc Compute the standard deviation of a list.
/// @splparam vals List of input values.
/// @splreturn Standard deviation of the input values.
/// @splnative <numeric T> public T stddev (list<T> vals)
/// @spleval $name = "stddev"; $ret = '@numeric@'; $numArgs = 1; $arg[0] = 'list<@numeric@>'
/// @splnative <numeric T>[N] public T stddev (list<T>[N] vals)
template<class T>
inline T stddev(const SPL::list<T>& vals, bool sample = false)
{
    if (vals.empty()) {
        return T();
    }
    T xmean = avg(vals);
    T xdiff = vals[0] - xmean;
    T x2sum = xdiff * xdiff;
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        xdiff = vals[i] - xmean;
        x2sum += xdiff * xdiff;
    }
    if (!sample) {
        return SPL::Functions::Math::sqrt(x2sum / static_cast<T>(vals.size()));
    } else {
        return SPL::Functions::Math::sqrt(x2sum / static_cast<T>(vals.size() - 1));
    }
}

/// Compute the standard deviation of a list.
/// @param vals List of input values.
/// @param sample Whether we want sample standard deviation, or population
/// standard deviation ('true' means sample, 'false' means population).
/// @return Standard deviation of the input values.
template<class T, int32_t N>
inline T stddev(const SPL::blist<T, N>& vals, bool sample = false)
{
    if (vals.empty()) {
        return T();
    }
    T xmean = avg(vals);
    T xdiff = vals[0] - xmean;
    T x2sum = xdiff * xdiff;
    for (size_t i = 1, ui = vals.size(); i < ui; ++i) {
        xdiff = vals[i] - xmean;
        x2sum += xdiff * xdiff;
    }
    if (!sample) {
        return SPL::Functions::Math::sqrt(x2sum / static_cast<T>(vals.size()));
    } else {
        return SPL::Functions::Math::sqrt(x2sum / static_cast<T>(vals.size() - 1));
    }
}

/// Return the floating point remainder (see man 3 fmod for details and return values).
/// @param x Dividend.
/// @param y Divisor.
/// @return Floating point remainder x - n*y for some n.
/// @splreturn Floating point remainder x - n\*y for some n.
/// @splnative <float T> public T fmod (T x, T y)
/// @spleval $name = "fmod"; $ret = '@float@'; $numArgs = 2; $arg[0] = '@float@'; $arg[1] = '@float@'
template<class T>
inline T fmod(T x, T y)
{
    return ::fmod(x, y);
}

}
}
}

#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_MATH_FUNCTIONS_H */
