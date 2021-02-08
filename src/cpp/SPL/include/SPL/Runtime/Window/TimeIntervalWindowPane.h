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

#ifndef SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWPANE_H
#define SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWPANE_H

/**
 * @file TimeIntervalWindowPane.h
 * @brief Definition of the SPL::TimeIntervalWindowPane class.
 */

#include <SPL/Runtime/Window/WindowCommon.h>
#include <SPL/Runtime/Window/TriggerInfoProvider.h>
#include <SPL/Runtime/Utility/TimeUtils.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

PUSH_DLL_PUBLIC
namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
  // Forward declarations
  template<typename T, typename G, typename D, typename S> class TimeIntervalWindow;
  template<typename T, typename G, typename D, typename S> class TimeIntervalWindowPaneImpl;
#endif

/**
 * @ingroup EventTime Window
 * @brief %Window which stores tuples with an event time which falls within
 * a specified interval.
 *
 * This window stores tuples with event time values greater or equal to the
 * window @c start and less than the window @c end.  The client does not
 * directly instantiate this class; instead, it creates an instance of
 * TimeIntervalWindow, which manages and assigns tuples to a set of window
 * time intervals.  For more details see the TimeIntervalWindowOptions class.
 *
 * @tparam T  type of the tuples stored in the window.  The %SPL operators
 *            which use event time windows use <tt>std::tr1:shared_ptr<Tuple></tt>,
 *            so the system does not have to create separate copies for tuples
 *            which are assigned to overlapping window time intervals.
 * @tparam G  type of the window partition specifier - the default value is
 *            @c int32_t
 * @tparam D  type of the tuple collection container - the default value is
 *            @c std::deque<T>
 * @tparam S  internal data structure used for storing tuples, which maps
 *            partitions to the tuple collection container - the default value
 *            is @c std::unordered_map<G, D>.
 *
 * An event-time window time interval operates by collecting tuples as they arrive.
 * When the trigger fires, the operator performs its processing against the
 * content of the window.
 *
 * A %TimeIntervalWindowPane has a @link getState() state @endlink which can
 * take one of the following values:
 *  - @link TimeIntervalWindowPane::Incomplete Incomplete @endlink - the latest inserted
 *    ::SPL::Watermark has a value which is less than the window's
 *    @link getEndTime() end time @endlink.
 *  - @link TimeIntervalWindowPane::Complete Complete @endlink - the latest inserted
 *    ::SPL::Watermark has a value which is greater than or equal to the
 *    window's @link getEndTime() end time @endlink
 *    but less than the end time plus the maximum tuple lateness as defined
 *    by the %TimeIntervalWindowOptions used for configuring the parent window.
 *  - @link TimeIntervalWindowPane::Incomplete Closed @endlink - the latest inserted
 *    ::SPL::Watermark is greater than the end time plus the maximum tuple
 *    lateness.
 *
 * %TimeIntervalWindowPane windows can be partitioned.  A partition is a set of
 * tuples that have the same values for the partitioning attributes.
 *
 * A %TimeIntervalWindowPane is triggered by default when one of the following
 * conditions is met:
 *  - a Watermark is inserted with a value greater or equal to the window's
 *    @link getEndTime() end time @endlink. The window's state transitions
 *    from State::Incomplete to State::Complete before the window triggers.
 *    A trigger event is emitted for each window partition.
 *  - a Watermark is inserted after one or more late events are inserted into
 *    the window (late events are those arriving after the window becomes
 *    %Complete).  Trigger events are emitted only for the updated partitions.
 *
 * A %TimeIntervalWindowPane fires the following @link SPL::WindowEvent
 * events @endlink:
 * - @c beforeTupleInsertionEvent - fired before a tuple is inserted into the
 *      window time interval
 * - @c afterTupleInsertionEvent - fired after a tuple is inserted into the
 *      window time interval
 * - @c windowInitialFullEvent - fired immediately after the window's state
 *      transitions from %Incomplete to %Complete
 * - @c windowTriggerEvent - fired every time when when the window triggers
 * - @c beforeWindowCloseEvent - fired right after the window's state transitions
 *      from %Complete to %Closed
 *
 * @since IBM Streams Version 4.3.0
 */
