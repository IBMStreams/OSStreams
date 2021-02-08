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

#ifndef SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_SUMMARIZER_H
#define SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_SUMMARIZER_H

/*!
 * \file TumblingWindowSummarizer.h \brief Definition of the SPL::WindowSummarizer class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    /// @ingroup Window
    /// @brief Class that defines an interface for tumbling window summarizers.
    ///
    /// A tumbling window summarizer may be used to remove the need for the
    /// Windowing library to retain all tuples in a tumbling window. If your
    /// operator uses the windowing library to group and partition tuples,
    /// but just needs to look at incoming tuples once, a tumbling window
    /// summarizer can be used.
    ///
    /// Inherit from this class and override select methods to implement
    /// summarizers for tumbling windows.
    template <class T, class G=int32_t>
    class TumblingWindowSummarizer
    {
    public:
        typedef T TupleType; //!< tuple type
        typedef G PartitionType; //!< partition type

        /// Destructor. The destructor is called immediately after the
        /// afterWindowFlushEvent window event.
        virtual ~TumblingWindowSummarizer() {}

        /// This event is fired when a new window is opened.
        /// A new window is opened when the first tuple that is part of the new
        /// window is received. The construction happens after the
        /// beforeTupleInsertionEvent window event and before the
        /// afterTupleInsertionEvent window event.
        /// @param partition the partition for which the summarizer is created
        virtual void onOpenWindowEvent(PartitionType const & partition) {}

        /// This event is fired when a new tuple is added to the window
        /// corresponding to this summarizer. This event is delivered after the
        /// beforeTupleInsertionEvent window event and before the
        /// afterTupleInsertionEvent window event.
        /// @param tuple the tuple that was inserted
        virtual void onTupleInsertionEvent(TupleType const & tuple) {}

        /// This event is fired when the current window is closed. An existing
        /// window is closed after the last tuple that is part of that window is
        /// received. This event is delivered before the beforeWindowFlushEvent.
        virtual void onCloseWindowEvent() {}

        /// This event is fired when the window's state is checkpointed. The
        /// summarizer should serialize its state into the specified
        /// checkpoint stream.
        virtual void onCheckpointEvent(Checkpoint & ckpt) const {}

        /// This event is fired when the window it is registered with is
        /// restored to the state provided by the specified checkpoint.
        /// The summarizer should restore its state by reading from the
        /// specified checkpoint stream.
        virtual void onResetEvent(Checkpoint & ckpt) {}
    };
}
POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_SUMMARIZER_H */
