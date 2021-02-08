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

//----------------------------------------------------------------------------
//
// Title-
//       utils_vector.h
//
// Purpose-
//       Overload the std::vector to add a new constructor
//
//----------------------------------------------------------------------------

#ifndef _UTILS_VECTOR_H_
#define _UTILS_VECTOR_H_

#include <UTILS/UTILSTypes.h>
#include <vector>

UTILS_NAMESPACE_BEGIN

template<typename T>
class utils_vector : public std::vector<T>
{

  public:
    typedef T value_type;

    utils_vector(int init_size = 0)
      : std::vector<T>(init_size)
    {
        // nothing to do
    }

    utils_vector(value_type vdata[], int data_size) { bulk_append(vdata, data_size); }

    void bulk_append(value_type vdata[], int data_size)
    {
        for (int i = 0; i < data_size; i++) {
            this->push_back(vdata[i]);
        }
    }
};

UTILS_NAMESPACE_END

#define UTILS_VECTOR(vec, type, data) bob::utils_vector<type> vec(data, sizeof(data) / sizeof(type))
#define UTILS_VECTOR_APPEND(vec, type, data) vec.bulk_append(data, sizeof(data) / sizeof(type))

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
