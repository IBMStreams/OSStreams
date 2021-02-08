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

#ifndef SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWIMPL_H
#define SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWIMPL_H

/**
 * @file TimeIntervalWindowImpl.h
 * @brief Definition of the SPL::TimeIntervalWindowImpl class.
 */
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Utility/TimeUtils.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPane.h>
#include <SPL/Runtime/Window/WindowImpl.h>

#include <tr1/memory>
#include <tr1/unordered_map>
#include <vector>

namespace SPL {

template<typename T, typename G, typename D, typename S>

class TimeIntervalWindowImpl : public WindowImpl<T, G, D, S>
{
    typedef WindowImpl<T, G, D, S> BaseType;
    typedef typename BaseType::TupleType TupleType;
    typedef typename BaseType::PartitionType PartitionType;
    typedef typename BaseType::StorageType StorageType;
    typedef typename BaseType::DataType DataType;

    typedef TimeIntervalWindow<T, G, D, S> WindowType;
    typedef TimeIntervalWindowPane<T, G, D, S> WindowPaneType;

    typedef EventTime<typename Referencer<T>::dereference_t> EventTimeType;
    typedef typename EventTimeType::EventTimeType time_point;
    typedef typename EventTimeType::EventTimeType duration;
    typedef interval<time_point> time_point_interval;

    typedef std::tr1::shared_ptr<WindowPaneType> WindowPanePtrType;
    typedef std::tr1::unordered_map<time_point_interval, WindowPanePtrType> Interval2PaneType;
    typedef typename std::tr1::unordered_map<time_point_interval, WindowPanePtrType>::iterator
      Interval2PaneIteratorType;
    typedef typename std::tr1::unordered_map<time_point_interval, WindowPanePtrType>::const_iterator
      Interval2PaneConstIteratorType;

  public:
    /**
     * @brief Constructs a new @c TimeIntervalWindowImpl with the specified
     *      configuration.
     *
     * @param intervalDuration duration between the lower and upper window boundary
     * @param creationPeriod  duration between the lower bounds of adjacent windows
     * @param intervalOffset  point in time when one of the windows starts
     *            (corresponds to its lower bound).  Default value is @c 0.
     */
    TimeIntervalWindowImpl(WindowType& window)
      : WindowImpl<T, G, D, S>(window)
      , intervalDuration_(EventTimeType::fromTimestamp(window.getOptions().getIntervalDuration()))
      , creationPeriod_(EventTimeType::fromTimestamp(window.getOptions().getCreationPeriod()))
      , intervalOffset_(EventTimeType::fromTimestamp(window.getOptions().getIntervalOffset()))
      , discardAge_(EventTimeType::fromTimestamp(window.getOptions().getDiscardAge()))
      , currentWatermark_(std::numeric_limits<time_point>::min())
      , currentDropPoint_(std::numeric_limits<time_point>::min())
      ,
      // TODO create time type-independent minimum value
      window_(window)
    {
        // TODO check overflow of intervalDuration, creationPeriod, intervalOffset, discardAge if
        // EventTimeType is int32.
        // This should probably be an exception.
    }

    /// @brief Destructs the instance.
    virtual ~TimeIntervalWindowImpl() {}

    /**
     * @brief Inserts a tuple into the window.
     *
     * The tuple time is converted to ticks before determining the
     * window(s) where the tuple is going to be inserted.
     *
     * @param tuple  tuple to be inserted
     * @param eventTime  the event time associated with the tuple
     * @param partition  the partition the @a tuple belongs to (optional)
     */
    void insert(TupleType const& tuple, PartitionType const& partition = PartitionType());

    /**
     * @brief Notify the window about the existence of a punctuation in the stream.
     * @param punct the punctuation that is received
     *
     * If a watermark punctuation, its value is converted to milliseconds before
     * notifying the window panes.
     */
    void insert(Punctuation const& punct);

    /// @copydoc
    void print(std::ostream& stream) const;

    void deleteWindowObjects();

    /// Invoked by Operator via BaseWindow::join()
    void join() {}

    /// Invoked by Operator via BaseWindow::start()
    void start() {}

  private:
    /**
     * @brief Creates a list of window time intervals containing the given
     * @c time_point.
     *
     * Returns a list of intervals corresponding to the windows a
     * tuple with the specified event time belongs to.
     *
     * @param      tp time point
     * @param[out] intervals vector where the window time intervals are added
     */
    void assign(time_point tp, std::vector<time_point_interval>& intervals);

