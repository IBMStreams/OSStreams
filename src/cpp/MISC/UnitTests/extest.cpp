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

//----------------------------------------------------------------------------
//
// Title-
//       TestAttribute.cpp
//
// Purpose-
//       Attribute unit tester.
//
// Last change date-
//       2004/11/12
//
//----------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/RuntimeMessages.h>
#include <UTILS/SBuffer.h>
#include <UTILS/SPCException.h>
#include <UTILS/UTILSTypes.h>

SPC_NAMESPACE_USE
UTILS_NAMESPACE_USE
using namespace std;

class ExceptionUnitTest : public DistilleryApplication
{

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test00
    //
    // Purpose-
    //       Compile test.
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test00(void) // Test case
    {
        cout << "*********************************************************   Start Test 0" << endl;
        try {

            THROW(NewAllocatorFailure, __FUNCTION__);

        } catch (NewAllocatorFailureException& ex) {
            cout << "Exception: " << ex << endl;

            // validate exception code and the number of substitution texts are not set
            if (ex.getExceptionCode().getMessageId() !=
                UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                cout << "exception code mismatch!" << endl;
                return 1;
            }
            std::vector<std::string> vec1 = ex.getSubstitutionText();
            if (vec1.size() != 0) {
                cout << "Substitution text mismatch!" << endl;
                return 1;
            }
        }

        cout << "*********************************************************   End Test 0" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test01
    //
    // Purpose-
    //       Store/fetch function test.
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test01(void) // Test case
    {

        cout << "*********************************************************   Start Test 1" << endl;
        try {
            THROW(NewAllocatorFailure, __FUNCTION__);
        } catch (DistilleryException& ex) {
            cout << "Exception: " << ex << endl;

            // validate exception code and the number of substitution texts are not set
            if (ex.getExceptionCode().getMessageId() !=
                UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                cout << "exception code mismatch!" << endl;
                return 1;
            }
            std::vector<std::string> vec1 = ex.getSubstitutionText();
            if (vec1.size() != 0) {
                cout << "Substitution text mismatch!" << endl;
                return 1;
            }
        }

        cout << "*********************************************************   End Test 1" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test02
    //
    // Purpose-
    //       Store/fetch function test.
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test02(void) // Test case
    {

        cout << "*********************************************************   Start Test 2" << endl;
        cout << "Instantior List: ";
        SPCExceptionInstantiator::print(cout);
        cout << endl;

        cout << "*********************************************************   End Test 2" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test03
    //
    // Purpose-
    //       Store/fetch function test.
    //
    //----------------------------------------------------------------------------

    void f5() { THROW(NewAllocatorFailure, __FUNCTION__); }

    void f4() { f5(); }

    void f3() { f4(); }

    void f2() { f3(); }

    void f1() { f2(); }

    int          // Error count
    test03(void) // Test case
    {
        int catched = 0;
        cout << "*********************************************************   Start Test 3" << endl;

        try {
            f1();
        } catch (NewAllocatorFailureException& ex) {
            cout << "Exception: " << ex << endl;
            catched = 1;

            // validate exception code and the number of substitution texts are not set
            if (ex.getExceptionCode().getMessageId() !=
                UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                cout << "exception code mismatch!" << endl;
                return 1;
            }
            std::vector<std::string> vec1 = ex.getSubstitutionText();
            if (vec1.size() != 0) {
                cout << "Substitution text mismatch!" << endl;
                return 1;
            }
        }

        cout << "*********************************************************   End Test 3" << endl;
        return catched ? 0 : 1;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test04
    //
    // Purpose-
    //       Store/fetch function test.
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test04(void) // Test case
    {
        cout << "*********************************************************   Start Test 4" << endl;
        SBuffer buffer;
        int catch_done = 0;

        SPCException df("foo1", "foo2");
        SBuffer m;
        df.serialize(m);
        cout << "Exception name is: " << m.getSTLString() << endl;

        buffer.addSTLString("SPC::SPCException");
        buffer.addSTLString("location");
        buffer.addSTLString("message");
        buffer.addSTLString("backtrace");
        buffer.addSTLString("987654321L");
        buffer.addUInt32((uint32_t)0);
        buffer.addSTLString("");

        try {
            SPCExceptionInstantiator::instantiateAndThrow(buffer);
        }
#if SPC_REGISTER_INSTANTIATORS
        catch (SPCException& newex) {
            cout << "New exception type: " << newex << endl;
            catch_done = 1;

            // validate exception code and the number of substitution texts are set correctly
            if (newex.getExceptionCode().getMessageId() != "987654321L") {
                cout << "exception code mismatch!" << endl;
                return 1;
            }
            std::vector<std::string> vec1 = newex.getSubstitutionText();
            if (vec1.size() != 0) {
                cout << "Substitution text mismatch!" << endl;
                return 1;
            }
        } catch (DistilleryException& newex) {
            cout << "Invalid exception type: " << newex << endl;
            return 1;
        }
#else
        catch (DistilleryException& newex) {
            cout << "instantiators are disabled" << endl;
            cout << "New exception type: " << newex << endl;
            catch_done = 1;

            // validate exception code and the number of substitution texts are set correctly
            if (newex.getExceptionCode().getMessageId() !=
                UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                cout << "exception code mismatch!" << endl;
                return 1;
            }
            std::vector<std::string> vec1 = newex.getSubstitutionText();
            if (vec1.size() != 0) {
                cout << "Substitution text mismatch!" << endl;
                return 1;
            }
        }
#endif

        if (!catch_done) {
            cout << "No exception catched" << endl;
            return 1;
        }

        cout << "*********************************************************   End Test 4" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test05
    //
    // Purpose-
    //       Store/fetch function test.
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test05(void) // Test case
    {
        cout << "*********************************************************   Start Test 5" << endl;
        DistilleryException* ex = 0;

        try {
            THROW(NewAllocatorFailure, "bla");
        } catch (NewAllocatorFailureException& newex) {
            ex = new NewAllocatorFailureException(newex);
        }

        if (ex) {
            int catch_done = 0;
            SBuffer buf;
            ex->serialize(buf);

            delete ex;

            try {
                SPCExceptionInstantiator::instantiateAndThrow(buf);
            }

#if SPC_REGISTER_INSTANTIATORS
            catch (NewAllocatorFailureException& newex) {
                cout << "New exception type: " << newex << endl;
                catch_done = 1;

                /// validate exception code and the number of substitution texts are not set
                if (newex.getExceptionCode().getMessageId() !=
                    UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                    cout << "exception code mismatch!" << endl;
                    return 1;
                }
                std::vector<std::string> vec1 = newex.getSubstitutionText();
                if (vec1.size() != 0) {
                    cout << "Substitution text mismatch!" << endl;
                    return 1;
                }
            } catch (DistilleryException& newex) {
                cout << "Invalid exception type: " << newex << endl;
                return 1;
            }
#else
            catch (DistilleryException& newex) {
                cout << "instantiors are disabled" << endl;
                cout << "New exception type: " << newex << endl;
                catch_done = 1;

                /// validate exception code and the number of substitution texts are not set
                if (newex.getExceptionCode().getMessageId() !=
                    UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                    cout << "exception code mismatch!" << endl;
                    return 1;
                }
                std::vector<std::string> vec1 = newex.getSubstitutionText();
                if (vec1.size() != 0) {
                    cout << "Substitution text mismatch!" << endl;
                    return 1;
                }
            }
#endif

            if (!catch_done) {
                cout << "No exception catched" << endl;
                return 1;
            }
        } else {
            cout << "No exception catched" << endl;
            return 1;
        }

        cout << "*********************************************************   End Test 5" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test01
    //
    // Purpose-
    //       Store/fetch function test.
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test06(void) // Test case
    {
        cout << "*********************************************************   Start Test 6" << endl;
        try {
            try {
                try {
                    THROW(NewAllocatorFailure, "bla");
                } catch (DistilleryException& ex) {
                    THROW_NESTED(MallocFailure, "bli", ex);
                }
            } catch (DistilleryException& ex) {
                THROW_NESTED(MallocFailure, "bli", ex);
            }
        } catch (DistilleryException& ex) {
            cout << "Exception: " << ex << endl;

            /// validate exception code and the number of substitution texts are not set
            if (ex.getExceptionCode().getMessageId() !=
                UTILS_NAMESPACE::DistilleryExceptionCode::NO_MESSAGE_ID) {
                cout << "exception code mismatch!" << endl;
                return 1;
            }
            std::vector<std::string> vec1 = ex.getSubstitutionText();
            if (vec1.size() != 0) {
                cout << "Substitution text mismatch!" << endl;
                return 1;
            }
        }

        cout << "*********************************************************   End Test 6" << endl;
        return 0;
    }

    ///
    /// Test new macro THROW_NESTED by passing an exception codes and
    /// substitution texts. Validate the correct exception code and
    /// substitution texts come through on the exception.
    ///
    int test07(void)
    {
        cout << "*********************************************************   Start Test 7" << endl;
        std::vector<std::string> vec1, vec2, vec3;
        vec1.push_back("1");
        vec1.push_back("2");
        vec1.push_back("3");
        vec1.push_back("4");
        vec1.push_back("5");
        vec1.push_back("6");

        vec2.push_back("One");
        vec2.push_back("Two");
        vec2.push_back("Three");
        vec2.push_back("Four");
        vec2.push_back("Five");
        vec2.push_back("Six");
        vec2.push_back("Seven");
        vec2.push_back("Eight");
        vec2.push_back("Nine");
        vec2.push_back("Ten");
        vec2.push_back("Eleven");
        vec2.push_back("Twelve");
        vec2.push_back("Thirteen");
        vec2.push_back("Fourteen");
        vec2.push_back("Fifteen");
        vec2.push_back("Sixteen");
        vec2.push_back("Seventeen");
        vec2.push_back("Eighteen");
        vec2.push_back("Nineteen");

        try {
            try {
                try {
                    THROW(NewAllocatorFailure, "bla");
                } catch (DistilleryException& ex) {
                    THROW_NESTED(MallocFailure, "bli", ex, "987654321", "One", "Two", "Three",
                                 "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven",
                                 "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen",
                                 "Seventeen", "Eighteen", "Nineteen");
                }
            } catch (DistilleryException& ex) {
                THROW_NESTED(MallocFailure, "bli", ex, "123456789", "1", "2", "3", "4", "5", "6");
            }
        } catch (DistilleryException& ex) {
            cout << "Exception: " << ex << endl;

            /// validate exception code and the number of substitution texts are set correctly
            if (ex.getExceptionCode().getMessageId() != "123456789") {
                cout << "ERROR ERROR - non matching exception code" << endl;
                return 1;
            }
            vec3 = ex.getSubstitutionText();
            if (vec3.size() != vec1.size()) {
                cout << "ERROR ERROR - non matching substiution text" << endl;
                return 1;
            }
            for (unsigned int i = 0; i < vec3.size(); i++) {
                if (vec3[i] != vec1[i]) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
            }
        }

        cout << "*********************************************************   End Test 7" << endl;
        return 0;
    }

    ///
    /// Test new macro THROW_WITH_EXCEPTION_CODE by passing an exception
    /// code and substitution text.  Validate the correct exception code and
    /// substitution texts come through on the exception.
    ///
    /// Also use instantiateAndThrow with a serialization buffer.
    ///
    int test08(void)
    {
        cout << "*********************************************************   Start Test 8" << endl;
        SBuffer buffer;
        DistilleryException* ex = 0;
        std::vector<std::string> vec1, vec2, vec3;
        vec1.push_back("1");
        vec1.push_back("2");
        vec1.push_back("3");
        vec1.push_back("4");
        vec1.push_back("5");
        vec1.push_back("6");

        try {
            THROW(NewAllocatorFailure, "bla", "42", "1", "2", "3", "4", "5", "6");
        } catch (NewAllocatorFailureException& newex) {
            ex = new NewAllocatorFailureException(newex);
        }

        if (ex) {
            int catch_done = 0;
            SBuffer buf;
            ex->serialize(buf);
            delete ex;
            try {
                SPCExceptionInstantiator::instantiateAndThrow(buf);
            }

#if SPC_REGISTER_INSTANTIATORS
            catch (NewAllocatorFailureException& newex) {
                cout << "New exception type: " << newex << endl;
                if (newex.getExceptionCode().getMessageId() != "42") {
                    cout << "ERROR ERROR - non matching exception code" << endl;
                    return 1;
                }
                vec3 = newex.getSubstitutionText();
                if (vec3.size() != vec1.size()) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
                for (unsigned int i = 0; i < vec3.size(); i++) {
                    if (vec3[i] != vec1[i]) {
                        cout << "ERROR ERROR - non matching substiution text" << endl;
                        return 1;
                    }
                }
                catch_done = 1;
            } catch (DistilleryException& newex) {
                cout << "Invalid exception type: " << newex << endl;
                return 1;
            }
#else
            catch (DistilleryException& newex) {
                cout << "instantiors are disabled" << endl;
                cout << "New exception type: " << newex << endl;
                if (newex.getExceptionCode().getMessageId() != "42") {
                    cout << "ERROR ERROR - non matching exception code" << endl;
                    return 1;
                }
                vec3 = newex.getSubstitutionText();
                if (vec3.size() != vec1.size()) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
                for (unsigned int i = 0; i < vec3.size(); i++) {
                    if (vec3[i] != vec1[i]) {
                        cout << "ERROR ERROR - non matching substiution text" << endl;
                        return 1;
                    }
                }

                catch_done = 1;
            }
#endif

            if (!catch_done) {
                cout << "No exception caught" << endl;
                return 1;
            }
        } else {
            cout << "No exception caught" << endl;
            return 1;
        }

        cout << "*********************************************************   End Test 8" << endl;
        return 0;
    }

    ///
    /// Test  macro THROW by passing an exception
    /// code and substitution text.  Validate the correct exception code and
    /// substitution texts come through on the exception.
    ///
    /// Also use instantiateAndThrow with an XML buffer.
    ///
    int test09(void)
    {
        cout << "*********************************************************   Start Test 9" << endl;
        SBuffer buffer;
        DistilleryException* ex = 0;
        std::vector<std::string> vec1, vec2, vec3;
        vec1.push_back("1");
        vec1.push_back("2");
        vec1.push_back("3");
        vec1.push_back("4");
        vec1.push_back("5");
        vec1.push_back("6");

        try {
            THROW(NewAllocatorFailure, "bla", "42", "1", "2", "3", "4", "5", "6");
        } catch (NewAllocatorFailureException& newex) {
            ex = new NewAllocatorFailureException(newex);
        }

        if (ex) {
            int catch_done = 0;
            std::string buf;
            buf = ex->toXMLString();
            delete ex;
            try {
                SPCExceptionInstantiator::instantiateAndThrow(buf);
            }

#if SPC_REGISTER_INSTANTIATORS
            catch (NewAllocatorFailureException& newex) {
                cout << "New exception type: " << newex << endl;
                if (newex.getExceptionCode().getMessageId() != "42") {
                    cout << "ERROR ERROR - non matching exception code" << endl;
                    return 1;
                }
                vec3 = newex.getSubstitutionText();
                if (vec3.size() != vec1.size()) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
                for (unsigned int i = 0; i < vec3.size(); i++) {
                    if (vec3[i] != vec1[i]) {
                        cout << "ERROR ERROR - non matching substiution text" << endl;
                        return 1;
                    }
                }
                catch_done = 1;
            } catch (DistilleryException& newex) {
                cout << "Invalid exception type: " << newex << endl;
                return 1;
            }
#else
            catch (DistilleryException& newex) {
                cout << "instantiors are disabled" << endl;
                cout << "New exception type: " << newex << endl;
                if (newex.getExceptionCode().getMessageId() != "42") {
                    cout << "ERROR ERROR - non matching exception code" << endl;
                    return 1;
                }
                vec3 = newex.getSubstitutionText();
                if (vec3.size() != vec1.size()) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
                for (unsigned int i = 0; i < vec3.size(); i++) {
                    if (vec3[i] != vec1[i]) {
                        cout << "ERROR ERROR - non matching substiution text" << endl;
                        return 1;
                    }
                }

                catch_done = 1;
            }
#endif
            if (!catch_done) {
                cout << "No exception caught" << endl;
                return 1;
            }
        } else {
            cout << "No exception caught" << endl;
            return 1;
        }

        cout << "*********************************************************   End Test 9" << endl;
        return 0;
    }

    ///
    /// Test  macro THROW by passing an exception
    /// code and substitution text.  Validate the correct exception code and
    /// substitution texts come through on the exception.
    ///
    /// Also use instantiateAndThrow with mulitple paramaters.
    ///
    int test10(void)
    {
        cout << "*********************************************************   Start Test 10" << endl;
        SBuffer buffer;
        DistilleryException* ex = 0;
        std::vector<std::string> vec1, vec2, vec3;
        vec1.push_back("1");
        vec1.push_back("2");
        vec1.push_back("3");
        vec1.push_back("4");
        vec1.push_back("5");
        vec1.push_back("6");

        try {
            THROW(NewAllocatorFailure, "bla", "42", "1", "2", "3", "4", "5", "6");
        } catch (NewAllocatorFailureException& newex) {
            ex = new NewAllocatorFailureException(newex);
        }

        if (ex) {
            int catch_done = 0;
            std::string s1, s2, s3, s4;

            std::vector<std::string> p6;
            s1 = ex->getType();
            s2 = ex->getLocation();
            s3 = ex->getExplanation();
            s4 = ex->getBacktrace();
            DistilleryExceptionCode p5(ex->getExceptionCode());
            p6 = ex->getSubstitutionText();
            delete ex;
            try {
                SPCExceptionInstantiator::instantiateAndThrow(s1, s2, s3, s4, p5, p6);
            }

#if SPC_REGISTER_INSTANTIATORS
            catch (NewAllocatorFailureException& newex) {
                cout << "New exception type: " << newex << endl;
                if (newex.getExceptionCode().getMessageId() != "42") {
                    cout << "ERROR ERROR - non matching exception code" << endl;
                    return 1;
                }
                vec3 = newex.getSubstitutionText();
                if (vec3.size() != vec1.size()) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
                for (unsigned int i = 0; i < vec3.size(); i++) {
                    if (vec3[i] != vec1[i]) {
                        cout << "ERROR ERROR - non matching substiution text" << endl;
                        return 1;
                    }
                }
                catch_done = 1;
            } catch (DistilleryException& newex) {
                cout << "Invalid exception type: " << newex << endl;
                return 1;
            }
#else
            catch (DistilleryException& newex) {
                cout << "instantiors are disabled" << endl;
                cout << "New exception type: " << newex << endl;
                if (newex.getExceptionCode().getMessageId() != "42") {
                    cout << "ERROR ERROR - non matching exception code" << endl;
                    return 1;
                }
                vec3 = newex.getSubstitutionText();
                if (vec3.size() != vec1.size()) {
                    cout << "ERROR ERROR - non matching substiution text" << endl;
                    return 1;
                }
                for (unsigned int i = 0; i < vec3.size(); i++) {
                    if (vec3[i] != vec1[i]) {
                        cout << "ERROR ERROR - non matching substiution text" << endl;
                        return 1;
                    }
                }

                catch_done = 1;
            }
#endif
            if (!catch_done) {
                cout << "No exception caught" << endl;
                return 1;
            }
        } else {
            cout << "No exception caught" << endl;
            return 1;
        }

        cout << "*********************************************************   End Test 10" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test11
    //
    // Purpose-
    //       Test printShort, printMedium, printLong
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test11(void) // Test case
    {
        cout << "*********************************************************   Start Test 11" << endl;
        try {

            THROW(NewAllocatorFailure, __FUNCTION__);

        } catch (NewAllocatorFailureException& ex) {
#define M(name)                                                                                    \
    cout << #name ": ";                                                                            \
    ex.print##name(cout);                                                                          \
    cout << endl;

            M(Short);
            M(Medium);
            M(Long);

#undef M
        }

        cout << "*********************************************************   End Test 11" << endl;
        return 0;
    }

    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       test12
    //
    // Purpose-
    //       Test THROW_CHAR
    //
    //----------------------------------------------------------------------------
    int          // Error count
    test12(void) // Test case
    {
        cout << "*********************************************************   Start Test 12" << endl;
        int tries = 5;
        for (int i = 0; i < tries; i++) {
            try {
                THROW_CHAR(NewAllocatorFailure, "THROW_CHAR(THROW_STRING) test!",
                           TRANSSocketTimeout);
            } catch (NewAllocatorFailureException& ex) {
                cout << "Exception: " << ex << endl;
                if (ex.getExplanation() != "THROW_CHAR(THROW_STRING) test!") {
                    cout << "Explanation " << ex.getExplanation()
                         << " mismatch! Expect 'THROW_CHAR(THROW_STRING) test!'" << endl;
                    return 1;
                }
                // validate exception code
                if (ex.getExceptionCode().getMessageId() != TRANSSocketTimeout) {
                    cout << "exception code " << ex.getExceptionCode().getMessageId()
                         << " mismatch! Expect " << TRANSSocketTimeout << endl;
                    return 1;
                }
                std::vector<std::string> vec1 = ex.getSubstitutionText();
                if (vec1.size() != 0) {
                    cout << "Substitution text mismatch!" << endl;
                    return 1;
                }
            }

            try {
                THROW_CHAR(NewAllocatorFailure, "THROW_CHAR(THROW_STRING) test!",
                           RuntimePropertyUpdated, "THROW_CHAR", "test12", "extest");
            } catch (NewAllocatorFailureException& ex) {
                cout << "Exception: " << ex << endl;
                if (ex.getExplanation() != "THROW_CHAR(THROW_STRING) test!") {
                    cout << "Explanation " << ex.getExplanation()
                         << " mismatch! Expect 'THROW_CHAR(THROW_STRING) test!'" << endl;
                    return 1;
                }
                // validate exception code
                if (ex.getExceptionCode().getMessageId() != RuntimePropertyUpdated) {
                    cout << "exception code " << ex.getExceptionCode().getMessageId()
                         << " mismatch! Expect " << TRANSSocketTimeout << endl;
                    return 1;
                }
                std::vector<std::string> vec1 = ex.getSubstitutionText();
                if (vec1.size() != 3) {
                    cout << "Substitution text mismatch!" << endl;
                    return 1;
                }
            }
        }

        cout << "*********************************************************   End Test 12" << endl;
        return 0;
    }
    //----------------------------------------------------------------------------
    //
    // Subroutine-
    //       main
    //
    // Purpose-
    //       Mainline code.
    //
    //----------------------------------------------------------------------------
  public:
    virtual int run(const std::vector<std::string>& remains)
    {
        int errorCount = 0; // Error counter

        //-------------------------------------------------------------------------
        // Drive the tests
        //-------------------------------------------------------------------------
        if (1) {
            errorCount += test00(); // Compilation test
        }
        if (1) {
            errorCount += test01(); // Store/fetch test
        }
        if (1) {
            errorCount += test02(); // Store/fetch test
        }
        if (1) {
            errorCount += test03(); // Store/fetch test
        }
        if (1) {
            errorCount += test04(); // Store/fetch test
        }
        if (1) {
            errorCount += test05(); // Store/fetch test
        }
        if (1) {
            errorCount += test06(); // Store/fetch test
        }
        if (1) {
            errorCount += test07();
        }
        if (1) {
            errorCount += test08();
        }
        if (1) {
            errorCount += test09();
        }
        if (1) {
            errorCount += test10();
        }
        if (1) {
            errorCount += test11();
        }
        if (1) {
            errorCount += test12();
        }

        //-------------------------------------------------------------------------
        // Exit
        //-------------------------------------------------------------------------
        cout << _argv_0 << ": Error count=" << errorCount << endl;
        if (errorCount > 0) {
            exit(EXIT_FAILURE);
        }

        return 0;
    }
};

MAIN_APP(ExceptionUnitTest)
