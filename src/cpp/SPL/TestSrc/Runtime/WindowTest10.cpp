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
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace Distillery;

namespace SPL {

/**
 * Checkpoint and reset tests for SlidingWindow with eviction and trigger
 * CountWindowPolicy, and PartitionAgePartitionEvictionPolicy.
 */
class WindowTest10 : public WindowTestBase
{
  public:
    const int winSize;  // Window size - defined by its eviction policy
    const int trigSize; // Trigger size - defined by its eviction policy
    const double age;

    WindowTest10()
      : WindowTestBase()
      , winSize(5)
      , trigSize(2)
      , age(1.1)
    {}

    enum SleepMode
    {
        /// Don't sleep between checkpoint and reset.
        NO_SLEEP_BEFORE_RESET,
        /// Sleep btw ckpoint and reset to trigger partition eviction on first insert after reset
        SLEEP_BEFORE_RESET
    };

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        // Window policies - count based
        CountWindowPolicy policy_C5(winSize);
        CountWindowPolicy policy_C2(trigSize);
        PartitionAgePartitionEvictionPolicy ageEviction(age);

        {
            test_winSlidingCount_resetInitial(oper, policy_C5, policy_C2, ageEviction);
            test_winSlidingCount_reset(oper, policy_C5, policy_C2, 7, ageEviction,
                                       NO_SLEEP_BEFORE_RESET);
            test_winSlidingCount_reset(oper, policy_C5, policy_C2, 7, ageEviction,
                                       SLEEP_BEFORE_RESET);
        }
    }

    void test_winSlidingCount_resetInitial(Operator& oper,
                                           WindowPolicy const& evict,
                                           WindowPolicy const& trigger,
                                           PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int partitionZero = 0;
        const int partitionOne = 1;
        TType tuple;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        SlidingWindow<TType> win(oper, 0, evict, trigger, partitionEviction);
#else
        SlidingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, trigger, partitionEviction);
#endif
        SlidingCountCountHandler handler(this->winSize, this->trigSize, true);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeTupleEvictionHandler(&handler);
        win.registerAfterTupleEvictionHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerSerializationHandler(&handler);
        // No partition eviction event handler

