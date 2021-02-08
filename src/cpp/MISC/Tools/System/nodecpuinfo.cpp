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

#include <UTILS/NodeCPUInfo.h>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE

int main(int argc, char** argv)
{
    cout << "Number of CPUs " << NodeCPUInfo::getNCPUs() << endl;
    cout << "Number of cores " << NodeCPUInfo::getNCores() << endl;
    cout << "Number of bogomips " << NodeCPUInfo::getBogomips() << endl;
    if (NodeCPUInfo::hasSSE()) {
        cout << "This node supports SSE extensions" << endl;
    } else {
        cout << "This node does not support SSE extensions" << endl;
    }
    if (NodeCPUInfo::hasSSE2()) {
        cout << "This node supports SSE2 extensions" << endl;
    } else {
        cout << "This node does not support SSE2 extensions" << endl;
    }
    cout << "------------------------------------------------" << endl;
    cout << "Complete info: " << endl;
    NodeCPUInfo::print(cout);
    return 0;
}
