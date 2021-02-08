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
// Definitions for the UTILS library

#ifndef UTILTYPES_H
#define UTILTYPES_H

#include <string>
#include <sys/syscall.h>

#define UTILS_NAMESPACE Distillery
#define UTILS_NAMESPACE_BEGIN namespace UTILS_NAMESPACE {
#define UTILS_NAMESPACE_END }
#define UTILS_NAMESPACE_USE using namespace UTILS_NAMESPACE;
#define UTILS_NAMESPACE_QUALIFIER UTILS_NAMESPACE::

#define UTILS_RUNTIME_NAMESPACE_BEGIN                                                              \
    namespace xmlns {                                                                              \
    namespace prod {                                                                               \
    namespace streams {                                                                            \
    namespace runtime {                                                                            \
    namespace utils {
#define UTILS_RUNTIME_NAMESPACE_END                                                                \
    }                                                                                              \
    }                                                                                              \
    }                                                                                              \
    }                                                                                              \
    }
#define UTILS_RUNTIME_NS ::xmlns::prod::streams::runtime::utils
#define UTILS_RUNTIME_NS_USE using namespace UTILS_RUNTIME_NS;

#define gettid() syscall(SYS_gettid)

UTILS_NAMESPACE_BEGIN
/**
 * Enum conversion support.  Use the following template to support conversion
 * of string labels into enum values, as in the following example.
 *
 * For the following enum type: enum colors { RED, GREEN, BLUE };
 * define conversion from the string representation of the enum values:
 *
 * \verbatim
 * #define COLORS_SIZE 3
 * static const char* const literals_[COLORS_SIZE] = { "RED", "GREEN", "BLUE" };
 * colors const indexes_[COLORS_SIZE] = { RED, GREEN, BLUE };
 *
 * static colors convert(const std::string& s)
 * {
 *   enum_comparator<char> comp(literals_);
 *   const colors* i(
 *     std::lower_bound(indexes_, indexes_ + COLORS_SIZE, s, comp));
 *
 *   if (i == indexes_ + COLORS_SIZE || literals_[*i] != s) {
 *     THROW(UTILS_NAMESPACE::InvalidArgument, s);
 *   }
 *   return *i;
 * }
 * #undef COLORS_SIZE
 * \endverbatim
 */

template<typename C>
struct enum_comparator
{
    enum_comparator(const C* const* table)
      : table_(table)
    {}

    bool operator()(std::size_t i, const std::basic_string<C>& s) const { return table_[i] < s; }

    bool operator()(const std::basic_string<C>& s, std::size_t i) const { return s < table_[i]; }

    bool operator()(std::size_t i, std::size_t j) const
    {
        return std::basic_string<C>(table_[i]) < table_[j];
    }

  private:
    const C* const* table_;
};

UTILS_NAMESPACE_END

#endif // UTILTYPES_H

// Local Variables: ***
// mode:c++ ***
// End: ***
