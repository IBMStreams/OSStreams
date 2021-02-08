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

#ifndef SPL_BIG_STRING_H
#define SPL_BIG_STRING_H

#include <UTILS/HashMapHelpers.h>
#include <string>

namespace SPL {
/// This class is used to compare big strings for equality faster than usual,
/// especially when most of the time the strings share a large prefix. It does
/// this by first compating the sha1 signature first

class BigString
{
  public:
    BigString() {}

    BigString(std::string const& instr)
      : str_(instr)
    {
        recomputeSignature();
    }

    bool operator==(const BigString& oth) const;
    bool operator!=(const BigString& oth) const { return !this->operator==(oth); }

    std::string const& str() const { return str_; }

    std::string const& sig() const { return sig_; }

    std::string& str() { return str_; }

    void recomputeSignature();

  private:
    std::string str_;
    std::string sig_;
};
};

namespace std {
namespace tr1 {
template<>
struct hash<SPL::BigString>
{
    size_t operator()(SPL::BigString const& bstr) const
    {
        hash<std::string> myhash;
        return myhash(bstr.sig());
    }
};
}
}

#endif /* SPL_BIG_STRING_H */
