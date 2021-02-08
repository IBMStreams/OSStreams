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

#ifndef TEST_RUNTIME_WINDOWTESTCOMMON_H
#define TEST_RUNTIME_WINDOWTESTCOMMON_H

#include <SPL/TestSrc/Utility/TestUtils.h>
#include <SPL/Runtime/Type/TMPTuple.h>
#include <SPL/Runtime/Window/Window.h>
#include <SPL/Runtime/Window/WindowEvent.h>
#include <SPL/Runtime/Window/TumblingWindowSummarizer.h>
#include <SPL/TestSrc/Runtime/MemoryDataStoreAdapter.h>
#include <SPL/Runtime/Common/RuntimeException.h>

#include <TRC/ConsoleLogger.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RollingFileTracer.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/BacktraceDumper.h>

#include <boost/lexical_cast.hpp>
#include <tr1/memory>

/// The macro TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT controls whether to use incremental deque (SPL::IncrDeque) and
/// unordered map (SPL::IncrUnorderedMap) to instantiate window templates.
/// Comment it out to use the default std::deque and std::tr1::unordered_map to instantiate SPL windows.
#define TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT 1

using namespace std;

namespace SPL
{
/// Test tracing aspect
const static std::string WINLIB_TEST = ":::WinTest";


/**
 * Test tuple class.
 */
class TType : public tuple<int32     FIELD(age),
                           timestamp FIELD(time),
                           rstring   FIELD(msg)>
{
public:
    typedef int32 age_type;
    typedef timestamp time_type;
    typedef rstring msg_type;
    virtual ~TType() {}
    int32 & get_age() { return getFIELD(age); }
    timestamp & get_time() { return getFIELD(time); }
    int32 const & get_age() const { return getFIELD(age); }
    timestamp const & get_time() const { return getFIELD(time); }
    rstring & get_msg() { return getFIELD(msg); }
    rstring const & get_msg() const { return getFIELD(msg); }
};


/**
 * Checkpoint RAII wrapper which calls Checkpoint#finishCheckpointing before
 * being destructed.
 */
class CheckpointSave
{
public:
    CheckpointSave(DataStoreEntry * e, const int64_t & key) : ckpt_(e, key, true) {}
    Checkpoint & get()
    { return ckpt_; }
    ~CheckpointSave() { ckpt_.finishCheckpointing(); }
private:
    Checkpoint ckpt_;
};

typedef std::tr1::shared_ptr<Checkpoint> CheckpointPtr;
typedef std::tr1::shared_ptr<CheckpointSave> CheckpointSavePtr;

/** Static Checkpoint data store Option factory */
inline Option makeOption(bool createIfMissing, bool errorIfExist, void * others)
{
    Option o; o.create_if_missing = createIfMissing; o.error_if_exist = errorIfExist; o.lowLevelOptions = others;
    return o;
}

/** Print dequeue of tuples */
inline void operator<<(std::ostream & s, const std::deque<SPL::TType> & data)
{
    for (std::deque<SPL::TType>::const_iterator dataIt = data.begin(); dataIt != data.end(); ++dataIt) {
        if (dataIt != data.begin()) {
            s << ", ";
        }
        s << *dataIt;
    }
}

#ifdef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
inline void operator<<(std::ostream & s, const SPL::IncrDeque<SPL::TType> & data)
{
    for (std::deque<SPL::TType>::const_iterator dataIt = data.begin(); dataIt != data.end(); ++dataIt) {
        if (dataIt != data.begin()) {
            s << ", ";
        }
        s << *dataIt;
    }
}
#endif

/**
 * Utility function which checkpoints window 'win' when
 * 'tupleCount' == 'checkpointCount'
 *
 * @param win window to checkpoint
 * @param ckpt shared pointer to a CheckpointSave object
 * @param tupleCount current tuple count
 * @param checkpointCount count at which to do checkpoint
 * @return @c true if checkpoint executed, otherwise @c false.
 */
template<typename W>
inline bool checkpointAtCount(W & win, CheckpointSavePtr ckpt,
        const int tupleCount, const int checkpointCount)
{
    if (tupleCount == checkpointCount) {
        SPCDBG(L_INFO, "Checkpoint window to checkpoint " << ckpt->get().getSequenceId() << " after " << checkpointCount << " tuples", WINLIB_TEST);
        win.checkpoint((*ckpt).get());
        SPCDBG(L_DEBUG, "Window after checkpoint " << ckpt->get().getSequenceId() << ": " << win.toString(), WINLIB_TEST);
        return true;
    }
    return false;
}


/**
 * Base of all windowing library unit tests.
 * Creates a default checkpoint DataStoreEntry.
 */
class WindowTestBase : public TestBase
{
public:
    WindowTestBase() : store_(), uniqueId_(0) {}

    /**
     * Return a writable Checkpoint with the specified id.
     * @param id checkpoint id
     */
    CheckpointSavePtr getCheckpointSave(int64_t id) {
        // XXX The new DataStoreEntry that getDataStoreEntry() allocates is deleted by the Checkpoint destructor
        // TODO find better alternative
        DataStoreEntry* entry = store_.getDataStoreEntry(
                WindowTestBase::dataStoreEntryName(id),
                makeOption(false, false, NULL));
        return CheckpointSavePtr(new CheckpointSave(entry, id));
    }

