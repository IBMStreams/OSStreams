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

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Utility/TimeUtils.h>
#include <SPL/Runtime/Window/TimeIntervalWindow.h>
#include <SPL/Runtime/Window/TimeIntervalWindowOptions.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPane.h>

#include <Runtime/Operator/EventTime/EventTimeContextImpl.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

using namespace std;
using namespace Distillery;

namespace SPL {

/**
 * Tests for TimeIntervalWindow with default trigger.
 */
class WindowTestEventTime1 : public WindowTestBase
{
  public:
    WindowTestEventTime1()
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
     * @param offset initial time offset
     */
    static void insertData(TimeIntervalWindow<EventTimeTType>& win,
                           int64_t const* const data,
                           int64_t offset)
    {
        // Insert tuples timestamped with 'data' + 'offset' values
        EventTimeTType tuple;

        for (int i = 0; data[i] != 0; ++i) {
            int64_t ts = data[i];
            if (ts > 0) {
                // Insert tuple with timestamp ts
                SPCDBG(L_INFO, "Insert tuple timestamp[" << i << "]=" << offset + ts, WINLIB_TEST);
                tuple.get_time() = timestamp(offset + ts, 0);
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

    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        {
            // Time values represent number of milliseconds
            TimeIntervalWindowOptions options = TimeIntervalWindowOptions()
                                                  .setIntervalDuration(3.000)
                                                  .setCreationPeriod(1.100)
                                                  .setDiscardAge(5.000)
                                                  .setIntervalOffset(0.200);

            test_winFixedEventTime_unsupported(oper, options);
            test_winFixedEventTime_underflow(oper, options);
            test_winFixedEventTime_overflow(oper, options);
            test_winFixedEventTime_happy(oper, options);
        }
        {
            // Time values represent number of seconds
            TimeIntervalWindowOptions options = TimeIntervalWindowOptions()
                                                  .setIntervalDuration(3.000)
                                                  .setCreationPeriod(1.000)
                                                  .setDiscardAge(5.000)
                                                  .setIntervalOffset(0);

            test_winFixedEventTime_late(oper, options);
        }
    }

    void test_winFixedEventTime_underflow(Operator& oper, TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTType> handler(options);
        TimeIntervalWindow<EventTimeTType> win(oper, 0, options);

        SPCDBG(L_TRACE, "handler=" << std::ios_base::hex << (void*)(&handler) << std::ios_base::dec,
               SPL_WINDOW_DBG);

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowCloseHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);

        SPCDBG(L_TRACE, "Window=" << &win << ' ' << win.toString(), WINLIB_TEST);

        // Insert tuples with time around zero - test the underflow checking logic
        EventTimeTType tuple;
        unsigned tuplesToInsert = 15;
        int64_t initialSeconds = 0;
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into TimeIntervalWindow with options "
                         << options << " starting with event time " << initialSeconds
                         << " (seconds)",
               WINLIB_TEST);

        for (uint64_t i = 0; i < tuplesToInsert; ++i) {
            tuple.get_time() = timestamp(initialSeconds + i, 0);
            win.insert(tuple, 0);
        }

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winFixedEventTime_overflow(Operator& oper, TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTType> handler(options);
        TimeIntervalWindow<EventTimeTType> win(oper, 0, options);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowCloseHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);

        // Insert tuples with time (seconds) around 2^63 - test the overflow checking logic
        EventTimeTType tuple;
        unsigned tuplesToInsert = 15;
        int64_t initialSeconds = (std::numeric_limits<int64_t>::max()) / 500 - 5;
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into TimeIntervalWindow with options "
                         << options << " starting with event time " << initialSeconds
                         << " (seconds)",
               WINLIB_TEST);

        for (uint64_t i = 0; i < tuplesToInsert; ++i) {
            tuple.get_time() = timestamp(initialSeconds + i, 500000000);
            win.insert(tuple, 0);
        }

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }

    // Happy path
    void test_winFixedEventTime_happy(Operator& oper, TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTType> handler(options);
        TimeIntervalWindow<EventTimeTType> win(oper, 0, options);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowCloseHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);

