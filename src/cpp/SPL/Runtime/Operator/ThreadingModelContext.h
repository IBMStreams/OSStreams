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

#ifndef SPL_RUNTIME_OPERATOR_THREADING_MODEL_CONTEXT_H
#define SPL_RUNTIME_OPERATOR_THREADING_MODEL_CONTEXT_H

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Operator/OptionalContext.h>

#include <string>

namespace SPL {
enum ThreadingModelKind
{
    MANUAL,
    AUTOMATIC,
    DYNAMIC,
    DEDICATED
};

/// This context can be acquired by using the THREADING string.
/// An operator has a ThreadingModelContext when it is in a composite that has a
/// has a \@threading annotation either on its definition or invocation.
/// For now, this is not a public interface
class ThreadingModelContext : public OptionalContext
{
  public:
    ThreadingModelContext(ADL::annotationType const& annot);

    /// Destructor
    virtual ~ThreadingModelContext() {}

    // @Override
    virtual std::string const& getName() const;

    /// Returns the threading model associated with this operator.
    /// @return threading model kind
    ThreadingModelKind getKind() const;

    /// Return the number of threads for scheduled ports. If it was not set explicity,
    /// uses the default of 1.
    /// @return number of scheduled port threads
    uint32_t getThreads() const;

    /// Returns if elasticity is turned on. If it was not set explicity, uses the
    /// default of "true".
    /// @return whether or not elasticity is turned on
    bool getElastic() const;

  private:
    const std::string name_;
    ThreadingModelKind kind_;
    uint32_t threads_;
    bool elastic_;
};
}

#endif // SPL_RUNTIME_OPERATOR_THREADING_MODEL_CONTEXT_H
