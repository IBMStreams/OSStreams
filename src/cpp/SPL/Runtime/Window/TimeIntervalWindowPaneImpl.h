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

#ifndef SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWPANEIMPL_H
#define SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWPANEIMPL_H

/**
 * @file TimeIntervalWindowPaneImpl.h
 * @brief Definition of the SPL::TimeIntervalWindowPaneImpl class.
 */
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Utility/TimeUtils.h>
#include <SPL/Runtime/Utility/WatermarkPunctPayload.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPane.h>
#include <SPL/Runtime/Window/TriggerInfoProvider.h>
#include <SPL/Runtime/Window/WindowImpl.h>
#include <SPL/Runtime/Window/WindowTracker.h>

#include <algorithm>
#include <tr1/unordered_map>

namespace SPL {

template<typename T, typename G, typename D, typename S>
class TimeIntervalWindowPaneImpl
  : public WindowImpl<T, G, D, S>
  , public TriggerInfoProvider
{
    typedef WindowImpl<T, G, D, S> BaseType;
    typedef typename BaseType::TupleType TupleType;
    typedef typename BaseType::PartitionType PartitionType;
    typedef typename BaseType::StorageType StorageType;
    typedef typename BaseType::DataType DataType;

    typedef TimeIntervalWindowPane<T, G, D, S> WindowType;
    typedef TimeIntervalWindow<T, G, D, S> ParentWindowType;
    typedef typename WindowType::State WindowStateType;
    typedef typename Referencer<TupleType>::dereference_t DerefTupleType;

    typedef EventTime<typename Referencer<T>::dereference_t> EventTimeType;
    typedef typename EventTimeType::EventTimeType time_point;
    typedef typename EventTimeType::EventTimeType duration;
    typedef interval<time_point> time_point_interval;

  public:
    /// @brief Constructs a TimeIntervalWindowPaneImpl.
    /// @param window window definition
    TimeIntervalWindowPaneImpl(WindowType& window,
                               time_point beginTime,
                               time_point endTime,
                               duration discardAge)
      : WindowImpl<T, G, D, S>(window)
      , window_(window)
      , startTime_(beginTime)
      , endTime_(endTime)
      , discardAge_(discardAge)
      , freshData_(false)
      , state_(WindowType::Incomplete)
      , paneTiming_(TriggerInfoProvider::paneEarly)
      , paneIndex_(0)
    {}

    virtual ~TimeIntervalWindowPaneImpl() {}

    WindowStateType getState() const { return state_; }

    timestamp getStartTime() const { return EventTime<DerefTupleType>::toTimestamp(startTime_); }

    timestamp getEndTime() const { return EventTime<DerefTupleType>::toTimestamp(endTime_); }

    TriggerInfoProvider::PaneTiming getPaneTiming() const { return paneTiming_; }

    uint64 getPaneIndex() const { return paneIndex_; }

    void insert(TupleType const& tuple, PartitionType const& partition);
    void insert(Punctuation const& punct);
    void insert(time_point const& wm);

    void emitBeforeTupleInsertionEvent(TupleType const& tuple, PartitionType const& partition);

    void emitAfterTupleInsertionEvent(TupleType& tuple, PartitionType const& partition);

    void emitAfterTupleInsertionEventForConstTuple(TupleType const& tuple,
                                                   PartitionType const& partition);

    // Emit onWindowInitialFullEvent to all partitions
    void emitOnWindowInitialFullEvent();

    // Emit onWindowTriggerEvent to updated partitions
    void emitOnWindowTriggerEvent();

    // Emit beforeWindowCloseEvent to all partitions
    void emitBeforeWindowCloseEvent();

    void print(std::ostream& stream) const;

#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = WindowImpl<T, G, D, S>::hash();

        result ^= std::tr1::hash<time_point>()(startTime_);
        result ^= std::tr1::hash<time_point>()(endTime_);
        result ^= std::tr1::hash<int64_t>()(state_);
        result ^= std::tr1::hash<duration>()(discardAge_);
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const;
    void doReset(Checkpoint& ckptStream);
    void doResetToInitialState(int cause);

  private:
    void insert(DataType& gdata, TupleType const& tuple, PartitionType const& partition);
    void complete();
    void close();

    void triggerIfPending();

    bool isTriggerPending() const { return ((state_ == WindowType::Complete) && freshData_); }

    void setTriggerPending(bool value) { freshData_ = value; }

    bool swap(std::tr1::unordered_map<PartitionType, bool>& parts,
              PartitionType const& partition,
              bool value);

    WindowType& window_; // TODO move to WindowImpl if possible
    std::tr1::unordered_map<PartitionType, bool> updatedPartitions_;
    time_point startTime_;
    time_point endTime_;
    duration discardAge_;
    bool freshData_;
    WindowStateType state_;
    TriggerInfoProvider::PaneTiming paneTiming_;
    uint64 paneIndex_;
    Distillery::Mutex mutex_;
};

/////////////////////////////////////////////////////////////////////////////
#include <SPL/Runtime/Window/DeltaWindowPolicy.h>

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::insert(TupleType const& tuple,
                                                    PartitionType const& partition)
{
    Distillery::AutoMutex am(mutex_);
#ifndef NDEBUG
    DerefTupleType const& tupled = Referencer<TupleType>::dereference(tuple);
    assert(EventTimeType::get(tupled) >= startTime_);
    assert(EventTimeType::get(tupled) < endTime_);
#endif

    // Perform any actions necessary before inserting a tuple
    this->beforeTupleInserted();

    DataType& gdata = this->insertPartition(partition);
    insert(gdata, tuple, partition);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::insert(Punctuation const& punct)
{
    assert(punct.getValue() == Punctuation::WatermarkMarker);
    time_point wm(
      EventTimeType::fromTimestamp(WatermarkPunctPayload::getPayload(punct)->getValue()));
    insert(wm);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::insert(time_point const& wm)
{
    Distillery::AutoMutex am(mutex_);
    if (wm >= endTime_) {
        this->complete();
        if (paneTiming_ == TriggerInfoProvider::paneEarly) {
            paneTiming_ = TriggerInfoProvider::paneOnComplete;
        }
    }

    this->triggerIfPending();

    if (wm >= endTime_ + discardAge_) {
        this->close();
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::emitBeforeTupleInsertionEvent(
  TupleType const& tuple,
  PartitionType const& partition)
{
    if (window_.beforeTupleInsertion_) {
        AutoSetWindow asw(&window_);
        window_.beforeTupleInsertion_->beforeTupleInsertionEvent(window_, tuple, partition);
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::emitAfterTupleInsertionEvent(
  TupleType& tuple,
  PartitionType const& partition)
{
    if (window_.afterTupleInsertion_) {
        AutoSetWindow asw(&window_);
        window_.afterTupleInsertion_->afterTupleInsertionEvent(window_, tuple, partition);
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::emitAfterTupleInsertionEventForConstTuple(
  TupleType const& tuple,
  PartitionType const& partition)
{
    if (window_.afterTupleInsertion_) {
        AutoSetWindow asw(&window_);
        TupleType ctuple(tuple);
        window_.afterTupleInsertion_->afterTupleInsertionEvent(window_, ctuple, partition);
    }
}

// Emit onWindowInitialFullEvent to all partitions
template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::emitOnWindowInitialFullEvent()
{
    if (window_.onWindowInitialFull_) {
        AutoSetWindow asw(&window_);
        for (typename StorageType::iterator it = this->data_.begin(); it != this->data_.end();
             it++) {
            PartitionType const& partition = it->first;
            window_.onWindowInitialFull_->onWindowInitialFullEvent(window_, partition);
        }
    }
}

// Emit onWindowTriggerEvent to all partitions
template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::emitOnWindowTriggerEvent()
{
    if (window_.onWindowTrigger_) {
        AutoSetWindow asw(&window_);
        for (typename StorageType::iterator it = this->data_.begin(); it != this->data_.end();
             it++) {
            PartitionType const& partition = it->first;
            // Trigger only partitions which have been updated
            bool triggerPending = false;
            triggerPending = swap(updatedPartitions_, partition, triggerPending);
            if (triggerPending) {
                window_.onWindowTrigger_->onWindowTriggerEvent(window_, partition);
            }
        }
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::emitBeforeWindowCloseEvent()
{
    if (window_.beforeWindowClose_) {
        AutoSetWindow asw(&window_);
        for (typename StorageType::iterator it = this->data_.begin(); it != this->data_.end();
             it++) {
            PartitionType const& partition = it->first;
            window_.beforeWindowClose_->beforeWindowCloseEvent(window_, partition);
        }
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::print(std::ostream& stream) const
{
    stream << "TimeIntervalWindowPaneImpl={";

    // Base class first
    WindowImpl<T, G, D, S>::print(stream);

    stream << ", beginTime_=" << startTime_ << ", endTime_=" << endTime_ << ", state_=" << state_;
    stream << '}';
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::doCheckpoint(Checkpoint& ckptStream) const
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidOperation, L_ERROR,
                        SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__),
                        SPL_WINDOW_DBG);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::doReset(Checkpoint& ckptStream)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidOperation, L_ERROR,
                        SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__),
                        SPL_WINDOW_DBG);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::doResetToInitialState(int cause)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidOperation, L_ERROR,
                        SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__),
                        SPL_WINDOW_DBG);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::insert(DataType& gdata,
                                                    TupleType const& tuple,
                                                    PartitionType const& partition)
{
    // insertion
    this->emitBeforeTupleInsertionEvent(tuple, partition);
    gdata.push_back(tuple);
    updatedPartitions_[partition] = true;
    this->emitAfterTupleInsertionEvent(gdata.back(), partition);
    this->setTriggerPending(true);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::complete()
{
    if (state_ == WindowType::Incomplete) {
        state_ = WindowType::Complete;
        this->emitOnWindowInitialFullEvent();
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::triggerIfPending()
{
    if (this->isTriggerPending()) {
        this->emitOnWindowTriggerEvent();
        this->setTriggerPending(false);
        paneTiming_ = TriggerInfoProvider::paneLate;
        ++paneIndex_;
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPaneImpl<T, G, D, S>::close()
{
    if (state_ == WindowType::Complete) {
        this->emitBeforeWindowCloseEvent();
        state_ = WindowType::Closed;
    }
}

template<typename T, typename G, typename D, typename S>
bool TimeIntervalWindowPaneImpl<T, G, D, S>::swap(
  std::tr1::unordered_map<PartitionType, bool>& parts,
  PartitionType const& partition,
  bool value)
{
    typename std::tr1::unordered_map<PartitionType, bool>::iterator elem =
      updatedPartitions_.find(partition);
    if (updatedPartitions_.end() != elem) {
        bool oldValue = false;
        std::swap(elem->second, oldValue);
        return oldValue;
    } else {
        THROW(SPLRuntimeAssertionFailed,
              "Partition " << partition << " was expected in window " << window_.toString());
    }
}
} // end namespace SPL

#endif // SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWPANEIMPL_H