        // Insert tuples starting with time 100 (seconds)
        EventTimeTType tuple;
        unsigned tuplesToInsert = 15;
        int64_t initialSeconds = 100;
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into TimeIntervalWindow with options "
                         << options << " starting with event time " << initialSeconds
                         << " (seconds)",
               WINLIB_TEST);

        for (uint64_t i = 0; i < tuplesToInsert; ++i) {
            tuple.get_time() = timestamp(initialSeconds + i, 0);
            win.insert(tuple, 0);
        }

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }

    // Late events
    void test_winFixedEventTime_late(Operator& oper, TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        EventTimeFixedWindowHandler<EventTimeTType> handler(options);
        TimeIntervalWindow<EventTimeTType> win(oper, 0, options);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowCloseHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);

        // Negative time values are encoded into watermarks,
        // positive time values are encoded into tuples
        int64_t timestamps[] = { 1, 2, -2, 3,
                                 -3, // trigger [-1..2) [0..3)
                                 8, 9, -5, 10, 11,
                                 -9, // trigger [1..4) [2..5), [6..9) [3..6) close [-1..2) [0..3)
                                 11, 13, -10,
                                 -12, // trigger [7..10) [8..11) [9..12), close [2..5) [3..6)
                                 5,   // ignored
                                 8,   //
                                 -18, // re-trigger [6..9) [7..10)
                                      // trigger [8..11) [10..13) [11..14) [12..15) [13..16)
                                      // close [6..9) [7..10) [9..12)
                                 -20, // close [11..14) [12..15)
                                 0 }; // must finish in 0

        // Timestamp offset, to avoid timestamp calculation underflow
        int64_t timestampsOffset = 100;

        SPCDBG(L_INFO,
               "Insert tuples into TimeIntervalWindow with options "
                 << options << " starting with event time " << timestampsOffset << " (seconds)",
               WINLIB_TEST);

        insertData(win, timestamps, timestampsOffset);

        win.deleteWindowObjects();
        SPCDBG_EXIT(WINLIB_TEST);
    }

    // Unsupported methods
    void test_winFixedEventTime_unsupported(Operator& oper,
                                            TimeIntervalWindowOptions const& options)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        TimeIntervalWindow<EventTimeTType> win(oper, 0, options);

        try {
            win.acquireData();
            FAIL("win.acquireData() expected to fail");
        } catch (SPLRuntimeInvalidOperationException const& e) {
            SPCDBG(L_INFO, "Expected exception: " << e.getExplanation(), WINLIB_TEST);
        }

        try {
            win.releaseData();
            FAIL("win.releaseData() expected to fail");
        } catch (SPLRuntimeInvalidOperationException const& e) {
            SPCDBG(L_INFO, "Expected exception: " << e.getExplanation(), WINLIB_TEST);
        }

        try {
            win.drain();
            FAIL("win.drain() expected to fail");
        } catch (SPLRuntimeInvalidOperationException const& e) {
            SPCDBG(L_INFO, "Expected exception: " << e.getExplanation(), WINLIB_TEST);
        }

        try {
            win.resetToInitialState();
            FAIL("win.resetToInitialState() expected to fail");
        } catch (SPLRuntimeInvalidOperationException const& e) {
            SPCDBG(L_INFO, "Expected exception: " << e.getExplanation(), WINLIB_TEST);
        }

        try {
            win.checkpoint(getCheckpointSave(0)->get());
            FAIL("win.checkpoint() expected to fail");
        } catch (SPLRuntimeInvalidOperationException const& e) {
            SPCDBG(L_INFO, "Expected exception: " << e.getExplanation(), WINLIB_TEST);
        }

        try {
            win.reset(*(getCheckpointRestore(0).get()));
            FAIL("win.reset() expected to fail");
        } catch (SPLRuntimeInvalidOperationException const& e) {
            SPCDBG(L_INFO, "Expected exception: " << e.getExplanation(), WINLIB_TEST);
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTestEventTime1)
