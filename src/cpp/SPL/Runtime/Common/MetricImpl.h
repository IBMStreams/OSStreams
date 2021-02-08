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

#ifndef SPL_RUNTIME_COMMON_METRIC_IMPL_H
#define SPL_RUNTIME_COMMON_METRIC_IMPL_H

#include <SPL/Runtime/Common/Metric.h>
#include <UTILS/SpinLock.h>
#include <UTILS/ThreadTimingInfo.h>

#include <boost/atomic/atomic.hpp>
#include <cassert>

namespace SPL {

/// Class that represents a metric object
class SystemMetricImpl;
class MetricImpl : public Metric
{
  public:
    /// Constructor
    /// @param myName metric name
    /// @param myLongName metric long name
    /// @param kind metric kind
    MetricImpl(std::string const& myName, std::string const& myLongName, Kind kind);

#if 0
        /// Constructor
        /// @param myName metric name
        /// @param myLongName metric long name
        /// @param kind metric kind
        /// @param reservoirSize number of samples to collect
        MetricImpl(std::string const & myName,
                   std::string const & myLongName,
                   Kind kind, uint32_t reservoirSize);
#endif

    /// Get the metric name
    /// @return name
    std::string const& getName() const { return name_; }

    /// Get the metric description
    /// @return description
    std::string const& getDescription() const { return longName_; }

    /// Get the metric kind
    /// @return kind
    Kind getKind() const { return kind_; }

    /// Get the metric kind as a string
    /// @return kind
    const std::string& getKindName() const { return kindMap_[kind_]; }

    /// Set the metric value (this is an atomic operation)
    /// @param v metric value
    void setValue(int64_t v) { value_.store(v, boost::memory_order_relaxed); }

    /// Set the metric value (without locking)
    /// @param v metric value
    void setValueNoLock(int64_t v) { value_.store(v, boost::memory_order_relaxed); }

    /// Get the metric value (this is an atomic operation)
    /// @return metric value
    int64_t getValue() const { return value_.load(boost::memory_order_relaxed); }

    /// Get the metric value (without locking)
    /// @return metric value
    int64_t getValueNoLock() const { return value_.load(boost::memory_order_relaxed); }

    /// Increment the metric value (this is an atomic operation)
    /// @param increment increment
    void incrementValue(int64_t increment = 1)
    {
        value_.fetch_add(increment, boost::memory_order_relaxed);
    }

    /// Increment the metric value (without locking)
    /// @param increment increment
    void incrementValueNoLock(int64_t increment = 1)
    {
        value_.store(value_.load(boost::memory_order_relaxed) + increment,
                     boost::memory_order_relaxed);
    }

    /// Destructor
    ~MetricImpl();

  private:
    friend class SystemMetricImpl;
    std::string name_;
    std::string longName_;
    Kind kind_;
    static const std::string kindMap_[];
    boost::atomic<int64_t> value_;
};

/// Class that represents a read-only system metric object
class SystemMetricImpl : public Metric
{
  public:
    /// Constructor
    /// @param myName metric name
    /// @param myLongName metric long name
    /// @param kind metric kind
    /// @param value value pointer
    SystemMetricImpl(std::string const& myName,
                     std::string const& myLongName,
                     Kind kind,
                     boost::atomic<int64_t>* myValue)
      : name_(myName)
      , longName_(myLongName)
      , kind_(kind)
      , value_(myValue)
    {}

    /// Destructor
    ~SystemMetricImpl() {}

    /// Get the metric name
    /// @return name
    std::string const& getName() const { return name_; }

    /// Get the metric description
    /// @return description
    std::string const& getDescription() const { return longName_; }

    /// Get the metric kind
    /// @return kind
    Kind getKind() const { return kind_; }

    /// Get the metric kind as a string
    /// @return kind
    const std::string& getKindName() const { return MetricImpl::kindMap_[kind_]; }

    /// Set the metric value (this is an atomic operation)
    /// @param v metric value
    void setValue(int64_t v) { assert(!"cannot happen"); }

    /// Set the metric value (without locking)
    /// @param v metric value
    void setValueNoLock(int64_t v) { assert(!"cannot happen"); }

    /// Get the metric value (this is an atomic operation)
    /// @return metric value
    int64_t getValue() const { return value_->load(boost::memory_order_relaxed); }

    /// Get the metric value (without locking)
    /// @return metric value
    int64_t getValueNoLock() const { return value_->load(boost::memory_order_relaxed); }

    /// Increment the metric value (this is an atomic operation)
    /// @param increment increment
    void incrementValue(int64_t increment = 1) { assert(!"cannot happen"); }

    /// Increment the metric value (without locking)
    /// @param increment increment
    void incrementValueNoLock(int64_t increment = 1) { assert(!"cannot happen"); }

    /// Increment the metric value (this is an atomic operation)
    /// @param increment increment
    virtual void incrementValueInternal()
    {
        value_->fetch_add((int64_t)1, boost::memory_order_relaxed);
    }

