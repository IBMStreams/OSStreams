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
 * Checkpoint and reset tests for SlidingWindow with DeltaWindowPolicy for
 * eviction and trigger.
 */
class WindowTest8 : public WindowTestBase
{
  public:
    const int winSize;  // Window size - delta range of eviction policy
    const int trigSize; // Trigger size - count of tuples between triggers

    WindowTest8()
      : WindowTestBase()
      , winSize(2)
      , trigSize(0)
    {}

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        DeltaWindowPolicy<TType,           // tuple type
                          TType::age_type, // attribute type
                          &TType::get_age> // attribute getter
          policy_Dage2(winSize);
        DeltaWindowPolicy<TType, TType::age_type, &TType::get_age> policy_Dage0(trigSize);
        {
            // Sliding windows without partition eviction
            test_winSlidingDeltaDelta_resetInitial(oper, policy_Dage2, policy_Dage0);
            test_winSlidingDeltaDelta_reset(oper, policy_Dage2, policy_Dage0, 3);
            test_winSlidingDeltaDelta_reset(oper, policy_Dage2, policy_Dage0, 4);
        }
    }

    void test_winSlidingDeltaDelta_resetInitial(Operator& oper,
                                                WindowPolicy const& evict,
                                                WindowPolicy const& trigger)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        const int partitionZero = 0;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        SlidingWindow<TType> win(oper, 0, evict, trigger);
#else
        SlidingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, trigger);
#endif
        SlidingDeltaDeltaHandler handler(this->winSize, this->trigSize);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeTupleEvictionHandler(&handler);
        win.registerAfterTupleEvictionHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerSerializationHandler(&handler);

        TType tuple;
        int z = 0;
        for (int i = 0; i < this->winSize + 1; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
        }
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize, handler.numOnTrigger_[partitionZero]);
        // Window is full, but we do not know it until we get the next tuple
        ASSERT_EQUALS(0, handler.numOnInitialFull_[partitionZero]);

        tuple.get_age() = z++;
        win.insert(tuple);
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(1, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(1, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize + 1, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        tuple.get_age() = z++;
        win.insert(tuple);
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(2, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize + 2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        z = 0;
        for (int i = 0; i < this->winSize + 3; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
        }
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(2, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(4, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        SPCDBG_EXIT(WINLIB_TEST);
    }

    /*
     * (1) Insert tuples t0, t1, ... t[checkpointCount-1]
     * (2) Checkpoint window
     * (3) Insert tuples t[checkpoint] .. t[end-1]
     * (4) Restore from checkpoint
     * (5) Insert tuples t[checkpoint] .. t[end-1]
     */
    void test_winSlidingDeltaDelta_reset(Operator& oper,
                                         WindowPolicy const& evict,
                                         WindowPolicy const& trigger,
                                         const int checkpointCount)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        const int partitionZero = 0;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        SlidingWindow<TType> win(oper, 0, evict, trigger);
#else
        SlidingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, trigger);
#endif
        SlidingDeltaDeltaHandler handler(this->winSize, this->trigSize);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeTupleEvictionHandler(&handler);
        win.registerAfterTupleEvictionHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerSerializationHandler(&handler);

        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);
        TType tuple;

        // (1) Insert tuples t0, t1, ... t[checkpointCount-1]
        int z = 0;
        for (int i = 0; i < this->winSize + 1; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
            SPCDBG(L_INFO, " Inserted age " << z - 1, WINLIB_TEST);
        }
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize, handler.numOnTrigger_[partitionZero]);
        // Window is full, but we do not know it until we get the next tuple
        ASSERT_EQUALS(0, handler.numOnInitialFull_[partitionZero]);

        // Checkpoint if checkpointCount == winSize+1
        checkpointAtCount(win, ckptSave, z, checkpointCount);
        tuple.get_age() = z++;
        win.insert(tuple);
        SPCDBG(L_INFO, " Inserted age " << z - 1, WINLIB_TEST);
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(1, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(1, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize + 1, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        // Checkpoint if checkpointCount == winSize+2
        checkpointAtCount(win, ckptSave, z, checkpointCount);
        tuple.get_age() = z++;
        win.insert(tuple);
        SPCDBG(L_INFO, " Inserted age " << z - 1, WINLIB_TEST);

        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(2, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize + 2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        // Release checkpoint pointer
        ckptSave.reset();

        // (4) Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_DEBUG, "Window after reset: " << win.toString(), WINLIB_TEST);

        // (5) Insert the same number of tuples and assert state
        int totalInsertedTuples = z;
        for (z = checkpointCount; z < totalInsertedTuples; ++z) {
            tuple.get_age() = z;
            win.insert(tuple);
            SPCDBG(L_INFO, " Inserted age " << z, WINLIB_TEST);
        }
        ASSERT_EQUALS(this->winSize + 1, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(2, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(this->winSize + 2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest8)
