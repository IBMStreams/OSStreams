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

#ifndef SPL_RUNTIME_COMMON_METRIC_H
#define SPL_RUNTIME_COMMON_METRIC_H

/*!
 * \file Metric.h \brief Definition of the SPL::Metric class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <string>
#include <stdint.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    /// Class that represents a metric object, used for port, operator, and pe metrics.
    class Metric
    {
    public:
        /// Kind of the metric
        enum Kind
        {
            Gauge,      //!< Value at a point in time
            Counter,    //!< Value increases over time or decreases over time
            Time        //!< Value represents time (relative or absolute)
        };

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        ///
        virtual ~Metric() {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get the metric name
        /// @return name
        virtual std::string const & getName() const = 0;

        /// Get the metric description
        /// @return description
        virtual std::string const & getDescription() const = 0;

        /// Get the metric kind
        /// @return kind of the metric
        virtual Kind getKind() const = 0;

        /// Set the metric value (this is an atomic operation)
        /// @param v metric value
        virtual void setValue(int64_t v) = 0;

        /// Set the metric value (same as setValue(), as locking is not needed)
        /// @param v metric value
        virtual void setValueNoLock(int64_t v) = 0;

        /// Get the metric value (this is an atomic operation)
        /// @return metric value
        virtual int64_t getValue() const = 0;

        /// Get the metric value (same as getValue(), as locking is not needed)
        /// @return metric value
        virtual int64_t getValueNoLock() const = 0;

        /// Increment the metric value (this is an atomic operation)
        /// @param increment increment amount
        virtual void incrementValue(int64_t increment = 1) = 0;

        /// Increment the metric value (without locking)
        /// @param increment increment amount
        virtual void incrementValueNoLock(int64_t increment = 1) = 0;
    };
}

#endif /* SPL_RUNTIME_COMMON_METRIC_H */
