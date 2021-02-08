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

#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Toolkit/MapFileParser.h>

void SPL::parseIndicies(const std::string& opClass,
                        const std::string& line,
                        std::stringstream& str,
                        SPL::list<int64_t>& indicies)
{
    str >> std::skipws;
    while (!str.eof()) {
        char c;
        str >> c;
        if (c == '#') {
            break;
        }
        if (c != ',') {
            SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR,
                                SPL_RUNTIME_FAILED_PARSE_INDICES(opClass, line), SPL_OPER_DBG);
        }
        int64 index;
        str >> index;
        if (str.fail()) {
            break;
        }
        indicies.push_back(index);
        str >> std::skipws;
    }
    if (str.fail()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR,
                            SPL_RUNTIME_FAILED_PARSE_INDICES(opClass, line), SPL_OPER_DBG);
    }
}
