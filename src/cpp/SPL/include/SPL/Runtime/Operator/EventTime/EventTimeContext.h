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

#ifndef SRC_SPL_RUNTIME_OPERATOR_EVENTTIMECONTEXT_H
#define SRC_SPL_RUNTIME_OPERATOR_EVENTTIMECONTEXT_H

/**
 * @defgroup EventTime Event Time
 *
 * @brief Classes for implementing event-time processing functionality.
 *
 * Event-time is a simple model which supports streams processing where time
 * is not derived from the system time of the machine Streams is running on,
 * but from a time value associated with each tuple.  In a graph enabled for
 * event-time, tuples have an attribute which holds their time value.
 * Having a time value for each tuple enables operations such as grouping
 * tuples with a time value falling within specified time intervals, and
 * running aggregate calculations on the group.
 * <ul>
 * <li><em>Watermarks</em> keep track of event-time progress in a data stream.
 * A watermark provides a metric of event time progress in a data stream.
 * Ideally:
 *   <ul>
 *   <li>In the context of an input stream, for a watermark with time X,
 *   all tuples with event time less than X have been received.</li>
 *   <li>In the context of an output stream, for a watermark with time X,
 *   all tuples with event time less than X have been submitted.</li>
 *   </ul>
 * Note that a watermark is only an estimate of completeness. If the event
 * time value is set before ingestion, the system cannot guarantee that late
 * tuples are not observed.</li>
 *
 * <li><em>Late tuples</em> - in real life scenarios, tuples with event times
 * earlier than X may be observed after a watermark with time X.  These tuples
 * are late with respect to the watermark.</li>
 *
 * <li><em>Event-time windows</em> collect tuples along event-time boundaries
 * into window time intervals.  A window time interval is specified by its low (t0) and high
 * (t0+T) boundaries and contains all tuples with event-time values within
 * the interval <tt>[t0, t0+T)</tt>.  A new %SPL window clause is provided to
 * specify event time windows. An event-time window partitions the time
 * domain into equally sized intervals of the form:
 * <tt>[N * creationPeriod + intervalOffset, (N + 1) * creationPeriod + intervalOffset + intervalDuration)</tt>
 * where intervalDuration, creationPeriod, and intervalOffset are
 * parameters defined by the %SPL window clause. An
 * event-time window operates by assigning tuples to the window pane
 * instances corresponding to the time intervals the tuple's event time
 * belongs to.</li>
 *
 * <li><em>Default trigger policy</em> - when the input event-time (as
 * specified by the latest watermark) becomes later than the window time interval's
 * high boundary, the time interval is complete and triggers.  If tuples with
 * event times within the time interval's interval arrive after the time interval has
 * triggered (late tuples) the time interval triggers again and the operator which
 * handles the trigger event re-calculates output taking into account the
 * late data.</li>
 *
 * <li>An event time window has a <em>maximum lateness</em> horizon
 * characterized by the discardAge %SPL window clause option. Tuples
 * which arrive later than the maximum lateness value are ignored by the
 * window. The window content is deleted after the window is closed.</li>
 *
 * <li>An event-time graph starts from the output port of an <em>event-time
 * source</em> operator. The operator inserts watermarks into the output
 * stream from time to time.</li>
 * </ul>
 *
 * @since IBM Streams Version 4.3.0
 */

#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Operator/OptionalContext.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Utility/TimeUtils.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
  #include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <boost/noncopyable.hpp>
#include <string>

/**
 * @file EventTimeContext.h
 * @brief Definition of the SPL::EventTimeContext interface.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
  namespace xmlns { namespace prod { namespace streams { namespace application { class annotationType; }}}}
  namespace SPL { class OperatorContextImpl; }
#endif

namespace SPL {

/**
 * @ingroup EventTime
 * @brief Event-time optional context interface for an %SPL Operator.
 *
 * The context provides functions for event-time related processing.
 * The client acquires the context using the %EventTimeContext
 * @link SPL::EventTimeContext::NAME name @endlink when the current
 * operator is within the reachability graph of an operator annotated as
 * an event source.
 *
 * @since IBM Streams Version 4.3.0
 */
