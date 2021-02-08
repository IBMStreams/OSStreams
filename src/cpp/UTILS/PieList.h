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

// -*- c++ -*-

#ifndef __PIE_LIST__H__
#define __PIE_LIST__H__

#include <UTILS/UTILSTypes.h>
#include <cassert>
#include <iostream>
#include <stdint.h>

UTILS_NAMESPACE_BEGIN

template<typename T>
class PieList
{
  protected:
    uint32_t _slice_count;
    uint32_t _first_slice_pos;
    uint32_t _list_size;
    uint32_t* _slice_size;

    T* _list;

  public:
    PieList(uint32_t slice_count, uint32_t list_size)
      : _slice_count(slice_count)
      , _first_slice_pos(0)
      , _list_size(list_size)
      , _slice_size(new uint32_t[slice_count])
      , _list(new T[list_size])
    {
        _slice_size[0] = _list_size;

        for (uint32_t i = 1; i < _slice_count; i++) {
            _slice_size[i] = 0;
        }
    }

    ~PieList()
    {
        delete[] _slice_size;
        delete[] _list;
    }

    uint32_t size() const { return _list_size; }

    uint32_t size(uint32_t slice) const
    {
        assert(slice < _slice_count);

        return _slice_size[slice];
    }

    uint32_t first_idx(uint32_t slice) const
    {
        assert(slice < _slice_count);
        assert(_slice_size[slice] > 0);

        uint32_t idx = _first_slice_pos;

        for (uint32_t i = 0; i <= slice; i++) {
            idx += _slice_size[i];
        }

        return (idx - 1) % _list_size;
    }

    uint32_t last_idx(uint32_t slice) const
    {
        assert(slice < _slice_count);
        assert(_slice_size[slice] > 0);

        uint32_t idx = _first_slice_pos;

        for (uint32_t i = 0; i < slice; i++) {
            idx += _slice_size[i];
        }

        return idx % _list_size;
    }

    T& first(uint32_t slice) const { return _list[first_idx(slice)]; }

    T& last(uint32_t slice) const { return _list[last_idx(slice)]; }

    T& shift(uint32_t slice)
    {
        assert(slice < _slice_count);
        assert(_slice_size[slice] > 0);

        uint32_t next_slice = (slice + 1) % _slice_count;

        _slice_size[slice]--;
        _slice_size[next_slice]++;

        if (slice == _slice_count - 1) {
            if (_first_slice_pos == 0) {
                _first_slice_pos = _list_size - 1;
            } else {
                _first_slice_pos--;
            }
        }

        return last(next_slice);
    }

    const T& at(uint32_t i) const
    {
        assert(i < _list_size);

        return _list[i];
    }

    T& at(uint32_t i)
    {
        assert(i < _list_size);

        return _list[i];
    }

    T& operator[](uint32_t i) { return at(i); }

    void print(std::ostream& strm) const
    {
        uint32_t idx = _first_slice_pos;

        strm << "{";
        for (uint32_t s = 0; s < _slice_count; s++) {
            if (s > 0) {
                strm << ",";
            }

            strm << "[" << s << "]<";
            for (uint32_t i = 0; i < _slice_size[s]; i++) {
                if (i > 0) {
                    strm << ",";
                }

                strm << _list[idx];
                idx = (idx + 1) % _list_size;
            }
            strm << ">";
        }
        strm << "}";
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& strm, const Distillery::PieList<T>& p);

UTILS_NAMESPACE_END

template<typename T>
std::ostream& Distillery::operator<<(std::ostream& strm, const Distillery::PieList<T>& p)
{
    p.print(strm);
    return strm;
}

#endif
