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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_METRICS_IMPL_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_METRICS_IMPL_H

#include <SPL/Runtime/Common/MetricImpl.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/ProcessingElement/PEMetrics.h>
#include <SPL/Runtime/ProcessingElement/PEMetricsInfo.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/SpinLock.h>
#include <UTILS/Thread.h>
#include <UTILS/ThreadTimingInfo.h>
#include <UTILS/UTILSTypes.h>

#include <cassert>
#include <string>
#include <vector>

namespace SPL {
// forward declaration
class PEImpl;
class PEMetricsService;
class PEMetricSyncher;

/// Class that represents PE metrics
class PEMetricsImpl : public PEMetrics
{
  public:
    // this should have the same order as Punctuation
    enum UpdateType
    {
        TUPLE = 0,
        WINDOW_PUNCT,      // -> Punctuation::WindowMarker
        FINAL_PUNCT,       // -> Punctuation::FinalMarker
        SWITCH_PUNCT,      // -> Punctuation::SwitchMarker
        DRAIN_PUNCT,       // -> Punctuation::DrainMarker
        RESET_PUNCT,       // -> Punctuation::ResetMarker
        RESUME_PUNCT = 127 // -> Punctuation:::ResumeMarker
    };

    /// destructor
    ~PEMetricsImpl();

    /// create a new object
    /// @param pe PE hosting the operator
    PEMetricsImpl(PEImpl& pe);

    /// Obtain an input port metric
    /// @param port port index
    /// @param metric metric name
    /// @return the metric reference
    /// @throw SPLRuntimeInvalidIndex when the index is out of bounds
    const Metric& getInputPortMetric(uint32_t port, InputPortMetricName metric) const
    {
        uint32_t nip = inputMetrics_.size();
        if (port >= nip) {
            THROW_STRING(SPLRuntimeInvalidIndex,
                         SPL_RUNTIME_INPUT_PORT_INDEX_OUT_OF_RANGE(port, nip));
        }
        return *inputMetrics_[port][metric];
    }

    /// Obtain an output port metric
    /// @param port port index
    /// @param metric metric name
    /// @return the metric reference
    /// @throw SPLRuntimeInvalidIndex when the index is out of bounds
    const Metric& getOutputPortMetric(uint32_t port, OutputPortMetricName metric) const
    {
        uint32_t nop = outputMetrics_.size();
        if (port >= nop) {
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                                SPL_RUNTIME_OUTPUT_PORT_INDEX_OUT_OF_RANGE(port, nop), SPL_PE_DBG);
        }
        return *outputMetrics_[port][metric];
    }

    /// Obtain an input port metric
    /// @param port port index
    /// @param metric metric name
    /// @return the metric reference
    Metric& getInputPortMetric(uint32_t port, InputPortMetricName metric)
    {
        return *inputMetrics_[port][metric];
    }

    /// Obtain an output port metric
    /// @param port port index
    /// @param metric metric name
    /// @return the metric reference
    Metric& getOutputPortMetric(uint32_t port, OutputPortMetricName metric)
    {
        return *outputMetrics_[port][metric];
    }

