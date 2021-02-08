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
 * Checkpoint and reset tests for SlidingWindow with CountWindowPolicy for
 * eviction and trigger.
 */
class WindowTest5 : public WindowTestBase
{
  public:
    const int winSize;  // Window size - defined by its eviction policy
    const int trigSize; // Trigger size - defined by its eviction policy

    WindowTest5()
      : WindowTestBase()
      , winSize(5)
      , trigSize(2)
    {}

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        // Window policies - count based
        CountWindowPolicy policy_C5(winSize);
        CountWindowPolicy policy_C2(trigSize);

        {
            test_winSlidingCount_resetInitial(oper, policy_C5, policy_C2);
            test_winSlidingCount_reset(oper, policy_C5, policy_C2, 0);
            test_winSlidingCount_reset(oper, policy_C5, policy_C2, 3);
            test_winSlidingCount_reset(oper, policy_C5, policy_C2, 4);
            test_winSlidingCount2_reset(oper, policy_C5, policy_C2, 4);
        }
    }

    void test_winSlidingCount_resetInitial(Operator& oper,
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
        SlidingCountCountHandler handler(this->winSize, this->trigSize);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeTupleEvictionHandler(&handler);
        win.registerAfterTupleEvictionHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerSerializationHandler(&handler);

        TType tuple;
        int z = 0;
        for (int i = 0; i < winSize; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
            SPCDBG(L_INFO, " insert tuple age=" << z - 1, WINLIB_TEST);
        }
        ASSERT_EQUALS(winSize, win.getWindowData(partitionZero).size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        tuple.get_age() = z++;
        win.insert(tuple);
        SPCDBG(L_INFO, " insert tuple age=" << z - 1, WINLIB_TEST);

        ASSERT_EQUALS(winSize, win.getWindowData(partitionZero).size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(1, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(1, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(3, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        tuple.get_age() = z++;
        win.insert(tuple);
        SPCDBG(L_INFO, " insert tuple age=" << z - 1, WINLIB_TEST);

        ASSERT_EQUALS(winSize, win.getWindowData(partitionZero).size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(2, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(3, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        z = 0;
        for (int i = 0; i < winSize; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
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

    void test_winSlidingCount_reset(Operator& oper,
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

        SlidingCountCountHandler handler(this->winSize, this->trigSize);
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
        // (2) Checkpoint window
        // (3) Insert tuples t[checkpoint] .. t[winSize-1]
        int z = 0;
        for (int i = 0; i < winSize; ++i) {
            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple.get_age() = z++;
            win.insert(tuple);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        ASSERT_EQUALS(winSize, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_[partitionZero]);
        ASSERT_EQUALS(z, handler.numAfterInsertion_[partitionZero]);
        ASSERT_EQUALS(0, handler.numBeforeEviction_[partitionZero]);
        ASSERT_EQUALS(0, handler.numAfterEviction_[partitionZero]);
        ASSERT_EQUALS(2, handler.numOnTrigger_[partitionZero]);
        ASSERT_EQUALS(1, handler.numOnInitialFull_[partitionZero]);

        // (4) Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_DEBUG, "Window after reset: " << win.toString(), WINLIB_TEST);

        // (5) Insert tuples t[checkpoint] .. t[winSize-1]
        z = checkpointCount;
        for (int i = checkpointCount; i < winSize; ++i) {
            tuple.get_age() = z++;
            win.insert(tuple);
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

    // Testing window of Tuple pointers
    void test_winSlidingCount2_reset(Operator& oper,
                                     WindowPolicy const& evict,
                                     WindowPolicy const& trigger,
                                     const int checkpointCount)
    {
        SPCDBG_ENTER(WINLIB_TEST);

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        SlidingWindow<TType*> win(oper, 0, evict, trigger);
#else
        SlidingWindow<TType*, int32_t, SPL::IncrDeque<TType*>,
                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType*> > >
          win(oper, 0, evict, trigger);
#endif
        SlidingCountCountHandler2 handler(this->winSize, this->trigSize);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeTupleEvictionHandler(&handler);
        win.registerAfterTupleEvictionHandler(&handler);
        win.registerOnWindowTriggerHandler(&handler);
        win.registerOnWindowInitialFullHandler(&handler);
        win.registerSerializationHandler(&handler);

        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);
        TType* tuple;

        // (1) Insert tuples t0, t1, ... t[checkpointCount-1]
        // (2) Checkpoint window
        // (3) Insert tuples t[checkpoint] .. t[winSize-1]
        int z = 0;
        for (int i = 0; i < winSize; ++i) {
            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple = new TType;
            tuple->get_msg() = std::string("a") + boost::lexical_cast<std::string>(z);
            tuple->get_age() = z++;
            win.insert(tuple);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        ASSERT_EQUALS(winSize, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_);
        ASSERT_EQUALS(z, handler.numAfterInsertion_);
        ASSERT_EQUALS(0, handler.numBeforeEviction_);
        ASSERT_EQUALS(0, handler.numAfterEviction_);
        ASSERT_EQUALS(2, handler.numOnTrigger_);
        ASSERT_EQUALS(1, handler.numOnInitialFull_);

        // (4) Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_DEBUG, "Window after reset: " << win.toString(), WINLIB_TEST);

        // (5) Insert tuples t[checkpoint] .. t[winSize-1]
        z = checkpointCount;
        for (int i = checkpointCount; i < winSize + 2; ++i) {
            tuple = new TType;
            tuple->get_msg() = std::string("a") + boost::lexical_cast<std::string>(z);
            tuple->get_age() = z++;
            win.insert(tuple);
        }

        ASSERT_EQUALS(winSize, win.getWindowData().size());
        ASSERT_EQUALS(z, handler.numBeforeInsertion_);
        ASSERT_EQUALS(z, handler.numAfterInsertion_);
        ASSERT_EQUALS(2, handler.numBeforeEviction_);
        ASSERT_EQUALS(2, handler.numAfterEviction_);
        ASSERT_EQUALS(3, handler.numOnTrigger_);
        ASSERT_EQUALS(1, handler.numOnInitialFull_);

        win.deleteWindowObjects();

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest5)