    /**
     * Return the Checkpoint with the specified id for read.
     * @param id checkpoint id
     */
    CheckpointPtr getCheckpointRestore(int64_t id) {
        std::string entryName = WindowTestBase::dataStoreEntryName(id);
        if (store_.isExistingDataStoreEntry(entryName)) {
            DataStoreEntry* entry = store_.getDataStoreEntry(entryName,
                    makeOption(false, false, NULL));
            return CheckpointPtr(new Checkpoint(entry, id, false));
        } else {
            THROW(SPLRuntime, "Could not find DataStoreEntry with name " << entryName);
        }
    }

    /**
     * Create new unique id. Used to generate checkpoint sequence ids.
     * @return new id
     */
    int64_t newId() {
        return uniqueId_++;
    }

    /**
     * Return the current id.
     */
    int64_t getId() {
        return uniqueId_;
    }

protected:
    virtual ~WindowTestBase() {}

private:
    static std::string dataStoreEntryName(int64_t id) {
        static std::string prefix("WindowTest_");
        return prefix + boost::lexical_cast<std::string>(id);
    }

    MemoryDataStoreAdapter store_;
    int64_t uniqueId_;
};

/**
 * Window utility methods.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class WindowEventUtils : public WindowEvent<TType>
#else
class WindowEventUtils : public WindowEvent<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
#endif
{
public:
    /**
     * Determines whether partition exists in window.
     * @param window
     * @param partition
     * @return true if partition exists, otherwise false.
     */
    static bool existsPartition(WindowType & window, PartitionType const & partition) {
        WindowType::StorageType & stg = window.getWindowStorage();
        WindowType::StorageType::iterator it = stg.find(partition);
        return (it!=stg.end());
    }

    /**
     * Get count of tuples stored in the partition data.
     * @param window
     * @param partition
     * @return count of tuples.
     */
    static size_t getPartitionSize(WindowType & window, PartitionType const & partition) {
        WindowType::StorageType & stg = window.getWindowStorage();
        WindowType::StorageType::iterator it = stg.find(partition);
        if (it!=stg.end()) {
            DataType & data = it->second;
            return data.size();
        } else {
            return 0;
        }
    }
};

/**
 * Base of TumblingWindow event handlers used for testing.
 * Supports two partitions.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class TumblingHandlerBase : public WindowEvent<TType>
#else
class TumblingHandlerBase : public WindowEvent<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
#endif
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > WindowType;
#endif

    TumblingHandlerBase(unsigned size) : windowSize_(size)
    {  init(); }

    void beforeTupleInsertionEvent(WindowType & window,
                                   TupleType const & tuple,
                                   PartitionType const & partition)
    {   numBeforeInsertion_[partition]++; }

    void afterTupleInsertionEvent(WindowType & window,
                                  TupleType& tuple,
                                  PartitionType const & partition)
    {
        numAfterInsertion_[partition]++;
        ASSERT_TRUE(numBeforeInsertion_[partition] == numAfterInsertion_[partition]);
    }

    void beforeWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
        numBeforeFlush_[partition]++;
        int offset=(numBeforeFlush_[partition]-1)*windowSize_;
        verifyDataElements(window.getWindowData(partition), offset);
    }

    void afterWindowFlushEvent(WindowType & window,
                               PartitionType const & partition)
    {
        numAfterFlush_[partition]++;
        ASSERT_TRUE(numBeforeFlush_[partition] == numAfterFlush_[partition]);
        DataType & data = window.getWindowData(partition);
        ASSERT_TRUE(data.empty());
    }

    void onCheckpointEvent(Checkpoint& ckpt) const
    {
        SPCDBG_ENTER(WINLIB_TEST);
        WRITE_MARKER(ckpt, "TumblingHandlerBase");
        ckpt << numBeforeInsertion_[0] << numBeforeInsertion_[1] <<
                numAfterInsertion_[0] << numAfterInsertion_[1] <<
                numBeforeFlush_[0] << numBeforeFlush_[1] <<
                numAfterFlush_[0] << numAfterFlush_[1];
    }

    void onResetEvent(Checkpoint& ckpt)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        VERIFY_MARKER(ckpt, "TumblingHandlerBase");
        ckpt >> numBeforeInsertion_[0] >> numBeforeInsertion_[1] >>
                numAfterInsertion_[0] >> numAfterInsertion_[1] >>
                numBeforeFlush_[0] >> numBeforeFlush_[1] >>
                numAfterFlush_[0] >> numAfterFlush_[1];
    }

    void onResetToInitialStateEvent()
    {
        SPCDBG_ENTER(WINLIB_TEST);
        init();
    }

protected:
    void verifyDataElements(DataType const & data, int offset)
    {
        int z=0;
        for(DataType::const_iterator it = data.begin(); it != data.end(); ++it) {
            SPCDBG(L_DEBUG, "z=" << z << " tuple.age = " << it->get_age(), WINLIB_TEST);
            ASSERT_EQUALS((z++)+offset, it->get_age());
        }
    }

    void init() {
        numBeforeInsertion_[0] = 0; numBeforeInsertion_[1] = 0;
        numAfterInsertion_[0] = 0; numAfterInsertion_[1] = 0;
        numBeforeFlush_[0] = 0; numBeforeFlush_[1] = 0;
        numAfterFlush_[0] = 0; numAfterFlush_[1] = 0;
    }

public:
    unsigned const windowSize_;
    unsigned numBeforeInsertion_[2];
    unsigned numAfterInsertion_[2];
    unsigned numBeforeFlush_[2];
    unsigned numAfterFlush_[2];
};

class TumblingCountPartitionHandlerBase : public TumblingHandlerBase {
public:
    TumblingCountPartitionHandlerBase(unsigned size) : TumblingHandlerBase(size), expectingFlush(), expectedFlush(), expectingEviction(), expectedEviction()
    {}

    void beforeWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
        numBeforeFlush_[partition]++;

        ASSERT_TRUE_MSG("An unexpected flush happened", expectingFlush);
        ASSERT_EQUALS(expectedFlush, partition);
        expectingFlush = false;
    }

    void afterWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
        numAfterFlush_[partition]++;

        ASSERT_EQUALS(0, window.getTupleCount(partition));
    }


    void onWindowPartitionEviction(WindowType & window,
                                   PartitionIterator const & begin,
                                   PartitionIterator const & end) {
        for (PartitionIterator it = begin; it != end; ++it) {
            SPCDBG(L_DEBUG, "Partition " << it.partition() << " evicted", WINLIB_TEST);
            ASSERT_TRUE_MSG("An unexpected eviction happened", expectingEviction);
            ASSERT_EQUALS(expectedEviction, it.partition());
            expectingEviction = false;
        }
    }

    void expectFlush (PartitionType const & partition) {
      expectingFlush = true;
      expectedFlush = partition;
    }

    void verifyFlush () {
      ASSERT_TRUE_MSG("An expected flush did not happen", !expectingFlush);
    }

    void expectEviction (PartitionType const & partition) {
      expectingEviction = true;
      expectedEviction = partition;
    }

    void verifyEviction () {
      ASSERT_TRUE_MSG("An expected eviction did not happen", !expectingEviction);
    }

    bool expectingFlush;
    PartitionType expectedFlush;
    bool expectingEviction;
    PartitionType expectedEviction;

};

/**
 * Event handler for TumblingWindow with delta-based policy used for testing.
 */
