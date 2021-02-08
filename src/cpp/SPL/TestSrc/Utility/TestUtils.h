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

#ifndef TEST_UTILITY_TESTUTILS_H
#define TEST_UTILITY_TESTUTILS_H

#include <TRC/RollingFileTracer.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace SPL {
/**
 * @internal @file TestUtils.h
 * @brief Miscellaneous test utilities.
 */
/// @defgroup OperatorTest SPL operator test classes and utility functions.
/// @{

/**
 * Test utility functions.
 */
class TestUtils
{
  public:
    static inline uint64_t getTimeMillis()
    {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return (ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec) / 1000000;
    }

    template<typename T>
    static inline std::string toString(T const& t)
    {
        std::ostringstream x;
        x << t;
        return x.str();
    }
};

/**
 * Test assertions.
 */
class TestAssert
{
  public:
    /**
     * Assert that the @c expected value is equal to the @c actual value.
     * If assertion fails, this implementation prints a message on std::cout
     * then invokes @c abort().
     * @param expected expected value
     * @param actual actual value
     * @param message test message to output in case of assertion failure in
     *  addition to a standard message indicating the current file and line
     *  number, expected value, and current value.
     * @param fileName file name where the assertion has failed
     * @param lineNo line number where the assertion has failed
     */
    template<typename T>
    static inline void assertEquals(const T& expected,
                                    const T& actual,
                                    const std::string& message,
                                    const char* fileName,
                                    const int lineNo)
    {
        if (expected != actual) {
            std::ostringstream out;
            out << fileName << ":" << lineNo << " Test assertion failed: " << message
                << "; expected: " << TestUtils::toString(expected)
                << ", was: " << TestUtils::toString(actual);
            abortTest(out.str());
        }
    }

    /**
     * Assert that the @c expected value is equal to the @c actual value.
     * If assertion fails, this implementation invokes @c abort().
     * @param expected expected value
     * @param actual actual value
     * @param fileName file name where the assertion has failed
     * @param lineNo line number where the assertion has failed
     */
    template<typename T>
    static inline void assertEquals(const T& expected,
                                    const T& actual,
                                    const char* fileName,
                                    const int lineNo)
    {
        assertEquals(expected, actual, "", fileName, lineNo);
    }

    /**
     * Helper for asserting equality of integer types.
     */
    static inline void assertEquals(const unsigned long long expected,
                                    const unsigned long long actual,
                                    const std::string& message,
                                    const char* fileName,
                                    const int lineNo)
    {
        if (expected != actual) {
            std::ostringstream out;
            out << fileName << ":" << lineNo << " Test assertion failed: " << message
                << "; expected: " << TestUtils::toString(expected)
                << ", was: " << TestUtils::toString(actual);
            abortTest(out.str());
        }
    }

    /**
     * Helper for asserting equality of C strings.
     */
    static inline void assertEquals(const char* expected,
                                    const std::string& actual,
                                    const std::string& message,
                                    const char* fileName,
                                    const int lineNo)
    {
        const std::string s(expected);
        assertEquals(s, actual, message, fileName, lineNo);
    }

    /**
     * Helper for asserting equality of C strings.
     */
    static inline void assertEquals(const char* expected,
                                    const std::string& actual,
                                    const char* fileName,
                                    const int lineNo)
    {
        const std::string s(expected);
        assertEquals(s, actual, fileName, lineNo);
    }

    /**
     * Assert the specified @c expression is @c true.
     * If assertion fails, this implementation prints a message on std::cout
     * then invokes @c abort().
     * @param expression
     * @param message test message to output when the assertion fails.
     * @param fileName file name where the assertion has failed
     * @param lineNo line number where the assertion has failed
     */
    static inline void assertTrue(bool expression,
                                  const std::string& message,
                                  const char* fileName,
                                  const int lineNo)
    {
        if (!expression) {
            std::ostringstream out;
            out << fileName << ":" << lineNo << " Test assertion failed: " << message;
            abortTest(out.str());
        }
    }

    /**
     * Assert the specified @c expression is @c true.
     * If assertion fails, this implementation prints a message on std::cout
     * then invokes @c abort().
     * @param expression
     * @param fileName file name where the assertion has failed
     * @param lineNo line number where the assertion has failed
     */
    static inline void assertTrue(bool expression, const char* fileName, const int lineNo)
    {
        assertTrue(expression, "", fileName, lineNo);
    }

    /**
     * Fails the current test.  This implementation prints a message on
     * std::cout then invokes @c abort().
     * @param message test message to output when the assertion fails.
     * @param fileName file name where the assertion has failed
     * @param lineNo line number where the assertion has failed
     */
    static inline void fail(const std::string& message, const char* fileName, const int lineNo)
    {
        std::ostringstream out;
        out << fileName << ":" << lineNo << " Test failed. " << message;
        abortTest(out.str());
    }