class DLL_PUBLIC EventTimeContext :
    private boost::noncopyable, public OptionalContext
{
public:
  /// The context name
  static const std::string NAME;

  /**
   * Event-time watermark processing modes.
   */
  enum WatermarkMode {
    /**
     * The operator's watermark is set by the Streams runtime as the minimum
     * of watermark values received on all input streams.
     */
    inputWatermark,

    /**
     * The operator's watermark is set by the Streams runtime based on
     * the event time of submitted tuples.
     *
     * Watermarks are generated assuming that tuples may be submitted out
     * of order, but only up to a certain interval.  After a tuple with
     * event-time @c N the operator may submit a tuple with an
     * event-time greater than or equal to <tt>N - lag</tt>, where
     * @c lag is defined by the current <tt>@eventTime</tt> SPL
     * annotation.  In order for such tuples to not be considered late, the
     * runtime sets the operator's watermark as the maximum event-time of
     * submitted tuples minus @c lag.
     */
    outputEventTime,

    /**
     * The operator's watermark is set by calling #setWatermark().
     * A new watermark is set only if it is greater than the current
     * operator watermark, otherwise it is ignored.
     */
    manual,

#ifndef DOXYGEN_SKIP_FOR_USERS
    uninitialized
#endif // DOXYGEN_SKIP_FOR_USERS
  };

  /**
   * @brief The current watermark metric name.
   *
   * This is an operator custom metric which indicates the current
   * watermark set for the operator.
   */
  static const std::string WATERMARK_METRIC_NAME;

  /// The current watermark metric description.
  static const std::string WATERMARK_METRIC_DESCRIPTION;

  /// @copydoc OptionalContext#getName()
  virtual std::string const & getName() const;

  /**
   * @brief Retrieves the %EventTimeContext for the specified operator @p op.
   *
   * A static helper which searches for the %EventTimeContext in the list of
   * optional contexts of the operator.
   * @return pointer to the operator's %EventTimeContext, or NULL if one
   * could not be found
   */
  static EventTimeContext* getContext(Operator const & op)
  { return dynamic_cast<EventTimeContext*>(
      op.getContext().getOptionalContext(EventTimeContext::NAME)); }

  /**
   * Sets the event-time operating @p mode. Should be called only while the
   * operator is being initialized.
   */
  void setMode(enum WatermarkMode mode);

  /**
   * @brief Returns the event-time operating mode.
   * @return the operating mode
   */
  enum WatermarkMode getMode() const;

  /// Sets the operator's current watermark @p value.
  void setWatermark(Watermark const & value);

  /// Gets the operator's current watermark value.
  Watermark currentWatermark() const;


#ifndef DOXYGEN_SKIP_FOR_USERS
  /// @brief Destructor.
  virtual ~EventTimeContext();

  // XXX used by the Custom operator and EventTimeWriterWindow operator
  // from the EventTimeTestToolkit.
  /**
   * @brief Extracts the @c Watermark value from a @c Punctuation.
   *
   * The caller must check that the @c Punctuation is of type
   * Punctuation::WatermarkMarker before calling this function.
   *
   * @param punct the received @c Punctuation
   * @return the received @c Watermark
   */
  static Watermark getWatermark(Punctuation const & punct);

  // XXX used by SPL Java via the JNIBridge
  /**
   * @brief Submits a @c Watermark to the specified output @c port.
   *
   * If the specified watermark is greater than the watermark of the
   * specified output port, then the system updates the output watermark
   * and submits it to the output port as a Punctuation.
   *
   * @param wm Watermark value
   * @param outputPort the index of the operator port on which the
   *    @c Watermark should be submitted
   * @return @c true if a watermark was submitted, otherwise @c false
   * @throws SPLRuntimeInvalidIndexException if the port is out of the
   *    valid range
   */
  bool submit(Watermark const & wm, uint32_t outputPort);

  // XXX used by SPL Java via the JNIBridge
  /**
   * @brief Submits a @c Watermark to all the output ports which provide
   * event time support.
   *
   * If the specified watermark is greater than the current output watermark,
   * then the system updates the output watermark and submits it to all the
   * output ports as a Punctuation.
   *
   * @param wm the Watermark
   * @see #submit(Watermark const &, uint32_t)
   */
  void submit(Watermark const & wm);

  class impl;
#endif // !DOXYGEN_SKIP_FOR_USERS

private:
  friend class OperatorContextImpl;
  friend class OperatorImpl;

  EventTimeContext(::xmlns::prod::streams::application::annotationType const & annot, OperatorContextImpl const & ctx, OperatorImpl & op);

  // Forward definition and pointer to the implementation
  std::auto_ptr<impl> impl_;
};

/// @ingroup EventTime
/// @brief Serialize the specified value.
std::ostream& operator<<(std::ostream& out, const EventTimeContext::WatermarkMode value) DLL_PUBLIC;
}
/////////////////////////////////////////////////////////////////////////////

namespace SPL {

inline std::string const & EventTimeContext::getName() const
{ return EventTimeContext::NAME; }

}

#endif /* SRC_SPL_RUNTIME_OPERATOR_EVENTTIMECONTEXT_H */
