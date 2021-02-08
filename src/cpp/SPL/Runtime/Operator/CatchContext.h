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

#ifndef SPL_RUNTIME_OPERATOR_CATCH_CONTEXT_H
#define SPL_RUNTIME_OPERATOR_CATCH_CONTEXT_H

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Operator/OptionalContext.h>

#include <stdint.h>
#include <string>

namespace SPL {
enum CatchExceptionKind
{
    STREAMS = 0,
    STD = 1,
    ALL = 2,
    NONE = 3
};

/// This context can be acquired by using the CATCH string.
/// An operator has a CatchContext when it has a \@catch annotation
/// in its instance.
/// For now, this is not a public interface
class CatchContext : public OptionalContext
{
  public:
    CatchContext(ADL::annotationType const& annot);

    /// Destructor
    virtual ~CatchContext() {}

    // @Override
    virtual std::string const& getName() const;

    /// Returns the highest level of exceptions that must be caught
    /// by the operator.
    /// @return exception kind
    CatchExceptionKind getExceptionKind() const;

    /// Checks if the operator must trace input tuples on an exception.
    /// This maps to the tupleTrace parameter of the @catch annotation
    /// @return true if tuples must be traced, false otherwise
    bool mustTraceTuples() const;

    /// Checks if the operator must print the stack trace on an exception.
    /// This maps to the stackTrace parameter of the @catch annotation
    /// @return true if the stack trace must be printed, false otherwise
    bool mustPrintStackTrace() const;

  private:
    const std::string name_;
    bool tupleTrace_;
    bool stackTrace_;
    CatchExceptionKind kind_;
};

}

#endif // SPL_RUNTIME_COMMON_CONSISTENT_REGION_CONTEXT_H
