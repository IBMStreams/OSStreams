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

#include "TestUtils.h"
#include <UTILS/ConcurrentHashMap.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/Mutex.h>
#include <UTILS/Thread.h>
#include <UTILS/UTILSTypes.h>

// XXX INT64_C and UINT64_C are defined in unicode/utypes.h, included by
// DistilleryDebug.h and redefined by Boost.
#undef UINT64_C
#undef INT64_C
#include <boost/thread/barrier.hpp>

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <string.h>
#include <time.h>

#define DBG_ASP "ConcurrentHashMapTest"

/**
 * \file ConcurrentHashMapTest.cpp
 * Multi-threaded test for ConcurrentHashMap.
 *
 * Building the test involves the following steps:
 * 1. Implement the map key type TestKey
 *    - define hash function and equal predicate
 * 2. Implement hash functor specialization for TestKey
 *    (see struct hash<Streams::TestKey>).
 * 3. Implement the map value type TestValue.
 * 4. Define wrapper interface TestBase and wrappers TestObject_m (map
 *    synchronized by Mutex) and TestObject_s (map synchronized by SpinLock).
 * 5. Define Distillery application ConcurrentHashMapTest.
 * 6. Define the tester thread TestWorker.
 */

///////////////////////////////////////////////////////////////////////////
// 1. The map key type TestKey
namespace Distillery {
class TestKey
{
  public:
    TestKey()
      : key1_(std::numeric_limits<uint64_t>::max())
      , key2_("")
    {}
    TestKey(int k1, std::string k2)
      : key1_(k1)
      , key2_(k2)
    {}
    TestKey(const TestKey& other)
      : key1_(other.key1_)
      , key2_(other.key2_)
    {}
    ~TestKey() {}
    const TestKey& operator=(const TestKey& rhs)
    {
        if (this != &rhs) {
            key2_ = rhs.key2_;
            key1_ = rhs.key1_;
        }
        return *this;
    }

    size_t hash() const
    {
        std::size_t h = static_cast<std::size_t>(2166136261UL);
        h ^= static_cast<std::size_t>(key1_);
        h *= 16777619UL;
        h ^= std::tr1::hash<std::string>()(key2_);
        h *= 16777619UL;
        return h;
    }

    bool operator==(const TestKey& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        return key1_ == rhs.key1_ && key2_ == rhs.key2_;
    }

    // Equal predicate used by std::tr1::unordered_map
    struct equal_to : public std::equal_to<TestKey>
    {
        bool operator()(const TestKey& k1, const TestKey& k2) const { return k1 == k2; }
    };

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& s, const TestKey& ref);

  private:
    std::ostream& print(std::ostream& s) const
    {
        s << "TestKey(" << key1_ << ", " << key2_ << ")";
        return s;
    }

    /// Data fields
    unsigned int key1_;
    std::string key2_;
};
} // namespace Distillery

std::ostream& Distillery::operator<<(std::ostream& s, const TestKey& ref)
{
    return ref.print(s);
}

///////////////////////////////////////////////////////////////////////////
// 2. Hash functor specialization for TestKey
namespace std {
namespace tr1 {
template<>
struct hash<Distillery::TestKey> : public std::unary_function<Distillery::TestKey, std::size_t>
{
    std::size_t operator()(const Distillery::TestKey& key) const { return key.hash(); }
};
}
} // std::tr1

///////////////////////////////////////////////////////////////////////////
// 3. The map value type
namespace Distillery {
class TestValue
{
  public:
    TestValue()
      : val1_(0)
      , val2_(false)
      , val3_("")
    {}
    TestValue(int v1, bool v2, std::string v3)
      : val1_(v1)
      , val2_(v2)
      , val3_(v3)
    {}
    TestValue(const TestValue& other)
      : val1_(other.val1_)
      , val2_(other.val2_)
      , val3_(other.val3_)
    {}
    const TestValue& operator=(const TestValue& rhs)
    {
        if (this != &rhs) {
            val1_ = rhs.val1_;
            val2_ = rhs.val2_;
            val3_ = rhs.val3_;
        }
        return *this;
    }
    ~TestValue() {}

    bool operator==(const TestValue& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        return val1_ == rhs.val1_ && val2_ == rhs.val2_ && val3_ == rhs.val3_;
    }

    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& s, const TestValue& ref);

    // members made public for convenience
    int val1_;
    bool val2_;
    std::string val3_;

