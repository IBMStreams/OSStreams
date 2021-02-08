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

#include <SPL/Runtime/Operator/OperatorState.h>
#include <UTILS/Directory.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/SBuffer.h>

#include <cstdlib>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

struct NonBinaryData
{
    int a_int;
    float b_float;
    double c_double;
    bool d_bool;
    short e_short;
    char f_char_array[2350];
};

#define NUM_TESTS_1 10
#define NUM_TESTS_2 10
#define TEST_ERROR(iter, error)                                                                    \
    {                                                                                              \
        cerr << "Test error: " << testName << " - iteration " << iter << "\n\t" << error << "\n";  \
        exit(0);                                                                                   \
    }

class OperatorStateTest : public DistilleryApplication
{
  public:
    char testName[200];
    unsigned iter_;

    void initRandom()
    {
        timeval currtime;
        gettimeofday(&currtime, NULL);
        srand(currtime.tv_sec);
    }

    void generateRandomString(char* str, int size)
    {
        int i;
        const int NUM_ASCII_CHARS = 94;
        const int BASE_CHAR = 32;

        for (i = 0; i < size - 1; ++i) {
            str[i] = (char)((rand() % NUM_ASCII_CHARS) + BASE_CHAR);
        }
        str[size - 1] = '\0';
    }

    void generateRandomNonBinary(NonBinaryData& data)
    {
        // Non-sense numbers
        data.a_int = rand() % 123456;
        data.b_float = (float)((rand() % 9876)) / ((rand() % 876) + 1);
        data.c_double = (double)((rand() % 8765)) / ((rand() % 123) + 1);
        data.d_bool = not(data.d_bool);
        data.e_short = (short)rand() % 123;
        generateRandomString(data.f_char_array, sizeof(data.f_char_array));
    }

    void* generateRandomBinary(int size)
    {
        int i;
        unsigned char* data;

        if (size == 0) {
            data = NULL;
        } else {
            data = new unsigned char[size];
        }

        for (i = 0; i < size; ++i) {
            data[i] = (unsigned char)(rand() % (UCHAR_MAX + 1));
        }

        return ((void*)data);
    }

    void printNonBinaryData(NonBinaryData& data)
    {
        cerr << "Generated struct:\n"
                "\t int: "
             << data.a_int
             << "\n"
                "\t float: "
             << data.b_float
             << "\n"
                "\t double: "
             << data.c_double
             << "\n"
                "\t bool: "
             << data.d_bool
             << "\n"
                "\t short: "
             << data.e_short
             << "\n"
                "\t string: "
             << data.f_char_array << "\n";
    }

    void compareNonBinaryData(NonBinaryData& st1, NonBinaryData& st2)
    {
        if (st1.a_int != st2.a_int) {
            cerr << st1.a_int << " " << st2.a_int << "\n";
            TEST_ERROR(iter_, "Integer comparison error");
        }
        if (memcmp(&st1.b_float, &st2.b_float, sizeof(float)))
            TEST_ERROR(iter_, "Float comparison error");
        if (memcmp(&st1.c_double, &st2.c_double, sizeof(double)))
            TEST_ERROR(iter_, "Double comparison error");
        if (st1.d_bool != st2.d_bool)
            TEST_ERROR(iter_, "Boolean comparison error");
        if (strncmp(st1.f_char_array, st2.f_char_array, sizeof(st1.f_char_array)))
            TEST_ERROR(iter_, "String comparison error");
    }

    void testCheckpointNonBinary()
    {
        NonBinaryData original;
        NonBinaryData* restoredPointer;
        OperatorState cpSaved;
        OperatorState cpRestored;
        char fileName[50];

        SBuffer ckptOrg;

        original.d_bool = true;

        iter_ = 0;
        for (int i = 0; i < NUM_TESTS_1; ++i) {
            generateRandomNonBinary(original);
            ckptOrg.addBlob(&original, sizeof(NonBinaryData));

            snprintf(fileName, sizeof(fileName) - 1, "UT_Checkpoint-%d-%d.ckpt", (int)getpid(),
                     iter_);

            // Saving checkpoint
            ofstream cpOutFile(fileName, ios_base::binary);
            cpSaved.clear();
            cpSaved.addOperatorState((void*)ckptOrg.getPtr(), ckptOrg.getContentSize());
            cpSaved.saveState(cpOutFile);
            cpSaved.release();
            cpOutFile.flush();
            cpOutFile.close();

            // Restoring checkpoint
            ifstream cpInFile(fileName, ios_base::binary);
            cpRestored.restoreState(cpInFile);
            SBuffer ckptRst((unsigned char*)cpRestored.getOperatorState(0),
                            cpRestored.getOperatorStateSize(0));
            uint32_t rstSize;
            restoredPointer = (NonBinaryData*)ckptRst.getBlob(rstSize);

            if (rstSize != sizeof(NonBinaryData))
                TEST_ERROR(iter_, "Size of saved and restore operator differ");
            cpInFile.close();

            compareNonBinaryData(original, *restoredPointer);
            cpRestored.clear();

            ckptOrg.setICursor(0);
            ckptRst.setICursor(0);

            // Delete checkpoint
            Directory::sunlink(fileName);

            iter_++;
        }
    }

