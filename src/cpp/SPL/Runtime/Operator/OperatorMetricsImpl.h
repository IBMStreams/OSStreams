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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_IMPL_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_IMPL_H

#include <SPL/Runtime/Common/MetricImpl.h>
#include <SPL/Runtime/Common/Prediction.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Runtime/Operator/Port/Inline.h>
#include <SPL/Runtime/ProcessingElement/PEMetricsInfo.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <UTILS/SpinLock.h>
#include <unistd.h>

#include <boost/atomic/atomic.hpp>
#include <cassert>

namespace SPL {
// forward declaration
class PEImpl;
class OperatorContextImpl;
class OperatorImpl;
class OperatorMetricsService;

/// Class that represents an operator metrics
class OperatorMetricsImpl : public OperatorMetrics
{
  public:
    // this should have the same order as Punctuation
    enum UpdateType
    {
        TUPLE = 0,
        WINDOW_PUNCT, // -> Punctuation::WindowMarker
        FINAL_PUNCT,  // -> Punctuation::FinalMarker

        WATERMARK_PUNCT = 123, // -> Watermark marker
        SWITCH_PUNCT,          // -> Punctuation::SwitchMarker
        DRAIN_PUNCT,           // -> Punctuation::DrainMarker
        RESET_PUNCT,           // -> Punctuation::ResetMarker
        RESUME_PUNCT           // -> Punctuation::ResumeMarker
    };

    /// Create a new object
    /// @param opctx operator context
    /// @param pe PE hosting the operator
    OperatorMetricsImpl(OperatorContextImpl& opctx, PEImpl& pe);

