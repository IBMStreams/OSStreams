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

#include <UTILS/DistilleryCallback.h>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE;

class bob
{
  public:
    void f(int x) { cout << "[bob] int=" << x << endl; }

    void g(const char* x) { cout << "[bob] string=" << x << endl; }

    void h(int x, const char* y) { cout << "[bob] " << x << "=" << y << endl; }
};

struct cbstruct
{
    int i;
    Callback_1<int> s;
};

int main(int argc, char** argv)
{
    bob b;

    MemberFunction_1<bob, int> mf(&b, &bob::f);
    mf.call(666);

    Callback_1<int> s = mf;
    s.call(777);
    s(13);

    cbstruct slt[2] = { { 0, mf }, { 0, MemberFunction_1<bob, int>(&b, &bob::f) } };

    slt[0].s.call(888);
    slt[1].s.call(999);

    Callback_1<const char*> ss(MemberFunction_1<bob, const char*>(&b, &bob::g));
    ss.call("moby");
    ss("moby");

    Callback_1<const char*> ss2(mem_cb(&b, &bob::g));
    ss2.call("dick");
    ss2("dick");

    cbstruct slt2[1] = { { 0, mem_cb(&b, &bob::f) } };

    slt2[0].s.call(333);
    slt2[0].s(11);

    Callback_2<int, const char*> ss3(mem_cb(&b, &bob::h));
    ss3.call(9, "9");
    ss3(9, "9");

    return 0;
}
