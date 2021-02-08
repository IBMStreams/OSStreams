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

#include <UTILS/SupportFunctions.h>
#include <UTILS/Thread.h>
#include <cassert>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE

int main(int argc, char** argv)
{
    cout << "this thread can use any of " << Thread::getNCurrentCandidateCPUs() << " CPUs" << endl;
    assert(Thread::getNCurrentCandidateCPUs() == SystemStatics::getNCPUs());
    Thread::setCPU(0);
    cout << "this thread can use any of " << Thread::getNCurrentCandidateCPUs() << " CPUs" << endl;
    assert(Thread::getNCurrentCandidateCPUs() == 1);
    if (SystemStatics::getNCPUs() > 1) {
        Thread::addCPU(1);
        cout << "this thread can use any of " << Thread::getNCurrentCandidateCPUs() << " CPUs"
             << endl;
        assert(Thread::getNCurrentCandidateCPUs() == 2);
        vector<int32_t> c(Thread::getCurrentCandidateCPUs());
        assert(c.size() == 2);
    }
    Thread::useAllAvailableCPUs();
    cout << "this thread can use any of " << Thread::getNCurrentCandidateCPUs() << " CPUs" << endl;
    assert(Thread::getNCurrentCandidateCPUs() == SystemStatics::getNCPUs());
    return 0;
}
