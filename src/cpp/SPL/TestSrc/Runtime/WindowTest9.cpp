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
 * Checkpoint and reset tests for TumblingWindow with CountWindowPolicy and
 * PartitionAgePartitionEvictionPolicy.
 */
class WindowTest9 : public WindowTestBase
{
  public:
    const int winSize; // Window size - defined by its eviction policy
    const double age;

    WindowTest9()
      : WindowTestBase()
      , winSize(5)
      , age(1.1)
    {}

  private:
    void runTests()
    {
        // window policies: count based
        Operator& oper = *((Operator*)NULL);
        CountWindowPolicy policy_C5(winSize);
        PartitionAgePartitionEvictionPolicy ageEviction(age);

        { // with partition eviction, tumbling windows, tumbling on count
            test_winTumblingCount_resetInitial(oper, policy_C5, ageEviction);
            test_winTumblingCount_reset(oper, policy_C5, ageEviction);
        }
    }

    /**
     * Insert tuples.
     * Verify state
     * resetToInitialState
     * Insert tuples
     * Verify state
     */
    void test_winTumblingCount_resetInitial(Operator& oper,
                                            WindowPolicy const& windowEviction,
                                            PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int n = winSize;
        const unsigned tuplesToInsert = 2 * n + n - 1;
        const int partitionZero = 0;
        const int partitionOne = 1;
        TType tuple;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, windowEviction, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, windowEviction, partitionEviction);
#endif
        TumblingCountHandler handler(n);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        // Insert tuplesToInsert tuples into first partition
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into partition " << partitionZero
                         << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partitionZero);
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData(partitionZero).size());

        sleep(age + 1);

        // Trigger partitionZero eviction.  The insertion triggers window eviction,
        // followed by partition eviction.
        SPCDBG(L_INFO,
               "Insert one tuple into partition " << partitionZero
                                                  << " to trigger partition eviction",
               WINLIB_TEST);
        tuple.get_age() = tuplesToInsert;
        win.insert(tuple, partitionZero);
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(!WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(1, WindowEventUtils::getPartitionSize(win, partitionZero));

        // Insert tuplesToInsert tuples into the second partition
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into partition " << partitionOne
                         << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partitionOne);
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData(partitionOne).size());

        // PartitionZero has been evicted, tuple inserted, partitionOne contains tuples
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(1, WindowEventUtils::getPartitionSize(win, partitionZero));
        ASSERT_EQUALS(tuplesToInsert % n, WindowEventUtils::getPartitionSize(win, partitionOne));

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowData().size());

        // Insert tuplesToInsert more tuples
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into partition " << partitionZero
                         << " of window of size " << n,
               WINLIB_TEST);
        CountWindowPolicy const& countPolicy =
          dynamic_cast<CountWindowPolicy const&>(win.getEvictionPolicy());
        unsigned numFlushes = handler.numAfterFlush_[0] + handler.numAfterFlush_[1];
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            // Tuple age continues in sequence
            tuple.get_age() = i + numFlushes * countPolicy.getSize();
            win.insert(tuple, partitionZero);
        }

        // Handler's state gets reset as well
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionZero]);

        sleep(age + 1);

        // Trigger partitionZero eviction.  The insertion triggers window eviction,
        // followed by partition eviction.
        SPCDBG(L_INFO,
               "Insert one tuple into partition " << partitionZero
                                                  << " to trigger partition eviction",
               WINLIB_TEST);
        tuple.get_age() = tuplesToInsert;
        win.insert(tuple, partitionZero);
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(!WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(1, WindowEventUtils::getPartitionSize(win, partitionZero));

        // Insert tuplesToInsert tuples into the second partition
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into partition " << partitionOne
                         << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i + numFlushes * countPolicy.getSize();
            win.insert(tuple, partitionOne);
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData(partitionOne).size());

        // PartitionOne contains tuples
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(tuplesToInsert % n, WindowEventUtils::getPartitionSize(win, partitionOne));

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
    void test_winTumblingCount_reset(Operator& oper,
                                     WindowPolicy const& windowEviction,
                                     PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        const unsigned n = winSize;
        const unsigned tuplesToInsert = 2 * n + n - 1;
        const unsigned checkpointCount = 2 * n + 1;
        const int partitionZero = 0;
        const int partitionOne = 1;
        TType tuple;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, windowEviction, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, windowEviction, partitionEviction);
