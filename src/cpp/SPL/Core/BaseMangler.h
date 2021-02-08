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

#ifndef BASE_MANGLER_H
#define BASE_MANGLER_H

#include <stdint.h>
#include <string>

namespace SPL {
//! Mangle names, optionally to no more than MaxMangledNameSize characters
class BaseMangler
{
  public:
    static long MaxMangledNameSize;

    /// Compress a mangled name to be smaller than MaxMangledNameSize
    /// @param name name to be mangled
    /// @param unique unique identifier
    /// @param truncate if true the name is truncated when too long and the
    /// unique identifier is appended, otherwise it is never truncated.
    /// @param addPrefix if true, add a prefix that identifies the type of the
    /// mangled name
    /// @return mangled name
    static std::string compress(const std::string& name,
                                uint64_t unique,
                                bool truncate,
                                bool addPrefix);

    /// Set the max mangled name size based on the application directory name
    /// @param dirname application directory name
    static void setMaxMangledNameSize(std::string const& dirname);

  protected:
    /// prefix for human readable strings
    ///
    static const char* readablePrefix;
    /// prefix for base 64 encoded strings
    ///
    static const char* base64Prefix;
    /// prefix for uniquely mangled strings
    ///
    static const char* uniquePrefix;
};
};

#endif /* BASE_MANGLER_H */
