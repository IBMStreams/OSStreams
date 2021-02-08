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
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <UTILS/Atomic.h>
#include <UTILS/Mutex.h>
#include <UTILS/Thread.h>

using namespace std;
UTILS_NAMESPACE_USE;

atomic_int32_t counter;
int bound;
Mutex mtx("bob");

class T : public Thread
{
    virtual void* run(void* threadArgs)
    {
        for (int i = 0; i < bound; i++) {
            // mtx.lock( ) ;
            // counter++ ;
            atomic_inc32(&counter);
            // mtx.unlock( ) ;
        }

        return 0;
    }
};

int main(int argc, char** argv)
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <threads> <iterations>" << endl;
        return 1;
    }

    mtx.lock();
    mtx.unlock();

    counter = 0;
    bound = atoi(argv[2]);

    int tc = atoi(argv[1]);
    T* ths[tc];
    for (int i = 0; i < tc; i++) {
        ths[i] = new T;
        ths[i]->create();
    }

    for (int i = 0; i < tc; i++) {
        ths[i]->join();
        delete ths[i];
    }

    cout << " -> " << counter << endl;

    return 0;
}
