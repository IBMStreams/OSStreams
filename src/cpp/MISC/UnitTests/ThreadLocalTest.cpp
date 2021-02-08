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
#include <UTILS/DistilleryApplication.h>
#include <UTILS/Thread.h>
#include <UTILS/ThreadLocal.h>
#include <UTILS/UTILSTypes.h>

// XXX INT64_C and UINT64_C are defined in unicode/utypes.h, included by
// DistilleryDebug.h and redefined by Boost.
#undef UINT64_C
#undef INT64_C
#include <boost/thread/barrier.hpp>

#include <iomanip>
#include <string.h>
#include <time.h>

#define DBG_ASP "ThreadLocalTest"

/**
 * \file ThreadLocalTest.cpp
 * Multi-threaded test for ThreadLocal.
 */

UTILS_NAMESPACE_BEGIN

// we test thread local variables of type string and uint
typedef UTILS_NAMESPACE::ThreadLocal<std::string> ThreadLocalString;
typedef UTILS_NAMESPACE::ThreadLocal<uint> ThreadLocalUInt;

/**
 * Test application.
 *
 * This class runs 'threadCount' TestWorker threads.  Test threads set
 * values on the <i>static</i> ThreadLocalString member, then assert its value
 * is the same as the one previously set.
 */
class ThreadLocalTest : public Distillery::DistilleryApplication
{
  public:
    /**
     * Number of tester threads that wait on test barriers.  This value must be
     * coordinated with the actual nuimber of test workers used by this test,
     * which are initialize in the run() function.
     */
    static const unsigned int threadCount = 5;

    ThreadLocalTest()
      : iterationCount_(1000000)
      , testDone_(threadCount + 1)
    {}

    virtual ~ThreadLocalTest() {}

    void getArguments(option_vector_t& options);
    virtual int run(const std::vector<std::string>& /*remains*/);

    // Option setters
    void setIterationCount(const option_t* option, int value)
    {
        if (value <= 0 || value > 10000000) {
            THROW(InvalidOption, "iterationCount must be between 1 and 10000000");
        }
        iterationCount_ = value;
    }

    unsigned int iterationCount() { return iterationCount_; }

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

  private:
    // test options
    unsigned int iterationCount_;

    // workers wait on this barrier to finish the test
    boost::barrier testDone_;
};

////////////////////////////////////////////////////////////////////////////
// The tester thread.
class TestWorker : public Distillery::Thread
{
  public:
    TestWorker(ThreadLocalTest& tester, uint threadIndex)
      : tester_(tester)
      , threadIndex_(threadIndex)
      , shutdown_(false)
    {}
    virtual ~TestWorker() {}

    void* run(void* /*args*/)
    {
        for (uint i = 0; i < tester_.iterationCount(); i++) {
            std::string s = UTILS_NAMESPACE::toString(threadIndex_);
            tls_.set(&s);
            tli_.set(&threadIndex_);
            SPCDBG(L_INFO, "[" << gettid() << "] tls_ and tli_ set " << threadIndex_, DBG_ASP);
            innerFunction(); // retrieves tls_value and asserts
            tls_.remove();
            tli_.remove();
        }
        tester_.waitUntilTestDone();
        return 0;
    }

    // Access the "static" thread-local variable in the inner function and
    // compare against the expected value
    void innerFunction()
    {
        std::string* const actual = tls_.get();
        SPCDBG(L_INFO,
               "[" << gettid() << "] tls_.get " << threadIndex_ << " returned \"" << (*actual)
                   << "\"",
               DBG_ASP);
        std::string expected = UTILS_NAMESPACE::toString(threadIndex_);
        ASSERT_EQUALS(expected, (*actual));

        uint* const actual_tli = tli_.get();
        SPCDBG(L_INFO,
               "[" << gettid() << "] tli_.get " << threadIndex_ << " returned " << (*actual_tli),
               DBG_ASP);
        ASSERT_EQUALS(threadIndex_, (*actual_tli));
    }

    void shutdown()
    {
        shutdown_ = true;
        SPCDBG(L_DEBUG, "[" << gettid() << "] shutdown", DBG_ASP);
    }

    static ThreadLocalString* tls() { return &tls_; }

  private:
    static ThreadLocalUInt tli_;
    static ThreadLocalString tls_;

    ThreadLocalTest& tester_;
    uint threadIndex_;
    volatile bool shutdown_;
};

UTILS_NAMESPACE_END

///////////////////////////////////////////////////////////////////////////
UTILS_NAMESPACE_USE

// Use lazy initialization function for tls_.
THREAD_LOCAL_INIT(tls_init, TestWorker::tls());
ThreadLocalString TestWorker::tls_(tls_init);

// Use eager initialization when initialization and usage happens in the same process.
ThreadLocalUInt TestWorker::tli_;

void ThreadLocalTest::getArguments(option_vector_t& options)
{
    option_t args[] = {
        { 'n', "iterationCount", ARG, "", "Number of test iterations (default 1000000)",
          INT_OPT(ThreadLocalTest::setIterationCount) },
    };
    APPEND_OPTIONS(options, args);
}

int ThreadLocalTest::run(const std::vector<std::string>&)
{
    std::cout << "[" << gettid() << "] Testing ThreadLocal with " << ThreadLocalTest::threadCount
              << " threads and " << iterationCount_ << " iterations." << std::endl;

    // mark test start
    uint64_t beginTime = TestUtils::getTimeMillis();

    // create a number of threads equal to threadCount
    TestWorker* t[ThreadLocalTest::threadCount];

    uint i = 0;
    for (i = 0; i < ThreadLocalTest::threadCount; i++) {
        t[i] = new TestWorker(*this, i);
    }
    for (i = 0; i < ThreadLocalTest::threadCount; i++) {
        t[i]->create();
    }

    waitUntilTestDone();

    // cleanup
    for (i = 0; i < ThreadLocalTest::threadCount; i++) {
        t[i]->shutdown();
    }
    for (i = 0; i < ThreadLocalTest::threadCount; i++) {
        t[i]->join();
    }
    for (i = 0; i < ThreadLocalTest::threadCount; i++) {
        delete t[i];
    }

    std::cout << "Success! Test ran for " << ((TestUtils::getTimeMillis() - beginTime) / 1000)
              << " secs" << std::endl;
    return 0;
}

MAIN_APP(Distillery::ThreadLocalTest)