        SPCDBG(L_INFO, "Insert " << winSize << " tuples into partition " << partitionZero,
               WINLIB_TEST);
        int z = 0;
        for (int i = 0; i < winSize; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple, partitionZero);
            SPCDBG(L_INFO, " inserted tuple age=" << z - 1, WINLIB_TEST);
        }
        ASSERT_EQUALS(winSize, win.getWindowData(partitionZero).size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(!WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(winSize, WindowEventUtils::getPartitionSize(win, partitionZero));

        sleep(age + 1);

        // Trigger tuple eviction and partition eviction
        // XXX TODO no tuple eviction event, only partition eviction event is generated here.
        // Investigate if this is according to docs.
        SPCDBG(L_INFO,
               "Triggering tuple eviction and partition eviction for partition " << partitionZero,
               WINLIB_TEST);
        tuple.get_age() = z++;
        win.insert(tuple, partitionZero);
        SPCDBG(L_INFO, " inserted tuple age=" << z - 1, WINLIB_TEST);

        // Adjust handler eviction counters to account for a partition eviction
        // (equivalent to 'winSize' tuple evictions.
        SPCDBG(L_INFO,
               "Adjusting handler eviction counters to account for eviction of partition "
                 << partitionZero,
               WINLIB_TEST);
        handler.numBeforeEviction_[partitionZero] = winSize;
        handler.numAfterEviction_[partitionZero] = winSize;

        for (int i = 0; i < winSize; ++i) {
            tuple.get_age() = z++;
            if (i == winSize - 1) {
                SPCDBG(L_INFO, "Triggering tuple eviction for partition " << partitionZero,
                       WINLIB_TEST);
            }
            win.insert(tuple, partitionZero);
            SPCDBG(L_INFO, " inserted tuple age=" << z - 1, WINLIB_TEST);
        }

        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(winSize + 1, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(winSize + 1, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(5, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnInitialFull_[partitionZero]);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        z = 0;
        for (int i = 0; i < winSize; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
            SPCDBG(L_INFO, " inserted tuple age=" << z - 1, WINLIB_TEST);
        }
        ASSERT_EQUALS(winSize, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        SPCDBG_EXIT(WINLIB_TEST);
    }

    /**
     * @param oper
     * @param evict
     * @param trigger
     * @param checkpointCount after how many inserted tuples we checkpoint
     *      must be between winSize+1 and 2*winSize
     * @param partitionEviction
     * @param sleepBeforeReset true indicates that a sleep happens between checkpoint and reset,
     *      thus the first insert after reset triggers partition eviction
     */
    void test_winSlidingCount_reset(Operator& oper,
                                    WindowPolicy const& evict,
                                    WindowPolicy const& trigger,
                                    const int checkpointCount,
                                    PartitionEvictionPolicy const& partitionEviction,
                                    int sleepMode)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const int partitionZero = 0;
        const int partitionOne = 1;
        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        SlidingWindow<TType> win(oper, 0, evict, trigger, partitionEviction);
#else
        SlidingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, trigger, partitionEviction);
#endif
        SlidingCountCountHandler handler(this->winSize, this->trigSize, true);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeTupleEvictionHandler(&handler);
        win.registerAfterTupleEvictionHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerSerializationHandler(&handler);
        // No partition eviction event handler

        SPCDBG(L_INFO, "Insert " << winSize << " tuples into partition " << partitionZero,
               WINLIB_TEST);
        int z = 0;
        for (; z < winSize; z++) {
            tuple.get_age() = z;
            win.insert(tuple, partitionZero);
            SPCDBG(L_INFO, " inserted tuple age=" << z, WINLIB_TEST);
        }
        ASSERT_EQUALS(winSize, win.getWindowData(partitionZero).size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        ASSERT_TRUE(WindowEventUtils::existsPartition(win, partitionZero));
        ASSERT_TRUE(!WindowEventUtils::existsPartition(win, partitionOne));
        ASSERT_EQUALS(winSize, WindowEventUtils::getPartitionSize(win, partitionZero));

        sleep(age + 1);

        // Trigger tuple eviction and partition eviction
        // XXX TODO no tuple eviction event, only partition eviction event is generated here.
        // Investigate if this is according to docs.
        SPCDBG(L_INFO,
               "Triggering tuple eviction and partition eviction for partition " << partitionZero,
               WINLIB_TEST);
        tuple.get_age() = z++;
        win.insert(tuple, partitionZero);
        SPCDBG(L_INFO, " inserted tuple age=" << z - 1, WINLIB_TEST);

        // Adjust handler eviction counters to account for a partition eviction
        // (equivalent to 'winSize' tuple evictions.
        SPCDBG(L_INFO,
               "Adjusting handler eviction counters to account for eviction of partition "
                 << partitionZero,
               WINLIB_TEST);
        handler.numBeforeEviction_[partitionZero] = winSize;
        handler.numAfterEviction_[partitionZero] = winSize;

        // Continue tuple insert and checkpoint
        for (; z < 2 * winSize + 1; z++) {
            tuple.get_age() = z;
            if (z % winSize == 0) {
                SPCDBG(L_INFO, "Triggering tuple eviction for partition " << partitionZero,
                       WINLIB_TEST);
            }
            win.insert(tuple, partitionZero);
            SPCDBG(L_INFO, " inserted tuple age=" << z, WINLIB_TEST);
            checkpointAtCount(win, ckptSave, z, checkpointCount);
        }

        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(winSize + 1, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(winSize + 1, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(5, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnInitialFull_[partitionZero]);

        if (sleepMode == SLEEP_BEFORE_RESET) {
            sleep(age + 1);

            // Trigger tuple eviction and partition eviction
            SPCDBG(L_INFO,
                   "Triggering tuple eviction and partition eviction for partition "
                     << partitionZero,
                   WINLIB_TEST);
        }
        tuple.get_age() = z++;
        win.insert(tuple, partitionZero);
        SPCDBG(L_INFO, " inserted tuple age=" << z - 1, WINLIB_TEST);

        // Release checkpoint pointer
        ckptSave.reset();

        // (4) Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_DEBUG, "Window after reset: " << win.toString(), WINLIB_TEST);

        for (z = checkpointCount + 1; z < 2 * winSize + 1; z++) {
            tuple.get_age() = z;
            win.insert(tuple);
            SPCDBG(L_INFO, " inserted tuple age=" << z, WINLIB_TEST);
        }

        SPCDBG(L_DEBUG, "win.getWindowData().size()=" << win.getWindowData().size(), WINLIB_TEST);
        SPCDBG(L_DEBUG, "partitionZero: ", WINLIB_TEST);
        SPCDBG(L_DEBUG,
               "  handler.numBeforeInsertion_=" << handler.numBeforeInsertion_[partitionZero],
               WINLIB_TEST);
        SPCDBG(L_DEBUG,
               "  handler.numAfterInsertion_=" << handler.numAfterInsertion_[partitionZero],
               WINLIB_TEST);
        SPCDBG(L_DEBUG,
               "  handler.numBeforeEviction_=" << handler.numBeforeEviction_[partitionZero],
               WINLIB_TEST);
        SPCDBG(L_DEBUG, "  handler.numAfterEviction_=" << handler.numAfterEviction_[partitionZero],
               WINLIB_TEST);
        SPCDBG(L_DEBUG, "  handler.numOnTrigger_=" << handler.numOnTrigger_[partitionZero],
               WINLIB_TEST);
        SPCDBG(L_DEBUG, "  handler.numOnInitialFull_=" << handler.numOnInitialFull_[partitionZero],
               WINLIB_TEST);

        if (sleepMode == SLEEP_BEFORE_RESET) {
            ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
            ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
            ASSERT_EQUALS(winSize, handler.numBeforeEviction_[partitionZero]);
            ASSERT_EQUALS(winSize, handler.numAfterEviction_[partitionZero]);
            // One time full, because of a partition eviction in the interim
            ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);
        } else if (sleepMode == NO_SLEEP_BEFORE_RESET) {
            ASSERT_EQUALS(winSize, win.getWindowData().size());
            ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
            ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
            ASSERT_EQUALS(winSize + 1, handler.numBeforeEviction_[partitionZero]);
            ASSERT_EQUALS(winSize + 1, handler.numAfterEviction_[partitionZero]);
            ASSERT_EQUALS(z / trigSize, handler.numOnTrigger_[partitionZero]);
            // Two times full, because of a partition eviction in the interim
            ASSERT_EQUALS(2, handler.numOnInitialFull_[partitionZero]);
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest10)
