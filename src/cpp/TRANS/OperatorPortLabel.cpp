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

#include <TRANS/OperatorPortLabel.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

const unsigned long long OperatorPortLabel::errorId_ /* = ULLONG_MAX */;

int OperatorPortLabel::parseIds() const
{
    // check for empty label
    if (label_.empty()) {
        setParseErrorIds();
        return EINVAL;
    }

    // label must have format "number.number.number"
    const char* dotPos = ::strchr(label_.c_str(), '.');
    if (dotPos == NULL) {
        setParseErrorIds();
        return EINVAL;
    }

    char* endPtr;
    long long jobId = ::strtoull(label_.c_str(), &endPtr, 10);
    if (endPtr != dotPos) {
        setParseErrorIds();
        return EINVAL;
    }

    dotPos = ::strchr(endPtr + 1, '.');
    if (dotPos == NULL) {
        setParseErrorIds();
        return EINVAL;
    }

    long long operatorIndex = ::strtoull(endPtr + 1, &endPtr, 10);
    if (endPtr != dotPos) {
        setParseErrorIds();
        return EINVAL;
    }

    long long operatorPortIndex = ::strtoull(endPtr + 1, &endPtr, 10);
    if (endPtr != label_.c_str() + label_.length()) {
        setParseErrorIds();
        return EINVAL;
    }

    jobId_ = static_cast<unsigned long long>(jobId);
    operatorIndex_ = static_cast<unsigned long long>(operatorIndex);
    operatorPortIndex_ = static_cast<unsigned long long>(operatorPortIndex);

    SPCDBG(L_TRACE,
           "After parsing of opertor label " << label_ << " : jobId_ = " << jobId_
                                             << ", operatorIndex_ = " << operatorIndex_
                                             << ", operatorPortIndex_ = " << operatorPortIndex_,
           CORE_TRANS);
    return 0;
}
