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

#ifndef SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOW_H
#define SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOW_H

/**
 * @file TimeIntervalWindow.h
 * @brief Definition of the SPL::TimeIntervalWindow class.
 */

#include <SPL/Runtime/Window/WindowCommon.h>
#include <SPL/Runtime/Window/TimeIntervalWindowOptions.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

PUSH_DLL_PUBLIC
namespace SPL {

#ifndef DOXYGEN_SKIP_FOR_USERS
  // Forward declarations
  class Operator;
  template <typename T, typename G, typename D, typename S> class TimeIntervalWindowImpl;
#endif

/**
 * @ingroup EventTime Window
 * @brief %Window which collects tuples into
 * @link SPL::TimeIntervalWindowPane window time intervals @endlink defined over
 * specific event-time intervals.
 *
 * Example: <tt> TimeIntervalWindow<TupleType, PartitionType, DataType, StorageType> </tt>
 *
 * @tparam T  type of the tuples stored in the window.  The %SPL operators
 *            which use event time windows use <tt>std::tr1::shared_ptr<Tuple></tt>,
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
 * A %TimeIntervalWindow partitions the time domain into equally sized intervals
 * of the form:
 * <tt>[N * creationPeriod + intervalOffset, (N + 1) * creationPeriod + intervalOffset + intervalDuration)</tt>
 * where @c intervalDuration, @c creationPeriod, and @c intervalOffset are
 * values set by the @link SPL::TimeIntervalWindowOptions options @endlink
 * passed to the window constructor.
 *
 * An event-time window collects tuples into fixed-duration event-time
 * intervals. The window operates by assigning tuples to the TimeIntervalWindowPane
 * instances corresponding to the time intervals the tuple belongs to.
 * A tuple may be inserted into one or more %TimeIntervalWindowPane instances.
 *
 * Clients must create windows of shared pointers to tuples (for example
 * <tt> TimeIntervalWindow<std::tr1::shared_ptr<Tuple> ></tt>), in order
 * for the system to not create tuple copies for overlapping window panes.</p>
 *
 * Clients can register event handlers for the following
 * @link SPL::WindowEvent events @endlink:
 * - @c beforeTupleInsertionEvent
 * - @c afterTupleInsertionEvent
 * - @c windowTriggerEvent
 * - @c windowInitialFullEvent
 * - @c beforeWindowCloseEvent
 *
 * Events are fired by individual window time intervals - see
 * SPL::TimeIntervalWindowPane for a detailed description.
 *
 * @since IBM Streams Version 4.3.0
 */
template <typename T, typename G=int32_t, typename D=std::deque<T>,
    typename S=std::tr1::unordered_map<G, D> >
class TimeIntervalWindow : public Window<T,G,D,S>, public WatermarkConsumer {
  typedef Window<T,G,D,S> BaseClass;

public:
  typedef typename BaseClass::TupleType TupleType;
  typedef typename BaseClass::PartitionType PartitionType;
  typedef typename BaseClass::StorageType StorageType;
  typedef typename BaseClass::DataType DataType;
  typedef typename BaseClass::WindowEventType WindowEventType;

  /**
   * @brief Constructs a @c TimeIntervalWindow.
   *
   * @param oper operator that this window is registered with
   * @param port index of the operator input port connected to this window
   * @param options window options
   */
  TimeIntervalWindow(Operator & oper, uint32_t port,
                  TimeIntervalWindowOptions const & options);

  /// @brief Destructs the window instance.
  virtual ~TimeIntervalWindow() {}

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

  /// @copydoc Window::drain()
  ///
  /// This method is not supported.
  void drain();

  /// @copydoc Window::checkpoint(Checkpoint &)
  ///
  /// This method is not supported.
  void checkpoint(Checkpoint & checkpoint) const;

  /// @copydoc Window::reset(Checkpoint &)
  ///
  /// This method is not supported.
  void reset(Checkpoint & checkpoint);

  /// @copydoc Window::resetToInitialState()
  ///
  /// This method is not supported.
  void resetToInitialState();

  /// @brief Returns the window's options.
  /// @return the window's options
  TimeIntervalWindowOptions const & getOptions() const
  { return options_; }

  /**
   * @brief Registers a handler for the window trigger event.
   *
   * @param e the event handler, could be NULL. It replaces the existing
   *        one, if any.
   * @return the old event handler, or NULL if none was found
   */
  WindowEventType* registerOnWindowTriggerHandler(WindowEventType* e);

  /**
   * @brief Registers a handler for window initial full event.
   *
   * @param e the event handler, could be NULL. It replaces the existing
   *        one, if any.
   * @return the old event handler, or NULL if none was found
   */
  WindowEventType* registerOnWindowInitialFullHandler(WindowEventType* e);

  /**
   * @brief Registers a handler for the before window close event.
   *
   * @param e the event handler, could be NULL. It replaces the existing
   *        one, if any.
   * @return the old event handler, or NULL if none was found
   */
  WindowEventType* registerBeforeWindowCloseHandler(WindowEventType * e);

  /**
   * @brief Retrieves the window trigger event handler.
   * @return the event handler, or NULL if none was registered
   */
  WindowEventType* getOnWindowTriggerHandler() const
  { return this->onWindowTrigger_; }

  /**
   * @brief Retrieves the window initial full event handler.
   * @return the event handler, or NULL if none was registered
   */
  WindowEventType* getOnWindowInitialFullHandler() const
  { return this->onWindowInitialFull_; }

