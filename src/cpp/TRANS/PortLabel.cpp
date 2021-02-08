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

#include <TRANS/PortLabel.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/SupportFunctions.h>

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

const unsigned long long PortLabel::errorId_ /* = ULLONG_MAX */;

int PortLabel::parseIds() const
{
    // check for empty label
    if (label_.empty()) {
        setParseErrorIds();
        return EINVAL;
    }

    // label must have format "number.number@number"
    std::vector<std::string> parts;
    tokenize(label_, parts, ".@", false);
    if (parts.size() != 3) {
        setParseErrorIds();
        return EINVAL;
    }

    try {
        peId_ = boost::lexical_cast<unsigned long long>(parts[0]);
        portId_ = boost::lexical_cast<unsigned long long>(parts[1]);
        jobId_ = boost::lexical_cast<unsigned long long>(parts[2]);
    } catch (boost::bad_lexical_cast const& e) {
        return EINVAL;
    }

    SPCDBG(L_TRACE,
           "After parsing of label " << label_ << " : peId_ = " << peId_
                                     << ", portId_ = " << portId_ << ", jobId_ = " << jobId_,
           CORE_TRANS);
    return 0;
}
