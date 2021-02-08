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

#ifndef SPL_MODEL_HELPER_H
#define SPL_MODEL_HELPER_H

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <istream>
#include <string>
#include <vector>

#include <SPL/Core/CompilerException.h>
#include <UTILS/HashMapHelpers.h>

#define SPL_SIG_SEP '\1'

namespace SPL {
template<class T>
inline void getCodeGenSignature(std::ostream& ostr, std::vector<T> const& x)
{
    ostr << x.size();
    typedef typename std::vector<T>::const_iterator iter;
    for (iter it = x.begin(); it != x.end(); ++it) {
        ostr << SPL_SIG_SEP;
        (*it)->getCodeGenSignature(ostr);
    }
}

template<>
inline void getCodeGenSignature<std::string>(std::ostream& ostr, std::vector<std::string> const& x)
{
    ostr << x.size();
    typedef std::vector<std::string>::const_iterator iter;
    for (iter it = x.begin(); it != x.end(); ++it) {
        ostr << SPL_SIG_SEP << (*it);
    }
}

template<>
inline void getCodeGenSignature<unsigned>(std::ostream& ostr, std::vector<unsigned> const& x)
{
    ostr << x.size();
    typedef std::vector<unsigned>::const_iterator iter;
    for (iter it = x.begin(); it != x.end(); ++it) {
        ostr << SPL_SIG_SEP << (*it);
    }
}
};

#define SPL_MODEL_ENUM(newname, ns, xsdname, enums...)                                             \
                                                                                                   \
    class newname                                                                                  \
    {                                                                                              \
      public:                                                                                      \
        enum value                                                                                 \
        {                                                                                          \
            enums                                                                                  \
        };                                                                                         \
        newname() {}                                                                               \
        newname(value v)                                                                           \
          : value_(v)                                                                              \
        {}                                                                                         \
        newname(const ns::xsdname& o)                                                              \
        {                                                                                          \
            int i = static_cast<ns::xsdname::value>(o);                                            \
            value_ = static_cast<newname::value>(i);                                               \
        }                                                                                          \
        newname& operator=(value v)                                                                \
        {                                                                                          \
            value_ = v;                                                                            \
            return *this;                                                                          \
        }                                                                                          \
        newname& operator=(const ns::xsdname& o)                                                   \
        {                                                                                          \
            int i = static_cast<ns::xsdname::value>(o);                                            \
            value_ = static_cast<newname::value>(i);                                               \
            return *this;                                                                          \
        }                                                                                          \
        value getValue() const { return value_; }                                                  \
        operator newname::value() const { return value_; }                                         \
        operator ns::xsdname::value() const { return (ns::xsdname::value)(value_); }               \
                                                                                                   \
      private:                                                                                     \
        value value_;                                                                              \
        friend std::ostream& operator<<(std::ostream&, const newname&);                            \
        friend std::istream& operator>>(std::istream&, newname&);                                  \
    };                                                                                             \
    inline std::ostream& operator<<(std::ostream& ostr, const newname& em)                         \
    {                                                                                              \
        int i = em.value_;                                                                         \
        ns::xsdname oem(static_cast<ns::xsdname::value>(i));                                       \
        ostr << oem;                                                                               \
        return ostr;                                                                               \
    }                                                                                              \
    inline std::istream& operator>>(std::istream& istr, newname& em)                               \
    {                                                                                              \
        std::string token;                                                                         \
        istr >> token;                                                                             \
        em = ns::xsdname(token.c_str());                                                           \
        return istr;                                                                               \
    }

#endif /* SPL_MODEL_HELPER_H */
