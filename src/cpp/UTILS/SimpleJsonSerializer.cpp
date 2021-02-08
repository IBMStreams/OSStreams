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

#include <TRC/DistilleryDebug.h>
#include <UTILS/SimpleJsonSerializer.h>
#include <UTILS/SupportFunctions.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <sstream>

using namespace std;
using namespace boost::property_tree;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

void SimpleJsonSerializer::toJson(ptree& pt) const {}
void SimpleJsonSerializer::toSimpleJson(SimpleJsonObj& pt) const {}

void SimpleJsonSerializer::serializeJson(string& jsonStr, bool pretty) const
{
    ptree pt;
    toJson(pt);
    stringstream ss("");
    json_parser::write_json(ss, pt);
    if (pretty) {
        jsonStr.assign(ss.str());
    } else {
        // Make sure all contents are in one line, as java side just parsing single line each time.
        // jsonStr.assign(removeCharIfPresent(removeUnprintableChars(ss.str()), ' '));
        jsonStr.assign(removeUnprintableChars(ss.str()));
    }
    SPCDBG(L_TRACE, "Exit with: " << jsonStr, "sslWriter");
}

void SimpleJsonSerializer::serializeSimpleJson(string& jsonStr, bool pretty) const
{
    SimpleJsonObj pt;
    toSimpleJson(pt);
    jsonStr.assign(pt.writeJson("", pretty));
    SPCDBG(L_TRACE, "Exit with: " << jsonStr, "sslWriter");
}

void SimpleJsonSerializer::deserializeJson(const string& jsonStr) {}