    /**
     * @brief Returns the start of the latest window pane that contains the
     * specified @c instant.
     *
     * All calculations are done after converting the time values to ticks.
     *
     * @param instant the time for which to calculate the last window start
     * @return the window start or std::numeric_limits<time_point>::max() if the value
     * cannot be calculated (e.g. it would cause overflow or underflow)
     */
    time_point lastStartFor(time_point instant);

    void insertTuple(TupleType const& tuple,
                     PartitionType const& partition,
                     time_point_interval const& interval);

    void printPanes(std::ostream& stream) const;

    /**
     * Remove window referenced by iterator if closed, return next iterator.
     */
    Interval2PaneConstIteratorType eraseClosed(Interval2PaneConstIteratorType it)
    {
        if (it->second->getState() == WindowPaneType::Closed) {
            it = windows_.erase(it);
        } else {
            it++;
        }
        return it;
    }

    /**
     * Returns iterator to window corresponding to the specified interval.
     * If a new window was created, sets newWindow true.
     *
     * @param interval the time interval corresponding to the window
     * @param[out] newWindow on return it is set to true if a new window was
     *     created, otherwise it is left unchanged
     */
    Interval2PaneIteratorType getWindow(time_point_interval const& interval, bool& newWindow);

    /**
     * @brief Determines if the current event time @c t is droppable.
     *
     * An event time value is droppable if it can be ignored by all the window
     * panes.
     * @return @c true if the time is droppable, otherwise @c false
     */
    bool isDroppable(time_point t) { return t < currentDropPoint_; }

    /**
     * @brief Determines if the current event time @c t is late.
     *
     * An event time value is late if it is earlier than the current window
     * watermark.
     * @return @c true if the time is late, otherwise @c false
     */
    bool isLate(time_point t) { return t < currentWatermark_; }

    /**
     * @brief Advance current watermark to the the specified value if it
     * is larger.
     *
     * @param wm the new watermark value
     * @return @c true if watermark has advanced, otherwise @c false
     */
    bool advanceWatermark(time_point wm)
    {
        if (wm > currentWatermark_) {
            currentWatermark_ = wm;
            // avoid underflow
            currentDropPoint_ =
              (currentWatermark_ >= discardAge_) ? (currentWatermark_ - discardAge_) : 0;
            return true;
        }
        return false;
    }

    // Duration between the lower and upper window boundary
    const duration intervalDuration_;

    // Amount of time between adjacent (in time) windows.
    const duration creationPeriod_;

    // Point in time when one of the window panes starts (corresponds to the
    // lower bound of the window pane).
    // Windows start at time <tt>N * start_ + intervalOffset_</tt>.
    const duration intervalOffset_;

    // Duration between the upper window boundary and the point in time when it closes
    const duration discardAge_;

    // The watermark most recently received by the window
    // The drop point is a time for which tuples received by the window with
    // an event time earlier than this value are ignored (dropped).
    time_point currentWatermark_;
    time_point currentDropPoint_;

    // Interval to window pane map
    Interval2PaneType windows_;

    WindowType& window_;
};

} // end namespace SPL

/////////////////////////////////////////////////////////////////////////////
#include <SPL/Runtime/Window/DeltaWindowPolicy.h>
#include <cassert>
#include <limits>

