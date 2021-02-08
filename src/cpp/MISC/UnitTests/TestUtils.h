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

#ifndef TESTUTILS_H_
#define TESTUTILS_H_

#include <UTILS/UTILSTypes.h>

#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdint.h>

/**
 * @internal
 * @file TestUtils.h
 * Utility functions used for unit testing.
 */

UTILS_NAMESPACE_BEGIN

class TestUtils {
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
 * JUnit-like test assertions.
 */
class TestAssert
{
public:
    template<typename T>
    static inline void assertEquals(const T& expected, const T& actual,
            const std::string& message, const char* fileName, const int lineNo)
    {
        if (expected != actual) {
            std::ostringstream out;
            out << fileName << ":" << lineNo << " Test assertion failed: " <<
                    message << "; expected: " << TestUtils::toString(expected) <<
                    ", was: " << TestUtils::toString(actual);
            abortTest(out.str());
        }
    }

    template<typename T>
    static inline void assertEquals(const T& expected, const T& actual,
            const char* fileName, const int lineNo)
    {
        assertEquals(expected, actual, "", fileName, lineNo);
    }

    // Static helpers for integer types
    static inline void assertEquals(const unsigned long long expected, const unsigned long long actual,
            const std::string& message, const char* fileName, const int lineNo)
    {
        if (expected != actual) {
            std::ostringstream out;
            out << fileName << ":" << lineNo << " Test assertion failed: " <<
                    message << "; expected: " << TestUtils::toString(expected) <<
                    ", was: " << TestUtils::toString(actual);
            abortTest(out.str());
        }
    }

    template<typename T>
    static inline void assertEquals(const unsigned long long expected, const unsigned long long actual,
            const char* fileName, const int lineNo)
    {
        assertEquals(expected, actual, "", fileName, lineNo);
    }

    static inline void assertEquals(const long long expected, const long long actual,
            const std::string& message, const char* fileName, const int lineNo)
    {
        assertEquals((const unsigned long long)expected, (const unsigned long long)actual, message, fileName, lineNo);
    }

    template<typename T>
    static inline void assertEquals(const long long expected, const long long actual,
            const char* fileName, const int lineNo)
    {
        assertEquals(expected, actual, "", fileName, lineNo);
    }

    static inline void assertEquals(const unsigned long expected, const unsigned long actual,
            const std::string& message, const char* fileName, const int lineNo)
    {
        assertEquals((const unsigned long long)expected, (const unsigned long long)actual, message, fileName, lineNo);
    }

    template<typename T>
    static inline void assertEquals(const unsigned long expected, const unsigned long actual,
            const char* fileName, const int lineNo)
    {
        assertEquals(expected, actual, "", fileName, lineNo);
    }

    static inline void assertEquals(const long expected, const long actual,
            const std::string& message, const char* fileName, const int lineNo)
    {
        assertEquals((const long long)expected, (const long long)actual, message, fileName, lineNo);
    }

    template<typename T>
    static inline void assertEquals(const long expected, const long actual,
            const char* fileName, const int lineNo)
    {
        assertEquals(expected, actual, "", fileName, lineNo);
    }

    // Static helpers for C strings
    static inline void assertEquals(const char* expected, const std::string& actual,
            const std::string& message, const char* fileName, const int lineNo)
    {
        const std::string s(expected);
        assertEquals(s, actual, message, fileName, lineNo);
    }

    static inline void assertEquals(const char* expected, const std::string& actual,
            const char* fileName, const int lineNo)
    {
        const std::string s(expected);
        assertEquals(s, actual, fileName, lineNo);
    }

    static inline void assertTrue(bool expression, const std::string& message,
            const char* fileName, const int lineNo)
    {
        if (!expression) {
            std::ostringstream out;
            out << fileName << ":" << lineNo << " Test assertion failed: " << message;
            abortTest(out.str());
        }
    }

    static inline void assertTrue(bool expression, const char* fileName, const int lineNo)
    {
        assertTrue(expression, "", fileName, lineNo);
    }

    static inline void fail(const std::string& message, const char* fileName, const int lineNo)
    {
        std::ostringstream out;
        out << fileName << ":" << lineNo << " Test failed. " << message;
        abortTest(out.str());
    }

    static inline void fail(const char* fileName, const int lineNo)
    {   fail("", fileName, lineNo);  }

private:
    static void print(const std::string& s)
    {   std::cerr << s << std::endl; }

    static void abortTest()
    {   ::abort(); }

    static void abortTest(const std::string& s)
    {   print(s); abortTest(); }
};

/**
 * Test assertion macros.
 */
#define ASSERT_EQUALS(expected, actual) \
        TestAssert::assertEquals(expected, actual, __FILE__, __LINE__);

#define ASSERT_EQUALS_MSG(message, expected, actual)                        \
        do {                                                                \
            std::ostringstream out;                                         \
            out << message;                                                 \
            TestAssert::assertEquals(expected, actual, out.str(), __FILE__, __LINE__); \
        } while(0)

#define ASSERT_TRUE(expression) \
        TestAssert::assertTrue(expression, #expression, __FILE__, __LINE__);

#define ASSERT_TRUE_MSG(message, expression)                                 \
        do {                                                                 \
            std::ostringstream out;                                          \
            out << message;                                                  \
            TestAssert::assertTrue(expression, out.str(), __FILE__, __LINE__);\
        } while(0)

#define FAIL(reason) \
        TestAssert::fail(#reason, __FILE__, __LINE__);

UTILS_NAMESPACE_END

#endif /* TESTUTILS_H_ */
