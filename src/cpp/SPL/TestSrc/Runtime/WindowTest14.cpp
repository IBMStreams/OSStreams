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
#include <SPL/Runtime/Operator/Operator.h>

using namespace std;
using namespace Distillery;

namespace SPL {

/**
 * Checkpoint and reset tests for TumblingWindow with DeltaWindowPolicy and
 * TupleCountPartitionEvictionPolicy.
 */
class WindowTest14 : public WindowTestBase
{
  public:
    // Window size - equal to the window value range as defined by its DeltaWindowPolicy
    const int winSize;
    const int tupleCount;
    WindowTest14()
      : WindowTestBase()
      , winSize(2)
      , tupleCount(5)
    {}

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        // Window policies - Delta based
        DeltaWindowPolicy<TType,           // tuple type
                          TType::age_type, // attribute type
                          &TType::get_age> // attribute getter
          policy_Dage2(winSize);
        TupleCountPartitionEvictionPolicy tupleCountEviction(tupleCount,
                                                             PartitionEvictionPolicy::LRU);

        { // Without partition eviction
            // tumbling windows, tumbling on delta
            test_winTumblingDelta_resetInitial(oper, policy_Dage2, tupleCountEviction);
            // Summarized variant
            test_winTumblingDeltaSum_resetInitial(oper, policy_Dage2, tupleCountEviction);
        }
    }

    void test_winTumblingDelta_resetInitial(Operator& oper,
                                            WindowPolicy const& evict,
                                            PartitionEvictionPolicy const& partitionEviction)

    {
        SPCDBG_ENTER(WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingDeltaWindowCountPartitionHandler handler(4);
        int n = 4 * winSize; // insert n tuples

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        TType tuple;
        int32_t partition = 1;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            if (i == 3 || i == 6) {
                handler.expectFlush(partition);
            }
            win.insert(tuple, partition);
            if (i == 3 || i == 6) {
                handler.verifyFlush();
            }
            logPartitionSizes(win);
        }
        ASSERT_EQUALS(n, handler.numBeforeInsertion_[partition]);
        ASSERT_EQUALS(n, handler.numAfterInsertion_[partition]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[partition]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[partition]);
        ASSERT_EQUALS(2, win.getWindowData(partition).size());
        ASSERT_EQUALS(2, win.getTupleCount(partition));
        ASSERT_TRUE(win.getWindowData(partition).begin()->get_age() == n - 2 &&
                    win.getWindowData(partition).rbegin()->get_age() == n - 1);

        // Partition has 2 tuples.  Insert 6 more, all with age 7,
        // and verify only one partition eviction
        SPCDBG(L_INFO, "Insert 6 tuples", WINLIB_TEST);
        for (int i = 0; i < 6; ++i) {
            tuple.get_age() = 7;
            if (i == 3) {
                handler.expectEviction(partition);
            }
            win.insert(tuple, partition);
            if (i == 3) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowStorage().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingDeltaSum_resetInitial(Operator& oper,
                                               WindowPolicy const& evict,
                                               PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingDeltaWindowCountPartitionHandler handler(winSize);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        int32_t partition = 1;
        TType tuple;
        for (int i = 0; i < 4 * winSize; ++i) {
            if (i == 3 || i == 6) {
                handler.expectFlush(partition);
            }
            tuple.get_age() = i;
            win.insert(tuple, partition);
            if (i == 3 || i == 6) {
                handler.verifyFlush();
            }
        }

        // Partition has 2 tuples.  Insert 6 more, all with age 7,
        // and verify only one partition eviction
        SPCDBG(L_INFO, "Insert 6 tuples", WINLIB_TEST);
        for (int i = 0; i < 6; ++i) {
            tuple.get_age() = 7;
            if (i == 3) {
                handler.expectEviction(partition);
            }
            win.insert(tuple, partition);
            if (i == 3) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();
        ASSERT_EQUALS(0, win.getWindowStorage().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest14)
