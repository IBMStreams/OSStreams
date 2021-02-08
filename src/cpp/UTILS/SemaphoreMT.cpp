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

#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/SemaphoreMT.h>
#include <cassert>
#include <iostream>

UTILS_NAMESPACE_USE

SemaphoreMT::SemaphoreMT(const int myN)
  : n(myN)
  , m(new Mutex)
  , cv(new CV)
{}

SemaphoreMT::SemaphoreMT(const SemaphoreMT&)
{
    assert(0);
}

bool SemaphoreMT::testBlock(void)
{
    bool ret;
    m->lock();
    ret = (n - 1 < 0) ? true : false;
    m->unlock();

    return ret;
}

void SemaphoreMT::wait(void)
{
    m->lock();
    n--;
    while (n < 0) {
        cv->wait(*m);
    }
    m->unlock();
}

void SemaphoreMT::post(void)
{
    m->lock();
    n++;
    if (n <= 0) {
        cv->signal();
    }
    m->unlock();
}

SemaphoreMT::~SemaphoreMT(void)
{
    delete m;
    delete cv;
}
