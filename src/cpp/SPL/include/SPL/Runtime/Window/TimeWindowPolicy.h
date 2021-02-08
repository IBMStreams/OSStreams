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

#ifndef SPL_RUNTIME_WINDOW_TIME_WINDOW_POLICY_H
#define SPL_RUNTIME_WINDOW_TIME_WINDOW_POLICY_H

/*!
 * \file TimeWindowPolicy.h \brief Definition of the SPL::TimeWindowPolicy class.
 */

namespace SPL
{
    /// @ingroup Window
    /// @brief Class that represents a time based window policy.
    ///
    /// - For a tumbling window, as an eviction policy
    ///
    /// The \a size defines the boundary of a full window in units of real time.
    ///
    /// Operational semantics: When a new tuple is received, it is inserted into
    /// the window. When the time elapsed since the last flush event exceeds (>)
    /// the specified size (in time units), then the whole window flushes (mass
    /// eviction).
    ///
    /// Ordering: Window flushes take place independent of tuple insertions.
    ///
    /// - For a sliding window, as an eviction policy
    ///
    /// The \a size defines the boundary of the window, as it slides, in units
    /// of real time.
    ///
    /// Operational semantics: When a new tuple is received, it is inserted into
    /// the window. Tuples that have been in the window longer than the
    /// specified \a size (in time units) are evicted.
    ///
    /// Ordering: Tuple evictions take place independent of tuple insertions.
    //
    /// - For a sliding window, as a trigger policy
    ///
    /// The \a size defines the inter-arrival time for successive trigger
    /// events.
    ///
    /// Operational sematics: When the time elapsed since the last trigger event
    /// exceeds (>) the specified size (in time units), a new trigger is
    /// generated.
    ///
    /// Ordering: The trigger policy (time based) processing takes place
    /// independent of the eviction policy processing.
    class TimeWindowPolicy : public WindowPolicy
    {
    public:
        /// Constructor
        /// @param size size of the window (in seconds)
        TimeWindowPolicy(double size)
            : WindowPolicy(Time),
              size_(size) {}

        /// Destructor
        ///
        virtual ~TimeWindowPolicy() {}

        /// Get size of the window
        /// @return size of the time based window in seconds (could be
        /// fractional)
        double getSize() const
            { return size_; }

        /// Clone
        /// @return a copy of this object
        WindowPolicy * clone () const
            { return new TimeWindowPolicy(*this); }
    private:
        double size_;
    };
};
#endif /* SPL_RUNTIME_WINDOW_TIME_WINDOW_POLICY_H */
