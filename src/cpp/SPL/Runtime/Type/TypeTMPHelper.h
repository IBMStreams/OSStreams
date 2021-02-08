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

#ifndef SPL_RUNTIME_TYPE_TYPE_TMP_HELPER_H
#define SPL_RUNTIME_TYPE_TYPE_TMP_HELPER_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Type/Integer.h>

namespace SPL {

/// Template Meta Programming stuff for finding the type to use for
/// representing the number of used characters
namespace TMP {
template<uint32_t s>
struct Log2
{
    enum
    {
        value = Log2<(s >> 1)>::value + 1
    };
};
template<>
struct Log2<1>
{
    enum
    {
        value = 0
    };
};
template<uint32_t s>
struct SizeTypeRaw
{
    typedef uint32 value;
};
template<>
struct SizeTypeRaw<8>
{
    typedef uint8 value;
};
template<>
struct SizeTypeRaw<16>
{
    typedef uint16 value;
};
template<uint32_t s>
struct SizeType
{
    enum
    {
        size = 8 * (1 + (Log2<s>::value) / 8)
    };
    typedef typename SizeTypeRaw<size>::value value;
};
inline uint8_t getSizeTypeSize(uint32_t n)
{
    if (n < 256) {
        return 1;
    }
    if (n < 63356) {
        return 2;
    }
    return 4;
}
};

};

#endif /* DOXYGEN_SKIP_FOR_USERS */

#endif /* SPL_RUNTIME_TYPE_TYPE_TMP_HELPER_ */
