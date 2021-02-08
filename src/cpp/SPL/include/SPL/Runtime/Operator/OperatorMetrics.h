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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_H

/*!
 * \file OperatorMetrics.h \brief Definition of the SPL::OperatorMetrics class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <inttypes.h>
#include <string>
#include <vector>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#include <SPL/Runtime/Common/Metric.h>

namespace SPL
{
    /// Class that represents operator metrics
    class OperatorMetrics
    {
    public:
        /// Enumerations for input port metrics
        enum InputPortMetricName
        {
            nTuplesProcessed,       //!< Number of tuples processed by the port (counter)
            nTuplesDropped,         //!< Number of tuples dropped by the port (counter)
            nTuplesQueued,          //!< Number of tuples queued by the port (gauge)
            nWindowPunctsProcessed, //!< Number of window punctuations processed by the port (counter)
            nFinalPunctsProcessed,  //!< Number of final punctuations processed by the port (counter)
            nWindowPunctsQueued,    //!< Number of window punctuations queued by the port (gauge)
            nFinalPunctsQueued,     //!< Number of final punctuations queued by the port (gauge)
            queueSize,              //!< Size of queue for the port or 0 if no queue (gauge)
            maxItemsQueued,         //!< Largest number of items queued to the port (gauge)
            recentMaxItemsQueued,   //!< Recent largest number of items queued to the port (gauge)
            recentMaxItemsQueuedInterval,  //!< Interval in milliseconds used to determine recentMaxItemsQueued (time)
            nEnqueueWaits           //!< Number of waits due to a full queue when enqueueing items for the port (counter)
        };

        /// Enumerations for output port metrics
        enum OutputPortMetricName
        {
            nTuplesSubmitted,        //!< Number of tuples submitted by the port (counter)
            nWindowPunctsSubmitted,  //!< Number of window punctuations submitted by the port (counter)
            nFinalPunctsSubmitted    //!< Number of final punctuations submitted by the port (counter)
        };

        /// Enumerations for system metrics
        enum SystemMetricName
        {
            relativeOperatorCost //!< Relative cost of the operator within PE scope
        };

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        ///
        virtual ~OperatorMetrics() {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Obtain an input port metric
        /// @param port port index
        /// @param metric metric name
        /// @return the metric reference
        /// @throws SPLRuntimeInvalidIndexException when the index is out of bounds
        virtual Metric const & getInputPortMetric(uint32_t port, InputPortMetricName metric) const = 0;

        /// Obtain an output port metric
        /// @param port port index
        /// @param metric metric name
        /// @return the metric reference
        /// @throws SPLRuntimeInvalidIndexException when the index is out of bounds
        virtual Metric const & getOutputPortMetric(uint32_t port, OutputPortMetricName metric) const = 0;

        /// @brief Create a new operator metric with a given name.
        /// If Metric @c name already exists, does nothing starting with IBM Streams v4.3.0 and throws
        /// @c SPLRuntimeInvalidMetricException for versions prior to 4.3.0.
        /// @param name metric name
        /// @param description metric description
        /// @param kind metric kind
        /// @return the metric reference
        /// @throws SPLRuntimeInvalidMetricException if the metric already exists (prior to IBM Streams v4.3.0)
        virtual Metric & createCustomMetric(std::string const & name,
                                            std::string const & description,
                                            Metric::Kind kind) = 0;

        /// Obtain a reference to an operator metric given its name
        /// @param name metric name
        /// @return the metric reference
        /// @throws SPLRuntimeInvalidMetricException if there is no metric with the given name
        virtual Metric & getCustomMetricByName(std::string const & name) = 0;

        /// Check if an operator metric exists, given its name
        /// @param name metric name
        /// @return true if the metric exists, false otherwise
        virtual bool hasCustomMetric(std::string const & name) = 0;

        /// Return all the names of the custom metrics for this operator
        /// @return a list of the names of all the custom metrics defined for this operator
        virtual std::vector<std::string> getCustomMetricNames() const = 0;

        /// Obtain a system metric
        /// @param metric metric name
        /// @return the metric value reference
        virtual Metric const & getOperatorMetric(SystemMetricName metric) const = 0;

    };
}
#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_H */
