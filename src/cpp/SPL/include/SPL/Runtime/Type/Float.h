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

#ifndef SPL_RUNTIME_TYPE_FLOAT_H
#define SPL_RUNTIME_TYPE_FLOAT_H

/*!
 * \file Float.h \brief Definitions of the %SPL float types.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <float.h>
#include <limits>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    typedef float float32;
    typedef double float64;

    // use ByteBufferHelpers.h for <<, >> binary de/serialization
    // use std::iostream for <<, >> character de/serializarion
    // use std::swap

    inline std::ostream & serializeWithPrecision(std::ostream & s, float32 const & v)
    {
        if (isnanf(v)) {
            s << (((* (const int32_t*) &v) < 0) ? "-nan":"nan");
        } else {
            std::streamsize p = s.precision(std::numeric_limits<float32>::digits10);
            s << v;
            s.precision(p);
        }
        return s;
    }

    inline std::ostream & serializeWithPrecision(std::ostream & s, float64 const & v)
    {
        if (isnan(v)) {
            s << (((* (const int64_t*) &v) < 0) ? "-nan":"nan");
        } else {
            std::streamsize p = s.precision(std::numeric_limits<float64>::digits10);
            s << v;
            s.precision(p);
        }
        return s;
    }

    inline void deserializeWithSuffix(std::istream & s, float32 & v)
    {
      parseWithSuffixes(s,v,'f','w');
    }

    inline void deserializeWithSuffix(std::istream & s, float64 & v)
    {
      parseWithSuffixes(s,v,'f','l');
    }

    inline void deserializeWithNanAndInfs(std::istream & s, float32 & v, bool withSuffix = false)
    {
      parseWithNanAndInfs(s,v);
      if (withSuffix) {
          parseSuffixes(s, 'd', 'w');
      }
    }

    inline void deserializeWithNanAndInfs(std::istream & s, float64 & v, bool withSuffix = false)
    {
      parseWithNanAndInfs(s,v);
      if (withSuffix) {
          parseSuffixes(s, 'd', 'l');
      }
    }

};

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_FLOAT_H */
