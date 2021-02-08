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

#ifndef SPL_TOOLKIT_BASE_PARAMETERS_H
#define SPL_TOOLKIT_BASE_PARAMETERS_H

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/ParameterValue.h>
#include <SPL/Runtime/Common/RuntimeException.h>

#include <functional>
#include <cassert>
#include <string>

/**
 * @file Parameters.h
 * @brief Utility functions for operator parameter access and validation.
 */

// TODO move to RuntimeMessages.h and change "CDISRXXXXE" to proper id
#define SPL_PARAMETER_ILLEGAL_VALUE(p0, p1) \
   (::SPL::FormattableMessage2<typeof(p0), typeof(p1)>("spl", "StreamsSPLRuntimeMessages", "en", "CDISRXXXXE", "Parameter {0} has illegal value {1}.", p0, p1))

namespace SPL { namespace Toolkit { namespace Base {

/**
 * @brief Operator parameter utility functions.
 */
class Parameters
{
public:

  /**
   * @brief Determines if a certain parameter is specified for the operator.
   *
   * @param op the Operator
   * @param name the operator parameter name
   * @return @c true if the parameter is specified, otherwise @c false
   */
  inline static bool hasParameter(SPL::Operator const & op, std::string const & name)
  { return op.getParameterNames().count(name); }

  /**
   * @brief Retrieves the value of a parameter.
   *
   * @param op the Operator
   * @param name the operator parameter name
   *
   * @return the parameter value
   * @throws SPLRuntimeInvalidArgumentException if the parameter is not
   *      specified for the operator
   */
  inline static SPL::ConstValueHandle getParameter(SPL::Operator const & op,
      std::string const & name)
  {
    const SPL::Operator::ParameterValueListType& values = op.getParameterValues(name);
    return values[0]->getValue();
  }

  /**
   * @brief Retrieves the value of an optional parameter.
   *
   * If the parameter is not specified, then the default value is returned.
   *
   * @param op the Operator
   * @param name the operator parameter name
   * @param defaultValue the default parameter value if the parameter was not
   *    specified
   * @tparam T  the parameter value type
   *
   * @return the parameter value, or the defaultValue if the parameter is not
   *    specified
   */
  template<typename T>
  struct getOptional {
    T operator()(SPL::Operator const & op, std::string const & name,
        T const & defaultValue) const
    {
      T value = defaultValue;
      if (Parameters::hasParameter(op, name)) {
        value = static_cast<T>(Parameters::getParameter(op, name));
      }
      return value;
    }
  };

  /**
   * @brief Retrieves the value of an optional operator parameter and
   * calls the specified checker functor to validate it.
   *
   * If the parameter is not specified, then the default value is returned.
   *
   * Example of retrieving the value of the parameter "myParam", of type
   * double. If the parameter is not specified, then the function returns
   * the specified default value 1.0. The function invokes the specified
   * `Parameters::greater` functor to check whether the parameter value
   * is greater than 0.0:
   * @code
     Operator & op = ...

     typedef SPL::Toolkit::Base::Parameters Params;
     double value = Params::getOptionalCheck<Params::greater<double>, double>()
        (*this, "myParam", 1.0, Params::greater<double>(0.0));
    @endcode
   *
   * @param op the Operator
   * @param name the operator parameter name
   * @param defaultValue the default parameter value if the parameter was not
   *    specified
   * @param checker functor used for validating the parameter value
   * @tparam T  the parameter value type
   *
   * @return the parameter value, or the defaultValue if the parameter is not
   *    specified
   * @throw SPLRuntimeInvalidArgumentException if validation fails
   */
  template<typename Op, typename T>
  struct getOptionalCheck {
    T operator()(SPL::Operator const & op, std::string const & name,
        T const & defaultValue, Op const & checker) const
    {
      T value = defaultValue;
      if (Parameters::hasParameter(op, name)) {
        value = static_cast<double>(Parameters::getParameter(op, name));
      }

      if (!checker(const_cast<const T&>(value))) {
        THROW_STRING(SPLRuntimeInvalidArgument, SPL_PARAMETER_ILLEGAL_VALUE(name.c_str(), value));
      }
      return value;
    }
  };

  /// Used as a base class for validation functors
  template<typename T>
  struct base_class {
    const T value;
    base_class(const T & t) : value(t) {}
  };

  /// Explicit validator bindings
  #define PARAM_DEFINE_VALIDATOR(NAME, PREDICATE)       \
  template<typename T>                                  \
  class NAME : protected base_class<T> {                \
    typedef base_class<T> base_;                        \
  public:                                               \
    explicit NAME(T const & limit) : base_(limit) {}    \
    bool operator()(const T & x) const                  \
      { return PREDICATE<T>()(x, base_::value); }       \
  }

  /// @brief `Parameters::greater<T>(x)` checks if `parameterValue > x`
  /// @see Parameters::getOptionalCheck
  PARAM_DEFINE_VALIDATOR(greater,       std::greater);
  /// @brief `Parameters::greater_equal<T>(x)` checks if `parameterValue >= x`
  /// @see Parameters::getOptionalCheck
  PARAM_DEFINE_VALIDATOR(greater_equal, std::greater_equal);
  /// @brief `Parameters::less<T>(x)` checks if `parameterValue < x`
  /// @see Parameters::getOptionalCheck
  PARAM_DEFINE_VALIDATOR(less,          std::less);
  /// @brief `Parameters::less_equal<T>(x)` checks if `parameterValue <= x`
  /// @see Parameters::getOptionalCheck
  PARAM_DEFINE_VALIDATOR(less_equal,    std::less_equal);
  /// @brief `Parameters::equal_to<T>(x)` checks if `parameterValue == x`
  /// @see Parameters::getOptionalCheck
  PARAM_DEFINE_VALIDATOR(equal_to,      std::equal_to);
  /// @brief `Parameters::not_equal_to<T>(x)` checks if `parameterValue != x`
  /// @see Parameters::getOptionalCheck
  PARAM_DEFINE_VALIDATOR(not_equal_to,  std::not_equal_to);

  #undef PARAM_DEFINE_VALIDATOR

};  // end Parameters
}}} // end SPL::Toolkit::Base
#endif // SPL_TOOLKIT_BASE_PARAMETERS_H