class TumblingDeltaHandler : public TumblingHandlerBase
{
public:
    TumblingDeltaHandler(unsigned size) : TumblingHandlerBase(size)
    {}

    void beforeWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
        ++numBeforeFlush_[partition];
        SPCDBG(L_DEBUG, "TumblingDeltaHandler::numBeforeFlush_[" << partition << "]=" << numBeforeFlush_[partition], WINLIB_TEST);

        int offset = (numBeforeFlush_[partition] - 1) * (windowSize_ + 1);
        verifyDataElements(window.getWindowData(partition), offset);
    }
};

class TumblingDeltaWindowCountPartitionHandler : public TumblingCountPartitionHandlerBase
{
public:
    TumblingDeltaWindowCountPartitionHandler(unsigned size) : TumblingCountPartitionHandlerBase(size)
    {}
};


/**
 * Event handler for TumblingWindow with count-based policy used for testing.
 */
class TumblingCountHandler : public TumblingHandlerBase
{
public:
    TumblingCountHandler(unsigned size) : TumblingHandlerBase(size)
    {}

    void beforeWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> & win = dynamic_cast<TumblingWindow<TType> &>(window);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > & win = dynamic_cast<TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > &>(window);
#endif
        CountWindowPolicy const & evict = dynamic_cast<CountWindowPolicy const &>(win.getEvictionPolicy());
        ASSERT_EQUALS(evict.getSize(), windowSize_);

        TumblingHandlerBase::beforeWindowFlushEvent(window, partition);
    }
};

class TumblingCountWindowCountPartitionHandler : public TumblingCountPartitionHandlerBase
{
public:
    TumblingCountWindowCountPartitionHandler(unsigned size) : TumblingCountPartitionHandlerBase(size)
    {}

    void beforeWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TType> & win = dynamic_cast<TumblingWindow<TType> &>(window);
#else
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > & win = dynamic_cast<TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > &>(window);
#endif
        CountWindowPolicy const & evict = dynamic_cast<CountWindowPolicy const &>(win.getEvictionPolicy());
        ASSERT_EQUALS(evict.getSize(), windowSize_);

        SPCDBG(L_DEBUG, "TumblingCountWindowCountPartitionHandler::beforeWindowFlush for partition " << partition << " at tuple " << numBeforeFlush_[partition], WINLIB_TEST);

        TumblingCountPartitionHandlerBase::beforeWindowFlushEvent(window, partition);
    }
};

/**
 * Event handler for TumblingWindow with punct-based policy used for testing.
 */
class TumblingPunctHandler : public TumblingHandlerBase
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > WindowType;
#endif
    TumblingPunctHandler(unsigned size) : TumblingHandlerBase(size), numEmptyPunct_(0)
    {}

    void onEmptyWindowPunctEvent(WindowType & window)
    {   numEmptyPunct_++; }

public:
    unsigned numEmptyPunct_;
};

class TumblingPunctWindowCountPartitionHandler : public TumblingCountPartitionHandlerBase
{
public:
    TumblingPunctWindowCountPartitionHandler(unsigned size) : TumblingCountPartitionHandlerBase(size)
    {}
};

/**
 * Tumbling summarizer used for testing.
 */
