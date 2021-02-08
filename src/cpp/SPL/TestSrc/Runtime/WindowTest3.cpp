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
 * Checkpoint and reset tests for TumblingWindow with PunctWindowPolicy
 */
class WindowTest3 : public WindowTestBase
{
  public:
    WindowTest3()
      : WindowTestBase()
    {}

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        // Window policies
        // punctuation based
        PunctWindowPolicy policy_P;

        { // Without partition eviction
            // tumbling windows, tumbling on punctuation
            test_winTumblingPunct_resetInitial(oper, policy_P);
            test_winTumblingPunct_reset(oper, policy_P);

            // summarized variant
            test_winTumblingSumPunct_resetInitial(oper, policy_P);
            test_winTumblingSumPunct_reset(oper, policy_P);
        }
    }

    /**
     * Insert n tuples.
     * Verify state
     * resetToInitialState
     * Verify state
     */
    void test_winTumblingPunct_resetInitial(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6;
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingPunctHandler handler(n / 2);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerOnEmptyWindowPunctEvent(&handler);
        win.registerSerializationHandler(&handler);

        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        TType tuple;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
            if ((i + 1) % (n / 2) == 0) {
                win.insert(Punctuation::WindowMarker);
            }
        }

        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = n;
        win.insert(tuple);

        ASSERT_EQUALS(n + 1, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(n + 1, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[0]);
        ASSERT_EQUALS(1, win.getWindowData().size());
        ASSERT_EQUALS(n, win.getWindowData().begin()->get_age());
        ASSERT_EQUALS(0, handler.numEmptyPunct_);
        win.insert(Punctuation::WindowMarker);
        win.insert(Punctuation::WindowMarker);
        ASSERT_EQUALS(1, handler.numEmptyPunct_);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();
        ASSERT_EQUALS(0, win.getWindowData().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingPunct_reset(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6; // insert n tuples
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingPunctHandler handler(n / 2);
        TType tuple;
        int64_t id = newId();

        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerOnEmptyWindowPunctEvent(&handler);
        win.registerSerializationHandler(&handler);

        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        // Timeline: t0, t1, marker, t2, t3, checkpoint, t4, marker, t5
        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
            if ((i + 1) % (n / 2) == 0) {
                win.insert(Punctuation::WindowMarker);
            }
            checkpointAtCount(win, ckptSave, i, 4);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        // Timeline: t6, reset, t7
        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = n;
        win.insert(tuple);

        ASSERT_EQUALS(n + 1, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(n + 1, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(2, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(2, handler.numAfterFlush_[0]);
        ASSERT_EQUALS(1, win.getWindowData().size());
        ASSERT_EQUALS(n, win.getWindowData().begin()->get_age());
        ASSERT_EQUALS(0, handler.numEmptyPunct_);

        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = n + 1;
        win.insert(tuple);
        ASSERT_EQUALS(3, win.getWindowData().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumPunct_resetInitial(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6;
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingPunctSumHandler handler(n / 2);

        win.registerBeforeWindowFlushHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerOnEmptyWindowPunctEvent(&handler);
        win.registerSerializationHandler(&handler);

        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        TType tuple;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
            if ((i + 1) % (n / 2) == 0) {
                win.insert(Punctuation::WindowMarker);
            }
        }
        ASSERT_EQUALS(2, handler.flushCount_);
        ASSERT_EQUALS(0, handler.numEmptyPunct_);

        SPCDBG(L_INFO, "Insert 1 WindowMarker", WINLIB_TEST);
        win.insert(Punctuation::WindowMarker);
        ASSERT_EQUALS(1, handler.numEmptyPunct_);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowData().size());

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumPunct_reset(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 6; // insert n tuples
        const int checkpointTime = 4;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingPunctSumHandler handler(n / 2);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();
        win.registerOnEmptyWindowPunctEvent(&handler);
        win.registerSerializationHandler(&handler);

        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        // Timeline: t0, t1, t2, marker, t3, t4, checkpoint, t5, marker
        SPCDBG(L_INFO, "Insert " << n << " tuples", WINLIB_TEST);
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
            if ((i + 1) % (n / 2) == 0) {
                win.insert(Punctuation::WindowMarker);
            }
            checkpointAtCount(win, ckptSave, i, checkpointTime);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        ASSERT_EQUALS(2, handler.flushCount_);
        ASSERT_EQUALS(0, handler.numEmptyPunct_);

        // Timeline: t6, reset (to point in time after t4), t5, marker
        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = n;
        win.insert(tuple);
        ASSERT_EQUALS(0, win.getWindowData().size()); // summarized window holds 0 tuples

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType>& twindow = dynamic_cast<TumblingWindow<TType>&>(win);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >& twindow =
          dynamic_cast<TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >&>(
            win);
#endif
        TumblingWindowSummarizer<TType>* summarizer = twindow.getSummarizer();
        TumblingSummarizer* mySummarizer = dynamic_cast<TumblingSummarizer*>(summarizer);
        ASSERT_EQUALS(-1, mySummarizer->getSum()); // -1 immediately after a window open event

        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_DEBUG, "Window after reset: " << win.toString(), WINLIB_TEST);

        SPCDBG(L_INFO, "Insert 1 tuple", WINLIB_TEST);
        tuple.get_age() = 5; // tuple age reset to after 'checkpointTime'
        win.insert(tuple);

        SPCDBG(L_INFO, "Insert last marker", WINLIB_TEST);
        win.insert(Punctuation::WindowMarker);

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest3)
