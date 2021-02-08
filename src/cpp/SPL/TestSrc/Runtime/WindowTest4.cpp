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
 * Checkpoint and reset tests for TumblingWindow with DeltaWindowPolicy.
 */
class WindowTest4 : public WindowTestBase
{
  public:
    // Window size - equal to the window value range as defined by its DeltaWindowPolicy
    const int winSize;

    WindowTest4()
      : WindowTestBase()
      , winSize(2)
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

        { // Without partition eviction
            // tumbling windows, tumbling on delta
            test_winTumblingDelta_resetInitial(oper, policy_Dage2);
            test_winTumblingDelta_reset(oper, policy_Dage2, 0);
            test_winTumblingDelta_reset(oper, policy_Dage2, 5);
            test_winTumblingDelta_reset(oper, policy_Dage2, 6);

            // Summarized variant
            test_winTumblingDeltaSum_resetInitial(oper, policy_Dage2);
            test_winTumblingDeltaSum_reset(oper, policy_Dage2, 0);
            test_winTumblingDeltaSum_reset(oper, policy_Dage2, 1);
            test_winTumblingDeltaSum_reset(oper, policy_Dage2, 4);
        }
    }

    void test_winTumblingDelta_resetInitial(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingDeltaHandler handler(winSize);
        int n = 4 * winSize; // insert n tuples

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        TType tuple;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }
        ASSERT_EQUALS(n, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(n, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[0]);
        ASSERT_EQUALS(2, win.getWindowData().size());
        ASSERT_TRUE(win.getWindowData().begin()->get_age() == n - 2 &&
                    win.getWindowData().rbegin()->get_age() == n - 1);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowData().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    /**
     * The test executes the following steps:
     * (1) Insert N tuples, (2) checkpoint, (3) insert more tuples, (4) reset, (5) insert tuples.
     *
     * @param oper operator
     * @param evict policy
     * @param checkpointCount number of inserted tuples after which a checkpoint is done
     */
    void test_winTumblingDelta_reset(Operator& oper,
                                     WindowPolicy const& evict,
                                     const int checkpointCount)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        SPCDBG(L_INFO,
               "Begin test_winTumblingDelta_reset() with checkpointCount = " << checkpointCount,
               WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingDeltaHandler handler(winSize);
        int n = 4 * winSize; // insert n tuples

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        // (1) Insert tuples t0, t1, ... t[checkpointCount-1]
        // (2) Checkpoint window
        // (3) Insert tuples t[checkpoint] .. t7
        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        for (int i = 0; i < n; ++i) {
            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple.get_age() = i;
            win.insert(tuple);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        ASSERT_EQUALS(n, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(n, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[0]);
        ASSERT_EQUALS(2, win.getWindowData().size());
        ASSERT_TRUE(win.getWindowData().begin()->get_age() == n - 2 &&
                    win.getWindowData().rbegin()->get_age() == n - 1);

        // (4) Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        // (5) Insert more tuples, all the way to n
        SPCDBG(L_INFO, "Insert " << n - checkpointCount << " tuple(s)", WINLIB_TEST);
        for (int i = checkpointCount; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }

        ASSERT_EQUALS(n, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(n, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[0]);
        ASSERT_EQUALS(2, win.getWindowData().size());
        ASSERT_TRUE(win.getWindowData().begin()->get_age() == n - 2 &&
                    win.getWindowData().rbegin()->get_age() == n - 1);

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingDeltaSum_resetInitial(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingDeltaSumHandler handler(winSize);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerSerializationHandler(&handler);

        TType tuple;
        for (int i = 0; i < 4 * winSize; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }
        ASSERT_EQUALS(2, handler.flushCount_);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();
        ASSERT_EQUALS(0, win.getWindowData().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    /**
     * The test executes the following steps:
     * (1) Insert N tuples, (2) checkpoint, (3) insert more tuples, (4) reset, (5) insert tuples.
     *
     * @param oper operator
     * @param evict policy
     * @param checkpointCount number of inserted tuples after which a checkpoint is done
     */
    void test_winTumblingDeltaSum_reset(Operator& oper,
                                        WindowPolicy const& evict,
                                        int const checkpointCount)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        SPCDBG(L_INFO,
               "Begin test_winTumblingDeltaSum_reset() with checkpointCount = " << checkpointCount,
               WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingDeltaSumHandler handler(winSize);
        int n = 4 * winSize; // insert n tuples

        win.registerBeforeWindowFlushHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerSerializationHandler(&handler);

        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        // (1) Insert tuples t0, t1, ... t[checkpointCount-1]
        // (2) Checkpoint window
        // (3) Insert tuples t[checkpoint] .. t7
        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        for (int i = 0; i < n; ++i) {
            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple.get_age() = i;
            win.insert(tuple);
        }
        ASSERT_EQUALS(2, handler.flushCount_);

        // Release checkpoint pointer
        ckptSave.reset();

        // (4) Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        // (5) Insert more tuples
        SPCDBG(L_INFO, "Insert " << n - checkpointCount << " tuple(s)", WINLIB_TEST);
        for (int i = checkpointCount; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }
        ASSERT_EQUALS(2, handler.flushCount_);

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest4)