template<typename T, typename G=int32_t, typename D=std::deque<T>,
    typename S=std::tr1::unordered_map<G,D> >
class TimeIntervalWindowPane : public Window<T,G,D,S>, public TriggerInfoProvider {
public:
  /**
   * @brief The window's state.
   */
  enum State
  {
    /// The current Watermark has not passed over the end of the window.
    Incomplete = 1,
    /// The current Watermark has passed over the end of the window.
    Complete = 2,
    /// The current Watermark is beyond the maximum lateness horizon and the window ignores further input.
    Closed = 3
  };

  typedef Window<T,G,D,S> BaseClass;
  typedef typename BaseClass::TupleType TupleType;
  typedef typename BaseClass::PartitionType PartitionType;
  typedef typename BaseClass::StorageType StorageType;
  typedef typename BaseClass::DataType DataType;
  typedef typename BaseClass::WindowEventType WindowEventType;

  typedef TimeIntervalWindow<T,G,D,S> ParentWindowType;
  typedef EventTime<typename Referencer<T>::dereference_t> EventTimeType;
  typedef typename EventTimeType::EventTimeType time_point;
  typedef typename EventTimeType::EventTimeType duration;
  typedef interval<time_point> time_point_interval;

  /// @brief Factory method for creating a new %TimeIntervalWindowPane.
  static TimeIntervalWindowPane<T,G,D,S>* newInstance(
      TimeIntervalWindow<T,G,D,S> const & parent, time_point left, time_point right);

  /// @brief Destructs this instance.
  virtual ~TimeIntervalWindowPane() {}

  /// @copydoc Window::acquireData()
  ///
  /// This implementation does not acquire a lock.
  StorageType const & acquireData() const;

  /// @copydoc Window::releaseData()
  void releaseData() const;

  /// @copydoc Window::insert(TupleType const &, PartitionType const &)
  void insert(TupleType const & tuple, PartitionType const & partition=PartitionType());

  /// @copydoc Window::insert(Punctuation const &)
  void insert(Punctuation const & punct);

  /**
   * @brief Notifies the window about the existence of a watermark
   * with the given value.
   *
   * @param tp the watermark value, as a time_point in the resolution of the window.
   */
  void insert(time_point const &tp);

  /// @copydoc Window::drain()
  ///
  /// This method is not supported.
  void drain();

  /// @copydoc Window::checkpoint(Checkpoint&)
  ///
  /// This method is not supported.
  void checkpoint(Checkpoint & checkpoint) const;

  /// @copydoc Window::reset(Checkpoint&)
  ///
  /// This method is not supported.
  void reset(Checkpoint & checkpoint);

  /// @copydoc Window::resetToInitialState()
  ///
  /// This method is not supported.
  void resetToInitialState();

  /**
   * @brief Closes the window.
   *
   * A @c TimeIntervalWindowPane is closed when the current Watermark becomes
   * larger or equal to the end time of the window plus the maximum tuple
   * lateness option.  When the window is closed, it emits a
   * WindowEvent::beforeWindowClose event before changing state.
   *
   * @see SPL::TimeIntervalWindowOptions::getDiscardAge()
   */
  void close();

  /**
   * @brief Retrieves the window @c State.
   * @return the window's @c State.
   */
  State getState() const;

  /**
   * @brief Retrieves the parent window.
   *
   * The parent window is the TimeIntervalWindow instance which manages this
   * window time interval.
   * @return the parent window
   */
  ParentWindowType const & getParent() const;

  /// @copydoc TriggerInfoProvider::getStartTime()
  virtual timestamp getStartTime() const;

  /// @copydoc TriggerInfoProvider::getEndTime()
  virtual timestamp getEndTime() const;