    void testCheckpointBinary(bool randOps, int minSize, int maxSize)
    {
        vector<SBuffer> savedState;
        OperatorState cpSaved;
        OperatorState cpRestored;
        char fileName[50];
        int numOps;
        int maxNumOps = 20;

        iter_ = 0;
        for (int i = 0; i < NUM_TESTS_2; ++i) {
            if (randOps) {
                numOps = (rand() % maxNumOps) + 1;
            } else {
                numOps = 1;
            }

            for (int j = 0; j < numOps; ++j) {
                int rndSize = (rand() % (maxSize - minSize)) + minSize;
                char* rndData = (char*)generateRandomBinary(rndSize);
                savedState.push_back(SBuffer((unsigned char*)rndData, rndSize));
                delete[] rndData;
            }
            snprintf(fileName, sizeof(fileName) - 1, "UT_Checkpoint-%d-%d.ckpt", (int)getpid(),
                     iter_);

            // Saving checkpoint
            ofstream cpOutFile(fileName, ios_base::binary);
            cpSaved.clear();
            for (int j = 0; j < numOps; ++j) {
                cpSaved.addOperatorState(savedState[j].getPtr(), savedState[j].getContentSize());
            }
            cpSaved.saveState(cpOutFile);
            cpSaved.release();
            cpOutFile.close();
            cpOutFile.flush();

            // Restoring checkpoint
            ifstream cpInFile(fileName, ios_base::binary);
            cpRestored.restoreState(cpInFile);
            if (numOps != cpRestored.getNumOfOperators())
                TEST_ERROR(iter_, "Number of saved and restored operators differ");
            for (int j = 0; j < numOps; ++j) {
                SBuffer restoredState((unsigned char*)cpRestored.getOperatorState(j),
                                      cpRestored.getOperatorStateSize(j));
                if ((int)savedState[j].getContentSize() != cpRestored.getOperatorStateSize(j))
                    TEST_ERROR(iter_, "Size saved and restored do not match");
                if (memcmp(savedState[j].getPtr(), restoredState.getPtr(),
                           cpRestored.getOperatorStateSize(j)))
                    TEST_ERROR(iter_, "Saved and restored data do not match");
            }
            cpInFile.close();

            savedState.clear();
            cpRestored.clear();

            // Delete checkpoint
            Directory::sunlink(fileName);

            iter_++;
        }
    }

    OperatorStateTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        initRandom();
        try {
            cerr << "######## Test 1 ########" << endl;
            strncpy(testName, "Checkpointing non-binary structures", sizeof(testName) - 1);
            testCheckpointNonBinary();
            cerr << "\t Test 1 completed successfully..." << endl;

            cerr << "######## Test 2 ########" << endl;
            strncpy(testName, "Checkpointing binary structures", sizeof(testName) - 1);
            testCheckpointBinary(false, 1, 524288);
            cerr << "\t Test 2 completed successfully..." << endl;

            cerr << "######## Test 3 ########" << endl;
            strncpy(testName, "Checkpointing binary structures with multiple operators",
                    sizeof(testName) - 1);
            testCheckpointBinary(true, 1, 524288);
            printf("\t Test 3 completed successfully...\n");

            printf("######## Test 4 ########\n");
            strncpy(testName, "Checkpointing binary structures with operators of size 0",
                    sizeof(testName) - 1);
            testCheckpointBinary(true, 0, 20);
            printf("\t Test 4 completed successfully...\n");
        } catch (DistilleryException& e) {
            cerr << "Failed Unit Tests - iteration " << iter_ << " - reason: " << e.getExplanation()
                 << endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::OperatorStateTest)