class TumblingSummarizer : public TumblingWindowSummarizer<TType>
{
public:
    TumblingSummarizer (const Operator&) : currentSum_(0), sum_(-1) {}

    void onOpenWindowEvent(PartitionType const & partition)
    {   sum_ = -1; currentSum_ = 0; }

    void onTupleInsertionEvent(TupleType const & tuple)
    {   currentSum_ += tuple.get_age(); }

    void onCloseWindowEvent()
    {   sum_ = currentSum_; }

    int getSum() const {  return sum_; }

    void onCheckpointEvent(Checkpoint & ckpt) const
    {
        SPCDBG_ENTER(WINLIB_TEST);
        WRITE_MARKER(ckpt, "TumblingSummarizer");
        ckpt << currentSum_ << sum_;
    }

    void onResetEvent(Checkpoint & ckpt)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        VERIFY_MARKER(ckpt, "TumblingSummarizer");
        ckpt >> currentSum_ >> sum_;
    }

private:
    int currentSum_;
    int sum_;
};


/**
 * Base of event handlers for TumblingWindow with summarizer.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class TumblingSumHandlerBase : public WindowEvent<TType>
#else
class TumblingSumHandlerBase : public WindowEvent<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
#endif
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > WindowType;
#endif
    TumblingSumHandlerBase(unsigned size) : windowSize_(size), flushCount_(0) {}

    void beforeWindowFlushEvent(WindowType & window, PartitionType const & partition)
    {
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        TumblingWindow<TupleType> & twindow = dynamic_cast<TumblingWindow<TupleType> &>(window);
#else
        TumblingWindow<TupleType, int32_t, SPL::IncrDeque<TupleType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TupleType> > > & twindow = dynamic_cast<TumblingWindow<TupleType, int32_t, SPL::IncrDeque<TupleType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TupleType> > > &>(window);
#endif
        TumblingWindowSummarizer<TupleType> & summarizer = *twindow.getSummarizer(partition);
        TumblingSummarizer & mySummarizer = dynamic_cast<TumblingSummarizer &>(summarizer);
        verifySummarizerBeforeWindowFlush(mySummarizer);
        flushCount_++;
    }

    void onCheckpointEvent(Checkpoint & ckpt) const
    {
        SPCDBG_ENTER(WINLIB_TEST);
        WRITE_MARKER(ckpt, "TumblingSumHandlerBase");
        ckpt << flushCount_;
    }

    void onResetEvent(Checkpoint & ckpt)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        VERIFY_MARKER(ckpt, "TumblingSumHandlerBase");
        ckpt >> flushCount_;
    }

    void onResetToInitialStateEvent()
    {
        SPCDBG_ENTER(WINLIB_TEST);
        flushCount_ = 0;
    }

public:
    unsigned const windowSize_;
    unsigned flushCount_;

protected:
    static unsigned sumZeroToNMinusOne(unsigned n)
    {   return n*(n-1)/2; }

    virtual void verifySummarizerBeforeWindowFlush(TumblingSummarizer & mySummarizer) {
        ASSERT_EQUALS(windowSize_*windowSize_*flushCount_+sumZeroToNMinusOne(windowSize_),
                mySummarizer.getSum());
    }
};


/**
 * Event handler for TumblingWindow with summarizer used for testing.
 */
class TumblingSumHandler : public TumblingSumHandlerBase
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > WindowType;
#endif
    TumblingSumHandler(unsigned size) : TumblingSumHandlerBase(size) {}
};


/**
 * Event handler for TumblingWindow with summarizer and punct-based policy
 * used for testing.
 */
class TumblingPunctSumHandler : public TumblingSumHandlerBase
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > WindowType;
#endif
    /** @param size indicates after how many tuples a WindowMarker is inserted */
    TumblingPunctSumHandler(unsigned size) : TumblingSumHandlerBase(size), numEmptyPunct_(0) {}

    void onCheckpointEvent(Checkpoint & ckpt) const
    {
        SPCDBG_ENTER(WINLIB_TEST);
        TumblingSumHandlerBase::onCheckpointEvent(ckpt);
        WRITE_MARKER(ckpt, "TumblingPunctSumHandler");
        ckpt << numEmptyPunct_;
    }

    void onResetEvent(Checkpoint & ckpt)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        TumblingSumHandlerBase::onResetEvent(ckpt);
        VERIFY_MARKER(ckpt, "TumblingPunctSumHandler");
        ckpt >> numEmptyPunct_;
    }

    void onResetToInitialStateEvent()
    {
        SPCDBG_ENTER(WINLIB_TEST);
        numEmptyPunct_ = 0;
    }

    void onEmptyWindowPunctEvent(WindowType & window)
    {   numEmptyPunct_++; }

public:
    unsigned numEmptyPunct_;
};


/**
 * Event handler for TumblingWindow with summarizer and delta-based policy
 * used for testing.
 */
class TumblingDeltaSumHandler : public TumblingSumHandlerBase
{
public:
    TumblingDeltaSumHandler(unsigned size) : TumblingSumHandlerBase(size)
    {}

protected:
    void verifySummarizerBeforeWindowFlush(TumblingSummarizer & mySummarizer) {
        if (flushCount_==0) {
            ASSERT_EQUALS((0+1+2), mySummarizer.getSum());
        } else {
            ASSERT_EQUALS((3+4+5), mySummarizer.getSum());
        }
    }
};

