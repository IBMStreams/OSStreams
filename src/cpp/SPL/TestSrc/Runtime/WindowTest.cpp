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

#include "../Utility/UtlTestCommon.h"

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Type/TMPTuple.h>
#include <SPL/Runtime/Window/TumblingWindowSummarizer.h>
#include <SPL/Runtime/Window/Window.h>
#include <SPL/Runtime/Window/WindowEvent.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {
class TType : public tuple<int32 FIELD(age), timestamp FIELD(time)>
{
  public:
    typedef int32 age_type;
    typedef timestamp time_type;
    virtual ~TType() {}
    int32& get_age() { return getFIELD(age); }
    timestamp& get_time() { return getFIELD(time); }
    int32 const& get_age() const { return getFIELD(age); }
    timestamp const& get_time() const { return getFIELD(time); }
};

class WindowTest : public DistilleryApplication
{
  public:
    WindowTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        Operator& oper = *((Operator*)NULL);

        // Partition Eviction policies
        PartitionAgePartitionEvictionPolicy ageEviction(1.3);
        PartitionCountPartitionEvictionPolicy partitionEviction(
          100, PartitionEvictionPolicy::OperatorDefined);
        TupleCountPartitionEvictionPolicy tupleEviction(100, PartitionEvictionPolicy::LRU);

        // window policies
        // count based
        CountWindowPolicy policy_C5(5);
        CountWindowPolicy policy_C2(2);
        // punctuation based
        PunctWindowPolicy policy_P;
        // delta based
        DeltaWindowPolicy<TType,           // tuple type
                          TType::age_type, // attribute type
                          &TType::get_age  // attribute getter
                          >
          policy_Dage2(2), policy_Dage0(0);
        DeltaWindowPolicy<TType,            // tuple type
                          TType::time_type, // attribute type
                          &TType::get_time  // attribute getter
                          >
          policy_Dtime2(2.0);
        // time based
        TimeWindowPolicy policy_T8(8.0);
        TimeWindowPolicy policy_T2(2.0);

        { // without partition eviction
            // tumbling windows
            // tumbling - count
            TumblingWindow<TType> win_T_C5(oper, 0, policy_C5);
            TumblingWindow<TType> win_T_C5_s(oper, 0, policy_C5); // summarized variant
            // tumbling - punct
            TumblingWindow<TType> win_T_P(oper, 0, policy_P);
            TumblingWindow<TType> win_T_P_s(oper, 0, policy_P); // summarized variant
            // tumbling - delta
            TumblingWindow<TType> win_T_Dage2(oper, 0, policy_Dage2);
            TumblingWindow<TType> win_T_Dage2_s(oper, 0, policy_Dage2); // summarized variant
            // tumbling - time
            /// TumblingWindow<TType> win_T_T8(oper, policy_T8);

            // sliding windows
            // sliding - count, count
            SlidingWindow<TType> win_S_C5_C2(oper, 0, policy_C5, policy_C2);
            // sliding - count, count
            SlidingWindow<TType, int> win_S_C5_C2_int(oper, 0, policy_C5, policy_C2);
            // sliding - count, delta
            SlidingWindow<TType> win_S_C5_Dage2(oper, 0, policy_C5, policy_Dage2);
            // sliding - count, time
            /// SlidingWindow<TType> win_S_C5_T8(oper, 0, policy_C5, policy_T8);
            // sliding - delta, count
            SlidingWindow<TType> win_S_Dage2_C5(oper, 0, policy_Dage2, policy_C5);
            // sliding - delta, delta
            SlidingWindow<TType> win_S_Dage2_Dage0(oper, 0, policy_Dage2, policy_Dage0);
            // sliding - delta, time
            /// SlidingWindow<TType> win_S_Dage2_T2(oper, 0, policy_Dage2, policy_T2);
            // sliding - time, count
            /// SlidingWindow<TType> win_S_T8_C5(oper, 0, policy_T8, policy_C5);
            // sliding - time, delta
            /// SlidingWindow<TType> win_S_T8_Dage2(oper, 0, policy_T8, policy_Dage2);
            // sliding - time, time
            /// SlidingWindow<TType> win_S_T8_T2(oper, 0, policy_T8, policy_T2);

            TType tuple;
            test_win_T_C5(win_T_C5);
            test_win_T_C5_s(win_T_C5_s);
            test_win_T_P(win_T_P);
            test_win_T_P_s(win_T_P_s);
            test_win_T_Dage2(win_T_Dage2);
            test_win_T_Dage2_s(win_T_Dage2_s);
            /// win_T_T8.insert(tuple);

            test_win_S_C5_C2(win_S_C5_C2);
            test_win_S_C5_C2_int(win_S_C5_C2_int);
            test_win_S_C5_Dage2(win_S_C5_Dage2);
            test_win_S_Dage2_C5(win_S_Dage2_C5);
            test_win_S_Dage2_Dage0(win_S_Dage2_Dage0);
            /// win_S_C5_T8.insert(tuple);
            /// win_S_Dage2_T2.insert(tuple);
            /// win_S_T8_C5.insert(tuple);
            /// win_S_T8_Dage2.insert(tuple);
            /// win_S_T8_T2.insert(tuple);
        }
        { // with partition eviction
            // tumbling windows
            // tumbling - count
            TumblingWindow<TType> win_T_C5(oper, 0, policy_C5, ageEviction);
            TumblingWindow<TType> win_T_C5_s(oper, 0, policy_C5, ageEviction); // summarized variant
            // tumbling - punct
            TumblingWindow<TType> win_T_P(oper, 0, policy_P, partitionEviction);
            TumblingWindow<TType> win_T_P_s(oper, 0, policy_P,
                                            partitionEviction); // summarized variant
            // tumbling - delta
            TumblingWindow<TType> win_T_Dage2(oper, 0, policy_Dage2, tupleEviction);
            TumblingWindow<TType> win_T_Dage2_s(oper, 0, policy_Dage2,
                                                tupleEviction); // summarized variant
            // tumbling - time
            /// TumblingWindow<TType> win_T_T8(oper, policy_T8);

            // sliding windows
            // sliding - count, count
            SlidingWindow<TType> win_S_C5_C2(oper, 0, policy_C5, policy_C2, tupleEviction);
            // sliding - count, count
            SlidingWindow<TType, int> win_S_C5_C2_int(oper, 0, policy_C5, policy_C2, ageEviction);
            // sliding - count, delta
            SlidingWindow<TType> win_S_C5_Dage2(oper, 0, policy_C5, policy_Dage2,
                                                partitionEviction);
            // sliding - count, time
            /// SlidingWindow<TType> win_S_C5_T8(oper, 0, policy_C5, policy_T8);
            // sliding - delta, count
            SlidingWindow<TType> win_S_Dage2_C5(oper, 0, policy_Dage2, policy_C5, tupleEviction);
            // sliding - delta, delta
            SlidingWindow<TType> win_S_Dage2_Dage0(oper, 0, policy_Dage2, policy_Dage0,
                                                   ageEviction);
            // sliding - delta, time
            /// SlidingWindow<TType> win_S_Dage2_T2(oper, 0, policy_Dage2, policy_T2);
            // sliding - time, count
            /// SlidingWindow<TType> win_S_T8_C5(oper, 0, policy_T8, policy_C5);
            // sliding - time, delta
            /// SlidingWindow<TType> win_S_T8_Dage2(oper, 0, policy_T8, policy_Dage2);
            // sliding - time, time
            /// SlidingWindow<TType> win_S_T8_T2(oper, 0, policy_T8, policy_T2);

            TType tuple;
            test_win_T_C5(win_T_C5);
            test_win_T_C5_s(win_T_C5_s);
            test_win_T_P(win_T_P);
            test_win_T_P_s(win_T_P_s);
            test_win_T_Dage2(win_T_Dage2);
            test_win_T_Dage2_s(win_T_Dage2_s);
            /// win_T_T8.insert(tuple);

            test_win_S_C5_C2(win_S_C5_C2);
            test_win_S_C5_C2_int(win_S_C5_C2_int);
            test_win_S_C5_Dage2(win_S_C5_Dage2);
            test_win_S_Dage2_C5(win_S_Dage2_C5);
            test_win_S_Dage2_Dage0(win_S_Dage2_Dage0);
            /// win_S_C5_T8.insert(tuple);
            /// win_S_Dage2_T2.insert(tuple);
            /// win_S_T8_C5.insert(tuple);
            /// win_S_T8_Dage2.insert(tuple);
            /// win_S_T8_T2.insert(tuple);
        }