    // Update receive counters
    // @param type type of the item being received
    // @param port index of the input port that received data
    // @param size how many bytes were received
    inline void updateReceiveCounters(UpdateType type, uint32_t port, uint32_t size)
    {
        boost::atomic<int64_t>* numberPtr;
        switch (type) {
            case TUPLE: {
                numberPtr = &inputMetricsRaw_[port][nTuplesProcessed];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                                 boost::memory_order_relaxed);
                numberPtr = &inputMetricsRaw_[port][nTupleBytesProcessed];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + size,
                                 boost::memory_order_relaxed);
                break;
            }
            case WINDOW_PUNCT: {
                numberPtr = &inputMetricsRaw_[port][nWindowPunctsProcessed];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                                 boost::memory_order_relaxed);
                break;
            }
            case FINAL_PUNCT: {
                numberPtr = &inputMetricsRaw_[port][nFinalPunctsProcessed];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                                 boost::memory_order_relaxed);
                break;
            }
            case SWITCH_PUNCT:
            case DRAIN_PUNCT:
            case RESET_PUNCT:
            case RESUME_PUNCT: {
                break;
            }
        }
    }

    // Update send counters
    // @param type type of the item being sent
    // @param port index of the output port that received data
    // @param size how many bytes were sent
    // @param nsubs number of downstream subscribers
    inline void updateSendCounters(UpdateType type, uint32_t port, uint32_t size, uint32_t nsubs)
    {
        boost::atomic<int64_t>* numberPtr;
        switch (type) {
            case TUPLE: {
                numberPtr = &outputMetricsRaw_[port][nTuplesSubmitted];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                                 boost::memory_order_relaxed);
                numberPtr = &outputMetricsRaw_[port][nTupleBytesSubmitted];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + size,
                                 boost::memory_order_relaxed);
                numberPtr = &outputMetricsRaw_[port][nTuplesTransmitted];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + nsubs,
                                 boost::memory_order_relaxed);
                numberPtr = &outputMetricsRaw_[port][nTupleBytesTransmitted];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + size * nsubs,
                                 boost::memory_order_relaxed);
                break;
            }
            case WINDOW_PUNCT: {
                numberPtr = &outputMetricsRaw_[port][nWindowPunctsSubmitted];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                                 boost::memory_order_relaxed);
                break;
            }
            case FINAL_PUNCT: {
                numberPtr = &outputMetricsRaw_[port][nFinalPunctsSubmitted];
                numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                                 boost::memory_order_relaxed);
                break;
            }
            case SWITCH_PUNCT:
            case DRAIN_PUNCT:
            case RESET_PUNCT:
            case RESUME_PUNCT: {
                break;
            }
        }
    }

    // Add an output port connection
    // @param port index of the output port adding a new connection
    inline void addOutputPortConnection(uint32_t port)
    {
        boost::atomic<int64_t>* numberPtr = &outputMetricsRaw_[port][nConnections];
        numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                         boost::memory_order_relaxed);
    }

    // Remove an output port connection
    // @param port index of the output port removing a connection
    inline void deleteOutputPortConnection(uint32_t port)
    {
        boost::atomic<int64_t>* numberPtr = &outputMetricsRaw_[port][nConnections];
        numberPtr->store(numberPtr->load(boost::memory_order_relaxed) - 1,
                         boost::memory_order_relaxed);
    }

    /// Get all PE metrics data (excluding oper metrics)
    /// @param peMetrics (out parameter) metrics to populate
    /// @pre peMetrics is newly constructed, and thus empty
    void getMetrics(PEMetricsInfo& peMetrics) const;

    // Update connection counters
    // @param metric metric to be updated
    // @param port index of the output port to be updated
    // @param incr how much to update
    inline void updateReconnectionCounters(PEMetrics::OutputPortMetricName metric,
                                           uint32_t port,
                                           uint64_t incr = 1)
    {
        outputMetricsRaw_[port][metric].fetch_add(incr, boost::memory_order_relaxed);
    }

  private:
    enum
    {
        numInputPortMetrics = 4
    };
    enum
    {
        numOutputPortMetrics = 10
    };

    void createInputPortMetric(uint32_t port,
                               PEMetrics::InputPortMetricName name,
                               std::string const& shortName);
    void createOutputPortMetric(uint32_t port,
                                PEMetrics::OutputPortMetricName name,
                                std::string const& shortName);
    SystemMetricImpl* createPortMetric(uint32_t port,
                                       std::string const& shortName,
                                       std::string const& longName,
                                       Metric::Kind kind,
                                       boost::atomic<int64_t>* value);
    void dumpMetricAtExit(Metric& metric);

    PEImpl& pe_;

    // Metric objects
    std::vector<std::vector<SystemMetricImpl*> > inputMetrics_;  // per port
    std::vector<std::vector<SystemMetricImpl*> > outputMetrics_; // per port

    // Structures to hold perf. sensitive metrics, per-port
    struct InputPortMetricBlock
    {
        boost::atomic<int64_t> metrics_[numInputPortMetrics];
        boost::atomic<int64_t>& operator[](size_t i) { return metrics_[i]; }
    };
    struct OutputPortMetricBlock
    {
        boost::atomic<int64_t> metrics_[numOutputPortMetrics];
        boost::atomic<int64_t>& operator[](size_t i) { return metrics_[i]; }
    };

    // Local variables for perf. sensitive metrics
    InputPortMetricBlock* inputMetricsRaw_;   // per-port
    OutputPortMetricBlock* outputMetricsRaw_; // per-port
};

};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_METRICS_IMPL_H */