class TumblingSumCountWindowCountPartitionHandler : public TumblingSumHandlerBase
{
public:
    TumblingSumCountWindowCountPartitionHandler(unsigned size) : TumblingSumHandlerBase(size), expectingFlush(), expectedFlush(), expectingEviction(), expectedEviction()
    {}

    void beforeWindowFlushEvent(WindowType & window,
                                PartitionType const & partition)
    {
        TumblingSumHandlerBase::beforeWindowFlushEvent(window, partition);

        ASSERT_TRUE(expectingFlush);
        ASSERT_EQUALS(expectedFlush, partition);
        expectingFlush = false;
    }
    void onWindowPartitionEviction(WindowType & window,
                                   PartitionIterator const & begin,
                                   PartitionIterator const & end) {
        for (PartitionIterator it = begin; it != end; ++it) {
            SPCDBG(L_DEBUG, "Partition " << it.partition() << " evicted", WINLIB_TEST);
            ASSERT_TRUE(expectingEviction);
            ASSERT_EQUALS(expectedEviction, it.partition());
            expectingEviction = false;
        }
    }

    void expectFlush (PartitionType const & partition) {
      expectingFlush = true;
      expectedFlush = partition;
    }

    void verifyFlush () {
      ASSERT_TRUE(!expectingFlush);
    }

    void expectEviction (PartitionType const & partition) {
      expectingEviction = true;
      expectedEviction = partition;
    }

    void verifyEviction () {
      ASSERT_TRUE(!expectingEviction);
    }

  bool expectingFlush;
  PartitionType expectedFlush;
  bool expectingEviction;
  PartitionType expectedEviction;
};

/**
 * Base of event handlers for SlidingWindow. Supports two partitions.
 * @see SlidingHandlerBase2 for template instantiation with
 * tuple pointers.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class SlidingHandlerBase : public WindowEvent<TType>
#else
class SlidingHandlerBase : public WindowEvent<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >
#endif
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType> WindowType;
#else
    typedef Window<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > WindowType;
#endif

    /**
     * @param wsize window size
     * @param tsize trigger size
     * @param hasPartitionEviction indicates whether the associated window has partition eviction
     */
    SlidingHandlerBase(unsigned wsize, unsigned tsize, bool hasPartitionEviction) :
            windowSize_(wsize), triggerSize_(tsize), windowHasPartitionEviction_(hasPartitionEviction)
    {   init(); }

    void beforeTupleInsertionEvent(WindowType & window,
                                   TupleType const & tuple,
                                   PartitionType const & partition)
    {   numBeforeInsertion_[partition]++; }

    void afterTupleInsertionEvent(WindowType & window,
                                  TupleType& tuple,
                                  PartitionType const & partition)
    {
        numAfterInsertion_[partition]++;
        SPCDBG(L_TRACE, "partition=" << partition << " numAfterInsertion_=" << numAfterInsertion_[partition] <<
                " tuple: " << tuple << " window data: " << window.getWindowData(partition), WINLIB_TEST);
        ASSERT_EQUALS(numBeforeInsertion_[partition], numAfterInsertion_[partition]);
    }

    void beforeTupleEvictionEvent(WindowType & window,
                                 TupleType& tuple,
                                 PartitionType const & partition)
    {   ++numBeforeEviction_[partition]; }

    void afterTupleEvictionEvent(WindowType & window,
                                 TupleType& tuple,
                                 PartitionType const & partition)
    {
        ++numAfterEviction_[partition];
        SPCDBG(L_TRACE, "partition=" << partition << " numAfterEviction_=" << numAfterEviction_[partition] <<
                " tuple: " << tuple << " window data: " << window.getWindowData(partition), WINLIB_TEST);
        ASSERT_EQUALS(numBeforeEviction_[partition], numAfterEviction_[partition]);

    }

    void onWindowTriggerEvent(WindowType & window,
                              PartitionType const & partition)
    {
        ++numOnTrigger_[partition];
        SPCDBG(L_TRACE, "partition=" << partition << " numOnTrigger_=" << numOnTrigger_[partition] <<
                " numAfterEviction_=" << numAfterEviction_[partition] <<
                " window data: " << window.getWindowData(partition), WINLIB_TEST);
        DataType const & data = window.getWindowData(partition);
        int z=0;
        int offset=numAfterEviction_[partition];
        DataType::const_iterator bit = data.begin();
        DataType::const_iterator eit = data.end();
        if (!windowHasPartitionEviction_) {
            // Sliding windows without partition eviction
            for (DataType::const_iterator it=bit; it!=eit; ++it) {
                ASSERT_TRUE(it->get_age()==(z++)+offset);
            }
        }

        verifyAfterWindowTriggerEvent(data, partition);
    }

    void onWindowInitialFullEvent(WindowType & window,
                                  PartitionType const & partition)
    {
        numOnInitialFull_[partition]++;
        SPCDBG(L_TRACE, "partition=" << partition << " window data: " << window.getWindowData(partition), WINLIB_TEST);
        if (!windowHasPartitionEviction_) {
            ASSERT_EQUALS(windowSize_, numAfterInsertion_[partition]);
            ASSERT_EQUALS((windowSize_-1)/triggerSize_, numOnTrigger_[partition]);
        }
        else {
            // A weaker assertion for windows with partition eviction:
            // the number for afterInsertion events is a multiple of the windowSize.
            ASSERT_EQUALS(0, numAfterInsertion_[partition] % windowSize_);
        }
    }

    void onCheckpointEvent(Checkpoint& ckpt) const
    {
        SPCDBG_ENTER(WINLIB_TEST);
        WRITE_MARKER(ckpt, "SlidingHandlerBase");
        ckpt << numBeforeInsertion_[0] << numBeforeInsertion_[1] << numAfterInsertion_[0] << numAfterInsertion_[1] <<
                numBeforeEviction_[0] << numBeforeEviction_[1] << numAfterEviction_[0] << numAfterEviction_[1] <<
                numOnTrigger_[0] << numOnTrigger_[1] << numOnInitialFull_[0] << numOnInitialFull_[1];
    }

    void onResetEvent(Checkpoint& ckpt)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        VERIFY_MARKER(ckpt, "SlidingHandlerBase");
        ckpt >> numBeforeInsertion_[0] >> numBeforeInsertion_[1] >> numAfterInsertion_[0] >> numAfterInsertion_[1] >>
                numBeforeEviction_[0] >> numBeforeEviction_[1] >> numAfterEviction_[0] >> numAfterEviction_[1] >>
                numOnTrigger_[0] >> numOnTrigger_[1] >> numOnInitialFull_[0] >> numOnInitialFull_[1];
    }

    void onResetToInitialStateEvent()
    {
        SPCDBG_ENTER(WINLIB_TEST);
        init();
    }