  private:
    std::ostream& print(std::ostream& s) const
    {
        s << "val1=" << val1_ << " val2=" << val2_ << " val3=" << val3_ << std::endl;
        return s;
    }
};

std::ostream& operator<<(std::ostream& s, const TestValue& ref)
{
    return ref.print(s);
}

/////////////////////////////////////////////////////////////////////////////
// 4. Define wrapper interface for the tested class
//
// We wrap the template specializations for Mutex and SpinLock into two
// classes implementing the interface for test: TestObject_m and TestObject_s.
class TestBase
{
  public:
    virtual ~TestBase() {}

    typedef ConcurrentHashMap<TestKey, TestValue, Mutex> ConcurrentHashMap_m;
    typedef ConcurrentHashMap<TestKey, TestValue, SpinLock> ConcurrentHashMap_s;

    virtual bool find(const TestKey& key, TestValue& result) const = 0;
    virtual bool insert(const TestKey& key, const TestValue& value) = 0;
    virtual void put(const TestKey& key, const TestValue& value) = 0;
    virtual bool update(const TestKey& key, const TestValue& value) = 0;
    virtual bool erase(const TestKey& key) = 0;
    virtual void clear() = 0;
    virtual std::size_t size() const = 0;

  protected:
    TestBase() {}
};

class TestObject_m : public TestBase
{
  public:
    TestObject_m(ConcurrentHashMap_m* other)
      : map_(other)
    {}
    virtual ~TestObject_m() { delete map_; }

    virtual bool find(const TestKey& key, TestValue& result) const
    {
        return map_->find(key, result);
    }
    virtual bool insert(const TestKey& key, const TestValue& value)
    {
        return map_->insert(key, value);
    }
    virtual void put(const TestKey& key, const TestValue& value) { map_->put(key, value); }
    virtual bool update(const TestKey& key, const TestValue& value)
    {
        return map_->update(key, value);
    }
    virtual bool erase(const TestKey& key) { return map_->erase(key); }
    virtual void clear() { return map_->clear(); }
    virtual std::size_t size() const { return map_->size(); }

  private:
    TestObject_m() {}
    ConcurrentHashMap_m* map_;
};

class TestObject_s : public TestBase
{
  public:
    TestObject_s(ConcurrentHashMap_s* other)
      : map_(other)
    {}
    virtual ~TestObject_s() { delete map_; }

    virtual bool find(const TestKey& key, TestValue& result) const
    {
        return map_->find(key, result);
    }
    virtual bool insert(const TestKey& key, const TestValue& value)
    {
        return map_->insert(key, value);
    }
    virtual void put(const TestKey& key, const TestValue& value) { map_->put(key, value); }
    virtual bool update(const TestKey& key, const TestValue& value)
    {
        return map_->update(key, value);
    }
    virtual bool erase(const TestKey& key) { return map_->erase(key); }
    virtual void clear() { return map_->clear(); }
    virtual std::size_t size() const { return map_->size(); }

  private:
    TestObject_s() {}
    ConcurrentHashMap_s* map_;
};

/**
 * 5. Distillery application ConcurrentHashMapTest.
 *
 * This class runs 'threadCount' TestWorker threads.  The test timeline is
 * marked by several barriers (test steps).  Test threads wait on barriers
 * before moving on to the next step:
 * <ul>
 *  <li>from start to insertDone: inserters insert new entries, updaters
 *      and erasers wait, readers find some entries and assert their value
 *  <li>from insertDone to updateDone: updaters update entries, erasers
 *      wait, readers find entries (assert success) and assert their value
 *  <li>from updateDone to eraseDone: readers find entries
 *      (assert success) and assert their value
 *  <li>from eraseDone to testDone: erasers delete entries, readers find
 *      some entries and assert their value
 * </ul>
 */
class ConcurrentHashMapTest : public Distillery::DistilleryApplication
{
  public:
    /// Default number of map entries to insert/update.
    static const unsigned int defaultCycleCount = 1000000;
    /// Default ConcurrentHashMap concurrency level.
    static const unsigned int defaultConcurencyLevel = 8;

    /**
     * Number of tester threads that wait on test barriers.  This value must be
     * coordinated with the actual nuimber of test workers used by this test.
     */
    static const unsigned int threadCount = 10;

    ConcurrentHashMapTest()
      : insertDone_(threadCount)
      , updateDone_(threadCount)
      , eraseDone_(threadCount)
      , entryCount_(defaultCycleCount)
      , concurencyLevel_(defaultConcurencyLevel)
      , mutexType_(spinlock)
      , map_(NULL)
      , testDone_(threadCount + 1)
    {}

