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
 * Checkpoint and reset tests for TumblingWindow with PunctWindowPolicy and
 * TupleCountPartitionEvictionPolicy
 */
class WindowTest13 : public WindowTestBase
{
  public:
    WindowTest13()
      : WindowTestBase()
      , tupleCount(4)
    {}
    const int tupleCount;

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        // Window policies
        // punctuation based
        PunctWindowPolicy policy_P;
        TupleCountPartitionEvictionPolicy tupleCountEviction(tupleCount,
                                                             PartitionEvictionPolicy::LRU);

        { // Without partition eviction
            // tumbling windows, tumbling on punctuation
            test_winTumblingPunct_resetInitial(oper, policy_P, tupleCountEviction);
            test_winTumblingPunct_reset(oper, policy_P, tupleCountEviction);

            // summarized variant
            test_winTumblingSumPunct_resetInitial(oper, policy_P, tupleCountEviction);
            test_winTumblingSumPunct_reset(oper, policy_P, tupleCountEviction);
        }
    }

    /**
     * Insert n tuples.
     * Verify state
     * resetToInitialState
     * Verify state
     */
    void test_winTumblingPunct_resetInitial(Operator& oper,
                                            WindowPolicy const& evict,
                                            PartitionEvictionPolicy const& partitionEviction)

    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6;
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingPunctWindowCountPartitionHandler handler(4);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerOnEmptyWindowPunctEvent(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        TType tuple;
        int32_t partition = 1;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partition);
            if ((i + 1) % (n / 2) == 0) {
                handler.expectFlush(partition);
                win.insert(Punctuation::WindowMarker);
                handler.verifyFlush();
            }
            logPartitionSizes(win);
        }

        SPCDBG(L_INFO, "Insert 6 tuples", WINLIB_TEST);
        for (int i = n; i < n + 6; ++i) {
            tuple.get_age() = i;
            if (i == n + 4) {
                handler.expectEviction(partition);
            }
            win.insert(tuple, partition);
            if (i == n + 4) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        ASSERT_EQUALS(n + 6, handler.numBeforeInsertion_[partition]);
        ASSERT_EQUALS(n + 6, handler.numAfterInsertion_[partition]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[partition]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[partition]);
        ASSERT_EQUALS(1, win.getWindowData(partition).size());
        ASSERT_EQUALS(1, win.getTupleCount(partition));

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();
        ASSERT_EQUALS(0, win.getWindowStorage().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingPunct_reset(Operator& oper,
                                     WindowPolicy const& evict,
                                     PartitionEvictionPolicy const& partitionEviction)

    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6; // insert n tuples
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingPunctWindowCountPartitionHandler handler(4);
        TType tuple;
        int64_t id = newId();

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        int32_t partition = 1;
        // Timeline: t0, t1, marker, t2, t3, checkpoint, t4, marker, t5
        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partition);
            if ((i + 1) % (n / 2) == 0) {
                handler.expectFlush(partition);
                win.insert(Punctuation::WindowMarker);
                handler.verifyFlush();
            }
            checkpointAtCount(win, ckptSave, i, 4);
            logPartitionSizes(win);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        // Timeline: t6, reset, t7
        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = n;
        win.insert(tuple, partition);

        ASSERT_EQUALS(n + 1, handler.numBeforeInsertion_[partition]);
        ASSERT_EQUALS(n + 1, handler.numAfterInsertion_[partition]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[partition]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[partition]);
        ASSERT_EQUALS(1, win.getWindowData(partition).size());
        ASSERT_EQUALS(n, win.getWindowData(partition).begin()->get_age());

        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        // The partition already has 2 tuples
        SPCDBG(L_INFO, "Insert 5 tuples", WINLIB_TEST);
        for (int i = n; i < n + 5; ++i) {
            tuple.get_age() = i;
            if (i == n + 2) {
                handler.expectEviction(partition);
            }
            win.insert(tuple, partition);
            if (i == n + 2) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumPunct_resetInitial(Operator& oper,
                                               WindowPolicy const& evict,
                                               PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6;
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingPunctWindowCountPartitionHandler handler(4);

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        TType tuple;
        int32_t partition = 1;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partition);
            if ((i + 1) % (n / 2) == 0) {
                handler.expectFlush(partition);
                win.insert(Punctuation::WindowMarker);
                handler.verifyFlush();
            }
            logPartitionSizes(win);
        }

        SPCDBG(L_INFO, "Insert 6 tuples", WINLIB_TEST);
        for (int i = n; i < n + 6; ++i) {
            tuple.get_age() = i;
            if (i == n + 4) {
                handler.expectEviction(partition);
            }
            win.insert(tuple, partition);
            if (i == n + 4) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        ASSERT_EQUALS(n + 6, handler.numBeforeInsertion_[partition]);
        ASSERT_EQUALS(n + 6, handler.numAfterInsertion_[partition]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[partition]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[partition]);
        ASSERT_EQUALS(1, win.getTupleCount(partition));

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowStorage().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumPunct_reset(Operator& oper,
                                        WindowPolicy const& evict,
                                        PartitionEvictionPolicy const& partitionEviction)

    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6; // insert n tuples
        const int checkpointTime = 4;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingPunctWindowCountPartitionHandler handler(4);

        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        int32_t partition = 1;
        // Timeline: t0, t1, t2, marker, t3, t4, checkpoint, t5, marker
        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partition);
            if ((i + 1) % (n / 2) == 0) {
                handler.expectFlush(partition);
                win.insert(Punctuation::WindowMarker);
                handler.verifyFlush();
            }
            checkpointAtCount(win, ckptSave, i, checkpointTime);
            logPartitionSizes(win);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        // Timeline: t6, reset (to point in time after t4), t5, marker
        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = n;
        win.insert(tuple, partition);

        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_DEBUG, "Window after reset: " << win.toString(), WINLIB_TEST);

        // There are already 2 tuples in the partition
        SPCDBG(L_INFO, "Insert 6 tuples", WINLIB_TEST);
        for (int i = n; i < n + 6; ++i) {
            tuple.get_age() = i;
            if (i == n + 2) {
                handler.expectEviction(partition);
            }
            win.insert(tuple, partition);
            if (i == n + 2) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest13)
