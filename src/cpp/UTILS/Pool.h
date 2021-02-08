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

//
// Class Description:
//
// Implements a pool of object

#ifndef __POOL_H__
#define __POOL_H__

#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>
#include <cassert>
#include <string.h>

UTILS_NAMESPACE_BEGIN

template<class T>
class Pool
{
  public:
    typedef T _object_type_t;
    typedef _object_type_t* _object_type_ptr_t;

    Pool(const size_t cap = 128)
      : _capacity(cap)
      , _size(0)
    {
        _data_pool = new _object_type_ptr_t[_capacity];
        memset(_data_pool, 0, sizeof(_object_type_ptr_t) * _capacity);
    }

    virtual ~Pool(void)
    {
        flush();
        delete[] _data_pool;
    }

    virtual size_t capacity(void) const { return _capacity; }

    virtual _object_type_ptr_t pull(void)
    {
        AutoMutex lck(_mutex);

        if (!_size) {
            return instantiateNew();
        } else {
            for (size_t i = 0; i < _capacity; i++) {
                if (_data_pool[i]) {
                    _object_type_ptr_t ptr = _data_pool[i];
                    _data_pool[i] = 0;
                    _size--;
                    return ptr;
                }
            }
            assert(0);
            throw "cannot happen";
            return 0;
        }
    }

    virtual void push(_object_type_ptr_t obj)
    {
        AutoMutex lck(_mutex);

        if (_size == _capacity) {
            delete obj;
        } else {
            for (size_t i = 0; i < _capacity; i++) {
                if (!_data_pool[i]) {
                    _data_pool[i] = obj;
                    _size++;
                    return;
                }
            }
            assert(0);
        }
    }

    virtual void flush(void)
    {
        AutoMutex lck(_mutex);

        if (_size) {
            for (size_t i = 0; i < _capacity; i++) {
                if (_data_pool[i]) {
                    delete _data_pool[i];
                    _data_pool[i] = 0;
                }
            }
            _size = 0;
        }
    }

  protected:
    virtual _object_type_ptr_t instantiateNew(void) { return 0; }

    Mutex _mutex;
    _object_type_ptr_t* _data_pool;
    size_t _capacity;
    size_t _size;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