    virtual ~ConcurrentHashMapTest() { delete map_; }

    void getArguments(option_vector_t& options);
    virtual int run(const std::vector<std::string>& /*remains*/);

    // Option setters
    void setEntryCount(const option_t* option, int value)
    {
        if (value < 0 || value > 10000000) {
            THROW(InvalidOption, "entryCount must be between 0 and 10000000");
        }
        entryCount_ = value;
    }

    void setConcurencyLevel(const option_t* option, int value)
    {
        if (value < 1 || value > 512) {
            THROW(InvalidOption, "concurencyLevel must be between 1 and 512");
        }
        concurencyLevel_ = value;
    }

    void setMutexType(const option_t* option, const char* value)
    {
        if (value == NULL) {
            THROW(InvalidOption, "mutexType must be { spinlock | mutex }");
        }
        if (!::strcmp("spinlock", value)) {
            mutexType_ = spinlock;
        } else if (!::strcmp("mutex", value)) {
            mutexType_ = mutex;
        } else {
            THROW(InvalidOption, "mutexType must be { spinlock | mutex }");
        }
    }

    /// Insert entries within the given range
    void test_insert(uint rangeMin, uint rangeMax);

    /**
     * Concurrently update entries within given range
     *    1. Update entries
     *    2. Wait until insertDone
     *    3. Update entries again
     *    4. Set updateDone
     */
    void test_update(uint rangeMin, uint rangeMax);

    // Concurrently erase entries within given range, assert success.
    // This test runs after all entries have been inserted.
    void test_erase(uint rangeMin, uint rangeMax);

    /// Read entries within given range, assert their value.
    void test_read(uint rangeMin, uint rangeMax);

    /**
     * Called by tester threads to synchronize on a given barrier.
     * @param b barrier
     * @message additional log message
     */
    void waitUntilDone(boost::barrier& b, const char* message)
    {
        SPCDBG(L_DEBUG, "[" << gettid() << "] : Enter " << message, DBG_ASP);
        b.wait();
        SPCDBG(L_DEBUG, "[" << gettid() << "] : Exit " << message, DBG_ASP);
    }

    /// Called by tester threads to synchronize before ending the test.
    void waitUntilTestDone() { waitUntilDone(testDone_, "Test done"); }

    /// Test steps
    boost::barrier insertDone_; ///< marks the end of insert step #1
    boost::barrier updateDone_; ///< marks the end of update step #2
    boost::barrier eraseDone_;  ///< marks the end of erase step #3

  private:
    enum mutexType_t
    {
        spinlock,
        mutex
    };
    static const char* mutexTypeToString(enum mutexType_t& ref);

    // test options
    unsigned int entryCount_;
    unsigned int concurencyLevel_;
    mutexType_t mutexType_;

    // testee
    TestBase* map_;

    // workers wait on this barrier to finish the test
    boost::barrier testDone_;
};

////////////////////////////////////////////////////////////////////////////
// 6. The tester thread
class TestWorker : public Distillery::Thread
{
  public:
    enum kind
    {
        inserter,
        updater,
        eraser,
        reader
    };

    TestWorker(ConcurrentHashMapTest& tester,
               TestWorker::kind threadKind,
               uint rangeMin,
               uint rangeMax)
      : tester_(tester)
      , shutdown_(false)
      , kind_(threadKind)
      , rangeMin_(rangeMin)
      , rangeMax_(rangeMax)
    {}
    virtual ~TestWorker() {}

    void* run(void* /*args*/)
    {
        switch (kind_) {
            case inserter:
                tester_.test_insert(rangeMin_, rangeMax_);
                break;
            case updater:
                tester_.test_update(rangeMin_, rangeMax_);
                break;
            case eraser:
                tester_.test_erase(rangeMin_, rangeMax_);
                break;
            case reader:
                tester_.test_read(rangeMin_, rangeMax_);
                break;
            default:
                SPCDBG(L_ERROR, "Unknown tester thread kind.", DBG_ASP);
        }
        tester_.waitUntilTestDone();
        return 0;
    }

    void shutdown()
    {
        shutdown_ = true;
        SPCDBG(L_DEBUG, "[" << gettid() << "] shutdown", DBG_ASP);
    }

  private:
    ConcurrentHashMapTest& tester_;
    volatile bool shutdown_;
    const kind kind_;
    const uint rangeMin_;
    const uint rangeMax_;
};

} // namespace Distillery