    /**
     * Fails the current test.  This implementation invokes @c abort().
     * @param fileName file name where the assertion has failed
     * @param lineNo line number where the assertion has failed
     */
    static inline void fail(const char* fileName, const int lineNo) { fail("", fileName, lineNo); }

  private:
    static void print(const std::string& s) { std::cerr << s << std::endl; }

    static void abortTest() { ::abort(); }

    static void abortTest(const std::string& s)
    {
        print(s);
        abortTest();
    }
};

/**
 * Test assertion macros.
 */
#define ASSERT_EQUALS(expected, actual)                                                            \
    do {                                                                                           \
        std::string s("");                                                                         \
        TestAssert::assertEquals(expected, actual, s, __FILE__, __LINE__);                         \
    } while (0)

#define ASSERT_EQUALS_MSG(message, expected, actual)                                               \
    do {                                                                                           \
        std::ostringstream out;                                                                    \
        out << message;                                                                            \
        TestAssert::assertEquals(expected, actual, out.str(), __FILE__, __LINE__);                 \
    } while (0)

#define ASSERT_TRUE(expression) TestAssert::assertTrue(expression, #expression, __FILE__, __LINE__);

#define ASSERT_TRUE_MSG(message, expression)                                                       \
    do {                                                                                           \
        std::ostringstream out;                                                                    \
        out << message;                                                                            \
        TestAssert::assertTrue(expression, out.str(), __FILE__, __LINE__);                         \
    } while (0)

#define FAIL(reason) TestAssert::fail(#reason, __FILE__, __LINE__);

/// Macros for method begin/end trace statements
#ifndef SPCDBG_ENTER
#define SPCDBG_ENTER(aspect) SPCDBG(L_INFO, "Enter", aspect)
#endif
#ifndef SPCDBG_EXIT
#define SPCDBG_EXIT(aspect) SPCDBG(L_DEBUG, "Exit", aspect)
#endif

/**
 * @brief Base of unit tests.
 */
class TestBase : public Distillery::DistilleryApplication
{
  public:
    TestBase()
      : appTracer_(NULL)
      , appTraceLevel_(0)
    {}

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 0, "app-tracing", ARG, "",
              "Application trace settings with format of filename.out:maxFileSize:maxFileNum (note "
              "the \".out\" file extension). If the file name part ends in \"{PID}\" then the "
              "actual process id is inserted replacing the \"{PID}\" substring.",
              STR_OPT(TestBase::setAppTracing) },
            { 0, "app-level", ARG, "",
              "Application trace level: OFF=0 (default), ERROR=1, WARN=2, INFO=3, DEBUG=4, TRACE=5",
              INT_OPT(TestBase::setAppLevel) },
        };
        APPEND_OPTIONS(options, args);
    }

    void setAppTracing(const option_t* option, const char* value)
    {
        if (value != NULL) {
            std::string pid("{PID}");
            std::string opt(value);
            std::string::size_type pos = opt.find(pid);
            // addPid if the filename ends in "{PID}"
            bool addPid = (pos != std::string::npos);
            std::string filename = (addPid)
                                     ? opt.substr(0, pos).append(opt.substr(pos + pid.size()))
                                     : std::string(value);
            appTracer_ = new Distillery::debug::RollingFileTracer(
              filename, appTraceLevel_, addPid /*Distillery::debug::RollingFileTracer::APP_TRC*/);
            Distillery::debug::set_app_tracer(appTracer_);
        }
    }

    void setAppLevel(const option_t* option, int value)
    {
        appTraceLevel_ = value;
        if (appTracer_ != NULL) {
            Distillery::debug::set_app_trclevel(appTraceLevel_);
        }
    }

    /**
     * Our own abort handler, which does not print a useless (in the context
     * of these tests) list of process memory pages.
     * The abort handler gets invoked on a failed assertion.
     */
    void sigAbortHandler(int sig)
    {
        async_safe_printf(2, "\nTest failed\n");
        sleep(1);
        _exit(1);
    }

    /**
     * Main application function: installs handler then runs tests.
     */
    int run(const std::vector<std::string>& /*remains*/)
    {
        removeSignalHandler(SIGABRT);
        installSignalHandler(SIGABRT, Distillery::mem_cb(this, &TestBase::sigAbortHandler));
        runTests();
        std::cout << "OK" << std::endl;
        return EXIT_SUCCESS;
    }

  protected:
    virtual ~TestBase() {}

    /// @brief Descendants run tests here.
    virtual void runTests() = 0;

  private:
    Distillery::debug::Tracer* appTracer_;
    int appTraceLevel_;
};

/// @}
}; // end namespace SPL

#endif /* TEST_UTILITY_TESTUTILS_H */
