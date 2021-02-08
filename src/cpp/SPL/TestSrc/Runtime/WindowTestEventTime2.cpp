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

#include "WindowTestCommon.h"
#include "WindowTestEventTimeCommon.h"

#include <SPL/Runtime/Utility/Interval.h>
#include <SPL/Runtime/Utility/TimeUtils.h>
#include <SPL/Runtime/Window/TimeIntervalWindow.h>
#include <SPL/Runtime/Window/TimeIntervalWindowOptions.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPane.h>

#include <Runtime/Operator/EventTime/EventTimeContextImpl.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

using namespace std;
using namespace Distillery;

namespace SPL {
/*
 * Must use shared tuple pointers to store tuples in multiple timeInterval, otherwise
 * the Window::deleteWindowObjects() method will attempt to delete the same.
 * tuple pointer multiple times.
 */
typedef std::tr1::shared_ptr<EventTimeTType> EventTimeTTypePtr;

/**
 * Tests for TimeIntervalWindow of tuple shared pointers with default trigger.
 */
class WindowTestEventTime2 : public WindowTestBase
{
  public:
    WindowTestEventTime2()
      : WindowTestBase()
    {}

  private:
    /**
     * Inserts data into the specified window.
     *
     * @param win the event time window
     * @param data array of time values. Negative values are encoded into
     *        watermarks, positive time values are encoded into tuples.
     *        Must finish in 0.
     * @param partition specifies which window partition the tuple should be
     *        inserted into
     * @param offset initial time offset
     */
    static void insertData(TimeIntervalWindow<EventTimeTTypePtr>& win,
                           int64_t const* const data,
                           /* int64_t partition, */ int64_t offset)
    {
        // Insert tuples timestamped with 'data' + 'offset' values
        EventTimeTTypePtr tuple;

        for (int i = 0; data[i] != 0; ++i) {
            int64_t ts = data[i];
            if (ts > 0) {
                // Insert tuple with timestamp ts
                SPCDBG(L_INFO, "Insert tuple timestamp[" << i << "]=" << offset + ts, WINLIB_TEST);
                tuple.reset(new EventTimeTType);
                tuple->get_time() = timestamp(offset + ts, 0);
                win.insert(tuple, 0);
            } else if (ts < 0) {
                // Insert watermark with timestamp - ts + 1 millisecond
                SPCDBG(L_INFO, "Insert punct timestamp[" << i << "]=" << offset - ts, WINLIB_TEST);
                SPL::Punctuation punct(SPL::Punctuation::WatermarkMarker);
                // punctuation timestamp is set to 1 millisecond later
                SPL::EventTimeContext::impl::setWatermark(punct, timestamp(offset - ts, 1000000));
                win.insert(punct);
            }
        }
    }

    /**
     * Inserts data into the specified window.
     *
     * @param win the event time window
     * @param data array of time values. Negative values are encoded into
     *        watermarks, positive time values are encoded into tuples.
     *        Must finish in 0.
     * @param offset initial time offset
     */
    static void insertDataPartition(TimeIntervalWindow<EventTimeTTypePtr>& win,
                                    std::pair<int64_t, int64_t> const* const data,
                                    int64_t offset)
    {
        // Insert tuples timestamped with 'data' + 'offset' values
        EventTimeTTypePtr tuple;

        for (int i = 0; data[i].first != 0; ++i) {
            int64_t ts = data[i].first;
            if (ts > 0) {
                // Insert tuple with timestamp ts
                SPCDBG(L_INFO, "Insert tuple timestamp[" << i << "]=" << offset + ts, WINLIB_TEST);
                tuple.reset(new EventTimeTType);
                tuple->get_time() = timestamp(offset + ts, 0);
                win.insert(tuple, data[i].second);
            } else if (ts < 0) {
                // Insert watermark with timestamp - ts + 1 millisecond
                SPCDBG(L_INFO, "Insert punct timestamp[" << i << "]=" << offset - ts, WINLIB_TEST);
                SPL::Punctuation punct(SPL::Punctuation::WatermarkMarker);
                // punctuation timestamp is set to 1 millisecond later
                SPL::EventTimeContext::impl::setWatermark(punct, timestamp(offset - ts, 1000000));
                win.insert(punct);
            }
        }
    }

    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        {
            // Time values represent number of seconds
            TimeIntervalWindowOptions options = TimeIntervalWindowOptions()
                                                  .setIntervalDuration(3.000)
                                                  .setCreationPeriod(1.100)
                                                  .setDiscardAge(5.000)
                                                  .setIntervalOffset(0.200);

            test_winFixedEventTime_happy(oper, options);
            test_winFixedEventTime_EmptyPartitions(oper, options);
        }