namespace SPL {

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::insert(TupleType const& tuple,
                                                PartitionType const& partition)
{
    time_point eventTime = EventTimeType::get(Referencer<T>::dereference(tuple));

    if (isDroppable(eventTime)) {
        APPTRC(L_DEBUG, "Received and dropped tuple with event time " << eventTime, SPL_WINDOW_DBG);
        return;
    }

    if (isLate(eventTime)) {
        APPTRC(L_DEBUG, "Received late tuple with event time " << eventTime, SPL_WINDOW_DBG);
        // continue
    }

    std::vector<time_point_interval> intervals;
    assign(eventTime, intervals);

    // For each interval, insert tuple into the corresponding window
    for (typename std::vector<time_point_interval>::const_iterator it = intervals.begin();
         it != intervals.end(); it++) {
        // Create window pane if it doesn't exist
        insertTuple(tuple, partition, *it);
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::insert(Punctuation const& punct)
{
    if (punct.getValue() != Punctuation::WatermarkMarker) {
        APPTRC(L_DEBUG, "Window ignores punctuation " << punct, SPL_WINDOW_DBG);
        return;
    }

    Watermark wm = WatermarkPunctPayload::getPayload(punct)->getValue();
    time_point tp = EventTimeType::fromTimestamp(wm);
    if (advanceWatermark(tp)) {
        // Broadcast watermark to windows
        // TODO use smarter algorithm to reduce the number of notified windows
        // based on the fact that punctuations are monotonically (strictly) increasing.
        for (Interval2PaneConstIteratorType it = windows_.begin(); it != windows_.end();
             it = eraseClosed(it)) {
            it->second->insert(tp);
        }
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::print(std::ostream& stream) const
{
    stream << "TimeIntervalWindowImpl={";

    // Base class first
    WindowImpl<T, G, D, S>::print(stream);

    stream << " intervalDuration=" << intervalDuration_ << ", creationPeriod=" << creationPeriod_
           << ", intervalOffset=" << intervalOffset_;
    stream << "]=";
    printPanes(stream);
    stream << '}';
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::assign(time_point tp,
                                                std::vector<time_point_interval>& intervals)
{
    // the most recent start of an interval containing tp
    time_point lastStart = lastStartFor(tp);
    if (lastStart == std::numeric_limits<time_point>::max()) {
        APPTRC(L_WARN,
               "Cannot assign timepoint "
                 << tp << " to window because the system could not calculate "
                 << "the start of the most recent window pane that contains this value "
                 << "without causing underflow or overflow",
               SPL_WINDOW_DBG);
        return;
    }

    // lower bound for an interval's start
    time_point minStart = (tp > intervalDuration_) ? tp - intervalDuration_ : 0;

    // for each interval containing tp do: add tp to interval
    for (time_point paneStart = lastStart; paneStart > minStart; paneStart -= creationPeriod_) {
        if (paneStart > std::numeric_limits<time_point>::max() - intervalDuration_) {
            APPTRC(L_WARN,
                   "Cannot create interval [" << paneStart << ", ...) for time point " << tp
                                              << " as it overflows on its upper endpoint",
                   SPL_WINDOW_DBG);
        } else {
            time_point_interval interval(paneStart, intervalDuration_);
            intervals.push_back(interval);
        }

        if (paneStart < creationPeriod_) {
            break; // start - creationPeriod_ would underflow
        }
    }
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowImpl<T, G, D, S>::time_point
TimeIntervalWindowImpl<T, G, D, S>::lastStartFor(time_point instant)
{

    if ((instant < intervalOffset_) ||
        (std::numeric_limits<time_point>::max() - instant < creationPeriod_)) {
        // calculations would cause overflow or underflow
        return time_point(std::numeric_limits<time_point>::max());
    }

    assert(instant + creationPeriod_ >= intervalOffset_);

    /*
     * We assume the compiler may evaluate operands of a commutative operation in any order:
     * 'instant + creationPeriod_ - intervalOffset_' may overflow, while
     * 'instant - intervalOffset_ + creationPeriod_' may not.
     */
    duration fromWinStartToTp = (instant - intervalOffset_ + creationPeriod_) % creationPeriod_;

    return (instant < fromWinStartToTp) ? time_point(std::numeric_limits<time_point>::max())
                                        : // calculation would cause underflow
             time_point(instant - fromWinStartToTp);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::insertTuple(TupleType const& tuple,
                                                     PartitionType const& partition,
                                                     time_point_interval const& interval)
{
    bool newWindow = false;
    Interval2PaneIteratorType it = windows_.end();
    try {
        // Lazily create a new window if needed
        it = getWindow(interval, newWindow);
        it->second->insert(tuple, partition);
    } catch (...) {
        // undo the successful steps then rethrow
        if (newWindow) {
            windows_.erase(it);
        }
        throw;
    }
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowImpl<T, G, D, S>::Interval2PaneIteratorType
TimeIntervalWindowImpl<T, G, D, S>::getWindow(time_point_interval const& interval, bool& newWindow)
{
    Interval2PaneIteratorType it = windows_.find(interval);
    if (it == windows_.end()) {
        WindowPanePtrType win(
          WindowPaneType::newInstance(window_, interval.left(), interval.right()));
        std::pair<Interval2PaneIteratorType, bool> result =
          windows_.insert(std::make_pair<time_point_interval, WindowPanePtrType>(interval, win));
        newWindow = result.second;
        return result.first;
    } else {
        return it;
    }
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::printPanes(std::ostream& stream) const
{
    stream << "windows_={size=" << windows_.size() << ", {";
    for (Interval2PaneConstIteratorType it = windows_.begin(); it != windows_.end(); ++it) {
        stream << "(" << it->first << ", " << it->second << "), ";
    }
    stream << "}}";
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowImpl<T, G, D, S>::deleteWindowObjects()
{
    for (Interval2PaneConstIteratorType it = windows_.begin(); it != windows_.end(); ++it) {
        it->second->deleteWindowObjects();
    }
}
} // end namespace SPL

#endif // SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWIMPL_H