    /// Destructor
    ///
    ~OperatorMetricsImpl();

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
                                SPL_RUNTIME_OUTPUT_PORT_INDEX_OUT_OF_RANGE(port, nop),
                                SPL_OPER_DBG);
        }
        return *outputMetrics_[port][metric];
    }

    /// Obtain an operator system metric
    /// @param operator operator index
    /// @param metric metric name
    /// @return the metric reference
    const Metric& getOperatorMetric(SystemMetricName metric) const;

    /// Create a new operator metric with a given name
    /// @param name metric name
    /// @param description metric description
    /// @param kind the metric kind
    /// @return the metric reference
    /// @throws SPLRuntimeInvalidMetric if the metric already exists
    Metric& createCustomMetric(std::string const& name,
                               std::string const& description,
                               Metric::Kind kind);

    /// Obtain a reference to a metric given its name
    /// @param name metric name
    /// @return the metric reference
    /// @throws SPLRuntimeInvalidMetric if there is no metric with the given name
    Metric& getCustomMetricByName(std::string const& name);

    /// Check if an operator metric exists, given its name
    /// @param name metric name
    /// @return true if the metric exists, false otherwise
    bool hasCustomMetric(std::string const& name);

    /// Return all the names of the custom metrics for this operator
    /// @return a list of the names of all the custom metrics defined for this operator
    std::vector<std::string> getCustomMetricNames() const;

    /// Obtain the name of the operator hosting this metric
    /// @return name of the operator hosting this methric
    std::string const& getOperatorName() const { return opName_; }

    /// Obtain metrics description suitable for the PerfCounter collection
    /// class construction
    /// @return metric description
    std::string getDescription() const;

    /* These are the metrics that are user-visible */

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

    /// Obtain an operator system metric
    /// @param operator operator index
    /// @param metric metric name
    /// @return the metric reference
    Metric& getOperatorMetric(SystemMetricName metric);

    /* BEGIN: Input port queue counters (These need to be thread safe) */

    /// We can use a call that is not thread safe for a metric that
    /// is never incremented or decremented concurrently.
    /// For nTuplesQueued, nWindowPunctsQueued, nFinalPunctsQueued,
    /// we maintain seperate enqueue and dequeue counters,
    /// so increments can be concurrent with decrements.

    /// Increment queue counters.
    /// Not thread safe (but see note above).
    /// @param port input port index
    /// @param name input port metric to update
    inline void incrementQueueCounterNoLock(uint32_t port, InputPortMetricName name) ALWAYS_INLINE
    {
        boost::atomic<int64_t>* numberPtr;
        if (LIKELY(name == nTuplesQueued)) {
            numberPtr = &inputMetricsRaw_[port].nTuplesEnqueued_;
        } else {
            numberPtr = &inputMetricsRaw_[port][name];
        }
        numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                         boost::memory_order_relaxed);
    }

    /// Decrement queue counters.
    /// Not thread safe (but see note above).
    /// @param port input port index
    /// @param name input port metric to update
    inline void decrementQueueCounterNoLock(uint32_t port, InputPortMetricName name) ALWAYS_INLINE
    {
        boost::atomic<int64_t>* numberPtr;
        int64_t delta;
        if (LIKELY(name == nTuplesQueued)) {
            numberPtr = &inputMetricsRaw_[port].nTuplesDequeued_;
            delta = 1;
        } else if (name == nWindowPunctsQueued) {
            numberPtr = &inputMetricsRaw_[port].nWindowPunctsDequeued_;
            delta = 1;
        } else if (name == nFinalPunctsQueued) {
            numberPtr = &inputMetricsRaw_[port].nFinalPunctsDequeued_;
            delta = 1;
        } else {
            numberPtr = &inputMetricsRaw_[port][name];
            delta = -1;
        }
        numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + delta,
                         boost::memory_order_relaxed);
    }

    /// Set Queue Size Metric.
    /// This also sets the recent max items queued interval metric so this is set only for queueing
    /// ports. Does not need to be thread safe.
    /// @param port input port index
    /// @param value value to be set
    inline void setQueueSize(uint32_t port, int32_t value)
    {
        inputMetricsRaw_[port][queueSize].store(value, boost::memory_order_relaxed);
        inputMetricsRaw_[port][recentMaxItemsQueuedInterval].store(
          1000 * getIntervalMetricsPeriod(), boost::memory_order_relaxed);
    }

    /// Update queue high water mark metrics.
    /// Not thread safe.
    /// @param port input port index
    /// @param length current length of queue
    inline void updateMaxItemsQueuedNoLock(uint32_t port, int32_t length) ALWAYS_INLINE
    {
        if (length >
            inputMetricsRaw_[port][recentMaxItemsQueued].load(boost::memory_order_relaxed)) {
            inputMetricsRaw_[port][recentMaxItemsQueued].store(length, boost::memory_order_relaxed);
            if (length > inputMetricsRaw_[port][maxItemsQueued].load(boost::memory_order_relaxed)) {
                inputMetricsRaw_[port][maxItemsQueued].store(length, boost::memory_order_relaxed);
            }
        }
    }

    /* END: Input port queue counters */

    /// Update receive counters (when profiling is off)
    /// @pre isProfiling() == false
    /// @param type type of the item being received
    /// @param port index of the input port that received data
    inline void updateReceiveCounters(UpdateType type, uint32_t port) ALWAYS_INLINE
    {
        boost::atomic<int64_t>* numberPtr = NULL;
        switch (type) {
            case TUPLE: {
                numberPtr = &inputMetricsRaw_[port][nTuplesProcessed];
                break;
            }
            case WINDOW_PUNCT: {
                numberPtr = &inputMetricsRaw_[port][nWindowPunctsProcessed];
                break;
            }
            case FINAL_PUNCT: {
                numberPtr = &inputMetricsRaw_[port][nFinalPunctsProcessed];
                break;
            }
            case SWITCH_PUNCT:
            case DRAIN_PUNCT:
            case RESET_PUNCT:
            case RESUME_PUNCT:
            case WATERMARK_PUNCT: {
                // TODO add metric counters for these punctuation types
                APPTRC(L_TRACE, "nTuplesReceived: " << inputMetricsRaw_[port][nTuplesProcessed],
                       SPL_OPER_DBG);
                return;
            }
        }
        if (isSingleThreadedOnInputs_) {
            numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                             boost::memory_order_relaxed);
        } else {
            numberPtr->fetch_add((int64_t)1, boost::memory_order_relaxed);
        }
    }

    // Update send counters (when profiling is off)
    // @pre isProfiling() == false
    // @param type type of the item being sent
    // @param port index of the output port that received data
    inline void updateSendCounters(UpdateType type, uint32_t port) ALWAYS_INLINE
    {
        boost::atomic<int64_t>* numberPtr = NULL;
        switch (type) {
            case TUPLE: {
                numberPtr = &outputMetricsRaw_[port][nTuplesSubmitted];
                break;
            }
            case WINDOW_PUNCT: {
                numberPtr = &outputMetricsRaw_[port][nWindowPunctsSubmitted];
                break;
            }
            case FINAL_PUNCT: {
                numberPtr = &outputMetricsRaw_[port][nFinalPunctsSubmitted];
                break;
            }
            case SWITCH_PUNCT:
            case DRAIN_PUNCT:
            case RESET_PUNCT:
            case RESUME_PUNCT:
            case WATERMARK_PUNCT: {
                // TODO add metric counters for these punctuation types
                APPTRC(L_TRACE, "nTuplesSubmitted: " << outputMetricsRaw_[port][nTuplesSubmitted],
                       SPL_OPER_DBG);
                return;
            }
        }
        if (isSingleThreadedOnOutputs_) {
            numberPtr->store(numberPtr->load(boost::memory_order_relaxed) + 1,
                             boost::memory_order_relaxed);
        } else {
            numberPtr->fetch_add((int64_t)1, boost::memory_order_relaxed);
        }
    }

    /// Flush metrics to disk
    void flush() const;

    /// Process operator metrics that are based on a time interval
    void processIntervalMetrics(OperatorImpl const& op) const;

    /// Fill in operator metrics into the pe metrics object
    /// @param peMetrics (out parameter) pe metrics object to fill in
    /// @param op the operator that owns the operator metrics
    void getMetrics(PEMetricsInfo& peMetrics, OperatorImpl const& op, int64_t const& cost) const;

  private:
    enum
    {
        numInputPortMetrics = 12
    };
    enum
    {
        numOutputPortMetrics = 3
    };
    enum
    {
        numSystemMetrics = 1
    };

