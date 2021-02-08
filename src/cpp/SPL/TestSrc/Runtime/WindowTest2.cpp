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
 * Checkpoint and reset tests for TumblingWindow with CountWindowPolicy.
 */
class WindowTest2 : public WindowTestBase
{
  public:
    WindowTest2()
      : WindowTestBase()
    {}

  private:
    void runTests()
    {
        test_CheckpointStream();

        // window policies: count based
        Operator& oper = *((Operator*)NULL);
        CountWindowPolicy policy_C5(5);
        { // without partition eviction, tumbling windows, tumbling on count
            test_winTumblingCount_resetInitial(oper, policy_C5);
            test_winTumblingSumCount_resetInitial(oper, policy_C5);
            test_winTumblingCount_reset(oper, policy_C5);
            test_winTumblingSumCount_reset(oper, policy_C5);
        }
    }

    /**
     * Insert tuples.
     * Verify state
     * resetToInitialState
     * Insert tuples
     * Verify state
     */
    void test_winTumblingCount_resetInitial(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 5;
        const unsigned tuplesToInsert = 2 * n + n - 1;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingCountHandler handler(n);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        // Insert tuplesToInsert tuples
        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        TType tuple;
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[0]);
        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData().size());

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowData().size());

        // Insert tuplesToInsert more tuples
        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        CountWindowPolicy const& countPolicy =
          dynamic_cast<CountWindowPolicy const&>(win.getEvictionPolicy());
        unsigned numFlushes = handler.numAfterFlush_[0];
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            // Tuple age continues in sequence
            tuple.get_age() = i + numFlushes * countPolicy.getSize();
            win.insert(tuple);
        }
        // Handler's state gets reset as well
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[0]);

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumCount_resetInitial(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const unsigned n = 5;
        const unsigned tuplesToInsert = 2 * n + n - 1;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict); // summarized variant
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingSumHandler handler(n);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        TType tuple;
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
            SPCDBG(L_INFO, "  tuple age " << i, WINLIB_TEST);
        }
        ASSERT_EQUALS(2, handler.flushCount_);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
            SPCDBG(L_INFO, "  tuple age " << i, WINLIB_TEST);
        }
        ASSERT_EQUALS(2, handler.flushCount_);
        SPCDBG_EXIT(WINLIB_TEST);
    }

    /**
     * For tumbling window:
     * Insert 'tuplesToInsert' tuples.
     * Save checkpoint after inserting 'checkpointTime' tuples
     * (continue inserting tuples up to 'tuplesToInsert')
     * Reset to checkpoint
     * Verify state
     * Insert 'tuplesToInsert' tuples.
     * Verify state
     */
    void test_winTumblingCount_reset(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        const unsigned n = 5; // window size
        const unsigned tuplesToInsert = 2 * n + n - 1;
        const unsigned checkpointCount = 2 * n + 1;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingCountHandler handler(n);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        // Create checkpoint structure, insert tuples, then do checkpoint window
        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple.get_age() = i;
            win.insert(tuple);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[0]);

        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData().size());

        // Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        ASSERT_EQUALS(1, win.getWindowData().size());

        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert - checkpointCount << " tuples in window of size " << n,
               WINLIB_TEST);
        for (unsigned i = checkpointCount; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }

        // Handler resets state after the window reset
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[0]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[0]);

        SPCDBG_EXIT(WINLIB_TEST);
    }

    /**
     * For tumbling window with summarizer:
     * Insert n tuples.
     * Save checkpoint
     * Insert m tuples
     * Reset to checkpoint
     * Verify state
     */
    void test_winTumblingSumCount_reset(Operator& oper, WindowPolicy const& evict)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = 5;
        const unsigned tuplesToInsert = 2 * n + n - 1;
        const unsigned checkpointTime = 2 * n + 1;
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict); // summarized variant
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict);
#endif
        TumblingSumHandler handler(n);
        TType tuple;
        int64_t id = newId();

        win.registerBeforeWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();

        {
            CheckpointSavePtr ckptSave = getCheckpointSave(id);
            SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
                   WINLIB_TEST);
            for (unsigned i = 0; i < tuplesToInsert; ++i) {
                checkpointAtCount(win, ckptSave, i, checkpointTime);
                tuple.get_age() = i;
                win.insert(tuple);
            }
        } // end ckptSave
        ASSERT_EQUALS(2, handler.flushCount_);

        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = checkpointTime + i;
            win.insert(tuple);
        }
        ASSERT_EQUALS(2 * tuplesToInsert / n, handler.flushCount_);
        ASSERT_EQUALS(0, win.getWindowData().size()); // window with summarizer, tuple data is 0
        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_CheckpointStream()
    {
        SPCDBG_ENTER(WINLIB_TEST);
        int64_t id = newId();

        // Quick sanity check
        {
            CheckpointSavePtr ckptSave = getCheckpointSave(id);
            (*ckptSave).get() << std::string("CountTumblingWindowImpl");
        }
        {
            CheckpointPtr ckptRestore = getCheckpointRestore(id);
            std::string s1;
            *ckptRestore >> s1;
            SPCDBG(L_INFO, "Read from buffer: " << s1, WINLIB_TEST);
            ASSERT_EQUALS(std::string("CountTumblingWindowImpl"), s1);
        }
        SPCDBG_EXIT(WINLIB_TEST);
    }
}; // end WindowTest2
}; // end namespace SPL

MAIN_APP(SPL::WindowTest2)
