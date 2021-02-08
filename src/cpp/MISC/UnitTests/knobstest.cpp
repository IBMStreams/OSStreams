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
#include <UTILS/DistilleryKnobs.h>
#include <cassert>
#include <cmath>
#include <iostream>

UTILS_NAMESPACE_USE
using namespace std;

class KnobsTest : public DistilleryApplication
{
  public:
    KnobsTest(void) {}

    virtual int run(const vector<string>& remains)
    {
        DistilleryKnobSet* ks1 = new DistilleryKnobSet(_dk, "rpc", "root");
        _dk.newKnobSet(ks1);

        try {
            // this should fail
            _dk.newKnobSet(ks1);
            assert(0);
        } catch (DistilleryException& e) {
            // as expected
        }

        ks1->newIntKnob("maxnumofretries", 12);
        ks1->newFloatKnob("maxretrydelay", 30.5);
        ks1->newStringKnob("RetryModel", "UNBOUNDED");

        cout << *ks1 << endl;
        cout << ks1->getIntKnob("maxnumofretries") << endl;

        ks1->setIntKnob("maxnumofretries", 14);
        int mrd = ks1->getIntKnob("maxnumofretries");
        if (mrd != 14) {
            THROW(Distillery, "assert(mrd==14)");
        }

        cout << ks1->getFloatKnob("maxretrydelay") << endl;

        DistilleryKnobSet* ks2 = new DistilleryKnobSet(_dk, "crpc", "rpc");
        _dk.newKnobSet(ks2);
        cout << *ks2 << endl;
        mrd = ks2->getIntKnob("maxnumofretries");
        if (mrd != 14) {
            THROW(Distillery, "assert(mrd==14)");
        }

        ks1->setFloatKnob("maxretrydelay", 12.7, true);
        float f = ks1->getFloatKnob("maxretrydelay");
        if (static_cast<int>(ceil(f)) != 13) {
            THROW(Distillery, "assert(static_cast<int>(ceil(f))==13)");
        }

        f = ks2->getFloatKnob("maxretrydelay");
        if (static_cast<int>(ceil(f)) != 13) {
            THROW(Distillery, "assert(static_cast<int>(ceil(f))==13)");
        }

        ks2->setFloatKnob("maxretrydelay", 13.7);
        f = ks2->getFloatKnob("maxretrydelay");

        if (static_cast<int>(ceil(f)) != 14) {
            THROW(Distillery, "assert(static_cast<int>(ceil(f))==14)");
        }

        ks1->setFloatKnob("maxretrydelay", 30.1);
        f = ks1->getFloatKnob("maxretrydelay");

        if (static_cast<int>(ceil(f)) != 31) {
            THROW(Distillery, "assert(static_cast<int>(ceil(f))==31)");
        }

        f = ks2->getFloatKnob("maxretrydelay");

        if (static_cast<int>(ceil(f)) != 14) {
            THROW(Distillery, "assert(static_cast<int>(ceil(f))==14)");
        }

        cout << "crpc.maxretrydelay: " << _dk.getFloatKnob("crpc.maxretrydelay") << endl;
        cout << "crpc.maxretrydelay: " << _dk.getFloatKnob("crpc.maxretrydelay") << endl;

        cout << "drpc.maxretrydelay: " << _dk.getFloatKnob("rpc.maxretrydelay") << endl;

        cout << _dk << endl;

        return 0;
    }
};

MAIN_APP(KnobsTest)
