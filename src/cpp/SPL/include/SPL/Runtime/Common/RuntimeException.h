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

#ifndef SPL_RUNTIME_COMMON_RUNTIME_EXCEPTION
#define SPL_RUNTIME_COMMON_RUNTIME_EXCEPTION

/*!
 * \file RuntimeException.h \brief Definitions of %SPL exception classes.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/UTILSTypes.h>
#include <UTILS/DistilleryException.h>
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{

PUSH_DLL_PUBLIC

DECL_EXCEPTION(SPL, SPLRuntime, Distillery::Distillery)

DECL_EXCEPTION(SPL, SPLRuntimeTypeMismatch,                  SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeDeserialization,               SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeSerialization,                 SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidIndex,                  SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidArgument,               SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidMetric,                 SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidCheckpoint,             SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidOperation,              SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidSlidingWindow,          SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInvalidTumblingWindow,         SPLRuntime)

DECL_EXCEPTION(SPL, SPLRuntimeBlobNotOwned,                  SPLRuntime)

DECL_EXCEPTION(SPL, SPLRuntimeImport,                        SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeStreamProperty,                SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeSubscription,                  SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeSubscriptionInvalidClause,     SPLRuntimeSubscription)
DECL_EXCEPTION(SPL, SPLRuntimeSubscriptionInvalidLiteral,    SPLRuntimeSubscription)
DECL_EXCEPTION(SPL, SPLRuntimeSubscriptionNoSubscriptWithIn, SPLRuntimeSubscription)

DECL_EXCEPTION(SPL, SPLRuntimeFilter,                        SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeFilterInvalidClause,           SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterInvalidLiteral,          SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterNoSubscriptWithIn,       SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterNoAttribute,             SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterInvalidAttributeType,    SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterTypeMismatch,            SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterModNotInt64,             SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterNegativeSubscript,       SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterValidationFailed,        SPLRuntimeFilter)
DECL_EXCEPTION(SPL, SPLRuntimeFilterParseFailed,             SPLRuntimeFilter)

DECL_EXCEPTION(SPL, SPLRuntimeInvalidXML,                    SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeOperator,                      SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeMissingDataDirectory,          SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeNullValueAccess,               SPLRuntime)

DECL_EXCEPTION(SPL, SPLRuntimeUnsupportedSubstitutionType,   SPLRuntime)

DECL_EXCEPTION(SPL, SPLRuntimeIO,                            SPLRuntime)
DECL_EXCEPTION(SPL, SPLControl,                              SPLRuntime)
DECL_EXCEPTION(SPL, SPLListenerNotFound,                     SPLControl)
DECL_EXCEPTION(SPL, SPLControlBean,                          SPLControl)
DECL_EXCEPTION(SPL, SPLControlBeanNotFound,                  SPLControl)
DECL_EXCEPTION(SPL, SPLRuntimeGetEventTimeMissingEventTimeAttribute, SPLRuntime)

POP_DLL_PUBLIC

#ifdef DOXYGEN_SKIP_FOR_USERS
    // Note: this is not real code, it is for documentation purposes

    /// \brief Class that represents %SPL runtime exceptions.
    class SPLRuntimeException : public std::exception
    {
    public:
        /// Consructor
        /// @param location location of the exception
        /// @param message exception message
        SPLRuntimeException(std::string const & location,
                            std::string const & message) {}
        /// Get the explanation associated with this exception
        /// @return explanation associated with this exception
        virtual std::string const & getExplanation() const;
        /// Get the type of the exception
        /// @return type of the exception
        virtual const char* what() const throw();
    };

    /// \brief Class that represents %SPL runtime type mismatch exceptions.
    class SPLRuntimeTypeMismatchException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime type deserialization exceptions.
    class SPLRuntimeDeserializationException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime type serialization exceptions.
    class SPLRuntimeSerializationException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid index exceptions.
    class SPLRuntimeInvalidIndexException :  public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid argument exceptions.
    class SPLRuntimeInvalidArgumentException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid metric exceptions.
    class SPLRuntimeInvalidMetricException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid checkpoint exceptions.
    class SPLRuntimeInvalidCheckpointException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid operation exceptions.
    /// Thrown when an unsupported operation is invoked.
    class SPLRuntimeInvalidOperationException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid sliding window exceptions.
    class SPLRuntimeInvalidSlidingWindowException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime invalid tumbling window exceptions.
    class SPLRuntimeInvalidTumblingWindowException : public SPLRuntimeException {};

    /// \brief Class that represents %SPL runtime stream property exceptions.
    class SPLRuntimeStreamPropertyException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime subscription exceptions.
    class SPLRuntimeSubscriptionException : public SPLRuntimeException {};
    /// \brief Class that represents %SPL runtime subscription invalid clause exceptions.
    class SPLRuntimeSubscriptionInvalidClauseException : public SPLRuntimeSubscriptionException {};
    /// \brief Class that represents %SPL runtime subscription invalid literal exceptions.
    class SPLRuntimeSubscriptionInvalidLiteralException : public SPLRuntimeSubscriptionException {};
    /// \brief Class that represents %SPL runtime subscription no subscript can
    /// be used with the 'In' operator exceptions.
    class SPLRuntimeSubscriptionNoSubscriptWithInException : public SPLRuntimeSubscriptionException {};

    /// \brief Class that represents %SPL runtime operator exceptions.
    class SPLRuntimeOperatorException : public SPLRuntimeException {};

    /// \brief Class that represents a missing data directory
    class SPLRuntimeMissingDataDirectory : public SPLRuntimeException {};

    /// \brief Class that represents an access to an optional type that has a null value.
    class SPLRuntimeNullValueAccess : public SPLRuntimeException {};

    /// \brief Indicates that an exception has occurred when executing an I/O operation.
    class SPLRuntimeIOException : public SPLRuntimeException {};

    /// \brief Root of all the exceptions thrown by the %SPL runtime control.
    class SPLControlException : public SPLRuntimeException {};
    /// \brief The specified listener is not registered with the Job Control Plane.
    class SPLListenerNotFoundException : public SPLControlException {};
    /// \brief Represents exceptions thrown by MBeans registered with the Job Control Plane.
    class SPLControlBeanException : public SPLControlException {};
    /// \brief The specified MBean is not registered with the Job Control Plane.
    class SPLControlBeanNotFoundException : public SPLControlException {};

#endif /*DOXYGEN_SKIP_FOR_USERS*/

#ifndef DOXYGEN_SKIP_FOR_USERS
PUSH_DLL_PUBLIC
    class BacktraceDumper
    {
    public:
        static int dump(std::ostream& s);
    };
POP_DLL_PUBLIC
#endif

};

#endif /* SPL_RUNTIME_COMMON_RUNTIME_EXCEPTION */
