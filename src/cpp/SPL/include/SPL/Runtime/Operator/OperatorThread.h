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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_THREAD_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_THREAD_H

/*!
 * \file OperatorThread.h \brief Definition of the SPL::OperatorThread class.
 */

#include <SPL/Runtime/Utility/Thread.h>
#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    class OperatorImpl;

    /// Class that represents an operator thread
    class DLL_PUBLIC OperatorThread : public Thread
    {
    public:
        /// Get the index of this operator thread
        /// @return index of this operator thread
        uint32_t getIndex()
        {
            return index_;
        }

    private:
        void * run(void * /*threadArgs*/);

        /// Constructor
        /// @param oper operator that holds this thread
        /// @param idx index of the operator thread
        OperatorThread(OperatorImpl & oper, uint32_t idx)
            : oper_(oper), index_(idx) {}

        OperatorImpl & oper_;
        uint32_t index_;
        friend class OperatorImpl;
    };
};

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_THREAD_H */