protected:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    virtual void verifyAfterWindowTriggerEvent(deque<TType> const & data, PartitionType const & partition)
#else
    virtual void verifyAfterWindowTriggerEvent(DataType const & data, PartitionType const & partition)
#endif
    {
        size_t dataSize = data.size();
        if (dataSize >= windowSize_) {
            SPCDBG(L_DEBUG, "partition=" << partition <<
                    ": ASSERT_EQUALS((numBeforeEviction_+1)/triggerSize_, numOnTrigger_-2): numBeforeEviction_=" <<
                    numBeforeEviction_[partition] << " triggerSize_=" << triggerSize_ <<
                    " numOnTrigger_=" << numOnTrigger_[partition], WINLIB_TEST);
            ASSERT_EQUALS((numBeforeEviction_[partition]+1)/triggerSize_, numOnTrigger_[partition]-2);
        }
        if (!windowHasPartitionEviction_ && numOnTrigger_[partition] > 2) {
            ASSERT_EQUALS(windowSize_, dataSize);
        }
    }

private:
    void init() {
        numBeforeInsertion_[0] = 0; numBeforeInsertion_[1] = 0;
        numAfterInsertion_[0] = 0; numAfterInsertion_[1] = 0;
        numBeforeEviction_[0] = 0; numBeforeEviction_[1] = 0;
        numAfterEviction_[0] = 0; numAfterEviction_[1] = 0;
        numOnTrigger_[0] = 0; numOnTrigger_[1] = 0;
        numOnInitialFull_[0] = 0; numOnInitialFull_[1] = 0;
    }

public:
    const unsigned windowSize_;
    const unsigned triggerSize_;
    const bool windowHasPartitionEviction_;
    unsigned numBeforeInsertion_[2];
    unsigned numAfterInsertion_[2];
    unsigned numBeforeEviction_[2];
    unsigned numAfterEviction_[2];
    unsigned numOnTrigger_[2];
    unsigned numOnInitialFull_[2];
};


/**
 * Event handler for SlidingWindow with count-based eviction and trigger
 * policies, used for testing.
 */
class SlidingCountCountHandler : public SlidingHandlerBase
{
public:
    SlidingCountCountHandler(unsigned wsize, unsigned tsize, bool hasPartitionEviction=false) :
        SlidingHandlerBase(wsize, tsize, hasPartitionEviction) {}
};


/**
 * Event handler for SlidingWindow with count-based eviction and delta-based
 * trigger policies, used for testing.
 */
class SlidingCountDeltaHandler : public SlidingHandlerBase
{
public:
    SlidingCountDeltaHandler(unsigned wsize, unsigned tsize, bool hasPartitionEviction=false) :
        SlidingHandlerBase(wsize, tsize, hasPartitionEviction) {}

    void onWindowInitialFullEvent(WindowType & window,
                                  PartitionType const & partition) {
        numOnInitialFull_[partition]++;
        SPCDBG(L_TRACE, "partition=" << partition << " window data: " << window.getWindowData(partition), WINLIB_TEST);
        ASSERT_EQUALS(windowSize_, numAfterInsertion_[partition]);
        ASSERT_EQUALS(windowSize_/(triggerSize_+1), numOnTrigger_[partition]);
    }

protected:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    void verifyAfterWindowTriggerEvent(deque<TType> const & data, PartitionType const & partition)
#else
    void verifyAfterWindowTriggerEvent(DataType const & data, PartitionType const & partition)
#endif
    {
        // make sure trigger happens before eviction
        size_t dataSize = data.size();
        if (dataSize >= windowSize_) {
            SPCDBG(L_DEBUG, "partition=" << partition <<
                    " ASSERT_EQUALS((numBeforeEviction_+2)/(triggerSize_+1), numOnTrigger_-1): numBeforeEviction_=" <<
                    numBeforeEviction_[partition] << " triggerSize_=" << triggerSize_ << " numOnTrigger_=" << numOnTrigger_[partition], WINLIB_TEST);
            ASSERT_EQUALS((numBeforeEviction_[partition]+2)/(triggerSize_+1), numOnTrigger_[partition]-1);
        }
        if (numOnTrigger_[partition] > 1) {
            ASSERT_EQUALS(windowSize_, dataSize);
        }
    }
};


