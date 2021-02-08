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
 * Event handler for TumblingWindow with count-based policy used for this test.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class TumblingCountHandler11 : public WindowEvent<TType>
#else
class TumblingCountHandler11
  : public WindowEvent<TType,
                       int32_t,
                       SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
#endif
{
  public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType,
                   int32_t,
                   SPL::IncrDeque<TType>,
                   SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
      WindowType;
#endif
    TumblingCountHandler11(unsigned size)
      : windowSize_(size)
    {
        init();
    }

    void beforeTupleInsertionEvent(WindowType& window,
                                   TupleType const& tuple,
                                   PartitionType const& partition)
    {
        numBeforeInsertion_[partition]++;
    }

    void afterTupleInsertionEvent(WindowType& window,
                                  TupleType& tuple,
                                  PartitionType const& partition)
    {
        numAfterInsertion_[partition]++;
        ASSERT_TRUE(numBeforeInsertion_[partition] == numAfterInsertion_[partition]);
    }

    void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
    {
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType>& win = dynamic_cast<TumblingWindow<TType>&>(window);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >& win =
          dynamic_cast<TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                                      SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >&>(
            window);
#endif
        CountWindowPolicy const& evict =
          dynamic_cast<CountWindowPolicy const&>(win.getEvictionPolicy());
        ASSERT_EQUALS(evict.getSize(), windowSize_);
    }

    void afterWindowFlushEvent(WindowType& window, PartitionType const& partition)
    {
        numAfterFlush_[partition]++;
        DataType& data = window.getWindowData(partition);
        ASSERT_TRUE(data.empty());
    }

  protected:
    void init()
    {
        numBeforeInsertion_[0] = 0;
        numBeforeInsertion_[1] = 0;
        numBeforeInsertion_[2] = 0;
        numAfterInsertion_[0] = 0;
        numAfterInsertion_[1] = 0;
        numAfterInsertion_[2] = 0;
        numBeforeFlush_[0] = 0;
        numBeforeFlush_[1] = 0;
        numBeforeFlush_[2] = 0;
        numAfterFlush_[0] = 0;
        numAfterFlush_[1] = 0;
        numAfterFlush_[2] = 0;
    }

  public:
    unsigned const windowSize_;
    unsigned numBeforeInsertion_[3];
    unsigned numAfterInsertion_[3];
    unsigned numBeforeFlush_[3];
    unsigned numAfterFlush_[3];
};

/**
 * Event handler for TumblingWindow with summarizer used for this test.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class TumblingSumHandler11 : public WindowEvent<TType>
#else
class TumblingSumHandler11
  : public WindowEvent<TType,
                       int32_t,
                       SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
#endif
{
  public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType,
                   int32_t,
                   SPL::IncrDeque<TType>,
                   SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
      WindowType;
#endif
    TumblingSumHandler11(unsigned size)
      : windowSize_(size)
      , flushCount_(0)
    {}

    void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
    {
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TupleType>& twindow = dynamic_cast<TumblingWindow<TupleType>&>(window);
#else
        TumblingWindow<TupleType, int32_t, SPL::IncrDeque<TupleType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TupleType> > >& twindow =
          dynamic_cast<
            TumblingWindow<TupleType, int32_t, SPL::IncrDeque<TupleType>,
                           SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TupleType> > >&>(window);
#endif
        TumblingWindowSummarizer<TupleType>& summarizer = *twindow.getSummarizer(partition);
        TumblingSummarizer& mySummarizer = dynamic_cast<TumblingSummarizer&>(summarizer);
        flushCount_++;
        SPCDBG(L_DEBUG,
               "partition=" << partition << " windowSize_=" << windowSize_ << " flushCount_="
                            << flushCount_ << " mySummarizer.getSum()=" << mySummarizer.getSum(),
               WINLIB_TEST);
    }

  public:
    unsigned const windowSize_;
    unsigned flushCount_;
};

/**
 * Checkpoint and reset tests for TumblingWindow with CountWindowPolicy
 * and TupleCountPartitionEvictionPolicy.  Regression test for RTC 28220.
 */