///////////////////////////////////////////////////////////////////////////
using namespace Distillery;

const char* ConcurrentHashMapTest::mutexTypeToString(enum mutexType_t& ref)
{
    switch (ref) {
        case ConcurrentHashMapTest::spinlock:
            return "spinlock";
        case ConcurrentHashMapTest::mutex:
            return "mutex";
        default:
            return "???";
    }
}

void ConcurrentHashMapTest::getArguments(option_vector_t& options)
{
    option_t args[] = {
        { 'n', "entryCount", ARG, "", "Create the specified number of entries (default 1000000)",
          INT_OPT(ConcurrentHashMapTest::setEntryCount) },
        { 'u', "concurencyLevel", ARG, "",
          "Configure the tested map to accept the specified concurrency level (default 8)",
          INT_OPT(ConcurrentHashMapTest::setConcurencyLevel) },
        { 'm', "mutexType", ARG, "",
          "Configure the mutex type used for synchronization: {spinlock | mutex (default)}.",
          STR_OPT(ConcurrentHashMapTest::setMutexType) },
    };
    APPEND_OPTIONS(options, args);
}

int ConcurrentHashMapTest::run(const std::vector<std::string>&)
{
    SPCDBG(L_INFO,
           "[" << gettid() << "]  Testing ConcurrentHashMap with entryCount=" << entryCount_
               << ", concurencyLevel=" << concurencyLevel_
               << ", mutexType=" << ConcurrentHashMapTest::mutexTypeToString(mutexType_),
           DBG_ASP);

    // mark test start
    uint64_t beginTime = TestUtils::getTimeMillis();

    switch (mutexType_) {
        case spinlock:
            map_ = new TestObject_s(new TestBase::ConcurrentHashMap_s(concurencyLevel_));
            break;
        case mutex:
            map_ = new TestObject_m(new TestBase::ConcurrentHashMap_m(concurencyLevel_));
            break;
        default:
            assert(0);
    }

    // must create a number of threads equal to threadCount
    TestWorker* t[ConcurrentHashMapTest::threadCount];
    uint i = 0;

    // 2 readers
    t[i++] = new TestWorker(*this, TestWorker::reader, 0, entryCount_);
    t[i++] = new TestWorker(*this, TestWorker::reader, 0, entryCount_);

    // 3 inserters
    t[i++] = new TestWorker(*this, TestWorker::inserter, 0, entryCount_ / 3);
    t[i++] = new TestWorker(*this, TestWorker::inserter, entryCount_ / 3, entryCount_ * 2 / 3);
    t[i++] = new TestWorker(*this, TestWorker::inserter, entryCount_ * 2 / 3, entryCount_);

    // 3 updaters
    t[i++] = new TestWorker(*this, TestWorker::updater, 0, entryCount_ / 3);
    t[i++] = new TestWorker(*this, TestWorker::updater, entryCount_ / 3, entryCount_ * 2 / 3);
    t[i++] = new TestWorker(*this, TestWorker::updater, entryCount_ * 2 / 3, entryCount_);

    // 2 erasers
    t[i++] = new TestWorker(*this, TestWorker::eraser, 0, entryCount_ / 2);
    t[i++] = new TestWorker(*this, TestWorker::eraser, entryCount_ / 2, entryCount_);

    assert(i == ConcurrentHashMapTest::threadCount);

    for (i = 0; i < ConcurrentHashMapTest::threadCount;) {
        t[i++]->create();
    }

    // the main thread periodically prints the map's size to track progress
    for (int count = 10; count > 0; --count) {
        SPCDBG(L_INFO, "[" << gettid() << "] ConcurrentHashMap::size()=" << map_->size(), DBG_ASP);
        sleep(1);
    }
    waitUntilTestDone();

    // cleanup
    for (i = 0; i < ConcurrentHashMapTest::threadCount;) {
        t[i++]->shutdown();
    }
    for (i = 0; i < ConcurrentHashMapTest::threadCount;) {
        t[i++]->join();
    }
    for (i = 0; i < ConcurrentHashMapTest::threadCount;) {
        delete t[i++];
    }

    SPCDBG(L_INFO,
           "Success! Test ran for " << ((TestUtils::getTimeMillis() - beginTime) / 1000) << " secs",
           DBG_ASP);
    return 0;
}

