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

#include <SPL/Runtime/Utility/RuntimeUtility.h>

#include <SAM/SAMHelperFunctions.h>
#include <SAM/SAMTypes.h>
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Type/SPLType.h>

#include <cxxabi.h>
#include <errno.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Distillery;

string RuntimeUtility::demangleTypeName(std::type_info const& tp)
{
    string type = tp.name();
    int status = 0;
    size_t length = 0;
    char* result = __cxxabiv1::__cxa_demangle(type.c_str(), NULL, &length, &status);
    if (status != 0 || result == NULL) {
        THROW_CHAR(SPLRuntime, "Unable to demangle class name through __cxa_demangle");
    }
    type = result;
    free(result);
    return type;
}

string RuntimeUtility::getErrorNoStr()
{
    return getErrorNoStr(errno);
}

string RuntimeUtility::getErrorNoStr(int errnum)
{
    const size_t mxlen = 1024;
    char errstr[mxlen];
    return strerror_r(errnum, errstr, mxlen);
}