        {
            TimeIntervalWindowOptions options = TimeIntervalWindowOptions()
                                                  .setIntervalDuration(3.000)
                                                  .setCreationPeriod(1.000)
                                                  .setDiscardAge(10.000);

            test_winFixedEventTime_LatePartitions(oper, options);
        }
    }

    // Happy path
    void test_winFixedEventTime_happy(Operator& oper, TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTTypePtr> handler(options);
        TimeIntervalWindow<EventTimeTTypePtr> win(oper, 0, options);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowCloseHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);

        // Insert tuples starting with time 100 (seconds)
        EventTimeTTypePtr tuple;
        unsigned tuplesToInsert = 15;
        int64_t initialSeconds = 100;
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples in TimeIntervalWindow with options "
                         << options << " starting with event time " << initialSeconds
                         << " (seconds)",
               WINLIB_TEST);

        uint64_t i = 0;
        for (; i < tuplesToInsert; ++i) {
            tuple.reset(new EventTimeTType);
            tuple->get_time() = timestamp(initialSeconds + i, 0);
            win.insert(tuple, 0);
        }

        SPCDBG(L_INFO, "Insert Watermark=" << initialSeconds + i + 1, WINLIB_TEST);
        SPL::Punctuation punct(SPL::Punctuation::WatermarkMarker);
        SPL::EventTimeContext::impl::setWatermark(punct, timestamp(initialSeconds + i + 1, 0));
        win.insert(punct);

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }

    // Insert in partition 0 then trigger the window
    void test_winFixedEventTime_EmptyPartitions(Operator& oper,
                                                TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTTypePtr> handler(options);
        TimeIntervalWindow<EventTimeTTypePtr> win(oper, 0, options);
        win.registerOnWindowTriggerHandler(&handler);

        int64_t timestamps[] = { 1, 2, -2, 3, -3, // trigger [-1..2) [0..3)
                                 0 };             // must finish in 0

        // Timestamp offset, to avoid timestamp calculation underflow
        int64_t timestampsOffset = 100;

        SPCDBG(L_INFO,
               "Insert tuples in TimeIntervalWindow with options "
                 << options << " starting with event time " << timestampsOffset << " (seconds)",
               WINLIB_TEST);

        insertData(win, timestamps, timestampsOffset);

        // A pane which triggers when it gets complete will not invoke the
        // handler for empty partitions.
        ASSERT_EQUALS(2, handler.numOnTrigger_[0]);
        ASSERT_EQUALS(0, handler.numOnTrigger_[1]);

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }

    // Insert tuples in both partitions, trigger timeInterval then insert late tuples
    // and re-trigger
    void test_winFixedEventTime_LatePartitions(Operator& oper,
                                               TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTTypePtr> handler(options);
        TimeIntervalWindow<EventTimeTTypePtr> win(oper, 0, options);
        win.registerOnWindowTriggerHandler(&handler);

        // Pairs of <tuple_time, partition>
        std::pair<int64_t, int64_t> timestamps[9];
        int i = 0;
        timestamps[i++] = std::make_pair(1, 0);  // 1->[-1..2), 1->[0..3) P0
        timestamps[i++] = std::make_pair(2, 0);  // 2->[0..3), [1..4), [2..5) P0
        timestamps[i++] = std::make_pair(2, 1);  // 2->[0..3), [1..4), [2..5) P1
        timestamps[i++] = std::make_pair(-2, 0); //   trigger [-1..2) P0
        timestamps[i++] = std::make_pair(3, 0);  // 3->[1..4), [2..5), [3..6) P0
        timestamps[i++] = std::make_pair(-3, 0); //   trigger [0..3) P0 P1
        timestamps[i++] = std::make_pair(2, 0);  // late 2->[0..3), [1..4), [2..5) P0
        timestamps[i++] = std::make_pair(-4, 0); //   re-trigger [0..3) P0, not P1
                                                 //   trigger [1..4) P0 P1
        timestamps[i++] = std::make_pair(0, 0);  // must end in 0

        // Timestamp offset, to avoid timestamp calculation underflow
        int64_t timestampsOffset = 100;

        SPCDBG(L_INFO,
               "Insert tuples in TimeIntervalWindow with options "
                 << options << " starting with event time " << timestampsOffset << " (seconds)",
               WINLIB_TEST);

        insertDataPartition(win, timestamps, timestampsOffset);

        SPCDBG(L_INFO,
               "handler.numOnTrigger_[0] =" << handler.numOnTrigger_[0]
                                            << ", handler.numOnTrigger_[1] = "
                                            << handler.numOnTrigger_[1],
               WINLIB_TEST);

        // A pane which triggers after receiving late events should invoke the
        // handler only for the updated partitions.
        ASSERT_EQUALS(4, handler.numOnTrigger_[0]);
        ASSERT_EQUALS(2, handler.numOnTrigger_[1]);

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTestEventTime2)
