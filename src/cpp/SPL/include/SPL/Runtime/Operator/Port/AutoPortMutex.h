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

#ifndef SPL_RUNTIME_OPERATOR_PORT_AUTO_PORT_MUTEX_H
#define SPL_RUNTIME_OPERATOR_PORT_AUTO_PORT_MUTEX_H

/*!
 * \file AutoPortMutex.h \brief Definition of the SPL::AutoPortMutex class.
 */

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Utility/Mutex.h>

namespace SPL
{
    /*! \brief Class that provides an RAII handler for critical sections in port processing.

        Protect a critical section of code using a passed in mutex, only if the
        operator whose context is provided could be called concurrently on its
        input ports by the upstream operator or processing element. E.g.:

        \code
        {  AutoPortMutext apm(myMutex, *this); // Obtain the lock
           // Code that manipulates operator state
           ...
        }  // Release the lock
        \endcode
    */
    class AutoPortMutex
    {
    public:
        /// Constructor
        /// @param mutex mutex to use
        /// @param oper operator
        AutoPortMutex(Mutex & mutex, Operator const & oper)
        : _threaded(oper.isMultiThreadedOnInputs()),
          _mutex(mutex)
        {
            if (_threaded) {
                _mutex.lock();
            }
        }

        /// Destructor
        ///
        ~AutoPortMutex()
        {
            if (_threaded) {
                _mutex.unlock();
            }
        }
    private:
        bool _threaded;
        Mutex & _mutex;
    };
}

#endif /* SPL_RUNTIME_OPERATOR_PORT_AUTO_PORT_MUTEX_H */
