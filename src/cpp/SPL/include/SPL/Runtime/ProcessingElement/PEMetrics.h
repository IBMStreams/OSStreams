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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_METRICS_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_METRICS_H

/*!
 * \file PEMetrics.h \brief Definition of the SPL::PEMetrics class.
 */

namespace SPL
{
    class Metric;

    /// Class that represents PE metrics
    class PEMetrics
    {
    public:
        /// Enumerations for input port metrics
        enum InputPortMetricName
        {
            nTuplesProcessed,       //!< Number of tuples processed by the port (counter)
            nTupleBytesProcessed,   //!< Number of bytes processed by the port (counter)
            nWindowPunctsProcessed, //!< Number of window punctuations processed by the port (counter)
            nFinalPunctsProcessed   //!< Number of final punctuations processed by the port (counter)
        };

        /// Enumerations for output port metrics
        enum OutputPortMetricName
        {
            nTuplesSubmitted,       //!< Number of tuples submitted by the port (counter)
            nTupleBytesSubmitted,   //!< Number of bytes submitted by the port (counter)
            nWindowPunctsSubmitted, //!< Number of window punctuations submitted by the port (counter)
            nFinalPunctsSubmitted,  //!< Number of final punctuations submitted by the port (counter)
            nBrokenConnections,     //!< Number of broken connections that have occurred on the port (counter)
            nRequiredConnecting,    //!< Number of required connections currently connecting on the port (gauge)
            nOptionalConnecting,    //!< Number of optional connections currently connecting on the port (gauge)
            nTuplesTransmitted,     //!< Number of tuples transmitted by the port (counter)
            nTupleBytesTransmitted, //!< Number of bytes transmitted by the port (counter)
            nConnections            //!< Number of PE input streams connected to the port (gauge)
        };

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        ///
        virtual ~PEMetrics() {}
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Obtain an input port metric
        /// @param port port index
        /// @param metric metric name
        /// @return the metric reference
        /// @throw SPLRuntimeInvalidIndex when the index is out of bounds
        virtual Metric const & getInputPortMetric(uint32_t port, InputPortMetricName metric) const = 0;

        /// Obtain an output port metric
        /// @param port port index
        /// @param metric metric name
        /// @return the metric reference
        /// @throw SPLRuntimeInvalidIndex when the index is out of bounds
        virtual Metric const & getOutputPortMetric(uint32_t port, OutputPortMetricName metric) const = 0;
    };
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_METRICS_H */