#if 0
        /// Enumerations for input port profile metrics
        enum InputPortProfMetricName { tProcess = 0, nProcess, sProcess, numInputPortProfMetrics };
        /// Enumerations for output port profile metrics
        enum OutputPortProfMetricName { tSubmit = 0, nSubmit, sSubmit, numOutputPortProfMetrics };
#endif

    void createInputPortMetric(uint32_t port,
                               InputPortMetricName name,
                               std::string const& shortName);
    void createOutputPortMetric(uint32_t port,
                                OutputPortMetricName name,
                                std::string const& shortName);
    void createSystemMetric(SystemMetricName name, std::string const& shortName);
    std::string makePortMetricDescription(uint32_t port, std::string const& longName);
    SystemMetricImpl* createPortMetric(uint32_t port,
                                       std::string const& shortName,
                                       std::string const& longName,
                                       Metric::Kind kind,
                                       boost::atomic<int64_t>* value);
    SystemMetricImpl* createItemsQueuedMetric(uint32_t port,
                                              InputPortMetricName name,
                                              std::string const& shortName,
                                              std::string const& longName,
                                              Metric::Kind kind);
    SystemMetricImpl* createRecentMaxItemsQueuedMetric(uint32_t port,
                                                       InputPortMetricName name,
                                                       std::string const& shortName,
                                                       std::string const& longName,
                                                       Metric::Kind kind);
    uint32_t getIntervalMetricsPeriod() const; // interval in seconds
    void dumpMetricAtExit(bool isSystem, Metric& metric);

    // PE/operator hosting this operator
    std::string opName_;
    uint32_t opIdx_;
    PEImpl& pe_;

    // Metric objects
    std::tr1::unordered_map<std::string, MetricImpl*> customMetrics_;
    std::vector<std::vector<SystemMetricImpl*> > inputMetrics_;  // per port
    std::vector<std::vector<SystemMetricImpl*> > outputMetrics_; // per port
    std::vector<MetricImpl*> systemMetrics_;

#define PADDING 256
    // Structures to hold perf. sensitive metrics, per-port
    struct InputPortMetricBlock
    {
        boost::atomic<int64_t> metrics_[numInputPortMetrics];
        boost::atomic<int64_t>& operator[](size_t i) { return metrics_[i]; }
        // For recentMaxItemsQueued, keep max from previous interval.
        int64_t prevRecentMaxItemsQueued_;
        // For nTuplesQueued, nWindowPunctsQueued, nFinalPunctsQueued,
        // we maintain seperate enqueue and dequeue counters,
        // so increments can be concurrent with decrements.
        // For nTuplesQueued, we also try to use separate cache lines.
        boost::atomic<int64_t> nWindowPunctsDequeued_;
        boost::atomic<int64_t> nFinalPunctsDequeued_;
        boost::atomic<int64_t> nTuplesEnqueued_ __attribute__((aligned(PADDING)));
        boost::atomic<int64_t> nTuplesDequeued_ __attribute__((aligned(PADDING)));
    } __attribute__((aligned(PADDING)));
    struct OutputPortMetricBlock
    {
        boost::atomic<int64_t> metrics_[numOutputPortMetrics];
        boost::atomic<int64_t>& operator[](size_t i) { return metrics_[i]; }
    } __attribute__((aligned(PADDING)));
#undef PADDING

#if 0
        struct InputPortProfMetricBlock
        {
            volatile int64_t metrics_[numInputPortProfMetrics];
            volatile int64_t & operator [] (size_t i) { return metrics_[i]; }
        };
        struct OutputPortProfMetricBlock
        {
            volatile int64_t metrics_[numOutputPortProfMetrics];
            volatile int64_t & operator [] (size_t i) { return metrics_[i]; }
        };
#endif

    // Local variables for perf. sensitive metrics
    InputPortMetricBlock* inputMetricsRaw_;   // per-port
    OutputPortMetricBlock* outputMetricsRaw_; // per-port

    bool isSingleThreadedOnInputs_;
    bool isSingleThreadedOnOutputs_;

    mutable Distillery::Mutex mutex_;
};
};

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_METRICS_IMPL_H */