  /// @copydoc TriggerInfoProvider::getPaneTiming()
  virtual TriggerInfoProvider::PaneTiming getPaneTiming() const;

  /// @copydoc TriggerInfoProvider::getPaneIndex()
  virtual uint64 getPaneIndex() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
  /**
   * @brief Registers a handler for the window trigger event.
   *
   * @param e the event handler, could be NULL. It replaces the existing
   *          one, if any.
   * @return the old event handler, or NULL if none was found
   */
  WindowEventType * registerOnWindowTriggerHandler(WindowEventType * e);

  /**
   * @brief Registers a handler for window initial full event.
   *
   * @param e the event handler, could be NULL. It replaces the existing
   *          one, if any.
   * @return the old event handler, or NULL if none was found
   */
  WindowEventType * registerOnWindowInitialFullHandler(WindowEventType * e);

  /**
   * @brief Registers a handler for the before window close event.
   *
   * @param e the event handler, could be NULL. It replaces the existing
   *          one, if any.
   * @return the old event handler, or NULL if none was found
   */
  WindowEventType * registerBeforeWindowCloseHandler(WindowEventType * e);

  /**
   * @brief Helper function to convert the integer value @c v into a State value.
   *
   * @throws std::domain_error if the value is out of range
   */
  static State toState(int32_t v);

protected:
  void print(std::ostream& ostream) const;
  void join();
  void start();
  void shutdown();

private:
  /*
   * @brief Constructs an instance of @c TimeIntervalWindowPane.
   *
   * @param parent the parent of this window
   * @param start the window start (point in time)
   * @param end the window end (point in time)
   * @throws SPLRuntimeInvalidWindowException if an invalid configuration
   *         is specified
   */
  TimeIntervalWindowPane(ParentWindowType const & parent, time_point start, time_point end);

  WindowEventType * onWindowTrigger_;
  WindowEventType * onWindowInitialFull_;
  WindowEventType * beforeWindowClose_;
  ParentWindowType const & parent_;
  std::auto_ptr<TimeIntervalWindowPaneImpl<T,G,D,S> > impl_;

  friend class TimeIntervalWindowPaneImpl<T,G,D,S>;
#endif  // DOXYGEN_SKIP_FOR_USERS
};
};  // end namespace SPL

/////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Window/WindowImpl.h>

#include <SPL/Runtime/Window/TimeIntervalWindow.h>
#include <SPL/Runtime/Window/TimeIntervalWindowOptions.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPaneImpl.h>

#include <sstream>
#include <stdexcept>