        return EXIT_SUCCESS;
    }

    class win_T_C5_handler : public WindowEvent<TType>
    {
      public:
        typedef Window<TType> WindowType;
        win_T_C5_handler(unsigned size)
          : windowSize_(size)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeFlush_(0)
          , numAfterFlush_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            TumblingWindow<TType>& win = dynamic_cast<TumblingWindow<TType>&>(window);
            CountWindowPolicy const& evict =
              dynamic_cast<CountWindowPolicy const&>(win.getEvictionPolicy());
            FASSERT(windowSize_ == evict.getSize());
            ++numBeforeFlush_;
            deque<TType>& data = win.getWindowData();
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            int z = 0, offset = (numBeforeFlush_ - 1) * windowSize_;
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
        }

        void afterWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            ++numAfterFlush_;
            FASSERT(numBeforeFlush_ == numAfterFlush_);
            TumblingWindow<TType>& win = dynamic_cast<TumblingWindow<TType>&>(window);
            deque<TType>& data = win.getWindowData();
            FASSERT(data.empty());
        }

      public:
        unsigned windowSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeFlush_;
        unsigned numAfterFlush_;
    };

    void test_win_T_C5(TumblingWindow<TType>& win_T_C5)
    {
        const int n = 5;
        win_T_C5_handler handler(n);
        win_T_C5.registerBeforeTupleInsertionHandler(&handler);
        win_T_C5.registerAfterTupleInsertionHandler(&handler);
        win_T_C5.registerBeforeWindowFlushHandler(&handler);
        win_T_C5.registerAfterWindowFlushHandler(&handler);

        TType tuple;
        for (int i = 0; i < 2 * n; ++i) {
            tuple.get_age() = i;
            win_T_C5.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == 2 * n);
        FASSERT(handler.numAfterInsertion_ == 2 * n);
        FASSERT(handler.numBeforeFlush_ == 2);
        FASSERT(handler.numAfterFlush_ == 2);
        for (int i = 0; i < n / 2; ++i) {
            tuple.get_age() = i;
            win_T_C5.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == 2 * n + n / 2);
        FASSERT(handler.numAfterInsertion_ == 2 * n + n / 2);
        FASSERT(handler.numBeforeFlush_ == 2);
        FASSERT(handler.numAfterFlush_ == 2);
    }

    class win_T_C5_summarizer : public TumblingWindowSummarizer<TType>
    {
      public:
        win_T_C5_summarizer(const Operator&) {}
        void onOpenWindowEvent(PartitionType const& partition)
        {
            sum_ = -1;
            currentSum_ = 0;
        }
        void onTupleInsertionEvent(TupleType const& tuple) { currentSum_ += tuple.get_age(); }
        void onCloseWindowEvent() { sum_ = currentSum_; }
        int getSum() { return sum_; }

      private:
        int currentSum_;
        int sum_;
    };

    class win_T_C5_s_handler : public WindowEvent<TType>
    {
      public:
        typedef Window<TType> WindowType;
        win_T_C5_s_handler(unsigned size)
          : windowSize_(size)
          , flushCount_(0)
        {}

        void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            TumblingWindow<TupleType>& twindow = dynamic_cast<TumblingWindow<TupleType>&>(window);
            TumblingWindowSummarizer<TupleType>& summarizer = *twindow.getSummarizer(partition);
            win_T_C5_summarizer& mySummarizer = dynamic_cast<win_T_C5_summarizer&>(summarizer);
            if (flushCount_ == 0) {
                FASSERT(mySummarizer.getSum() == (0 + 1 + 2 + 3 + 4));
            } else {
                FASSERT(mySummarizer.getSum() == (5 + 6 + 7 + 8 + 9));
            }
            flushCount_++;
        }

      public:
        unsigned windowSize_;
        unsigned flushCount_;
    };

    void test_win_T_C5_s(TumblingWindow<TType>& win_T_C5)
    {
        const int n = 5;
        win_T_C5_s_handler handler(n);
        win_T_C5.registerBeforeWindowFlushHandler(&handler);
        win_T_C5.registerWindowSummarizer<win_T_C5_summarizer>();

        TType tuple;
        for (int i = 0; i < 2 * n; ++i) {
            tuple.get_age() = i;
            win_T_C5.insert(tuple);
        }
        FASSERT(handler.flushCount_ == 2);
    }

    class win_T_Dage2_handler : public WindowEvent<TType>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_T_Dage2_handler(unsigned size)
          : windowSize_(size)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeFlush_(0)
          , numAfterFlush_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            ++numBeforeFlush_;
            deque<TType> const& data = window.getWindowData();
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            int z = 0, offset = (numBeforeFlush_ - 1) * (windowSize_ + 1);
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
        }

        void afterWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            ++numAfterFlush_;
            FASSERT(numBeforeFlush_ == numAfterFlush_);
            deque<TType> const& data = window.getWindowData();
            FASSERT(data.size() == 0);
        }

      public:
        unsigned windowSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeFlush_;
        unsigned numAfterFlush_;
    };

    void test_win_T_Dage2(TumblingWindow<TType>& win_T_Dage2)
    {
        const int d = 2;
        win_T_Dage2_handler handler(d);
        win_T_Dage2.registerBeforeTupleInsertionHandler(&handler);
        win_T_Dage2.registerAfterTupleInsertionHandler(&handler);
        win_T_Dage2.registerBeforeWindowFlushHandler(&handler);
        win_T_Dage2.registerAfterWindowFlushHandler(&handler);

        TType tuple;
        for (int i = 0; i < 4 * d; ++i) {
            tuple.get_age() = i;
            win_T_Dage2.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == 4 * d);
        FASSERT(handler.numAfterInsertion_ == 4 * d);
        FASSERT(handler.numBeforeFlush_ == 2);
        FASSERT(handler.numAfterFlush_ == 2);
        FASSERT(win_T_Dage2.getWindowData().size() == 2);
        FASSERT(win_T_Dage2.getWindowData().begin()->get_age() == 4 * d - 2 &&
                win_T_Dage2.getWindowData().rbegin()->get_age() == 4 * d - 1);
    }

    class win_T_Dage2_summarizer : public TumblingWindowSummarizer<TType>
    {
      public:
        win_T_Dage2_summarizer(const Operator&) {}
        void onOpenWindowEvent(PartitionType const& partition)
        {
            sum_ = -1;
            currentSum_ = 0;
        }
        void onTupleInsertionEvent(TupleType const& tuple) { currentSum_ += tuple.get_age(); }
        void onCloseWindowEvent() { sum_ = currentSum_; }
        int getSum() { return sum_; }

      private:
        int currentSum_;
        int sum_;
    };

    class win_T_Dage2_s_handler : public WindowEvent<TType>
    {
      public:
        typedef Window<TType> WindowType;
        win_T_Dage2_s_handler(unsigned size)
          : windowSize_(size)
          , flushCount_(0)
        {}

        void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            TumblingWindow<TupleType>& twindow = dynamic_cast<TumblingWindow<TupleType>&>(window);
            TumblingWindowSummarizer<TupleType>& summarizer = *twindow.getSummarizer(partition);
            win_T_Dage2_summarizer& mySummarizer =
              dynamic_cast<win_T_Dage2_summarizer&>(summarizer);
            if (flushCount_ == 0) {
                FASSERT(mySummarizer.getSum() == (0 + 1 + 2));
            } else {
                FASSERT(mySummarizer.getSum() == (3 + 4 + 5));
            }
            flushCount_++;
        }

      public:
        unsigned windowSize_;
        unsigned flushCount_;
    };

    void test_win_T_Dage2_s(TumblingWindow<TType>& win_T_Dage2)
    {
        const int d = 2;
        win_T_Dage2_s_handler handler(d);
        win_T_Dage2.registerBeforeWindowFlushHandler(&handler);
        win_T_Dage2.registerWindowSummarizer<win_T_Dage2_summarizer>();

        TType tuple;
        for (int i = 0; i < 4 * d; ++i) {
            tuple.get_age() = i;
            win_T_Dage2.insert(tuple);
        }
        FASSERT(handler.flushCount_ == 2);
    }

    class win_T_P_handler : public WindowEvent<TType>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_T_P_handler(unsigned size)
          : windowSize_(size)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeFlush_(0)
          , numAfterFlush_(0)
          , numEmptyPunct_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void onEmptyWindowPunctEvent(WindowType& window) { numEmptyPunct_++; }

        void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            ++numBeforeFlush_;
            deque<TType> const& data = window.getWindowData();
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            int z = 0, offset = (numBeforeFlush_ - 1) * windowSize_;
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
        }

        void afterWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            ++numAfterFlush_;
            FASSERT(numBeforeFlush_ == numAfterFlush_);
            deque<TType> const& data = window.getWindowData();
            FASSERT(data.size() == 0);
        }

      public:
        unsigned windowSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeFlush_;
        unsigned numAfterFlush_;
        unsigned numEmptyPunct_;
    };

    void test_win_T_P(TumblingWindow<TType>& win_T_P)
    {
        const int n = 6;
        win_T_P_handler handler(n / 2);
        win_T_P.registerBeforeTupleInsertionHandler(&handler);
        win_T_P.registerAfterTupleInsertionHandler(&handler);
        win_T_P.registerBeforeWindowFlushHandler(&handler);
        win_T_P.registerAfterWindowFlushHandler(&handler);
        win_T_P.registerOnEmptyWindowPunctEvent(&handler);

        TType tuple;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win_T_P.insert(tuple);
            if ((i + 1) % (n / 2) == 0) {
                win_T_P.insert(Punctuation::WindowMarker);
            }
        }
        tuple.get_age() = n;
        win_T_P.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == n + 1);
        FASSERT(handler.numAfterInsertion_ == n + 1);
        FASSERT(handler.numBeforeFlush_ == 2);
        FASSERT(handler.numAfterFlush_ == 2);
        FASSERT(win_T_P.getWindowData().size() == 1);
        FASSERT(win_T_P.getWindowData().begin()->get_age() == n);
        FASSERT(handler.numEmptyPunct_ == 0);
        win_T_P.insert(Punctuation::WindowMarker);
        win_T_P.insert(Punctuation::WindowMarker);
        FASSERT(handler.numEmptyPunct_ == 1);
    }

    class win_T_P_summarizer : public TumblingWindowSummarizer<TType>
    {
      public:
        win_T_P_summarizer(const Operator&) {}
        void onOpenWindowEvent(PartitionType const& partition)
        {
            sum_ = -1;
            currentSum_ = 0;
        }
        void onTupleInsertionEvent(TupleType const& tuple) { currentSum_ += tuple.get_age(); }
        void onCloseWindowEvent() { sum_ = currentSum_; }
        int getSum() { return sum_; }

      private:
        int currentSum_;
        int sum_;
    };

    class win_T_P_s_handler : public WindowEvent<TType>
    {
      public:
        typedef Window<TType> WindowType;
        win_T_P_s_handler(unsigned size)
          : windowSize_(size)
          , flushCount_(0)
          , numEmptyPunct_(0)
        {}

        void beforeWindowFlushEvent(WindowType& window, PartitionType const& partition)
        {
            TumblingWindow<TupleType>& twindow = dynamic_cast<TumblingWindow<TupleType>&>(window);
            TumblingWindowSummarizer<TupleType>& summarizer = *twindow.getSummarizer(partition);
            win_T_P_summarizer& mySummarizer = dynamic_cast<win_T_P_summarizer&>(summarizer);
            if (flushCount_ == 0) {
                FASSERT(mySummarizer.getSum() == (0 + 1 + 2));
            } else {
                FASSERT(mySummarizer.getSum() == (3 + 4 + 5));
            }
            flushCount_++;
        }
        void onEmptyWindowPunctEvent(WindowType& window) { numEmptyPunct_++; }

      public:
        unsigned windowSize_;
        unsigned flushCount_;
        unsigned numEmptyPunct_;
    };

    void test_win_T_P_s(TumblingWindow<TType>& win_T_P)
    {
        const int n = 6;
        win_T_P_s_handler handler(n / 2);
        win_T_P.registerBeforeWindowFlushHandler(&handler);
        win_T_P.registerWindowSummarizer<win_T_P_summarizer>();
        win_T_P.registerOnEmptyWindowPunctEvent(&handler);
        TType tuple;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = i;
            win_T_P.insert(tuple);
            if ((i + 1) % (n / 2) == 0) {
                win_T_P.insert(Punctuation::WindowMarker);
            }
        }
        FASSERT(handler.flushCount_ == 2);
        FASSERT(handler.numEmptyPunct_ == 0);
        win_T_P.insert(Punctuation::WindowMarker);
        FASSERT(handler.numEmptyPunct_ == 1);
    }

    class win_S_C5_C2_handler : public WindowEvent<TType>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_S_C5_C2_handler(unsigned wsize, unsigned tsize)
          : windowSize_(wsize)
          , triggerSize_(tsize)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeEviction_(0)
          , numAfterEviction_(0)
          , numOnTrigger_(0)
          , numOnInitialFull_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void beforeTupleEvictionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            ++numBeforeEviction_;
        }

        void afterTupleEvictionEvent(WindowType& window,
                                     TupleType& tuple,
                                     PartitionType const& partition)
        {
            ++numAfterEviction_;
            FASSERT(numBeforeEviction_ == numAfterEviction_);
        }

        void onWindowTriggerEvent(WindowType& window, PartitionType const& partition)
        {
            ++numOnTrigger_;
            deque<TType> const& data = window.getWindowData();
            int z = 0, offset = numAfterEviction_;
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
            if (data.size() >= windowSize_) {
                FASSERT(numOnTrigger_ - 2 == (numBeforeEviction_ + 1) / triggerSize_);
            }
            if (numOnTrigger_ > 2) {
                FASSERT(data.size() == windowSize_);
            }
        }

        void onWindowInitialFullEvent(WindowType& window, PartitionType const& partition)
        {
            numOnInitialFull_++;
            FASSERT(numAfterInsertion_ == windowSize_);
            FASSERT(numOnTrigger_ == (windowSize_ - 1) / triggerSize_);
        }

      public:
        unsigned windowSize_;
        unsigned triggerSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeEviction_;
        unsigned numAfterEviction_;
        unsigned numOnTrigger_;
        unsigned numOnInitialFull_;
    };

    void test_win_S_C5_C2(SlidingWindow<TType>& win_S_C5_C2)
    {
        const int n = 5, m = 2;
        win_S_C5_C2_handler handler(n, m);
        win_S_C5_C2.registerBeforeTupleInsertionHandler(&handler);
        win_S_C5_C2.registerAfterTupleInsertionHandler(&handler);
        win_S_C5_C2.registerBeforeTupleEvictionHandler(&handler);
        win_S_C5_C2.registerAfterTupleEvictionHandler(&handler);
        win_S_C5_C2.registerOnWindowTriggerHandler(&handler);
        win_S_C5_C2.registerOnWindowInitialFullHandler(&handler);

        TType tuple;
        int z = 0;
        for (int i = 0; i < n; ++i) {
            tuple.get_age() = z++;
            win_S_C5_C2.insert(tuple);
        }
        FASSERT(win_S_C5_C2.getWindowData().size() == n);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 0);
        FASSERT(handler.numAfterEviction_ == 0);
        FASSERT(handler.numOnTrigger_ == 2);
        FASSERT(handler.numOnInitialFull_ == 1)

        tuple.get_age() = z++;
        win_S_C5_C2.insert(tuple);
        FASSERT(win_S_C5_C2.getWindowData().size() == n);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 1);
        FASSERT(handler.numAfterEviction_ == 1);
        FASSERT(handler.numOnTrigger_ == 3);
        FASSERT(handler.numOnInitialFull_ == 1)

        tuple.get_age() = z++;
        win_S_C5_C2.insert(tuple);
        FASSERT(win_S_C5_C2.getWindowData().size() == n);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 2);
        FASSERT(handler.numAfterEviction_ == 2);
        FASSERT(handler.numOnTrigger_ == 3);
        FASSERT(handler.numOnInitialFull_ == 1)
    }

    class win_S_C5_C2_int_handler : public WindowEvent<TType, int>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_S_C5_C2_int_handler(unsigned wsize, unsigned tsize)
          : windowSize_(wsize)
          , triggerSize_(tsize)
          , numBeforeInsertion1_(0)
          , numAfterInsertion1_(0)
          , numBeforeEviction1_(0)
          , numAfterEviction1_(0)
          , numOnTrigger1_(0)
          , numOnInitialFull1_(0)
          , numBeforeInsertion2_(0)
          , numAfterInsertion2_(0)
          , numBeforeEviction2_(0)
          , numAfterEviction2_(0)
          , numOnTrigger2_(0)
          , numOnInitialFull2_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            if (partition == 1) {
                numBeforeInsertion1_++;
            } else if (partition == 2) {
                numBeforeInsertion2_++;
            }
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            if (partition == 1) {
                numAfterInsertion1_++;
                FASSERT(numBeforeInsertion1_ == numAfterInsertion1_);
            } else if (partition == 2) {
                numAfterInsertion2_++;
                FASSERT(numBeforeInsertion2_ == numAfterInsertion2_);
            }
        }

        void beforeTupleEvictionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            if (partition == 1) {
                ++numBeforeEviction1_;
            } else if (partition == 2) {
                ++numBeforeEviction2_;
            }
        }

        void afterTupleEvictionEvent(WindowType& window,
                                     TupleType& tuple,
                                     PartitionType const& partition)
        {
            if (partition == 1) {
                ++numAfterEviction1_;
                FASSERT(numBeforeEviction1_ == numAfterEviction1_);
            } else if (partition == 2) {
                ++numAfterEviction2_;
                FASSERT(numBeforeEviction2_ == numAfterEviction2_);
            }
        }

        void onWindowTriggerEvent(WindowType& window, PartitionType const& partition)
        {
            if (partition == 1) {
                ++numOnTrigger1_;
                deque<TType> const& data = window.getWindowData(partition);
                int z = 0, offset = numAfterEviction1_;
                deque<TType>::const_iterator bit = data.begin();
                deque<TType>::const_iterator eit = data.end();
                for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                    FASSERT(it->get_age() == 2 * ((z++) + offset));
                }
                if (data.size() >= windowSize_) {
                    FASSERT(numOnTrigger1_ - 2 == (numBeforeEviction1_ + 1) / triggerSize_);
                }
                if (numOnTrigger1_ > 2) {
                    FASSERT(data.size() == windowSize_);
                }
            } else if (partition == 2) {
                ++numOnTrigger2_;
                deque<TType> const& data = window.getWindowData(partition);
                int z = 0, offset = numAfterEviction2_;
                deque<TType>::const_iterator bit = data.begin();
                deque<TType>::const_iterator eit = data.end();
                for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                    FASSERT(it->get_age() == 2 * ((z++) + offset) + 1);
                }
                if (data.size() >= windowSize_) {
                    FASSERT(numOnTrigger2_ - 2 == (numBeforeEviction2_ + 1) / triggerSize_);
                }
                if (numOnTrigger2_ > 2) {
                    FASSERT(data.size() == windowSize_);
                }
            }
        }

        void onWindowInitialFullEvent(WindowType& window, PartitionType const& partition)
        {
            if (partition == 1) {
                numOnInitialFull1_++;
                FASSERT(numAfterInsertion1_ == windowSize_);
                FASSERT(numOnTrigger1_ == (windowSize_ - 1) / triggerSize_);
            } else if (partition == 2) {
                numOnInitialFull2_++;
                FASSERT(numAfterInsertion2_ == windowSize_);
                FASSERT(numOnTrigger2_ == (windowSize_ - 1) / triggerSize_);
            }
        }

      public:
        unsigned windowSize_;
        unsigned triggerSize_;
        unsigned numBeforeInsertion1_;
        unsigned numAfterInsertion1_;
        unsigned numBeforeEviction1_;
        unsigned numAfterEviction1_;
        unsigned numOnTrigger1_;
        unsigned numOnInitialFull1_;
        unsigned numBeforeInsertion2_;
        unsigned numAfterInsertion2_;
        unsigned numBeforeEviction2_;
        unsigned numAfterEviction2_;
        unsigned numOnTrigger2_;
        unsigned numOnInitialFull2_;
    };

    void test_win_S_C5_C2_int(SlidingWindow<TType, int>& win_S_C5_C2_int)
    {
        const int n = 5, m = 2;
        win_S_C5_C2_int_handler handler(n, m);
        win_S_C5_C2_int.registerBeforeTupleInsertionHandler(&handler);
        win_S_C5_C2_int.registerAfterTupleInsertionHandler(&handler);
        win_S_C5_C2_int.registerBeforeTupleEvictionHandler(&handler);
        win_S_C5_C2_int.registerAfterTupleEvictionHandler(&handler);
        win_S_C5_C2_int.registerOnWindowTriggerHandler(&handler);
        win_S_C5_C2_int.registerOnWindowInitialFullHandler(&handler);

        TType tuple;
        int z = 0, i;
        for (i = 0; i < n; ++i) {
            tuple.get_age() = z++;
            win_S_C5_C2_int.insert(tuple, 1 + i % 2);
        }
        for (i = n; i < 2 * n; ++i) {
            tuple.get_age() = z++;
            win_S_C5_C2_int.insert(tuple, 1 + i % 2);
        }

        FASSERT(win_S_C5_C2_int.getWindowData(1).size() == n);
        FASSERT(win_S_C5_C2_int.getWindowData(2).size() == n);
        FASSERT(handler.numBeforeInsertion1_ == z / 2);
        FASSERT(handler.numAfterInsertion1_ == z / 2);
        FASSERT(handler.numBeforeEviction1_ == 0);
        FASSERT(handler.numAfterEviction1_ == 0);
        FASSERT(handler.numOnTrigger1_ == 2);
        FASSERT(handler.numOnInitialFull1_ == 1)
        FASSERT(handler.numBeforeInsertion2_ == z / 2);
        FASSERT(handler.numAfterInsertion2_ == z / 2);
        FASSERT(handler.numBeforeEviction2_ == 0);
        FASSERT(handler.numAfterEviction2_ == 0);
        FASSERT(handler.numOnTrigger2_ == 2);
        FASSERT(handler.numOnInitialFull2_ == 1)

        tuple.get_age() = z++;
        win_S_C5_C2_int.insert(tuple, 1 + (i++ % 2));
        tuple.get_age() = z++;
        win_S_C5_C2_int.insert(tuple, 1 + (i++ % 2));

        FASSERT(win_S_C5_C2_int.getWindowData(1).size() == n);
        FASSERT(handler.numBeforeInsertion1_ == z / 2);
        FASSERT(handler.numAfterInsertion1_ == z / 2);
        FASSERT(handler.numBeforeEviction1_ == 1);
        FASSERT(handler.numAfterEviction1_ == 1);
        FASSERT(handler.numOnTrigger1_ == 3);
        FASSERT(handler.numOnInitialFull1_ == 1)
        FASSERT(win_S_C5_C2_int.getWindowData(2).size() == n);
        FASSERT(handler.numBeforeInsertion2_ == z / 2);
        FASSERT(handler.numAfterInsertion2_ == z / 2);
        FASSERT(handler.numBeforeEviction2_ == 1);
        FASSERT(handler.numAfterEviction2_ == 1);
        FASSERT(handler.numOnTrigger2_ == 3);
        FASSERT(handler.numOnInitialFull2_ == 1)

        tuple.get_age() = z++;
        win_S_C5_C2_int.insert(tuple, 1 + (i++ % 2));
        tuple.get_age() = z++;
        win_S_C5_C2_int.insert(tuple, 1 + (i++ % 2));
        FASSERT(win_S_C5_C2_int.getWindowData(1).size() == n);
        FASSERT(handler.numBeforeInsertion1_ == z / 2);
        FASSERT(handler.numAfterInsertion1_ == z / 2);
        FASSERT(handler.numBeforeEviction1_ == 2);
        FASSERT(handler.numAfterEviction1_ == 2);
        FASSERT(handler.numOnTrigger1_ == 3);
        FASSERT(handler.numOnInitialFull1_ == 1);
        FASSERT(win_S_C5_C2_int.getWindowData(2).size() == n);
        FASSERT(handler.numBeforeInsertion2_ == z / 2);
        FASSERT(handler.numAfterInsertion2_ == z / 2);
        FASSERT(handler.numBeforeEviction2_ == 2);
        FASSERT(handler.numAfterEviction2_ == 2);
        FASSERT(handler.numOnTrigger2_ == 3);
        FASSERT(handler.numOnInitialFull2_ == 1);
    }

    class win_S_C5_Dage2_handler : public WindowEvent<TType>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_S_C5_Dage2_handler(unsigned wsize, unsigned tsize)
          : windowSize_(wsize)
          , triggerSize_(tsize)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeEviction_(0)
          , numAfterEviction_(0)
          , numOnTrigger_(0)
          , numOnInitialFull_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void beforeTupleEvictionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            ++numBeforeEviction_;
        }

        void afterTupleEvictionEvent(WindowType& window,
                                     TupleType& tuple,
                                     PartitionType const& partition)
        {
            ++numAfterEviction_;
            FASSERT(numBeforeEviction_ == numAfterEviction_);
        }

        void onWindowTriggerEvent(WindowType& window, PartitionType const& partition)
        {
            ++numOnTrigger_;
            deque<TType> const& data = window.getWindowData();
            int z = 0, offset = numAfterEviction_;
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
            // make sure trigger is before eviction
            if (data.size() >= windowSize_) {
                FASSERT(numOnTrigger_ - 1 == (numBeforeEviction_ + 2) / (triggerSize_ + 1));
            }
            if (numOnTrigger_ > 1) {
                FASSERT(data.size() == windowSize_);
            }
        }

        void onWindowInitialFullEvent(WindowType& window, PartitionType const& partition)
        {
            numOnInitialFull_++;
            FASSERT(numAfterInsertion_ == windowSize_);
            FASSERT(numOnTrigger_ == windowSize_ / (triggerSize_ + 1));
        }

      public:
        unsigned windowSize_;
        unsigned triggerSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeEviction_;
        unsigned numAfterEviction_;
        unsigned numOnTrigger_;
        unsigned numOnInitialFull_;
    };

    void test_win_S_C5_Dage2(SlidingWindow<TType>& win_S_C5_Dage2)
    {
        const int n = 5, m = 2;
        win_S_C5_Dage2_handler handler(n, m);
        win_S_C5_Dage2.registerBeforeTupleInsertionHandler(&handler);
        win_S_C5_Dage2.registerAfterTupleInsertionHandler(&handler);
        win_S_C5_Dage2.registerBeforeTupleEvictionHandler(&handler);
        win_S_C5_Dage2.registerAfterTupleEvictionHandler(&handler);
        win_S_C5_Dage2.registerOnWindowTriggerHandler(&handler);
        win_S_C5_Dage2.registerOnWindowInitialFullHandler(&handler);

        TType tuple;
        int z = 0;
        for (int i = 0; i < 3; ++i) {
            tuple.get_age() = z++;
            win_S_C5_Dage2.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 0);
        FASSERT(handler.numAfterEviction_ == 0);
        FASSERT(handler.numOnTrigger_ == 0);
        FASSERT(handler.numOnInitialFull_ == 0);

        tuple.get_age() = z++;
        win_S_C5_Dage2.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 0);
        FASSERT(handler.numAfterEviction_ == 0);
        FASSERT(handler.numOnTrigger_ == 1);
        FASSERT(handler.numOnInitialFull_ == 0);

        tuple.get_age() = z++;
        win_S_C5_Dage2.insert(tuple);
        FASSERT(win_S_C5_Dage2.getWindowData().size() == n);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 0);
        FASSERT(handler.numAfterEviction_ == 0);
        FASSERT(handler.numOnTrigger_ == 1);
        FASSERT(handler.numOnInitialFull_ == 1);

        tuple.get_age() = z++;
        win_S_C5_Dage2.insert(tuple);
        FASSERT(win_S_C5_Dage2.getWindowData().size() == n);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 1);
        FASSERT(handler.numAfterEviction_ == 1);
        FASSERT(handler.numOnTrigger_ == 1);
        FASSERT(handler.numOnInitialFull_ == 1);

        tuple.get_age() = z++;
        win_S_C5_Dage2.insert(tuple);
        FASSERT(win_S_C5_Dage2.getWindowData().size() == n);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 2);
        FASSERT(handler.numAfterEviction_ == 2);
        FASSERT(handler.numOnTrigger_ == 2);
        FASSERT(handler.numOnInitialFull_ == 1);

        for (int i = 0; i < 3; ++i) {
            tuple.get_age() = z++;
            win_S_C5_Dage2.insert(tuple);
        }
    }

    class win_S_Dage2_C5_handler : public WindowEvent<TType>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_S_Dage2_C5_handler(unsigned wsize, unsigned tsize)
          : windowSize_(wsize)
          , triggerSize_(tsize)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeEviction_(0)
          , numAfterEviction_(0)
          , numOnTrigger_(0)
          , numOnInitialFull_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void beforeTupleEvictionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            ++numBeforeEviction_;
        }

        void afterTupleEvictionEvent(WindowType& window,
                                     TupleType& tuple,
                                     PartitionType const& partition)
        {
            ++numAfterEviction_;
            FASSERT(numBeforeEviction_ == numAfterEviction_);
        }

        void onWindowTriggerEvent(WindowType& window, PartitionType const& partition)
        {
            ++numOnTrigger_;
            deque<TType> const& data = window.getWindowData();
            int z = 0, offset = numAfterEviction_;
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
            FASSERT(numOnTrigger_ == (numBeforeInsertion_ / triggerSize_));
            FASSERT(data.rbegin()->get_age() - data.begin()->get_age() == windowSize_);
        }

        void onWindowInitialFullEvent(WindowType& window, PartitionType const& partition)
        {
            numOnInitialFull_++;
            FASSERT(numAfterInsertion_ == windowSize_ + 1);
            FASSERT(numOnTrigger_ == (windowSize_ + 1) / (triggerSize_ + 1));
        }

      public:
        unsigned windowSize_;
        unsigned triggerSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeEviction_;
        unsigned numAfterEviction_;
        unsigned numOnTrigger_;
        unsigned numOnInitialFull_;
    };

    void test_win_S_Dage2_C5(SlidingWindow<TType>& win_S_Dage2_C5)
    {
        const int n = 2, m = 5;
        win_S_Dage2_C5_handler handler(n, m);
        win_S_Dage2_C5.registerBeforeTupleInsertionHandler(&handler);
        win_S_Dage2_C5.registerAfterTupleInsertionHandler(&handler);
        win_S_Dage2_C5.registerBeforeTupleEvictionHandler(&handler);
        win_S_Dage2_C5.registerAfterTupleEvictionHandler(&handler);
        win_S_Dage2_C5.registerOnWindowTriggerHandler(&handler);
        win_S_Dage2_C5.registerOnWindowInitialFullHandler(&handler);

        TType tuple;
        int z = 0;
        for (int i = 0; i < n + 1; ++i) {
            tuple.get_age() = z++;
            win_S_Dage2_C5.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 0);
        FASSERT(handler.numAfterEviction_ == 0);
        FASSERT(handler.numOnTrigger_ == 0);
        FASSERT(handler.numOnInitialFull_ == 0); // it is full, but we do not
                                                 // know it untill we get the next

        tuple.get_age() = z++;
        win_S_Dage2_C5.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 1);
        FASSERT(handler.numAfterEviction_ == 1);
        FASSERT(handler.numOnTrigger_ == 0);
        FASSERT(handler.numOnInitialFull_ == 1);

        tuple.get_age() = z++;
        win_S_Dage2_C5.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 2);
        FASSERT(handler.numAfterEviction_ == 2);
        FASSERT(handler.numOnTrigger_ == 1);
        FASSERT(handler.numOnInitialFull_ == 1);

        for (int i = 0; i < m; ++i) {
            tuple.get_age() = z++;
            win_S_Dage2_C5.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 7);
        FASSERT(handler.numAfterEviction_ == 7);
        FASSERT(handler.numOnTrigger_ == 2);
        FASSERT(handler.numOnInitialFull_ == 1);

        z++;
        tuple.get_age() = z++;
        win_S_Dage2_C5.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == z - 1);
        FASSERT(handler.numAfterInsertion_ == z - 1);
        FASSERT(handler.numBeforeEviction_ == 9);
        FASSERT(handler.numAfterEviction_ == 9);
        FASSERT(handler.numOnTrigger_ == 2);
        FASSERT(handler.numOnInitialFull_ == 1);
    }

    class win_S_Dage2_Dage0_handler : public WindowEvent<TType>
    {
      public:
        typedef TType TupleTyp;
        typedef Window<TType> WindowType;
        win_S_Dage2_Dage0_handler(unsigned wsize, unsigned tsize)
          : windowSize_(wsize)
          , triggerSize_(tsize)
          , numBeforeInsertion_(0)
          , numAfterInsertion_(0)
          , numBeforeEviction_(0)
          , numAfterEviction_(0)
          , numOnTrigger_(0)
          , numOnInitialFull_(0)
        {}

        void beforeTupleInsertionEvent(WindowType& window,
                                       TupleType const& tuple,
                                       PartitionType const& partition)
        {
            numBeforeInsertion_++;
        }

        void afterTupleInsertionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            numAfterInsertion_++;
            FASSERT(numBeforeInsertion_ == numAfterInsertion_);
        }

        void beforeTupleEvictionEvent(WindowType& window,
                                      TupleType& tuple,
                                      PartitionType const& partition)
        {
            ++numBeforeEviction_;
        }

        void afterTupleEvictionEvent(WindowType& window,
                                     TupleType& tuple,
                                     PartitionType const& partition)
        {
            ++numAfterEviction_;
            FASSERT(numBeforeEviction_ == numAfterEviction_);
        }

        void onWindowTriggerEvent(WindowType& window, PartitionType const& partition)
        {
            ++numOnTrigger_;
            deque<TType> const& data = window.getWindowData();
            int z = 0, offset = numAfterEviction_;
            deque<TType>::const_iterator bit = data.begin();
            deque<TType>::const_iterator eit = data.end();
            for (deque<TType>::const_iterator it = bit; it != eit; ++it) {
                FASSERT(it->get_age() == (z++) + offset);
            }
            // make sure trigger happens before eviction
            if (data.size() >= windowSize_ + 1) {
                FASSERT(numOnTrigger_ == (windowSize_ + numBeforeEviction_ + 1));
            }
            if (numOnTrigger_ > windowSize_) {
                FASSERT(data.rbegin()->get_age() - data.begin()->get_age() == windowSize_);
            }
        }

        void onWindowInitialFullEvent(WindowType& window, PartitionType const& partition)
        {
            numOnInitialFull_++;
            FASSERT(numAfterInsertion_ == windowSize_ + 1);
            FASSERT(numOnTrigger_ == (windowSize_ + 1) / (triggerSize_ + 1));
        }

      public:
        unsigned windowSize_;
        unsigned triggerSize_;
        unsigned numBeforeInsertion_;
        unsigned numAfterInsertion_;
        unsigned numBeforeEviction_;
        unsigned numAfterEviction_;
        unsigned numOnTrigger_;
        unsigned numOnInitialFull_;
    };

    void test_win_S_Dage2_Dage0(SlidingWindow<TType>& win_S_Dage2_Dage0)
    {
        const int n = 2, m = 0;
        win_S_Dage2_Dage0_handler handler(n, m);
        win_S_Dage2_Dage0.registerBeforeTupleInsertionHandler(&handler);
        win_S_Dage2_Dage0.registerAfterTupleInsertionHandler(&handler);
        win_S_Dage2_Dage0.registerBeforeTupleEvictionHandler(&handler);
        win_S_Dage2_Dage0.registerAfterTupleEvictionHandler(&handler);
        win_S_Dage2_Dage0.registerOnWindowTriggerHandler(&handler);
        win_S_Dage2_Dage0.registerOnWindowInitialFullHandler(&handler);

        TType tuple;
        int z = 0;
        for (int i = 0; i < n + 1; ++i) {
            tuple.get_age() = z++;
            win_S_Dage2_Dage0.insert(tuple);
        }
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 0);
        FASSERT(handler.numAfterEviction_ == 0);
        FASSERT(handler.numOnTrigger_ == 2);
        FASSERT(handler.numOnInitialFull_ == 0); // window is full, but we
                                                 // won't know it until next tuple

        tuple.get_age() = z++;
        win_S_Dage2_Dage0.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 1);
        FASSERT(handler.numAfterEviction_ == 1);
        FASSERT(handler.numOnTrigger_ == 3);
        FASSERT(handler.numOnInitialFull_ == 1);

        tuple.get_age() = z++;
        win_S_Dage2_Dage0.insert(tuple);
        FASSERT(handler.numBeforeInsertion_ == z);
        FASSERT(handler.numAfterInsertion_ == z);
        FASSERT(handler.numBeforeEviction_ == 2);
        FASSERT(handler.numAfterEviction_ == 2);
        FASSERT(handler.numOnTrigger_ == 4);
        FASSERT(handler.numOnInitialFull_ == 1);
    }
};
};

MAIN_APP(SPL::WindowTest)
