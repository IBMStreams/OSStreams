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

#ifndef SPL_RUNTIME_UTILITY_THREAD_H
#define SPL_RUNTIME_UTILITY_THREAD_H

/*!
 * \file Thread.h \brief Definition of the SPL::Thread class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/Thread.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#include <string>

namespace SPL
{
    /// Class that implements a wrapper for a pthread thread
    class Thread : protected Distillery::Thread
    {
    public:
        /// Constructor
        ///
        Thread() : Distillery::Thread() {}

        /// Create a thread
        /// @param attr the attributes used by pthread_create
        /// @param detached create thread in detached state
        /// @param args the arguments passed to the thread
        /// @param waitStart wait until the thread has started
        /// @param stackSize thread stack size (uses pthread default if 0)
        /// @param id thread id representing this thread
        /// @return return value from the \c pthread_create call
        int create(pthread_attr_t * attr = NULL, bool detached = false,
                   void * args = NULL, bool waitStart = false,
                   size_t stackSize = 0, pthread_t * id = NULL)
        {
            return Distillery::Thread::create(attr, detached, args, waitStart, stackSize, id);
        }

        /// Join a thread
        /// @param status returns the value returned by a thread once it finishes
        int join(void * status = NULL)
        {
            return Distillery::Thread::join(status);
        }

        /// Detach a thread
        ///
        void detach()
        {
            Distillery::Thread::detach();
        }

        /// Send a signal to a thread
        /// @param sig signal to send (see kill(1) for the possible signal values)
        void kill(int sig = 9)
        {
            Distillery::Thread::kill(sig);
        }

        /// Cancel the thread
        ///
        void cancel()
        {
            Distillery::Thread::cancel();
        }

        /// Turn on cancel
        /// @param type pthread cancel type
        void enableCancel(int type)
        {
            Distillery::Thread::enableCancel(type);
        }

        /// Turn off cancel
        void disableCancel()
        {
            Distillery::Thread::disableCancel();
        }

        /// Execute the task a thread is supposed to do. This method is customized
        /// by someone writing the code that is supposed to be executed as a thread
        /// @param args the arguments received by a thread
        virtual void * run(void * args) = 0;

        /// Obtain the thread id
        /// @return the thread id for the thread
        virtual pthread_t getPThreadId() const
        {
            return Distillery::Thread::getPTHid();
        }

        /// Register the thread with the PE.
        /// @param name the name to provide to the OS for the thread
        void registerThread(const std::string& name);
    };

}

#endif /* SPL_RUNTIME_UTILITY_THREAD_H */