namespace SPL {

template<typename T, typename G, typename D, typename S>
TimeIntervalWindowPane<T,G,D,S>::TimeIntervalWindowPane(
    TimeIntervalWindow<T,G,D,S> const & parent, time_point startTime,
    time_point endTime) : Window<T,G,D,S>(),
    onWindowTrigger_(NULL), onWindowInitialFull_(NULL),
    beforeWindowClose_(NULL), parent_(parent)
{
  impl_.reset(new TimeIntervalWindowPaneImpl<T,G,D,S>(
    *this, startTime, endTime, EventTimeType::fromTimestamp(parent.getOptions().getDiscardAge())));
}

template <typename T, typename G, typename D, typename S>
TimeIntervalWindowPane<T,G,D,S>* TimeIntervalWindowPane<T,G,D,S>::
newInstance(TimeIntervalWindow<T,G,D,S> const & parent, time_point left, time_point right)
{
  std::auto_ptr<TimeIntervalWindowPane<T,G,D,S> > win(new TimeIntervalWindowPane<T,G,D,S>(parent, left, right));
  win->registerBeforeTupleInsertionHandler(parent.getBeforeTupleInsertionHandler());
  win->registerAfterTupleInsertionHandler(parent.getAfterTupleInsertionHandler());
  win->registerOnWindowTriggerHandler(parent.getOnWindowTriggerHandler());
  win->registerOnWindowInitialFullHandler(parent.getOnWindowInitialFullHandler());
  win->registerBeforeWindowCloseHandler(parent.getBeforeWindowCloseHandler());

  APPTRC(L_DEBUG, "TimeIntervalWindowPane starts at "
      << win->getStartTime() << " ends at " << win->getEndTime(), SPL_WINDOW_DBG);
  return win.release();
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::join()
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::start()
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::shutdown()
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::StorageType const & TimeIntervalWindowPane<T,G,D,S>::acquireData() const
{
  impl_->acquireData();
  return this->data_;
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::releaseData() const
{ impl_->releaseData(); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::insert(TupleType const & tuple, PartitionType const & partition)
{ impl_->insert(tuple, partition); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::insert(Punctuation const & punct)
{ impl_->insert(punct); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::insert(time_point const & tp)
{ impl_->insert(tp); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::drain()
{ impl_->drain(); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::checkpoint(Checkpoint & data) const
{ impl_->checkpoint(data); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::reset(Checkpoint & data)
{ impl_->reset(data); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::resetToInitialState()
{ impl_->resetToInitialState(); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::close()
{ impl_->close(); }

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::State TimeIntervalWindowPane<T,G,D,S>::getState() const
{ return impl_->getState(); }

template <typename T, typename G, typename D, typename S>
timestamp TimeIntervalWindowPane<T,G,D,S>::getStartTime() const
{ return impl_->getStartTime(); }

template <typename T, typename G, typename D, typename S>
timestamp TimeIntervalWindowPane<T,G,D,S>::getEndTime() const
{ return impl_->getEndTime(); }

template <typename T, typename G, typename D, typename S>
TriggerInfoProvider::PaneTiming TimeIntervalWindowPane<T,G,D,S>::getPaneTiming() const
{ return impl_->getPaneTiming(); }

template <typename T, typename G, typename D, typename S>
uint64 TimeIntervalWindowPane<T,G,D,S>::getPaneIndex() const
{ return impl_->getPaneIndex(); }

template <typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::ParentWindowType const &
TimeIntervalWindowPane<T,G,D,S>::getParent() const
{ return this->parent_; }

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::WindowEventType*
TimeIntervalWindowPane<T,G,D,S>::registerOnWindowTriggerHandler(WindowEventType* e)
{
  WindowEventType* o = onWindowTrigger_;
  onWindowTrigger_ = e;
  return o;
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::WindowEventType*
TimeIntervalWindowPane<T,G,D,S>::registerOnWindowInitialFullHandler(WindowEventType* e)
{
  WindowEventType* o = onWindowInitialFull_;
  onWindowInitialFull_ = e;
  return o;
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::WindowEventType*
TimeIntervalWindowPane<T,G,D,S>::registerBeforeWindowCloseHandler(WindowEventType* e)
{
  WindowEventType* o = beforeWindowClose_;
  beforeWindowClose_ = e;
  return o;
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindowPane<T,G,D,S>::print(std::ostream& s) const
{
  s << "TimeIntervalWindowPane={" << ')' << ' ';

  BaseClass::print(s);

#ifdef WINDOW_PRINT_HANDLERS
  s << " onWindowTrigger_=" << onWindowTrigger_ <<
      " onWindowInitialFull_=" << onWindowInitialFull_ <<
      " beforeWindowClose_=" << beforeWindowClose_;
#endif

  s << ' ';
  impl_->print(s);
  s << '}';
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindowPane<T,G,D,S>::State TimeIntervalWindowPane<T,G,D,S>::toState(int32_t v)
{
    if (v < 1 || v > 3) {
      std::stringstream s;
      s << "Argument value " << v << "must be within range [1, 3]";
      throw std::domain_error(s.str());
    }
    return static_cast<TimeIntervalWindowPane<T,G,D,S>::State>(v);
}
};

#endif // DOXYGEN_SKIP_FOR_USERS
POP_DLL_PUBLIC

#endif // SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWPANE_H