    /// Decrement the metric value (this is an atomic operation)
    /// @param increment increment
    virtual void decrementValueInternal()
    {
        value_->fetch_add((int64_t)-1, boost::memory_order_relaxed);
    }

    /// Set the metric value (this is an atomic operation)
    /// @param v metric value
    virtual void setValueInternal(int64_t v) { value_->store(v, boost::memory_order_relaxed); }

    /// Set the metric to a new maximum value (an atomic operation)
    /// @param v metric value
    virtual void setMaxValueInternal(int64_t v)
    {
        int64_t currentValue, oldValue, newValue;
        currentValue = value_->load(boost::memory_order_relaxed);
        newValue = v;
        while (newValue > currentValue) {
            // allow for concurrent updates
            // this will not be unbounded provided max has a ceiling
            oldValue = value_->exchange(newValue, boost::memory_order_relaxed);
            currentValue = newValue;
            newValue = oldValue;
        }
    }

  protected:
    std::string name_;
    std::string longName_;
    Kind kind_;
    boost::atomic<int64_t>* value_;
};

/// Class that represents a read-only system metric object
/// for a count of the number of items queued.
/// The items may be tuples or punctuations.
/// The maximum queue length can include tuples and punctuations and
/// is used to ensure the returned count does not exceed the maximum.
class ItemsQueuedSystemMetricImpl : public SystemMetricImpl
{
  public:
    /// Constructor
    /// @param myName metric name
    /// @param myLongName metric long name
    /// @param kind metric kind
    /// @param valueEn value pointer for enqueued count
    /// @param valueDe value pointer for dequeued count
    /// @param valueMx value pointer for max queue length
    ItemsQueuedSystemMetricImpl(std::string const& myName,
                                std::string const& myLongName,
                                Kind kind,
                                boost::atomic<int64_t>* valueEn,
                                boost::atomic<int64_t>* valueDe,
                                boost::atomic<int64_t>* valueMx)
      : SystemMetricImpl(myName, myLongName, kind, valueEn)
      , valueDe_(valueDe)
      , valueMx_(valueMx)
    {}

    /// Destructor
    ~ItemsQueuedSystemMetricImpl() {}

    /// Get the metric value (this is an atomic operation)
    /// @return metric value
    int64_t getValue() const
    {
        // Return num enqueued - num dequeued.
        // Ensure metric remains in bounds if there are concurrent
        // enqueues and dequeues changing the counters.
        int64_t value =
          value_->load(boost::memory_order_relaxed) - valueDe_->load(boost::memory_order_relaxed);
        if (value < 0) {
            value = 0;
        } else {
            int64_t maxValue = valueMx_->load(boost::memory_order_relaxed);
            if (value > maxValue) {
                value = maxValue;
            }
        }
        return value;
    }

    /// Get the metric value (without locking)
    /// @return metric value
    int64_t getValueNoLock() const { return getValue(); }

    /// Increment the metric value (this is an atomic operation)
    /// @param increment increment
    void incrementValueInternal() { value_->fetch_add((int64_t)1, boost::memory_order_relaxed); }

    /// Decrement the metric value (this is an atomic operation)
    /// @param increment increment
    void decrementValueInternal() { valueDe_->fetch_add((int64_t)1, boost::memory_order_relaxed); }

  private:
    boost::atomic<int64_t>* valueDe_;
    boost::atomic<int64_t>* valueMx_;
};

/// Class that represents a read-only system metric object
/// for the recent maximum of the number of items queued.
/// A maximum is kept for each of the current and previous
/// measuring intervals, and the recent maximum is calculated
/// as the maximum of the two.
class RecentMaxItemsQueuedSystemMetricImpl : public SystemMetricImpl
{
  public:
    /// Constructor
    /// @param myName metric name
    /// @param myLongName metric long name
    /// @param kind metric kind
    /// @param valueCur value pointer for current interval max queue length
    /// @param valuePrv value pointer for previous interval max queue length
    RecentMaxItemsQueuedSystemMetricImpl(std::string const& myName,
                                         std::string const& myLongName,
                                         Kind kind,
                                         boost::atomic<int64_t>* valueCur,
                                         int64_t* valuePrv)
      : SystemMetricImpl(myName, myLongName, kind, valueCur)
      , valuePrv_(valuePrv)
    {}

    /// Destructor
    ~RecentMaxItemsQueuedSystemMetricImpl() {}

    /// Get the metric value (this is an atomic operation)
    /// @return metric value
    int64_t getValue() const
    {
        // Return max of current and previous interval.
        int64_t value = value_->load(boost::memory_order_relaxed);
        if (value < *valuePrv_) {
            value = *valuePrv_;
        }
        return value;
    }

    /// Get the metric value (without locking)
    /// @return metric value
    int64_t getValueNoLock() const { return getValue(); }

  private:
    int64_t* valuePrv_;
};
};

#endif /* SPL_RUNTIME_COMMON_METRIC_IMPL_H */
