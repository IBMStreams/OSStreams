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

#include <UTILS/DistilleryApplication.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/Thread.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace std;
UTILS_NAMESPACE_USE;

static int print_method = 0;

static void try_print(ostream& strm, int fd, const char* msg, int idx)
{
    switch (print_method) {
        default:
        case 0:
            strm << msg << idx << endl;
            break;
        case 1:
            if (fd == 2) {
                fprintf(stderr, "%s %d\n", msg, idx);
            } else {
                printf("%s %d\n", msg, idx);
            }
            break;
        case 2: {
            stringstream sstrm;
            sstrm << msg << " " << idx << endl;
            write(2, sstrm.str().data(), sstrm.str().length());
        } break;
        case 3:
            async_safe_printf(2, "%s %d\n", msg, idx);
            break;
    }
}

class T : public Thread
{
    virtual void* run(void* threadArgs)
    {
        for (int i = 0;; i++) {
            try_print(cout, 1, "waiting<2>", i);
            try_print(cerr, 2, "waiting<2>", i);
        }

        return NULL;
    }
};

class sigkill : public DistilleryApplication
{
  public:
    virtual int run(const vector<string>& args)
    {
        removeSignalHandler(SIGPIPE);
        installSignalHandler(SIGABRT, mem_cb(this, &sigkill::mySigAbortHandler));
        removeSignalHandler(SIGINT);
        installSignalHandler(SIGINT, mem_cb(this, &sigkill::mySigIntHandler));

        T t;
        int ret = t.create();
        if (ret < 0) {
            throw "thread creation failed";
        }

        for (int i = 0;; i++) {
            try_print(cout, 1, "waiting<1>", i);
            try_print(cerr, 2, "waiting<1>", i);
        }

        return 0;
    }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'm', "method", ARG | REQUIRED, "", "Method used to print",
              INT_OPT(sigkill::setMethod) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setMethod(const option_t* option, int value) { print_method = value; }

  protected:
    void mySigAbortHandler(int mysig)
    {
        async_safe_printf(2, "oops, abort\n");
        exit(1);
    }

    void mySigIntHandler(int mysig)
    {
        try_print(cout, 1, "I want to live !!!", 1);
        try_print(cerr, 2, "I want to live !!!", 0);
    }
};

MAIN_APP(sigkill);
