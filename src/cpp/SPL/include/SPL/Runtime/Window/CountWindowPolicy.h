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

#ifndef SPL_RUNTIME_WINDOW_COUNT_WINDOW_POLICY_H
#define SPL_RUNTIME_WINDOW_COUNT_WINDOW_POLICY_H

/*!
 * \file CountWindowPolicy.h \brief Definition of the SPL::CountWindowPolicy class.
 */

namespace SPL
{
    /// @ingroup Window
    /// @brief Class that represents a count based window policy.
    ///
    /// - For a tumbling window, as an eviction policy
    ///
    /// The \a size defines the number of tuples that constitute a full window.
    ///
    /// Operational semantics: When a new tuple is received, it is inserted into
    /// the window. After the insertion, if the number of tuples in the window
    /// is equal to the specified \a size (a full window), then the whole window
    /// is flushed (mass eviction).
    ///
    /// - For a sliding window, as an eviction policy
    ///
    /// The \a size defines the number of tuples to be kept in the window, as it
    /// slides.
    ///
    /// Operational semantics: When a new tuple is received, if the number of
    /// existing tuples in the window is equal to the \a size specified, then
    /// the oldest tuple is evicted from the window. The new tuple is inserted
    /// into the window after the eviction (if any) is performed.
    ///
    /// - For a sliding window, as a trigger policy
    ///
    /// The \a size defines the number of tuples to be received since the last
    /// trigger, before triggering the window again.
    ///
    /// Operational semantics: When a new tuple is received, the number of
    /// tuples received since the last trigger is updated. If this number is
    /// equal to the specified \a size, then the window is triggered again.
    ///
    /// Ordering: The trigger policy (count based) processing takes place after
    /// the eviction policy (count or delta based) processing.
    class CountWindowPolicy : public WindowPolicy
    {
    public:
        /// Constructor
        /// @param size size of the count based window
        CountWindowPolicy(uint32_t size)
            : WindowPolicy(Count),
              size_(size) {}

        /// Destructor
        ///
        virtual ~CountWindowPolicy() {}

        /// Get size
        /// @return the size of the count based window
        uint32_t getSize() const
            { return size_; }

        /// Clone
        /// @return a copy of this object
        WindowPolicy * clone () const
            { return new CountWindowPolicy(*this); }
    private:
        uint32_t size_;
    };
};

#endif /* SPL_RUNTIME_WINDOW_COUNT_WINDOW_POLICY_H */
