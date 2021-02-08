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

#ifndef SPL_RUNTIME_OPERATOR_PARAMETERVALUE_H
#define SPL_RUNTIME_OPERATOR_PARAMETERVALUE_H

/*!
 * \namespace SPL \brief C++ classes for accessing Parameter values at runtime.
 */

/*!
 * \file ParameterValue.h \brief Definition of the SPL::ParameterValue class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Type/ValueHandle.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    /// \brief Class representing one parameter value at runtime
    class ParameterValue
    {
    public:
        /// Is this parameter value a simple value representable by a ConstValueHandle?
        /// This corresponds to a parameter with an expressionMode kind that is not
        /// 'Expression' or 'Attribute'.
        /// @return 'true' if the parameter expressionMode is not 'Expression' or
        /// 'Attribute', and is representable as a ConstValueHandle at runtime.
        virtual bool isSimpleValue() = 0;

        /// Is this parameter value only representable as an expression?
        /// This corresponds to a parameter with an expressionMode kind that is 'Expression'
        /// or 'Attribute'.
        /// @return 'true' if the parameter expressionMode is 'Expression' or 'Attribute'
        virtual bool isExpressionValue() = 0;

        /// Return the concrete value for the parameter
        /// @pre isSimpleValue() == true
        /// @return the ConstValueHandle representing the parameter value
        /// @throws SPLRuntimeInvalidOperationException if isSimpleValue() == false
        virtual ConstValueHandle& getValue() = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Create a Value ParameterValue
        /// @param value runtime ConstValueHandle value
        /// @return ParameterValue representing runtime Value
        static ParameterValue& create(ConstValueHandle const& value);

        /// Create a non-Value ParameterValue
        /// @return ParameterValue representing a non-value
        static ParameterValue& create();
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// The destructor
        virtual ~ParameterValue() {}
    };

};

#endif /* SPL_RUNTIME_OPERATOR_PARAMETERVALUE_H */
