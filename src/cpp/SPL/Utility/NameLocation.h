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

#ifndef SPL_NAMELOCATION_H
#define SPL_NAMELOCATION_H

#include <boost/filesystem/path.hpp>
#include <string>
#include <tr1/functional_hash.h>
#include <typeinfo>

namespace SPL {
class NameLocation
{
  public:
    NameLocation(const std::string& nameIn, uint32_t lineIn, uint32_t colIn)
      : _name(nameIn)
      , _line(lineIn)
      , _col(colIn)
    {}
    const std::string& name() const { return _name; }
    uint32_t line() const { return _line; }
    uint32_t col() const { return _col; }
    bool operator==(const NameLocation& rhs) const { return name() == rhs.name(); }

  private:
    std::string _name;
    uint32_t _line;
    uint32_t _col;
};
}

namespace std {
namespace tr1 {
template<>
struct hash<SPL::NameLocation>
{
    size_t operator()(SPL::NameLocation const& n) const
    {
        hash<std::string> myhash;
        return myhash(n.name());
    }
};
}
}
#endif /*SPL_NAMELOCATION_H*/