  /**
   * @brief Retrieves the before window close event handler.
   * @return the event handler, or NULL if none was registered
   */
  WindowEventType* getBeforeWindowCloseHandler() const
  { return this->beforeWindowClose_; }

  /**
   * @brief Retrieves the before tuple insertion event handler.
   * @return the event handler, or NULL if none was registered
   */
  WindowEventType* getBeforeTupleInsertionHandler() const
  { return this->beforeTupleInsertion_; }

  /**
   * @brief Retrieves the after tuple insertion event handler.
   * @return the event handler, or NULL if none was registered
   */
  WindowEventType* getAfterTupleInsertionHandler() const
  { return this->afterTupleInsertion_; }

  /**
   * @copydoc Window#deleteWindowObjects()
   *
   * This is a non-virtual override of Window::deleteWindowObjects(),
   * The implementation erases all the window time intervals managed by this window.
   */
  void deleteWindowObjects();

#ifndef DOXYGEN_SKIP_FOR_USERS
protected:
  void join();
  void start();
  void shutdown();
  void print(std::ostream& ostream) const;

private:
  friend class TimeIntervalWindowImpl<T,G,D,S>;
  TimeIntervalWindowImpl<T,G,D,S> * getImplPointer();

  std::auto_ptr<WindowImpl<T,G,D,S> > impl_;
  TimeIntervalWindowOptions const options_;
  WindowEventType * onWindowTrigger_;
  WindowEventType * onWindowInitialFull_;
  WindowEventType * beforeWindowClose_;
#endif // DOXYGEN_SKIP_FOR_USERS
};
} // end namespace SPL

///////////////////////////////////////////////////////////////////////////
// Implementation
#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Window/WindowPolicyCommon.h>
#include <SPL/Runtime/Window/TimeIntervalWindowImpl.h>

#include <string>

namespace SPL {
template <typename T, typename G, typename D, typename S>
TimeIntervalWindowImpl<T,G,D,S> * TimeIntervalWindow<T,G,D,S>::getImplPointer() {
  return static_cast<TimeIntervalWindowImpl<T,G,D,S> * >(impl_.get());
}

template <typename T, typename G, typename D, typename S>
TimeIntervalWindow<T,G,D,S>::TimeIntervalWindow(Operator & oper, uint32_t port,
    TimeIntervalWindowOptions const & options) : Window<T,G,D,S>(oper, port),
    options_(options), onWindowTrigger_(NULL),
    onWindowInitialFull_(NULL), beforeWindowClose_(NULL)
{
  impl_.reset(new TimeIntervalWindowImpl<T, G, D, S>(*this));
}

template <typename T, typename G, typename D, typename S>
typename TimeIntervalWindow<T,G,D,S>::WindowEventType*
TimeIntervalWindow<T,G,D,S>::registerOnWindowTriggerHandler(WindowEventType* e)
{
  WindowEventType* o = onWindowTrigger_;
  onWindowTrigger_ = e;
  return o;
}

template <typename T, typename G, typename D, typename S>
typename TimeIntervalWindow<T,G,D,S>::WindowEventType*
TimeIntervalWindow<T,G,D,S>::registerOnWindowInitialFullHandler(WindowEventType* e)
{
  WindowEventType* o = onWindowInitialFull_;
  onWindowInitialFull_ = e;
  return o;
}

template<typename T, typename G, typename D, typename S>
typename TimeIntervalWindow<T,G,D,S>::WindowEventType*
TimeIntervalWindow<T,G,D,S>::registerBeforeWindowCloseHandler(WindowEventType* e)
{
  WindowEventType* o = beforeWindowClose_;
  beforeWindowClose_ = e;
  return o;
}

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T, G, D, S>::join()
{ getImplPointer()->join(); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T, G, D, S>::start()
{ getImplPointer()->start(); }

template<typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T, G, D, S>::shutdown()
{ impl_->shutdown(); }

template <typename T, typename G, typename D, typename S>
typename TimeIntervalWindow<T,G,D,S>::StorageType const & TimeIntervalWindow<T,G,D,S>::acquireData() const
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::releaseData() const
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::insert(TupleType const & tuple, PartitionType const & partition)
{ impl_->insert(tuple, partition); }

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::insert(Punctuation const & punct)
{ impl_->insert(punct); }

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::drain()
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::checkpoint(Checkpoint & data) const
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::reset(Checkpoint & data)
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::resetToInitialState()
{
  SPLTRACEMSGANDTHROW (SPLRuntimeInvalidOperation, L_ERROR,
      SPL_APPLICATION_RUNTIME_UNSUPPORTED_OPERATION(__FUNCTION__), SPL_WINDOW_DBG);
}

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::deleteWindowObjects()
{ getImplPointer()->deleteWindowObjects(); }

template <typename T, typename G, typename D, typename S>
void TimeIntervalWindow<T,G,D,S>::print(std::ostream& s) const
{
  s << " TimeIntervalWindow={";

  BaseClass::print(s);

  // TODO add WindowPolicy::print() and toString()

#ifdef WINDOW_PRINT_HANDLERS
  s << " onWindowTrigger_=" << (void*)onWindowTrigger_ <<
      " onWindowInitialFull_=" << (void*)onWindowInitialFull_ <<
      " beforeWindowClose_=" << (void*)beforeWindowClose_;
#endif

  s << ' ';
  impl_->print(s);
  s << '}';
}
} // end namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS
POP_DLL_PUBLIC

#endif // SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOW_H
