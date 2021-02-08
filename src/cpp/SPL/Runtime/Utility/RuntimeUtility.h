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

#ifndef SPL_RUNTIME_UTILITY_H
#define SPL_RUNTIME_UTILITY_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>
#include <string>
#include <typeinfo>
#include <vector>

namespace xmlns {
namespace prod {
namespace streams {
namespace application {
class applicationSetType;
}
}
}
}

namespace SPL {
template<class T>
class list;

class DLL_PUBLIC RuntimeUtility
{
  public:
    /// Demangle class name T
    /// @return demangled type name
    template<class T>
    static std::string demangleTypeName()
    {
        return demangleTypeName(typeid(T));
    }

    /// Demangle class name
    /// @param tp type
    /// @return demangled type name
    static std::string demangleTypeName(std::type_info const& tp);

    /// Get the string representation of errno
    /// @return string representation of errno
    static std::string getErrorNoStr();

    /// Get the string representation of errnum
    /// @param errnum the error number.
    /// @return string representation of errnum
    static std::string getErrorNoStr(int errnum);
};
};

#endif /*SPL_RUNTIME_UTILITY_H*/
