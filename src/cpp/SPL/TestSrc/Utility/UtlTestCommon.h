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

#ifndef UTILITY_TEST_COMMON_H
#define UTILITY_TEST_COMMON_H

#include <UTILS/DistilleryException.h>

#include <cassert>
#include <cstdlib>

#define FASSERT(cond)                                                                              \
    if (!(cond)) {                                                                                 \
        std::cerr << "Assertion failed.\n";                                                        \
        assert(false);                                                                             \
        ::abort();                                                                                 \
    }

namespace SPL {
template<class T>
std::string toString(T const& t)
{
    std::ostringstream x;
    x << t;
    return x.str();
}
}

#endif /* UTILITY_TEST_COMMON_H */