/**
 * Event handler for SlidingWindow with delta-based eviction and count-based
 * trigger policies, used for testing.
 */
class SlidingDeltaCountHandler : public SlidingHandlerBase
{
public:
    SlidingDeltaCountHandler(unsigned wsize, unsigned tsize, bool hasPartitionEviction=false) :
        SlidingHandlerBase(wsize, tsize, hasPartitionEviction) {}

    void onWindowInitialFullEvent(WindowType & window,
                                  PartitionType const & partition) {
        numOnInitialFull_[partition]++;
        SPCDBG(L_TRACE, "partition=" << partition << " window data: " << window.getWindowData(partition), WINLIB_TEST);
        ASSERT_EQUALS(windowSize_+1, numAfterInsertion_[partition]);
        ASSERT_EQUALS((windowSize_+1)/(triggerSize_+1), numOnTrigger_[partition]);
    }

protected:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    void verifyAfterWindowTriggerEvent(deque<TType> const & data, PartitionType const & partition)
#else
    void verifyAfterWindowTriggerEvent(DataType const & data, PartitionType const & partition)
#endif
    {
        ASSERT_EQUALS(numBeforeInsertion_[partition]/triggerSize_, numOnTrigger_[partition]);
        ASSERT_EQUALS(windowSize_, data.rbegin()->get_age()-data.begin()->get_age());
    }
};


/**
 * Event handler for SlidingWindow with delta-based eviction and trigger
 * policies, used for testing.
 */
class SlidingDeltaDeltaHandler : public SlidingHandlerBase
{
public:
    SlidingDeltaDeltaHandler(unsigned wsize, unsigned tsize, bool hasPartitionEviction=false) :
        SlidingHandlerBase(wsize, tsize, hasPartitionEviction) {}

    void onWindowInitialFullEvent(WindowType & window,
                                  PartitionType const & partition) {
        numOnInitialFull_[partition]++;
        SPCDBG(L_TRACE, "partition=" << partition << " window data: " << window.getWindowData(partition), WINLIB_TEST);
        ASSERT_EQUALS(windowSize_+1, numAfterInsertion_[partition]);
        ASSERT_EQUALS((windowSize_+1)/(triggerSize_+1), numOnTrigger_[partition]);
    }

protected:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    void verifyAfterWindowTriggerEvent(deque<TType> const & data, PartitionType const & partition)
#else
    void verifyAfterWindowTriggerEvent(DataType const & data, PartitionType const & partition)
#endif
    {
        // make sure trigger happens before eviction
        if (data.size() >= windowSize_ + 1) {
            ASSERT_EQUALS((windowSize_ + numBeforeEviction_[partition] + 1),
                          numOnTrigger_[partition]);
        }
        if (numOnTrigger_[partition] > windowSize_) {
            ASSERT_EQUALS(windowSize_, data.rbegin()->get_age() - data.begin()->get_age());
        }
    }
};