#endif
        TumblingCountHandler handler(n);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        // Create checkpoint structure, insert tuples, then checkpoint window
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        // Insert tuplesToInsert tuples into first partition
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into partition " << partitionZero
                         << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple.get_age() = i;
            win.insert(tuple, partitionZero);
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData(partitionZero).size());

        sleep(age + 1);

        // Trigger partitionZero eviction: if the tuple triggers a window eviction, then
        // the order is: window eviction, partition eviction, tuple insertion
        SPCDBG(L_INFO,
               "Insert one tuple into partition " << partitionZero
                                                  << " to trigger partition eviction",
               WINLIB_TEST);
        tuple.get_age() = tuplesToInsert;
        win.insert(tuple, partitionZero);
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(!WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(1, WindowEventUtils::getPartitionSize(win, partitionZero));

        // Insert tuplesToInsert tuples into the second partition
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert << " tuples into partition " << partitionOne
                         << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partitionOne);
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert % n, win.getWindowData(partitionOne).size());

        // PartitionZero has been evicted, partitionOne contains tuples
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(tuplesToInsert % n, WindowEventUtils::getPartitionSize(win, partitionOne));

        // Release checkpoint pointer
        ckptSave.reset();

        // Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        ASSERT_EQUALS(1, win.getWindowData(partitionZero).size());

        // Continue inserting tuples
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert - checkpointCount << " more tuples into partition "
                         << partitionZero << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = checkpointCount; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple);
        }

        // Handler's state gets reset as well
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numBeforeFlush_[partitionZero]);
        ASSERT_EQUALS(tuplesToInsert / n, handler.numAfterFlush_[partitionZero]);

        sleep(age + 1);

        // Trigger partitionZero eviction: if the tuple triggers a window eviction, then
        // the order is: window eviction, partition eviction, tuple insertion
        SPCDBG(L_INFO,
               "Insert one tuple into partition " << partitionZero
                                                  << " to trigger partition eviction",
               WINLIB_TEST);
        tuple.get_age() = tuplesToInsert;
        win.insert(tuple, partitionZero);
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(!WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(1, WindowEventUtils::getPartitionSize(win, partitionZero));

        // Insert tuples and trigger partitionOne eviction
        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert - 1 << " tuples into partition " << partitionOne
                         << " of window of size " << n,
               WINLIB_TEST);
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            tuple.get_age() = i;
            win.insert(tuple, partitionOne);

            if (i == 0) {
                SPCDBG(L_INFO,
                       "First insertion triggers partition "
                         << partitionOne << " eviction (then tuple is inserted)",
                       WINLIB_TEST);
                ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionOne));
                ASSERT_EQUALS(1, WindowEventUtils::getPartitionSize(win, partitionOne));
            }
        }
        ASSERT_EQUALS(tuplesToInsert, handler.numBeforeInsertion_[partitionOne]);
        ASSERT_EQUALS(tuplesToInsert, handler.numAfterInsertion_[partitionOne]);
        ASSERT_EQUALS((tuplesToInsert) / n, handler.numBeforeFlush_[partitionOne]);
        ASSERT_EQUALS((tuplesToInsert) / n, handler.numAfterFlush_[partitionOne]);
        ASSERT_EQUALS((tuplesToInsert) % n, win.getWindowData(partitionOne).size());

        // PartitionZero has been evicted, partitionOne contains tuples
        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS((tuplesToInsert) % n, WindowEventUtils::getPartitionSize(win, partitionOne));

        SPCDBG_EXIT(WINLIB_TEST);
    }
}; // end WindowTest9
}; // end namespace SPL

MAIN_APP(SPL::WindowTest9)
