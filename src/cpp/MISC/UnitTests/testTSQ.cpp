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

#include <TRC/DistilleryDebug.h>
#include <UTILS/Mutex.h>
#include <UTILS/TSharedQueue.h>
#include <cassert>
#include <iostream>

class A
{
  public:
    int id;
    float value;
};
using namespace std;
UTILS_NAMESPACE_USE
int main(int argc, char** argv)
{
    TSharedQueue<A*> q(10);

    A* b = NULL;
    A* a = new A;

    a->id = 15;
    a->value = 12.34;

    q.enqueue(a);

    q.dequeue(&b);

    cout << "b " << b->id << " " << b->value << endl;

    delete b;
    return 0;
}