void ConcurrentHashMapTest::test_insert(uint rangeMin, uint rangeMax)
{
    SPCDBG(L_INFO, "[" << gettid() << "] Inserter started", DBG_ASP);

    for (uint k = rangeMin; k < rangeMax; k++) {
        TestKey key(k, "aaa");
        TestValue value;
        map_->insert(key, value);
    }
    waitUntilDone(insertDone_, "Insert done");
    waitUntilDone(updateDone_, "Update done");
    waitUntilDone(eraseDone_, "Erase done");
    SPCDBG(L_INFO, "[" << gettid() << "] Inserter ended", DBG_ASP);
}

void ConcurrentHashMapTest::test_update(uint rangeMin, uint rangeMax)
{
    SPCDBG(L_INFO, "[" << gettid() << "] Updater started", DBG_ASP);

    // update existing elements while inserters create entries
    for (uint k = rangeMin; k < rangeMax; k++) {
        TestKey key(k, "aaa");
        TestValue value(1, true, "updated");
        map_->update(key, value);
    }
    waitUntilDone(insertDone_, "Insert done");

    // update existing elements after all entries have been created
    for (uint k = rangeMin; k < rangeMax; k++) {
        TestKey key(k, "aaa");
        TestValue value(1, true, "updated");
        map_->update(key, value);
    }
    waitUntilDone(updateDone_, "Update done");
    waitUntilDone(eraseDone_, "Erase done");
    SPCDBG(L_INFO, "[" << gettid() << "] Updater ended", DBG_ASP);
}

void ConcurrentHashMapTest::test_erase(uint rangeMin, uint rangeMax)
{
    SPCDBG(L_INFO, "[" << gettid() << "] Eraser started", DBG_ASP);

    waitUntilDone(insertDone_, "Insert done");
    waitUntilDone(updateDone_, "Update done");

    // erase existing elements, assert success
    for (uint k = rangeMin; k < rangeMax; k++) {
        TestKey key(k, "aaa");
        bool success = map_->erase(key);
        ASSERT_TRUE_MSG("Entry '" << k << "' was not erased", success);
        // subsequent finds must fail
        TestValue result;
        bool found = map_->find(key, result);
        ASSERT_TRUE(!found);
    }
    waitUntilDone(eraseDone_, "Erase done");
    SPCDBG(L_INFO, "[" << gettid() << "] Eraser ended", DBG_ASP);
}

void ConcurrentHashMapTest::test_read(uint rangeMin, uint rangeMax)
{
    SPCDBG(L_INFO, "[" << gettid() << "] Reader started", DBG_ASP);

    // Updaters starts at the same time with inserters, so TestValue instances
    // may be either (0,false,"") or (1,true,"updated").
    for (uint k = rangeMin; k < rangeMax; k++) {
        TestKey key(k, "aaa");
        TestValue result;
        bool found = map_->find(key, result);
        if (found) {
            ASSERT_TRUE_MSG(
              "value for key '" << k << "' must be either (0,false,'') or (1,true,'updated')",
              (result.val1_ == 0 && result.val2_ == false && result.val3_ == "") ||
                (result.val1_ == 1 && result.val2_ == true && result.val3_ == "updated"));
        }
    }
    waitUntilDone(insertDone_, "Insert done");

    // After the insert phase is done, all elements must be found.
    // Elements are either (0,false,"") or (1,true,"updated").
    for (uint k = rangeMin; k < rangeMax; k++) {
        TestKey key(k, "aaa");
        TestValue result;
        bool found = map_->find(key, result);
        ASSERT_TRUE(found);
        ASSERT_TRUE_MSG("value must be either (0,false,'') or (1,true,'updated')",
                        (result.val1_ == 0 && result.val2_ == false && result.val3_ == "") ||
                          (result.val1_ == 1 && result.val2_ == true && result.val3_ == "updated"));
    }
    waitUntilDone(updateDone_, "Update done");

    // After the update phase is done, elements that are found are
    // (1,true,"updated").  Not all elements are found (eraser is running).
    for (uint k = 0; k < entryCount_; k++) {
        TestKey key(k, "aaa");
        TestValue result;
        bool found = map_->find(key, result);
        if (found) {
            ASSERT_TRUE_MSG("value must be (1,true,'updated')",
                            result.val1_ == 1 && result.val2_ == true && result.val3_ == "updated");
        }
    }
    waitUntilDone(eraseDone_, "Erase done");
    SPCDBG(L_INFO, "[" << gettid() << "] Reader ended", DBG_ASP);
}

MAIN_APP(Distillery::ConcurrentHashMapTest)
