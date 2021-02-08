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

#include <iostream>
#include <stdlib.h>

#include <TRC/DistilleryDebug.h>
#include <UTILS/BacktraceDumper.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/LinkedList.h>

#ifdef GNUPROF
#include "IntLinkedList.h"
#define intLL IntLinkedList
#else
#define intLL LinkedList<int>
#endif

using namespace std;
UTILS_NAMESPACE_USE;

typedef struct
{
    int v1;
    int v2;
} ss;

ostream& operator<<(ostream& strm, const ss& v)
{
    strm << "(" << v.v1 << "," << v.v2 << ")";
    return strm;
}

class lltest : public DistilleryApplication
{
  public:
    int s;

    lltest()
      : s(100)
    {}

    void setSize(const option_t* option, int value) { s = value; }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 's', "size", ARG, "", "list size", INT_OPT(lltest::setSize) },
        };

        APPEND_OPTIONS(options, args);
    }

    virtual ostream& printDescription(ostream& o) const
    {
        o << "This is the linked list test application";

        return o;
    }

    virtual int run(const arg_vector_t& args)
    {
        intLL l;

        for (int i = 1; i <= s; i++) {
            l.append(i);
        }

        cout << "append: size=" << l.size() << endl;
        cout << "append: " << l << endl;

        {
            intLL::iterator it = l.begin();
            while (!it.end()) {
                cout << "iterator: " << *it << endl;
                it++;
            }
        }

        {
            const intLL& cl = l;
            intLL::const_iterator it = cl.begin();
            while (!it.end()) {
                cout << "const_iterator: " << *it << endl;
                it++;
            }
        }

        {
            intLL::iterator it = l.begin();
            l.erase(it);
        }

        cout << "erase: size=" << l.size() << endl;
        cout << "erase: " << l << endl;

        {
            const intLL& cl = l;
            intLL::const_iterator it = cl.begin();
            l.erase(it);
            l.erase(it);
        }

        cout << "erase: size=" << l.size() << endl;
        cout << "erase: " << l << endl;

        while (l.size() > 0) {
            l.erase(l.begin());
            cout << "erase: " << l << endl;
        }

        for (int i = 1; i <= s; i++) {
            l.append(i);
        }

        cout << "append: " << l << endl;

        l.clear();

        cout << "clear: " << l << endl;

        for (int i = 1; i <= s; i++) {
            l.append(i);
        }

        cout << "append: " << l << endl;

        srand(getpid());

        while (l.size() > 0) {
            unsigned int sz = l.size();

            intLL::iterator it(l.begin());
            while (sz > 1) {
                it++;
                sz--;
            }

            l.erase(it);
            cout << "erase: " << l << endl;
        }

        for (int i = 1; i <= s; i++) {
            l.append(i);
        }

        cout << "append: " << l << endl;

        while (l.size() > 0) {
            unsigned int middle = rand() % l.size();

            intLL::iterator it(l.begin());
            while (middle) {
                it++;
                middle--;
            }

            l.erase(it);
            cout << "erase: " << l << endl;
        }

        LinkedList<string> sl;
        sl.append("lltest");
        cout << "append: " << sl << endl;

        LinkedList<ss> stl;
        ss v = { 1, 2 };
        stl.append(v);
        v.v1 = 3;
        v.v2 = 4;
        stl.append(v);
        cout << "append: " << stl << endl;

        for (int i = 1; i <= s; i++) {
            l.append(i);
        }

        {
            intLL speed;
            for (int i = 0; i < 1000000; i++) {
                speed.append(i);
            }

            int64_t begin = debug::TimeStamp::TOD_us();
            int sum = 0;
            intLL::iterator speedit = speed.begin();
            while (!speedit.end()) {
                sum += *speedit;
                speedit++;
            }
            int64_t end = debug::TimeStamp::TOD_us();

            int64_t loops = 5000000 / (end - begin);

            begin = debug::TimeStamp::TOD_us();
            int lp = loops;
            while (lp) {
                sum = 0;
                intLL::iterator speedit2 = speed.begin();
                while (!speedit2.end()) {
                    sum += *speedit2;
                    speedit2++;
                }

                lp--;
            }
            end = debug::TimeStamp::TOD_us();

            cout << (double)(end - begin) / (double)(5000000 * loops) << " us/iter" << endl;
        }

        return 0;
    }
};

MAIN_APP(lltest);