/**
 * Base of event handlers for SlidingWindow with tuple pointers.
 * @see SlidingHandlerBase for template instantiation with tuples.
 */
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
class SlidingHandlerBase2 : public WindowEvent<TType*>
#else
class SlidingHandlerBase2 : public WindowEvent<TType*, int32_t, SPL::IncrDeque<TType*>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType*> > >
#endif
{
public:
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    typedef Window<TType*> WindowType;
#else
    typedef Window<TType*, int32_t, SPL::IncrDeque<TType*>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType*> > > WindowType;
#endif
    SlidingHandlerBase2(unsigned wsize, unsigned tsize)
        : windowSize_(wsize),
          triggerSize_(tsize),
          numBeforeInsertion_(0),
          numAfterInsertion_(0),
          numBeforeEviction_(0),
          numAfterEviction_(0),
          numOnTrigger_(0),
          numOnInitialFull_(0)
    {}

    void beforeTupleInsertionEvent(WindowType & window,
                                   TupleType const & tuple,
                                   PartitionType const & partition)
    {   numBeforeInsertion_++; }

    void afterTupleInsertionEvent(WindowType & window,
                                  TupleType& tuple,
                                  PartitionType const & partition)
    {
        numAfterInsertion_++;
        ASSERT_EQUALS(numBeforeInsertion_, numAfterInsertion_);
    }

    void beforeTupleEvictionEvent(WindowType & window,
                                 TupleType& tuple,
                                 PartitionType const & partition)
    {
        ++numBeforeEviction_;
        delete tuple;
    }

    void afterTupleEvictionEvent(WindowType & window,
                                 TupleType& tuple,
                                 PartitionType const & partition)
    {
        ++numAfterEviction_;
        ASSERT_EQUALS(numBeforeEviction_, numAfterEviction_);

    }

    void onWindowTriggerEvent(WindowType & window,
                              PartitionType const & partition)
    {
        ++numOnTrigger_;
        SPCDBG(L_DEBUG, "onWindowTriggerEvent(): numOnTrigger_=" << numOnTrigger_, WINLIB_TEST);
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
        deque<TType*> const & data = window.getWindowData(partition);
#else
        WindowType::DataType const & data = window.getWindowData(partition);
#endif
        int z=0, offset=numAfterEviction_;
        deque<TType*>::const_iterator bit = data.begin();
        deque<TType*>::const_iterator eit = data.end();
        for (deque<TType*>::const_iterator it=bit; it!=eit; ++it) {
            ASSERT_TRUE((*it)->get_age()==(z++)+offset);
        }
        verifyAfterWindowTriggerEvent(data.size());
    }

    void onWindowInitialFullEvent(WindowType & window,
                                  PartitionType const & partition) {
        numOnInitialFull_++;
        ASSERT_EQUALS(windowSize_, numAfterInsertion_);
        ASSERT_EQUALS((windowSize_-1)/triggerSize_, numOnTrigger_);
    }

    void onCheckpointEvent(Checkpoint& ckpt) const
    {
        SPCDBG_ENTER(WINLIB_TEST);
        WRITE_MARKER(ckpt, "SlidingHandlerBase");
        ckpt << numBeforeInsertion_ << numAfterInsertion_ << numBeforeEviction_ <<
                numAfterEviction_ << numOnTrigger_ << numOnInitialFull_;
    }

    void onResetEvent(Checkpoint& ckpt)
    {
        SPCDBG_ENTER(WINLIB_TEST);
        VERIFY_MARKER(ckpt, "SlidingHandlerBase");
        ckpt >> numBeforeInsertion_ >> numAfterInsertion_ >> numBeforeEviction_ >>
                numAfterEviction_ >> numOnTrigger_ >> numOnInitialFull_;
    }

    void onResetToInitialStateEvent()
    {
        SPCDBG_ENTER(WINLIB_TEST);
        numBeforeInsertion_ = 0;
        numAfterInsertion_ = 0;
        numBeforeEviction_ = 0;
        numAfterEviction_  = 0;
        numOnTrigger_ = 0;
        numOnInitialFull_ = 0;
    }

protected:
    virtual void verifyAfterWindowTriggerEvent(size_t dataSize)
    {
        if (dataSize >= windowSize_) {
            SPCDBG(L_DEBUG, "ASSERT_EQUALS((numBeforeEviction_+1)/triggerSize_, numOnTrigger_-2): numBeforeEviction_=" <<
                    numBeforeEviction_ << " triggerSize_=" << triggerSize_ <<
                    " numOnTrigger_=" << numOnTrigger_, WINLIB_TEST);
            ASSERT_EQUALS((numBeforeEviction_+1)/triggerSize_, numOnTrigger_-2);
        }
        if (numOnTrigger_ > 2) {
            ASSERT_EQUALS(windowSize_, dataSize);
        }
    }

public:
    const unsigned windowSize_;
    const unsigned triggerSize_;
    unsigned numBeforeInsertion_;
    unsigned numAfterInsertion_;
    unsigned numBeforeEviction_;
    unsigned numAfterEviction_;
    unsigned numOnTrigger_;
    unsigned numOnInitialFull_;
};


/**
 * Event handler for SlidingWindow of tuple pointers with count-based
 * eviction and trigger policies, used for testing.
 */
class SlidingCountCountHandler2 : public SlidingHandlerBase2
{
public:
    SlidingCountCountHandler2(unsigned wsize, unsigned tsize) : SlidingHandlerBase2(wsize, tsize) {}
};

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
void logPartitionSizes(TumblingWindow<TType> const& win)
#else
void logPartitionSizes(TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > const& win)
#endif
{
    std::ostringstream message;
#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    AutoWindowDataAcquirer<TType> autoData(win);
    TumblingWindow<TType>::StorageType const & storage = autoData.getWindowStorage();
#else
    AutoWindowDataAcquirer<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > > autoData(win);
    TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >::StorageType const & storage = autoData.getWindowStorage();
#endif

#ifndef TEST_WINDOW_USE_INCREMENTAL_CHECKPOINT
    for (TumblingWindow<TType>::StorageType::const_iterator dataIt = storage.begin(); dataIt != storage.end(); ++dataIt)
    {
        TumblingWindow<TType>::PartitionType const & partitionIndex = dataIt->first;
        TumblingWindow<TType>::DataType::size_type size = 0;
#else
    for (TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >::StorageType::const_iterator dataIt = storage.begin(); dataIt != storage.end(); ++dataIt)
    {
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >::PartitionType const & partitionIndex = dataIt->first;
        TumblingWindow<TType, int32_t, SPL::IncrDeque<TType>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<TType> > >::DataType::size_type size = 0;
#endif
        if (dataIt != storage.end()) {
            size = win.getTupleCount(partitionIndex);
        }

        if (message.str().empty()) {
            message << "Partitions: " ;
        } else {
            message << ", ";
        }
        message << "P" << partitionIndex << "(" << size << ")";
     }
    SPCDBG(L_INFO, message.str(), WINLIB_TEST);
}

} // end namespace SPL

#endif // TEST_RUNTIME_WINDOWTESTCOMMON_H