class WindowTest11 : public WindowTestBase
{
  public:
    const int winSize;    // Window size - defined by its eviction policy
    const int tupleCount; // Tuple count which triggers partition eviction
    const std::string
      partitions; // Each character points to the partition where to insert the next tuple

    WindowTest11()
      : WindowTestBase()
      , winSize(2)
      , tupleCount(5)
      , partitions("0120120120120120120")
    {}

  private:
    void runTests()
    {
        Operator& oper = *((Operator*)NULL);

        // Window and partition eviction policies
        CountWindowPolicy policy_C5(winSize);
        TupleCountPartitionEvictionPolicy tupleCountEviction(tupleCount,
                                                             PartitionEvictionPolicy::LRU);

        {
            test_winTumblingCount_resetInitial(oper, policy_C5, tupleCountEviction);
            test_winTumblingSumCount_resetInitial(oper, policy_C5, tupleCountEviction);
        }
    }

    void test_winTumblingCount_resetInitial(Operator& oper,
                                            WindowPolicy const& evict,
                                            PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        TType tuple;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingCountHandler11 handler(this->winSize);
        win.registerBeforeTupleInsertionHandler(&handler);
        win.registerAfterTupleInsertionHandler(&handler);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerAfterWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);

        // No partition eviction event handler

        SPCDBG(L_INFO, "Insert " << partitions.size() << " tuples", WINLIB_TEST);
        int z = 0;
        for (size_t i = 0; i < partitions.size(); ++i) {
            tuple.get_age() = z;
            int32_t partition = partitions.at(i) - '0';
            SPCDBG(L_INFO, " before inserting tuple age=" << z << " into partition " << partition,
                   WINLIB_TEST);
            win.insert(tuple, partition);
            SPCDBG(L_INFO, " inserted tuple age=" << z << " into partition " << partition,
                   WINLIB_TEST);
            logPartitionSizes(win);
            int dataSize = 0;
            int expectedDataSize =
              (((((z % 6) - partition) >= 0) && (((z % 6) - partition) <= 2)) ? 1 : 0);
            try {
                dataSize = win.getTupleCount(partition);
                ASSERT_EQUALS(1, dataSize);
            } catch (std::out_of_range const&) {
                // ignore and continue
                ASSERT_EQUALS(0, expectedDataSize);
            }
            ++z;
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }

    void test_winTumblingSumCount_resetInitial(Operator& oper,
                                               WindowPolicy const& evict,
                                               PartitionEvictionPolicy const& partitionEviction)
    {
        SPCDBG_ENTER(WINLIB_TEST);

        TType tuple;

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> win(oper, 0, evict, partitionEviction);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>,
                       SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
          win(oper, 0, evict, partitionEviction);
#endif
        TumblingSumHandler11 handler(this->winSize);
        win.registerBeforeWindowFlushHandler(&handler);
        win.registerSerializationHandler(&handler);
        win.registerWindowSummarizer<TumblingSummarizer>();

        // No partition eviction event handler

        SPCDBG(L_INFO, "Insert " << partitions.size() << " tuples", WINLIB_TEST);
        int z = 0;
        for (size_t i = 0; i < partitions.size(); ++i) {
            tuple.get_age() = z;
            int32_t partition = partitions.at(i) - '0';
            SPCDBG(L_INFO, " before inserting tuple age=" << z << " into partition " << partition,
                   WINLIB_TEST);
            win.insert(tuple, partition);
            SPCDBG(L_INFO, " inserted tuple age=" << z << " into partition " << partition,
                   WINLIB_TEST);
            logPartitionSizes(win);
            int dataSize = 0;
            int expectedDataSize =
              (((((z % 6) - partition) >= 0) && (((z % 6) - partition) <= 2)) ? 1 : 0);
            try {
                dataSize = win.getTupleCount(partition);
                ASSERT_EQUALS(1, dataSize);
            } catch (std::out_of_range const&) {
                // ignore and continue
                ASSERT_EQUALS(0, expectedDataSize);
            }
            ++z;
        }

        SPCDBG_EXIT(WINLIB_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WindowTest11)
