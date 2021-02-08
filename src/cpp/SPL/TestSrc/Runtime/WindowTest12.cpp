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
 * TupleCountPartitionEvictionPolicy
 */
class WindowTest12 : public WindowTestBase
{
  public:
    WindowTest12()
      : WindowTestBase()
      , tupleCount(5)
    {}
    const int tupleCount; // Tuple count which triggers partition eviction

  private:
    void runTests()
    {
        test_CheckpointStream();

        // window policies: count based
        Operator& oper = *((Operator*)NULL);
        CountWindowPolicy policy_C4(4);
        TupleCountPartitionEvictionPolicy tupleCountEviction(tupleCount,
                                                             PartitionEvictionPolicy::LRU);
        { // without partition eviction, tumbling windows, tumbling on count
            test_winTumblingCount_resetInitial(oper, policy_C4, tupleCountEviction);
            test_winTumblingSumCount_resetInitial(oper, policy_C4, tupleCountEviction);
            test_winTumblingCount_reset(oper, policy_C4, tupleCountEviction);
            test_winTumblingSumCount_reset(oper, policy_C4, tupleCountEviction);
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
                                            WindowPolicy const& evict,
                                            PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const unsigned tuplesToInsert = 14;
        // window size
        const unsigned n = (dynamic_cast<CountWindowPolicy const&>(evict)).getSize();
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingCountWindowCountPartitionHandler handler(n);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        // Insert tuplesToInsert tuples
        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        TType tuple;
        int32_t partition = 0;
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            if (i < 4) {
                partition = 0;
            } else if (i >= 4 && i < 8) {
                partition = 1;
            } else {
                partition = i % 2;
            }
            tuple.get_age() = i;

            if (i == 3 || i == 7) {
                handler.expectFlush(partition);
            } else if (i == 13) {
                handler.expectEviction((partition + 1) % 2); // the other partition
            }
            win.insert(tuple, partition);
            if (i == 3 || i == 7) {
                handler.verifyFlush();
            } else if (i == 13) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }
        ASSERT_EQUALS(tuplesToInsert,
                      handler.numBeforeInsertion_[0] + handler.numBeforeInsertion_[1]);
        ASSERT_EQUALS(tuplesToInsert,
                      handler.numAfterInsertion_[0] + handler.numAfterInsertion_[1]);

        // We expect one partition, number 1, with three tuples.
        ASSERT_EQUALS(1, win.getWindowStorage().size());
        ASSERT_EQUALS(3, win.getWindowData(1).size());
        ASSERT_EQUALS(3, win.getTupleCount(1));

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();

        ASSERT_EQUALS(0, win.getWindowStorage().size());

        // Insert tuplesToInsert more tuples
        SPCDBG(L_INFO, "Insert " << 5 << " tuples in window of size " << n, WINLIB_TEST);

        for (unsigned i = tuplesToInsert; i <= tuplesToInsert + 5; ++i) {
            partition = i % 2;
            // Tuple age continues in sequence
            tuple.get_age() = i;
            if (i == tuplesToInsert + 5) {
                handler.expectEviction((partition + 1) % 2);
            }
            win.insert(tuple, partition);
            if (i == tuplesToInsert + 5) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumCount_resetInitial(Operator& oper,
                                               WindowPolicy const& evict,
                                               PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        const unsigned tuplesToInsert = 14;
        const unsigned n = (dynamic_cast<CountWindowPolicy const&>(evict)).getSize();
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingSumCountWindowCountPartitionHandler handler(n);

        win.registerBeforeWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        TType tuple;
        int32_t partition = 0;
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            if (i < 4) {
                partition = 0;
            } else if (i >= 4 && i < 8) {
                partition = 1;
            } else {
                partition = i % 2;
            }
            tuple.get_age() = i;

            if (i == 3 || i == 7) {
                handler.expectFlush(partition);
            } else if (i == 13) {
                handler.expectEviction((partition + 1) % 2); // the other partition
            }
            win.insert(tuple, partition);
            if (i == 3 || i == 7) {
                handler.verifyFlush();
            } else if (i == 13) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }
        ASSERT_EQUALS(2, handler.flushCount_);

        // We expect one partition, number 1, with three tuples.
        ASSERT_EQUALS(1, win.getWindowStorage().size());
        ASSERT_EQUALS(3, win.getTupleCount(1));

        SPCDBG(L_INFO, "Reset to initial state", WINLIB_TEST);
        win.resetToInitialState();
        ASSERT_EQUALS(0, win.getWindowStorage().size());

        // Insert 5 more tuples
        SPCDBG(L_INFO, "Insert " << 5 << " tuples in window of size " << n, WINLIB_TEST);

        for (unsigned i = tuplesToInsert; i <= tuplesToInsert + 5; ++i) {
            partition = i % 2;
            // Tuple age continues in sequence
            tuple.get_age() = i;
            if (i == tuplesToInsert + 5) {
                handler.expectEviction((partition + 1) % 2);
            }
            win.insert(tuple, partition);
            if (i == tuplesToInsert + 5) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

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
                                     WindowPolicy const& evict,
                                     PartitionEvictionPolicy const& partitionEviction)

    {
        SPCDBG_ENTER(WINLIB_TEST);

        const unsigned tuplesToInsert = 14;
        // window size
        const unsigned n = (dynamic_cast<CountWindowPolicy const&>(evict)).getSize();
        const unsigned checkpointCount = 2 * n + 1;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingCountWindowCountPartitionHandler handler(n);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);

        // Create checkpoint structure, insert tuples, then do checkpoint window
        TType tuple;
        int64_t id = newId();
        CheckpointSavePtr ckptSave = getCheckpointSave(id);

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        int32_t partition = 0;
        for (unsigned i = 0; i < tuplesToInsert; ++i) {
            if (i < 4) {
                partition = 0;
            } else if (i >= 4 && i < 8) {
                partition = 1;
            } else {
                partition = i % 2;
            }

            checkpointAtCount(win, ckptSave, i, checkpointCount);
            tuple.get_age() = i;

            if (i == 3 || i == 7) {
                handler.expectFlush(partition);
            } else if (i == 13) {
                handler.expectEviction((partition + 1) % 2); // the other partition
            }
            win.insert(tuple, partition);
            if (i == 3 || i == 7) {
                handler.verifyFlush();
            } else if (i == 13) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        // Release checkpoint pointer
        ckptSave.reset();

        ASSERT_EQUALS(tuplesToInsert,
                      handler.numBeforeInsertion_[0] + handler.numBeforeInsertion_[1]);
        ASSERT_EQUALS(tuplesToInsert,
                      handler.numAfterInsertion_[0] + handler.numAfterInsertion_[1]);

        // We expect one partition, number 1, with three tuples.
        ASSERT_EQUALS(1, win.getWindowStorage().size());
        ASSERT_EQUALS(3, win.getWindowData(1).size());
        ASSERT_EQUALS(3, win.getTupleCount(1));

        // Restore from checkpoint
        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        ASSERT_EQUALS(1, win.getTupleCount(0));
        ASSERT_EQUALS(1, win.getWindowStorage().size());

        SPCDBG(L_INFO,
               "Insert " << tuplesToInsert - checkpointCount << " tuples in window of size " << n,
               WINLIB_TEST);
        for (unsigned i = checkpointCount; i < tuplesToInsert; ++i) {
            if (i < 4) {
                partition = 0;
            } else if (i >= 4 && i < 8) {
                partition = 1;
            } else {
                partition = i % 2;
            }

            tuple.get_age() = i;

            if (i == 3 || i == 7) {
                handler.expectFlush(partition);
            } else if (i == 13) {
                handler.expectEviction((partition + 1) % 2); // the other partition
            }
            win.insert(tuple, partition);
            if (i == 3 || i == 7) {
                handler.verifyFlush();
            } else if (i == 13) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        ASSERT_EQUALS(tuplesToInsert,
                      handler.numBeforeInsertion_[0] + handler.numBeforeInsertion_[1]);
        ASSERT_EQUALS(tuplesToInsert,
                      handler.numAfterInsertion_[0] + handler.numAfterInsertion_[1]);

        // We expect one partition, number 1, with three tuples.
        ASSERT_EQUALS(1, win.getWindowStorage().size());
        ASSERT_EQUALS(3, win.getWindowData(1).size());
        ASSERT_EQUALS(3, win.getTupleCount(1));

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
    void test_winTumblingSumCount_reset(Operator& oper,
                                        WindowPolicy const& evict,
                                        PartitionEvictionPolicy const& partitionEviction)

    {
        SPCDBG_ENTER(WINLIB_TEST);

        const unsigned tuplesToInsert = 14;
        const unsigned n = (dynamic_cast<CountWindowPolicy const&>(evict)).getSize();
        const unsigned checkpointCount = 2 * n + 1;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingSumCountWindowCountPartitionHandler handler(n);

        win.registerBeforeWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerOnWindowPartitionEviction(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();

        TType tuple;
        int64_t id = newId();
        int32_t partition = 0;
        {
            CheckpointSavePtr ckptSave = getCheckpointSave(id);
            SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
                   WINLIB_TEST);
            for (unsigned i = 0; i < tuplesToInsert; ++i) {
                if (i < 4) {
                    partition = 0;
                } else if (i >= 4 && i < 8) {
                    partition = 1;
                } else {
                    partition = i % 2;
                }
                tuple.get_age() = i;

                if (i == 3 || i == 7) {
                    handler.expectFlush(partition);
                } else if (i == 13) {
                    handler.expectEviction((partition + 1) % 2); // the other partition
                }
                checkpointAtCount(win, ckptSave, i, checkpointCount);
                win.insert(tuple, partition);
                if (i == 3 || i == 7) {
                    handler.verifyFlush();
                } else if (i == 13) {
                    handler.verifyEviction();
                }
                logPartitionSizes(win);
            }
        } // end ckptSave
        ASSERT_EQUALS(2, handler.flushCount_);

        // We expect one partition, number 1, with three tuples.
        ASSERT_EQUALS(1, win.getWindowStorage().size());
        ASSERT_EQUALS(3, win.getTupleCount(1));

        CheckpointPtr ckptRestore = getCheckpointRestore(id);
        SPCDBG(L_INFO, "Reset to checkpoint " << ckptRestore->getSequenceId(), WINLIB_TEST);
        win.reset(*ckptRestore);
        SPCDBG(L_INFO, "Window after reset: " << win.toString(), WINLIB_TEST);

        SPCDBG(L_INFO, "Insert " << tuplesToInsert << " tuples in window of size " << n,
               WINLIB_TEST);
        for (unsigned i = checkpointCount; i < tuplesToInsert; ++i) {
            if (i < 4) {
                partition = 0;
            } else if (i >= 4 && i < 8) {
                partition = 1;
            } else {
                partition = i % 2;
            }
            tuple.get_age() = i;

            if (i == 3 || i == 7) {
                handler.expectFlush(partition);
            } else if (i == 13) {
                handler.expectEviction((partition + 1) % 2); // the other partition
            }
            win.insert(tuple, partition);
            if (i == 3 || i == 7) {
                handler.verifyFlush();
            } else if (i == 13) {
                handler.verifyEviction();
            }
            logPartitionSizes(win);
        }

        // We expect one partition, number 1, with three tuples.
        ASSERT_EQUALS(1, win.getWindowStorage().size());
        ASSERT_EQUALS(3, win.getTupleCount(1));

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
}; // end WindowTest12
}; // end namespace SPL

MAIN_APP(SPL::WindowTest12)
